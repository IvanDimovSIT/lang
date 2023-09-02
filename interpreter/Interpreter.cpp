#include "Interpreter.h"
#include "../util/TokenSubArrayFinder.h"
#include "InterpreterCalculator.h"
#include <cassert>

Interpreter::Interpreter(IRuntimeErrorReporter* errorReporter, IInterpreterIO* interpreterIO)
{
    assert(interpreterIO != nullptr);
    this->errorReporter = errorReporter;
    this->interpreterIO = interpreterIO;
}

bool Interpreter::execute(std::vector<Token*> &tokens, std::map<std::string, Function>& functions, Value& result)
{
    ProgramSymbols programSymbols = {functions,{}};
    Value empty;
    return execute(tokens, programSymbols,  empty, empty, result);
}


bool Interpreter::execute(
    std::vector<Token*> &tokens,
    ProgramSymbols& programSymbols,
    Value& left,
    Value& right,
    Value& result)
{
    const int size = tokens.size();
    bool hadError = false;
    std::unique_ptr<Value> leftParameter = std::make_unique<Value>();
    std::unique_ptr<Value> rightParameter = std::make_unique<Value>();
    std::unique_ptr<Value> lastResult = std::make_unique<Value>();
    Token* operation = nullptr;
    LoopStack loopStack;

    for(int i=0; i<size; i++){
        if(loopStack.size() != 0 && i == loopStack.top().loopEnd){
            Value conditionResult;
            if(!execute(loopStack.top().condition, programSymbols, left, right, conditionResult))
                return false;

            if(conditionResult[0] != 0.0)
                i = loopStack.top().loopStart;
            else
                loopStack.pop();
            
            continue;
        }

        if(tokens[i]->id == TokenIdEndLine || tokens[i]->id == TokenIdOpenCurly || tokens[i]->id == TokenIdCloseCurly){
            if(leftParameter->size() != 0){
                lastResult = std::move(leftParameter);
                leftParameter = std::make_unique<Value>();
                rightParameter = std::make_unique<Value>();
            }
            continue;
        }

        if(tokens[i]->id == TokenIdIf){
            std::vector<Token*> condition;
            Value conditionResult;
            int endCondition = TokenSubArrayFinder::findFirstTokenIdInLine(tokens, i, TokenIdOpenCurly);
            if(endCondition == TOKEN_INDEX_NOT_FOUND){
                if(errorReporter)
                    errorReporter->report(RuntimeErrorTypeMissingIfCondition);
                return false;
            }
            TokenSubArrayFinder::findSubArray(tokens, condition, i+1, endCondition-1);
            if(!execute(condition, programSymbols, left, right, conditionResult))
                return false;
            
            if(conditionResult[0] != 0.0){
                i = endCondition;
            }else{
                int afterIfBody = TokenSubArrayFinder::findClosingCurly(tokens, endCondition);
                if(afterIfBody == TOKEN_INDEX_NOT_FOUND){
                    if(errorReporter)
                        errorReporter->report(RuntimeErrorTypeInvalidIfSyntax);
                    return false;
                }
                i = afterIfBody;
            }
            continue;
        }

        if(tokens[i]->id == TokenIdLoop){
            LoopReturn loopData;
            Value conditionResult;
            loopData.loopStart = TokenSubArrayFinder::findFirstTokenIdInLine(tokens, i, TokenIdOpenCurly);
            if(loopData.loopStart == TOKEN_INDEX_NOT_FOUND){
                if(errorReporter)
                    errorReporter->report(RuntimeErrorTypeMissingLoopCondition);
                return false;
            }
            TokenSubArrayFinder::findSubArray(tokens, loopData.condition, i+1, loopData.loopStart-1);
            loopData.loopEnd = TokenSubArrayFinder::findClosingCurly(tokens, loopData.loopStart);
            if(!execute(loopData.condition, programSymbols, left, right, conditionResult))
                return false;

            if(conditionResult[0] != 0){
                loopStack.push(loopData);
                i = loopData.loopStart;
            }else{
                i = loopData.loopEnd;
            }
            continue;
        }

        if(tokens[i]->id == TokenIdEquals && i-1>=0 && tokens[i-1]->id == TokenIdVariable){
            const int statementEnd = TokenSubArrayFinder::findStatementEnd(tokens, i);
            std::vector<Token*> statement;
            TokenSubArrayFinder::findSubArray(tokens, statement, i+1, statementEnd);

            hadError = !execute(
                statement,
                programSymbols,
                left,
                right,
                *leftParameter);
            if(hadError)
                return false;
            
            lastResult = std::move(leftParameter);
            leftParameter = std::make_unique<Value>();
            programSymbols.variables[tokens[i - 1]->str] = *lastResult;
            i = statementEnd;
            continue;
        }

        if(tokens[i]->id == TokenIdWrite || tokens[i]->id == TokenIdWriteText){
            const int statementEnd = TokenSubArrayFinder::findStatementEnd(tokens, i);
            Value output;
            std::vector<Token*> statement;
            TokenSubArrayFinder::findSubArray(tokens, statement, i+1, statementEnd);
            hadError = !execute(
                statement,
                programSymbols,
                left,
                right,
                output);
            if(hadError)
                return false;
            
            if(output.size() >= 1){
                if(tokens[i]->id == TokenIdWrite)
                    interpreterIO->write(output);
                else
                    interpreterIO->writeText(output);
                *lastResult = output;
            }
            i = statementEnd;
            continue;
        }

        if((leftParameter->size() == 0 && tokens[i]->id != TokenIdFunction && operation == nullptr) ||
            isFunctionWithoutParameters(*tokens[i], programSymbols))
        {
            leftParameter = getNextArgument(i, tokens, programSymbols, left, right, hadError);
        }else if(operation == nullptr){
            operation = tokens[i];
        }else{
            rightParameter = getNextArgument(i, tokens, programSymbols, left, right, hadError);
        }

        if(hadError)
            return false;

        if(!checkForCalculation(tokens, i, programSymbols, left, right, operation, std::ref(leftParameter), std::ref(rightParameter)))
            return false;
    }


    if(leftParameter->size() != 0)
        lastResult = std::move(leftParameter);
    result = *lastResult;
    return true;
}

bool Interpreter::checkForCalculation(
    std::vector<Token*> &tokens,
    int& position,
    ProgramSymbols& programSymbols,
    Value& left,
    Value& right,
    Token*& operation,
    std::unique_ptr<Value>& leftParameter,
    std::unique_ptr<Value>& rightParameter)
{
    if(operation == nullptr)
        return true;
    
    bool leftArg, rightArg, hadError;
    hadError = !getOperatorOrFunctionParamerters(*operation, leftArg, rightArg, programSymbols); 
    if(hadError){
        if(errorReporter)
                errorReporter->report(RuntimeErrorTypeNotAnOperation);
        return false;
    }
    if(rightArg && rightParameter->size() > 0){
        leftParameter = executeOperationOrFunction(*leftParameter, *rightParameter, *operation, left, right, programSymbols, hadError);
        rightParameter = std::make_unique<Value>();
                
    }else if(operation->id == TokenIdApplyToEach){
        leftParameter = executeModifier(*leftParameter, tokens, programSymbols, left, right, position, hadError);
        position++;
    }else if(leftArg && leftParameter->size() > 0 && (!rightArg)){
        leftParameter = executeOperationOrFunction(*leftParameter, *rightParameter, *operation, left, right, programSymbols, hadError);
    }else{
        return true;
    }

    operation = nullptr;
    return true;
}

    
std::unique_ptr<Value> Interpreter::getNextArgument(
    int& position,
    std::vector<Token*> &tokens,
    ProgramSymbols& programSymbols,
    Value& left,
    Value& right,
    bool& hadError)
{
    std::unique_ptr<Value> result;

    switch(tokens[position]->id)
    {
    case TokenIdVariable:
        result = std::make_unique<Value>(programSymbols.variables[tokens[position]->str]);
        return std::move(result);
    break;
    case TokenIdLiteral:
        result = std::make_unique<Value>(tokens[position]->val);
        return std::move(result);
    break;
    case TokenIdLeftParam:
        result = std::make_unique<Value>(left);
        return std::move(result);
    break;
    case TokenIdRightParam:
        result = std::make_unique<Value>(right);
        return std::move(result);
    break;
    case TokenIdFunction:{
        bool ArgLeft, ArgRight;
        getOperatorOrFunctionParamerters(*(tokens[position]), ArgLeft, ArgRight, programSymbols);
        if((!ArgLeft) && (!ArgRight)){
            result = std::make_unique<Value>();
            hadError = !execute(programSymbols.functions[tokens[position]->str].body, programSymbols, left, right, *result);
            return std::move(result);
        }else{
            hadError = true;
            if(errorReporter)
                errorReporter->report(RuntimeErrorTypeOperationAsParameter);
            return std::make_unique<Value>();
        }
    }
    break;
    case TokenIdOpenParenthesis:{
        int endPos = TokenSubArrayFinder::findClosingParenthesis(tokens, position);
        if(endPos == TOKEN_INDEX_NOT_FOUND){
            hadError = true;
            if(errorReporter)
                errorReporter->report(RuntimeErrorTypeNoClosingParenthesis);
            return std::make_unique<Value>();
        }
        std::vector<Token*> sub;
        TokenSubArrayFinder::findSubArray(tokens, sub, position+1, endPos-1);
        result = std::make_unique<Value>();
        hadError = !execute(sub, programSymbols, left, right, *result);
        position = endPos;
        return std::move(result);
    }
    break;
    case TokenIdRead:
        return interpreterIO->read();
    break;
    case TokenIdReadText:
        return interpreterIO->readText();
    break;
    default:
        // ...
        break;
    }

    hadError = true;
    if(errorReporter)
        errorReporter->report(RuntimeErrorTypeEvaluationError);

    return std::move(result);
}
    
bool Interpreter::getOperatorOrFunctionParamerters(Token& operation,  bool& leftParam, bool& rightParam, ProgramSymbols& programSymbols)
{
    if(operation.id == TokenIdFunction){
        auto func = &programSymbols.functions[operation.str];
        leftParam = func->left;
        rightParam = func->right;
        return true;
    }

    switch (operation.id)
    {
    case TokenIdCount:
    case TokenIdLogicalNot:
    case TokenIdIterate:
    case TokenIdSumAll:
    case TokenIdMultiplyAll:
    case TokenIdRandom:
    case TokenIdFloor:
    case TokenIdCeil:
    case TokenIdRound:
    case TokenIdSort:
    case TokenIdReverse:
    case TokenIdApplyToEach:
    case TokenIdMakeSet:
        leftParam = true;
        rightParam = false;
        return true;
    break;
    case TokenIdAdd:
    case TokenIdSubtract:
    case TokenIdMultiply:
    case TokenIdDivide:
    case TokenIdMod:
    case TokenIdPower:
    case TokenIdLessThan:
    case TokenIdGreaterThan:
    case TokenIdLessThanOrEquals:
    case TokenIdGreaterThanOrEquals:
    case TokenIdIsEquals:
    case TokenIdNotEquals:
    case TokenIdUnion:
    case TokenIdSelect:
    case TokenIdLeftRotate:
    case TokenIdRightRotate:
        leftParam = true;
        rightParam = true;
        return true;
    break;
    default:break;
    }

    return false;
}



std::unique_ptr<Value> Interpreter::executeOperationOrFunction(
    Value& leftOfOperator,
    Value& rightOfOperator,
    Token& operation,
    Value& left,
    Value& right,
    ProgramSymbols& programSymbols,
    bool& hadError)
{
    switch (operation.id)
    {
    case TokenIdFunction:
        {
            auto result = std::make_unique<Value>();
            hadError = !execute(programSymbols.functions[operation.str].body, programSymbols, leftOfOperator, rightOfOperator, *result);
            return result;
        }
    case TokenIdAdd:
        return InterpreterCalculator::add(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdSubtract:
        return InterpreterCalculator::subtract(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdMultiply:
        return InterpreterCalculator::multiply(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdDivide:
        return InterpreterCalculator::divide(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdMod:
        return InterpreterCalculator::mod(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdPower:
        return InterpreterCalculator::power(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdIterate:
        return InterpreterCalculator::iterate(leftOfOperator, hadError, errorReporter);
    case TokenIdLogicalNot:
        return InterpreterCalculator::logicalNot(leftOfOperator, hadError, errorReporter);
    case TokenIdCount:
        return InterpreterCalculator::count(leftOfOperator, hadError, errorReporter);
    case TokenIdSumAll:
        return InterpreterCalculator::sumAll(leftOfOperator, hadError, errorReporter);
    case TokenIdMultiplyAll:
        return InterpreterCalculator::multiplyAll(leftOfOperator, hadError, errorReporter);
    case TokenIdLessThan:
        return InterpreterCalculator::lessThan(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdGreaterThan:
        return InterpreterCalculator::greaterThan(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdLessThanOrEquals:
        return InterpreterCalculator::lessThanOrEquals(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdGreaterThanOrEquals:
        return InterpreterCalculator::greaterThanOrEquals(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdIsEquals:
        return InterpreterCalculator::equals(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdNotEquals:
        return InterpreterCalculator::notEquals(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdUnion:
        return InterpreterCalculator::findUnion(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdSelect:
        return InterpreterCalculator::select(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdRandom:
        return InterpreterCalculator::randomize(leftOfOperator, hadError, errorReporter);
    case TokenIdMakeSet:
        return InterpreterCalculator::makeSet(leftOfOperator, hadError, errorReporter);
    case TokenIdCeil:
        return InterpreterCalculator::findCeil(leftOfOperator, hadError, errorReporter);
    case TokenIdFloor:
        return InterpreterCalculator::findFloor(leftOfOperator, hadError, errorReporter);
    case TokenIdRound:
        return InterpreterCalculator::findRound(leftOfOperator, hadError, errorReporter);
    case TokenIdSort:
        return InterpreterCalculator::sortArray(leftOfOperator, hadError, errorReporter);
    case TokenIdReverse:
        return InterpreterCalculator::reverseArray(leftOfOperator, hadError, errorReporter);
    case TokenIdLeftRotate:
        return InterpreterCalculator::leftRotate(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdRightRotate:
        return InterpreterCalculator::rightRotate(leftOfOperator, rightOfOperator, hadError, errorReporter);
    default:break;
    }

    hadError = true;
    if(errorReporter)
        errorReporter->report(RuntimeErrorTypeNotAnOperation);

    return std::make_unique<Value>();
}

bool Interpreter::isFunctionWithoutParameters(Token& function, ProgramSymbols& programSymbols)
{
    bool left, right;
    if(function.id != TokenIdFunction || !getOperatorOrFunctionParamerters(function, left, right, programSymbols))
        return false;

    return (!left) && (!right);
}

std::unique_ptr<Value> Interpreter::executeModifier(
    Value& leftParameter,
    std::vector<Token*> &tokens,
    ProgramSymbols& programSymbols,
    Value& left,
    Value& right,
    int position,
    bool& hadError)
{
    auto result = std::make_unique<Value>();
    if(position == 0){
        hadError = true;
        if(errorReporter)
            errorReporter->report(RuntimeErrorTypeNoOperatorToModify);
        return std::move(result);
    }

    const int size = leftParameter.size();
    if(size == 0){
        hadError = true;
        if(errorReporter)
            errorReporter->report(RuntimeErrorTypeNoOperatorToModify);
        return std::move(result);
    }

    if(position+1 >= tokens.size()){
        hadError = true;
        if(errorReporter)
            errorReporter->report(RuntimeErrorTypeNoOperatorToModify);
        return std::move(result);
    }

    Token* operation = tokens[position+1];
    for(int i=0; i<size; i++){
        Value first = {leftParameter[i]};
        Value second = {(i+1>=size? 0.0:leftParameter[i+1])};

        auto operationResult = executeOperationOrFunction(first, second, *operation, left, right, programSymbols, hadError);
        for(const auto& j:*operationResult)
            result->push_back(j);
    }

    if(result->size() == 0){
        hadError = true;
        if(errorReporter)
            errorReporter->report(RuntimeErrorTypeNoOperatorToModify);
    }
    return std::move(result);
}
