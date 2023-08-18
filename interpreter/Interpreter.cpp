#include <stack>
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

bool Interpreter::execute(std::vector<Token*> &tokens, std::map<std::string, Function>& functions, std::vector<double>& result)
{
    std::map<std::string, std::vector<double>> localVariables;
    std::vector<double> empty;
    return execute(tokens, functions, localVariables,  empty, empty, result);
}


bool Interpreter::execute(
    std::vector<Token*> &tokens,
    std::map<std::string, Function>& functions,
    std::map<std::string, std::vector<double>>& localVariables,
    std::vector<double>& left,
    std::vector<double>& right,
    std::vector<double>& result)
{
    const int size = tokens.size();
    bool hadError = false;
    std::unique_ptr<std::vector<double>> leftParameter = std::make_unique<std::vector<double>>();
    std::unique_ptr<std::vector<double>> rightParameter = std::make_unique<std::vector<double>>();
    std::unique_ptr<std::vector<double>> lastResult = std::make_unique<std::vector<double>>();
    Token* operation = nullptr;
    std::stack<LoopReturn> loopStack;

    for(int i=0; i<size; i++){
        if(loopStack.size() != 0 && i == loopStack.top().loopEnd){
            std::vector<double> conditionResult;
            if(!execute(loopStack.top().condition, functions, localVariables, left, right, conditionResult))
                return false;

            if(conditionResult[0] != 0.0){
                i = loopStack.top().loopStart;
                continue;
            }else{
                loopStack.pop();
                continue;
            }
        }

        if(tokens[i]->id == TokenIdEndLine || tokens[i]->id == TokenIdOpenCurly || tokens[i]->id == TokenIdCloseCurly){
            if(leftParameter->size() != 0){
                lastResult = std::move(leftParameter);
                leftParameter = std::make_unique<std::vector<double>>();
                rightParameter = std::make_unique<std::vector<double>>();
            }
            continue;
        }

        if(tokens[i]->id == TokenIdIf){
            std::vector<Token*> condition;
            std::vector<double> conditionResult;
            int endCondition = TokenSubArrayFinder::findFirstTokenIdInLine(tokens, i, TokenIdOpenCurly);
            if(endCondition == TOKEN_INDEX_NOT_FOUND){
                if(errorReporter)
                    errorReporter->report(RuntimeErrorTypeMissingIfCondition);
                return false;
            }
            TokenSubArrayFinder::findSubArray(tokens, condition, i+1, endCondition-1);
            if(!execute(condition, functions, localVariables, left, right, conditionResult))
                return false;
            
            if(conditionResult[0] != 0.0){
                i = endCondition;
                continue;
            }else{
                int afterIfBody = TokenSubArrayFinder::findClosingParenthesis(tokens, endCondition);
                if(afterIfBody == TOKEN_INDEX_NOT_FOUND){
                    if(errorReporter)
                        errorReporter->report(RuntimeErrorTypeInvalidIfSyntax);
                    return false;
                }
                i = afterIfBody;
                continue;
            }
        }

        if(tokens[i]->id == TokenIdLoop){
            LoopReturn loopData;
            std::vector<double> conditionResult;
            loopData.loopStart = TokenSubArrayFinder::findFirstTokenIdInLine(tokens, i, TokenIdOpenCurly);
            if(loopData.loopStart == TOKEN_INDEX_NOT_FOUND){
                if(errorReporter)
                    errorReporter->report(RuntimeErrorTypeMissingLoopCondition);
                return false;
            }
            TokenSubArrayFinder::findSubArray(tokens, loopData.condition, i+1, loopData.loopStart-1);
            loopData.loopEnd = TokenSubArrayFinder::findClosingCurly(tokens, loopData.loopStart);
            if(!execute(loopData.condition, functions, localVariables, left, right, conditionResult))
                return false;

            if(conditionResult[0] != 0){
                loopStack.push(loopData);
                i = loopData.loopStart;
                continue;
            }else{
                i = loopData.loopEnd;
                continue;
            }
        }

        if(tokens[i]->id == TokenIdEquals && i-1>=0 && tokens[i-1]->id == TokenIdVariable){
            const int statementEnd = TokenSubArrayFinder::findStatementEnd(tokens, i);
            std::vector<Token*> statement;
            TokenSubArrayFinder::findSubArray(tokens, statement, i+1, statementEnd);

            hadError = !execute(
                statement,
                functions,
                localVariables,
                left,
                right,
                *leftParameter);
            if(hadError)
                return false;
            
            /// TODO: Could be optimised
            lastResult = std::move(leftParameter);
            leftParameter = std::make_unique<std::vector<double>>();
            localVariables[tokens[i - 1]->str] = *lastResult;
            i = statementEnd;
            continue;
        }

        if(tokens[i]->id == TokenIdWrite){
            const int statementEnd = TokenSubArrayFinder::findStatementEnd(tokens, i);
            std::vector<double> output;
            std::vector<Token*> statement;
            TokenSubArrayFinder::findSubArray(tokens, statement, i+1, statementEnd);
            hadError = !execute(
                statement,
                functions,
                localVariables,
                left,
                right,
                output);
            if(hadError)
                return false;
            
            if(output.size() >= 1)
                interpreterIO->write(output);
            i = statementEnd;
            continue;
        }

        if(leftParameter->size() == 0 && tokens[i]->id != TokenIdFunction && operation == nullptr){
            leftParameter = getNextArgument(i, tokens, functions, localVariables, left, right, hadError);
        }else if(operation == nullptr){
            operation = tokens[i];
        }else{
            rightParameter = getNextArgument(i, tokens, functions, localVariables, left, right, hadError);
        }

        if(hadError)
            return false;


        //check for calculation
        if(operation != nullptr){
            bool leftArg, rightArg;
            hadError = !getOperatorOrFunctionParamerters(*operation, leftArg, rightArg, functions); 
            if(hadError){
                if(errorReporter)
                    errorReporter->report(RuntimeErrorTypeNotAnOperation);
                return false;
            }
            if(rightArg && rightParameter->size() > 0){
                leftParameter = executeOperationOrFunction(*leftParameter, *rightParameter, *operation, left, right, functions, localVariables, hadError);
                rightParameter = std::make_unique<std::vector<double>>();
                operation = nullptr;
            }else if(leftArg && leftParameter->size() > 0 && (!rightArg)){
                leftParameter = executeOperationOrFunction(*leftParameter, *rightParameter, *operation, left, right, functions, localVariables, hadError);
                operation = nullptr;
            }else{
                continue;
            }
        }

        if(hadError)
            return false;
    }


    if(leftParameter->size() != 0)
        lastResult = std::move(leftParameter);
    result = *lastResult;
    return true;
}

    
std::unique_ptr<std::vector<double>> Interpreter::getNextArgument(
    int& position,
    std::vector<Token*> &tokens,
    std::map<std::string, Function>& functions,
    std::map<std::string, std::vector<double>>& localVariables,
    std::vector<double>& left,
    std::vector<double>& right,
    bool& hadError)
{
    std::unique_ptr<std::vector<double>> result;

    switch(tokens[position]->id)
    {
    case TokenIdVariable:
        result = std::make_unique<std::vector<double>>(localVariables[tokens[position]->str]);
        return std::move(result);
    break;
    case TokenIdLiteral:
        result = std::make_unique<std::vector<double>>(tokens[position]->val);
        return std::move(result);
    break;
    case TokenIdLeftParam:
        result = std::make_unique<std::vector<double>>(left);
        return std::move(result);
    break;
    case TokenIdRightParam:
        result = std::make_unique<std::vector<double>>(right);
        return std::move(result);
    break;
    case TokenIdFunction:{
        bool ArgLeft, ArgRight;
        getOperatorOrFunctionParamerters(*(tokens[position]), ArgLeft, ArgRight, functions);
        if((!ArgLeft) && (!ArgRight)){
            result = std::make_unique<std::vector<double>>();
            hadError = !execute(functions[tokens[position]->str].body, functions, localVariables, left, right, *result);
            return result;
        }else{
            hadError = true;
            if(errorReporter)
                errorReporter->report(RuntimeErrorTypeOperationAsParameter);
            return std::make_unique<std::vector<double>>();
        }
    }
    break;
    case TokenIdOpenParenthesis:{
        int endPos = TokenSubArrayFinder::findClosingParenthesis(tokens, position);
        if(endPos == TOKEN_INDEX_NOT_FOUND){
            hadError = true;
            if(errorReporter)
                errorReporter->report(RuntimeErrorTypeNoClosingParenthesis);
            return std::make_unique<std::vector<double>>();
        }
        std::vector<Token*> sub;
        TokenSubArrayFinder::findSubArray(tokens, sub, position+1, endPos-1);
        result = std::make_unique<std::vector<double>>();
        hadError = !execute(sub, functions, localVariables, left, right, *result);
        position = endPos;
        return std::move(result);
    }
    break;
    case TokenIdRead:
        return interpreterIO->read();
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
    
bool Interpreter::getOperatorOrFunctionParamerters(Token& operation,  bool& leftParam, bool& rightParam, std::map<std::string, Function>& functions)
{
    if(operation.id == TokenIdFunction){
        auto func = &functions[operation.str];
        leftParam = func->left;
        rightParam = func->right;
        return true;
    }

    switch (operation.id)
    {
    case TokenIdIterate:
        leftParam = true;
        rightParam = false;
        return true;
    break;
    case TokenIdAdd:
    case TokenIdSubtract:
    case TokenIdMultiply:
    case TokenIdDivide:
        leftParam = true;
        rightParam = true;
        return true;
    break;
    default:break;
    }

    return false;
}



std::unique_ptr<std::vector<double>> Interpreter::executeOperationOrFunction(
    std::vector<double>& leftOfOperator,
        std::vector<double>& rightOfOperator,
        Token& operation,
        std::vector<double>& left,
        std::vector<double>& right,
        std::map<std::string, Function>& functions,
        std::map<std::string, std::vector<double>>& localVariables,
        bool& hadError)
{
    switch (operation.id)
    {
    case TokenIdFunction:
        {
            auto result = std::make_unique<std::vector<double>>();
            hadError = !execute(functions[operation.str].body, functions, localVariables, leftOfOperator, rightOfOperator, *result);
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
    case TokenIdIterate:
        return InterpreterCalculator::iterate(leftOfOperator, hadError, errorReporter);
    default:break;
    }

    hadError = true;
    if(errorReporter)
        errorReporter->report(RuntimeErrorTypeNotAnOperation);

    return std::make_unique<std::vector<double>>();
}
