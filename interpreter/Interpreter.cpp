#include "Interpreter.h"
#include "../util/TokenSubArrayFinder.h"
#include "InterpreterCalculator.h"
#include <cassert>

struct LoopReturn{
    std::vector<Token*> condition;
    int loopStart;
    int loopEnd;
};

Interpreter::Interpreter(IRuntimeErrorReporter* errorReporter, IInterpreterIO* interpreterIO)
{
    assert(interpreterIO != nullptr);
    this->errorReporter = errorReporter;
    this->interpreterIO = interpreterIO;
}

bool Interpreter::execute(std::vector<Token*> &tokens, std::map<std::string, Function>& functions, Value& result)
{
    ProgramState programState = {functions,{}, {}};
    initVariables(tokens, programState);
    Value empty;
    bool successfulExecution = execute(tokens, programState,  empty, empty, result);
    joinThreads(programState);

    return successfulExecution;
}

 bool Interpreter::execute(std::vector<Token*> &tokens, ProgramState& programState, Value& result)
 {
    Value empty;
    bool successfulExecution = execute(tokens, programState,  empty, empty, result);
    joinThreads(programState);

    return successfulExecution;
 }

void Interpreter::initVariables(std::vector<Token*> &tokens, ProgramState& programState)
{
    for(const auto& i: tokens){
        if(i->id != TokenIdVariable)
            continue;
        
        programState.variables[i->str].value = {0.0};
    }

    for(const auto& i: programState.functions){
        for(const auto& j: i.second.body){
            if(j->id != TokenIdVariable)
            continue;
        
        programState.variables[j->str].value = {0.0};
        }
    }
}

void Interpreter::executeOnThread(
    std::vector<Token*> tokens,
    ProgramState& programState,
    Value argumentA,
    Value argumentB)
{
    Value ignore;
    if(!execute(tokens, programState, argumentA, argumentB, ignore)){
        report(tokens, 0, RuntimeErrorTypeThreadHadError);
    }
}

bool Interpreter::execute(
    std::vector<Token*> &tokens,
    ProgramState& programState,
    Value& argumentA,
    Value& argumentB,
    Value& result)
{
    const int size = tokens.size();
    bool hadError = false;
    std::unique_ptr<Value> leftParameter = std::make_unique<Value>();
    std::unique_ptr<Value> rightParameter = std::make_unique<Value>();
    std::unique_ptr<Value> lastResult = std::make_unique<Value>();
    Token* operation = nullptr;
    std::stack<LoopReturn> loopStack;

    for(int i=0; i<size; i++){
        if(loopStack.size() != 0 && i == loopStack.top().loopEnd){
            Value conditionResult;
            if(!execute(loopStack.top().condition, programState, argumentA, argumentB, conditionResult))
                return false;

            if(conditionResult[0] != 0.0)
                i = loopStack.top().loopStart;
            else
                loopStack.pop();
            
            continue;
        }

        switch(tokens[i]->id){
            case TokenIdEndLine:
            case TokenIdOpenCurly:
            case TokenIdCloseCurly:
            {
                endStatement(lastResult, leftParameter, rightParameter);
                continue;
            }
            case TokenIdAsyncStart:
            {
                if(!executeAsync(tokens, i, programState, argumentA, argumentB)){
                    report(tokens, i, RuntimeErrorTypeThreadHadError);
                    return false;
                }
                continue;
            }
            case TokenIdAsyncJoin:
            {
                joinThreads(programState);
                continue;
            }
            case TokenIdIf:
            {
                std::vector<Token*> condition;
                Value conditionResult;
                int endCondition = TokenSubArrayFinder::findFirstTokenIdInLine(tokens, i, TokenIdOpenCurly);
                if(endCondition == TokenSubArrayFinder::TOKEN_INDEX_NOT_FOUND){
                    report(tokens, i, RuntimeErrorTypeMissingIfCondition);
                    return false;
                }
                TokenSubArrayFinder::findSubArray(tokens, condition, i+1, endCondition-1);
                if(!execute(condition, programState, argumentA, argumentB, conditionResult))
                    return false;
            
                if(conditionResult[0] != 0.0){
                    i = endCondition;
                }else{
                    int afterIfBody = TokenSubArrayFinder::findClosingCurly(tokens, endCondition);
                    if(afterIfBody == TokenSubArrayFinder::TOKEN_INDEX_NOT_FOUND){
                        report(tokens, i, RuntimeErrorTypeInvalidIfSyntax);
                        return false;
                    }
                    i = afterIfBody;
                }
                continue;
            }
            case TokenIdLoop:
            {
                LoopReturn loopData;
                Value conditionResult;
                loopData.loopStart = TokenSubArrayFinder::findFirstTokenIdInLine(tokens, i, TokenIdOpenCurly);
                if(loopData.loopStart == TokenSubArrayFinder::TOKEN_INDEX_NOT_FOUND){
                    report(tokens, i, RuntimeErrorTypeMissingLoopCondition);
                    return false;
                }
                TokenSubArrayFinder::findSubArray(tokens, loopData.condition, i+1, loopData.loopStart-1);
                loopData.loopEnd = TokenSubArrayFinder::findClosingCurly(tokens, loopData.loopStart);
                if(!execute(loopData.condition, programState, argumentA, argumentB, conditionResult))
                    return false;

                if(conditionResult[0] != 0){
                    loopStack.push(loopData);
                    i = loopData.loopStart;
                }else{
                    i = loopData.loopEnd;
                }
                continue;
            }
            case TokenIdWrite:
            case TokenIdWriteText:
            {
                const int statementEnd = TokenSubArrayFinder::findStatementEnd(tokens, i);
                Value output;
                std::vector<Token*> statement;
                TokenSubArrayFinder::findSubArray(tokens, statement, i+1, statementEnd);
                hadError = !execute(
                    statement,
                    programState,
                    argumentA,
                    argumentB,
                    output);
                if(hadError)
                    return false;
            
                if(output.size() >= 1){
                    if(tokens[i]->id == TokenIdWrite)
                        writeNumbers(programState, output);
                    else
                        writeText(programState, output);
                    *lastResult = output;
                }
                i = statementEnd;
                continue;
            }
            case TokenIdEquals:
            if(i-1>=0 && tokens[i-1]->id == TokenIdVariable){
                const int statementEnd = TokenSubArrayFinder::findStatementEnd(tokens, i);
                std::vector<Token*> statement;
                TokenSubArrayFinder::findSubArray(tokens, statement, i+1, statementEnd);

                hadError = !execute(
                    statement,
                    programState,
                    argumentA,
                    argumentB,
                    *leftParameter);
                if(hadError)
                    return false;
            
                lastResult = std::move(leftParameter);
                leftParameter = std::make_unique<Value>();
                setVariable(*lastResult, tokens[i - 1]->str, programState);
                i = statementEnd;
                continue;
            }
            break;
            default:break;
        }

        if((leftParameter->size() == 0 && tokens[i]->id != TokenIdFunction && operation == nullptr) ||
            isFunctionWithoutParameters(*tokens[i], programState))
        {
            leftParameter = getNextArgument(i, tokens, programState, argumentA, argumentB, hadError);
        }else if(operation == nullptr){
            operation = tokens[i];
        }else{
            rightParameter = getNextArgument(i, tokens, programState, argumentA, argumentB, hadError);
        }

        if(hadError)
            return false;

        if(!checkForCalculation(tokens, i, programState, argumentA, argumentB, operation, std::ref(leftParameter), std::ref(rightParameter)))
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
    ProgramState& programState,
    Value& argumentA,
    Value& argumentB,
    Token*& operation,
    std::unique_ptr<Value>& leftParameter,
    std::unique_ptr<Value>& rightParameter)
{
    if(operation == nullptr)
        return true;
    
    bool hasLeft, hasRight, hadError;
    hadError = !getOperatorOrFunctionParamerters(*operation, hasLeft, hasRight, programState); 
    if(hadError){
        report(tokens, position, RuntimeErrorTypeNotAnOperation);

        return false;
    }
    if(hasRight && rightParameter->size() > 0){
        leftParameter = executeOperationOrFunction(*leftParameter, *rightParameter, *operation, argumentA, argumentB, programState, hadError);
        if(hadError){
            report(tokens, position, RuntimeErrorTypeOperatorError);

            return false;
        }
        rightParameter = std::make_unique<Value>();
                
    }else if(operation->id == TokenIdApplyToEach){
        leftParameter = executeModifier(*leftParameter, tokens, programState, argumentA, argumentB, position, hadError);
        position++;
    }else if(hasLeft && leftParameter->size() > 0 && (!hasRight)){
        leftParameter = executeOperationOrFunction(*leftParameter, *rightParameter, *operation, argumentA, argumentB, programState, hadError);
        if(hadError){
            report(tokens, position, RuntimeErrorTypeOperatorError);

            return false;
        }
    }else{
        return true;
    }

    operation = nullptr;
    return true;
}

    
std::unique_ptr<Value> Interpreter::getNextArgument(
    int& position,
    std::vector<Token*> &tokens,
    ProgramState& programState,
    Value& argumentA,
    Value& argumentB,
    bool& hadError)
{
    std::unique_ptr<Value> result;

    switch(tokens[position]->id)
    {
    case TokenIdVariable:
        result = std::make_unique<Value>();
        getVariable(*result, tokens[position]->str, programState);
        return std::move(result);
    break;
    case TokenIdLiteral:
        result = std::make_unique<Value>(tokens[position]->val);
        return std::move(result);
    break;
    case TokenIdLeftParam:
        result = std::make_unique<Value>(argumentA);
        return std::move(result);
    break;
    case TokenIdRightParam:
        result = std::make_unique<Value>(argumentB);
        return std::move(result);
    break;
    case TokenIdFunction:
    {
        bool hasLeft, hasRight;
        getOperatorOrFunctionParamerters(*(tokens[position]), hasLeft, hasRight, programState);
        if((!hasLeft) && (!hasRight)){
            result = std::make_unique<Value>();
            hadError = !execute(programState.functions[tokens[position]->str].body, programState, argumentA, argumentB, *result);
            return std::move(result);
        }else{
            hadError = true;
            report(tokens, position, RuntimeErrorTypeOperationAsParameter);
            return std::make_unique<Value>();
        }
    }
    break;
    case TokenIdOpenParenthesis:
    {
        int endPos = TokenSubArrayFinder::findClosingParenthesis(tokens, position);
        if(endPos == TokenSubArrayFinder::TOKEN_INDEX_NOT_FOUND){
            hadError = true;
            report(tokens, position, RuntimeErrorTypeNoClosingParenthesis);
            return std::make_unique<Value>();
        }
        std::vector<Token*> sub;
        TokenSubArrayFinder::findSubArray(tokens, sub, position+1, endPos-1);
        result = std::make_unique<Value>();
        hadError = !execute(sub, programState, argumentA, argumentB, *result);
        position = endPos;
        return std::move(result);
    }
    break;
    case TokenIdRead:
        return readNumbers(programState);
    break;
    case TokenIdReadText:
        return readText(programState);
    break;
    default:
    break;
    }

    hadError = true;
    report(tokens, position, RuntimeErrorTypeEvaluationError);

    return std::move(result);
}
    
bool Interpreter::getOperatorOrFunctionParamerters(Token& operation,  bool& hasLeftParam, bool& hasRightParam, ProgramState& programState)
{
    if(operation.id == TokenIdFunction){
        auto func = &programState.functions[operation.str];
        hasLeftParam = func->hasLeft;
        hasRightParam = func->hasRight;
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
        hasLeftParam = true;
        hasRightParam = false;
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
        hasLeftParam = true;
        hasRightParam = true;
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
    Value& argumentA,
    Value& argumentB,
    ProgramState& programState,
    bool& hadError)
{
    switch (operation.id)
    {
    case TokenIdFunction:
        {
            auto result = std::make_unique<Value>();
            hadError = !execute(programState.functions[operation.str].body, programState, leftOfOperator, rightOfOperator, *result);
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
    default:
        hadError = true;
        report(RuntimeErrorTypeNotAnOperation);

        return std::make_unique<Value>();
    }
}

bool Interpreter::isFunctionWithoutParameters(Token& function, ProgramState& programState)
{
    bool hasLeft, hasRight;
    if(function.id != TokenIdFunction || !getOperatorOrFunctionParamerters(function, hasLeft, hasRight, programState))
        return false;

    return (!hasLeft) && (!hasRight);
}

inline void Interpreter::setVariable(const Value& value, const std::string& variableName, ProgramState& programState)
{
    programState.variables[variableName].mut.lock();
    programState.variables[variableName].value = value;
    programState.variables[variableName].mut.unlock();
}
    
inline void Interpreter::getVariable(Value& value, const std::string& variableName, ProgramState& programState)
{
    programState.variables[variableName].mut.lock();
    value = programState.variables[variableName].value;
    programState.variables[variableName].mut.unlock();
}


std::unique_ptr<Value> Interpreter::executeModifier(
    Value& leftParameter,
    std::vector<Token*> &tokens,
    ProgramState& programState,
    Value& argumentA,
    Value& argumentB,
    int position,
    bool& hadError)
{
    auto result = std::make_unique<Value>();
    if(position == 0){
        hadError = true;
        report(tokens, position, RuntimeErrorTypeNoOperatorToModify);
        return std::move(result);
    }

    const int size = leftParameter.size();
    if(size == 0){
        hadError = true;
        report(tokens, position, RuntimeErrorTypeNoOperatorToModify);
        return std::move(result);
    }

    if(position+1 >= tokens.size()){
        hadError = true;
        report(tokens, position, RuntimeErrorTypeNoOperatorToModify);
        return std::move(result);
    }

    Token* operation = tokens[position+1];
    for(int i=0; i<size; i++){
        Value first = {leftParameter[i]};
        Value second = {(i+1>=size? 0.0:leftParameter[i+1])};

        auto operationResult = executeOperationOrFunction(first, second, *operation, argumentA, argumentB, programState, hadError);
        if(hadError){
            report(tokens, position, RuntimeErrorTypeOperatorError);
            return result;
        }
        for(const auto& j:*operationResult)
            result->push_back(j);
    }

    if(result->size() == 0){
        hadError = true;
        report(tokens, position, RuntimeErrorTypeNoOperatorToModify);
    }
    return std::move(result);
}

bool Interpreter::executeAsync(
    std::vector<Token*> &tokens,
    int& position,
    ProgramState& programState,
    Value& argumentA,
    Value& argumentB)
{
    std::vector<Token*> toExecute;
    int endPosition = TokenSubArrayFinder::findFirstTokenId(tokens, position, TokenIdAsyncEnd);
    if(endPosition == TokenSubArrayFinder::TOKEN_INDEX_NOT_FOUND)
        return false;
    TokenSubArrayFinder::findSubArray(tokens, toExecute, position+1, endPosition-1);
    programState.threads.push_back(std::thread(&Interpreter::executeOnThread, this, toExecute, std::ref(programState), argumentA, argumentB));
    position = endPosition;

    return true;
}

void Interpreter::joinThreads(ProgramState& programState)
{
    for(auto& i: programState.threads)
        i.join();

    programState.threads.clear();
}

int Interpreter::currentLineNumber(std::vector<Token*> &tokens, int position)
{
    const int size = tokens.size();
    for(int i=position; i<size; i++){
        if(tokens[i]->id == TokenIdEndLine){
            return tokens[i]->val[0];
        }
    }

    for(int i=position; i>=0; i--){
        if(tokens[i]->id == TokenIdEndLine){
            return tokens[i]->val[0]+1;
        }
    }

    return IRuntimeErrorReporter::LINE_NOT_FOUND;
}

inline void Interpreter::report(RuntimeErrorType errorType)
{
    if(errorReporter)
        errorReporter->report(errorType);
}

inline void Interpreter::report(std::vector<Token*> &tokens, int position, RuntimeErrorType errorType)
{
    if(errorReporter)
        errorReporter->report(currentLineNumber(tokens, position), errorType); 
}

inline void Interpreter::endStatement(std::unique_ptr<Value>& lastResult, std::unique_ptr<Value>& leftParameter, std::unique_ptr<Value>& rightParameter)
{
    if(leftParameter->size() != 0){
        lastResult = std::move(leftParameter);
        leftParameter = std::make_unique<Value>();
        rightParameter = std::make_unique<Value>();
    }
}

inline std::unique_ptr<Value> Interpreter::readNumbers(ProgramState& programState)
{
    programState.IOReadLock.lock();
    auto result = interpreterIO->read();
    programState.IOReadLock.unlock();
    return std::move(result);
}

inline std::unique_ptr<Value> Interpreter::readText(ProgramState& programState)
{
    programState.IOReadLock.lock();
    auto result = interpreterIO->readText();
    programState.IOReadLock.unlock();
    return std::move(result);
}
    
inline void Interpreter::writeNumbers(ProgramState& programState, Value& value)
{
    programState.IOWriteLock.lock();
    interpreterIO->write(value);
    programState.IOWriteLock.unlock();
}
    
inline void Interpreter::writeText(ProgramState& programState, Value& value)
{
    programState.IOWriteLock.lock();
    interpreterIO->writeText(value);
    programState.IOWriteLock.unlock();
}
