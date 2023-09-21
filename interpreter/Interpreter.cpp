#include "Interpreter.h"
#include "../util/TokenSubArrayFinder.h"
#include "InterpreterCalculator.h"
#include "../token/OperatorArguments.h"
#include <cassert>

struct Loop{
    std::vector<const Token*> condition;
    int loopStart;
    int loopEnd;
};

Interpreter::Interpreter(IRuntimeErrorReporter* errorReporter, IInterpreterIO* interpreterIO)
{
    assert(interpreterIO != nullptr);
    this->errorReporter = errorReporter;
    this->interpreterIO = interpreterIO;
}

bool Interpreter::execute(const std::vector<const Token*> &tokens, const std::unordered_map<std::string, Function>& functions, Value& result)
{
    ProgramState programState = {functions,{}, {}};
    Value empty;
    bool successfulExecution = execute(tokens, programState,  empty, empty, result);
    joinThreads(programState);

    return successfulExecution;
}

 bool Interpreter::execute(const std::vector<const Token*> &tokens, ProgramState& programState, Value& result)
 {
    Value empty;
    bool successfulExecution = execute(tokens, programState,  empty, empty, result);
    joinThreads(programState);

    return successfulExecution;
}

void Interpreter::executeOnThread(
    std::vector<const Token*> tokens,
    ProgramState& programState,
    Value argumentA,
    Value argumentB)
{
    Value ignore;
    if(!execute(tokens, programState, argumentA, argumentB, ignore)){
        report(RuntimeErrorTypeThreadHadError);
    }
}

bool Interpreter::execute(
    const std::vector<const Token*> &tokens,
    ProgramState& programState,
    const Value& argumentA,
    const Value& argumentB,
    Value& result)
{
    const int size = tokens.size();
    bool hadError = false;
    std::unique_ptr<Value> leftParameter = std::make_unique<Value>();
    std::unique_ptr<Value> rightParameter = std::make_unique<Value>();
    std::unique_ptr<Value> lastResult = std::make_unique<Value>();
    const Token* operation = nullptr;
    std::stack<Loop> loopStack;

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
                std::vector<const Token*> condition;
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
                Loop loopData;
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
                std::vector<const Token*> statement;
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
                std::vector<const Token*> statement;
                TokenSubArrayFinder::findSubArray(tokens, statement, i+1, statementEnd);

                hadError = !execute(
                    statement,
                    programState,
                    argumentA,
                    argumentB,
                    *leftParameter);
                if(hadError)
                    return false;
            
                if(leftParameter->size() == 0){
                    report(tokens, i, RuntimeErrorTypeEmptyData);
                    return false;
                }
                lastResult = std::move(leftParameter);
                leftParameter = std::make_unique<Value>();
                setVariable(*lastResult, tokens[i - 1]->str, programState);
                i = statementEnd;
                continue;
            }else{
                report(tokens, i, RuntimeErrorTypeOperatorError);
                return false;
            }
            break;
            default:break;
        }

        if(!getArgumentsAndOperation(i, tokens, programState, leftParameter, rightParameter, argumentA, argumentB, operation))
            return false;

        if(!checkForCalculation(tokens, i, programState, argumentA, argumentB, operation, leftParameter, rightParameter))
            return false;
    }

    if(operation != nullptr){
        report(tokens, tokens.size()-1, RuntimeErrorTypeMissingParameter);
        return false;
    }
    if(leftParameter->size() != 0)
        lastResult = std::move(leftParameter);
    result = *lastResult;
    return true;
}

bool Interpreter::checkForCalculation(
    const std::vector<const Token*> &tokens,
    int& position,
    ProgramState& programState,
    const Value& argumentA,
    const Value& argumentB,
    const Token*& operation,
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

inline bool Interpreter::getArgumentsAndOperation(
    int& position,
    const std::vector<const Token*> &tokens,
    ProgramState& programState,
    std::unique_ptr<Value>& leftParameter,
    std::unique_ptr<Value>& rightParameter,
    const Value& argumentA,
    const Value& argumentB,
    const Token*& operation)
{
    bool hadError = false;
    if((leftParameter->size() == 0 && tokens[position]->id != TokenIdFunction && operation == nullptr) ||
        isFunctionWithoutParameters(*tokens[position], programState))
    {
        leftParameter = getNextArgument(position, tokens, programState, argumentA, argumentB, hadError);
    }else if(operation == nullptr){
        operation = tokens[position];
    }else{
        rightParameter = getNextArgument(position, tokens, programState, argumentA, argumentB, hadError);
    }

    return !hadError;
}
    
std::unique_ptr<Value> Interpreter::getNextArgument(
    int& position,
    const std::vector<const Token*> &tokens,
    ProgramState& programState,
    const Value& argumentA,
    const Value& argumentB,
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
        if(argumentA.size() == 0)
            result->push_back(0.0);
        
        return std::move(result);
    break;
    case TokenIdRightParam:
        result = std::make_unique<Value>(argumentB);
        if(argumentB.size() == 0)
            result->push_back(0.0);
    
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
        std::vector<const Token*> insideParenthesis;
        TokenSubArrayFinder::findSubArray(tokens, insideParenthesis, position+1, endPos-1);
        result = std::make_unique<Value>();
        hadError = !execute(insideParenthesis, programState, argumentA, argumentB, *result);
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
    
bool Interpreter::getOperatorOrFunctionParamerters(const Token& operation,  bool& hasLeftParam, bool& hasRightParam, ProgramState& programState)
{
    if(operation.id == TokenIdFunction){
        const auto function = &programState.functions[operation.str];
        hasLeftParam = function->hasLeft;
        hasRightParam = function->hasRight;
        return true;
    }
    else 
        return OperatorArguments::getArgs(operation.id, hasLeftParam, hasRightParam);
}

std::unique_ptr<Value> Interpreter::executeOperationOrFunction(
    const Value& leftOfOperator,
    const Value& rightOfOperator,
    const Token& operation,
    const Value& argumentA,
    const Value& argumentB,
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
    case TokenIdSine:
        return InterpreterCalculator::sine(leftOfOperator, hadError, errorReporter);
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
    case TokenIdRemove:
        return InterpreterCalculator::remove(leftOfOperator, rightOfOperator, hadError, errorReporter);
    case TokenIdRemain:
        return InterpreterCalculator::remain(leftOfOperator, rightOfOperator, hadError, errorReporter);
    default:
        hadError = true;
        report(RuntimeErrorTypeNotAnOperation);

        return std::make_unique<Value>();
    }
}

bool Interpreter::isFunctionWithoutParameters(const Token& function, ProgramState& programState)
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
    const auto variable = programState.variables.find(variableName);
    if(variable == programState.variables.end()){
        value = {0.0};
        return;
    }

    variable->second.mut.lock();
    value = variable->second.value;
    variable->second.mut.unlock();
}


std::unique_ptr<Value> Interpreter::executeModifier(
    Value& leftParameter,
    const std::vector<const Token*> &tokens,
    ProgramState& programState,
    const Value& argumentA,
    const Value& argumentB,
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

    const Token* operation = tokens[position+1];
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
        report(RuntimeErrorTypeNoOperatorToModify);
    }
    return std::move(result);
}

bool Interpreter::executeAsync(
    const std::vector<const Token*> &tokens,
    int& position,
    ProgramState& programState,
    const Value& argumentA,
    const Value& argumentB)
{
    std::vector<const Token*> toExecute;
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

void Interpreter::report(RuntimeErrorType errorType)
{
    if(errorReporter)
        errorReporter->report(errorType);
}

void Interpreter::report(const std::vector<const Token*> &tokens, int position, RuntimeErrorType errorType)
{
    if(errorReporter != nullptr && tokens.size() > 0)
        errorReporter->report(tokens, position, errorType);
}

