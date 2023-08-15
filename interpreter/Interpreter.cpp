#include "Interpreter.h"
#include "../util/TokenSubArrayFinder.h"
#include "InterpreterCalculator.h"

bool Interpreter::execute(std::vector<Token*> &tokens, std::map<std::string, Function>& functions, std::vector<double>& result, IRuntimeErrorReporter* errorReporter)
{
    std::map<std::string, std::vector<double>> localVariables;
    std::vector<double> empty;
    return execute(tokens, functions, localVariables,  empty, empty, result, errorReporter);
}


bool Interpreter::execute(
    std::vector<Token*> &tokens,
    std::map<std::string, Function>& functions,
    std::map<std::string, std::vector<double>>& localVariables,
    std::vector<double>& left,
    std::vector<double>& right,
    std::vector<double>& result,
    IRuntimeErrorReporter* errorReporter)
{
    const int size = tokens.size();
    bool hadError = false;
    std::unique_ptr<std::vector<double>> leftParameter = std::make_unique<std::vector<double>>();
    std::unique_ptr<std::vector<double>> rightParameter = std::make_unique<std::vector<double>>();
    std::unique_ptr<std::vector<double>> lastResult = std::make_unique<std::vector<double>>();
    Token* operation = nullptr;

    for(int i=0; i<size; i++){
        if(tokens[i]->id == TokenIdEndLine || tokens[i]->id == TokenIdOpenCurly || tokens[i]->id == TokenIdCloseCurly){
            if(leftParameter->size() != 0){
                lastResult = std::move(leftParameter);
                leftParameter = std::make_unique<std::vector<double>>();
                rightParameter = std::make_unique<std::vector<double>>();
            }
            continue;
        }

        if(tokens[i]->id == TokenIdEquals && i-1>=0 && tokens[i-1]->id == TokenIdVariable){
            const int statementEnd = TokenSubArrayFinder::findStatementEnd(tokens, i);
            std::vector<Token*> statement;
            TokenSubArrayFinder::findSubArray(tokens, statement, i+1, statementEnd);

            localVariables[tokens[i-1]->str].clear();
            hadError = !execute(
                statement,
                functions,
                localVariables,
                left,
                right,
                localVariables[tokens[i-1]->str],
                errorReporter);
            if(hadError)
                return false;
            
            i = statementEnd;
            continue;
        }

        if(leftParameter->size() == 0 && tokens[i]->id != TokenIdFunction && operation == nullptr){
            leftParameter = getNextArgument(i, tokens, functions, localVariables, left, right, hadError, errorReporter);
        }else if(operation == nullptr){
            operation = tokens[i];
        }else{
            rightParameter = getNextArgument(i, tokens, functions, localVariables, left, right, hadError, errorReporter);
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
                leftParameter = executeOperationOrFunction(*leftParameter, *rightParameter, *operation, left, right, functions, localVariables, hadError, errorReporter);
                std::move(rightParameter);//delete right parameter
            }else if(leftArg && leftParameter->size() > 0 && (!rightArg)){
                leftParameter = executeOperationOrFunction(*leftParameter, *rightParameter, *operation, left, right, functions, localVariables, hadError, errorReporter);
            }else{
                continue;
                //if(errorReporter)
                //    errorReporter->report(RuntimeErrorTypeParameterError);
                //return false;
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
    bool& hadError,
    IRuntimeErrorReporter* errorReporter)
{
    std::unique_ptr<std::vector<double>> result;

    if(tokens[position]->id == TokenIdVariable){
        result = std::make_unique<std::vector<double>>(localVariables[tokens[position]->str]);
        return std::move(result);
    }else if(tokens[position]->id == TokenIdLiteral){
        result = std::make_unique<std::vector<double>>(tokens[position]->val);
        return std::move(result);
    }else if(tokens[position]->id == TokenIdLeftParam){
        result = std::make_unique<std::vector<double>>(left);
        return std::move(result);
    }else if(tokens[position]->id == TokenIdRightParam){
        result = std::make_unique<std::vector<double>>(right);
        return std::move(result);
    }else if(tokens[position]->id == TokenIdFunction){
        bool ArgLeft, ArgRight;
        getOperatorOrFunctionParamerters(*(tokens[position]), ArgLeft, ArgRight, functions);
        if((!ArgLeft) && (!ArgRight)){
            result = std::make_unique<std::vector<double>>();
            hadError = !execute(functions[tokens[position]->str].body, functions, localVariables, left, right, *result, errorReporter);
            return result;
        }else{
            hadError = true;
            if(errorReporter)
                errorReporter->report(RuntimeErrorTypeOperationAsParameter);
            return std::make_unique<std::vector<double>>();
        }
    }else if(tokens[position]->id == TokenIdOpenParenthesis){
        int endPos = TokenSubArrayFinder::findClosingParenthesis(tokens, position);
        if(endPos == TOKEN_INDEX_NOT_FOUND){
            hadError = true;
            if(errorReporter)
                errorReporter->report(RuntimeErrorTypeNoClosingParenthesis);
            return std::make_unique<std::vector<double>>();
        }
        std::vector<Token*> sub;
        TokenSubArrayFinder::findSubArray(tokens, sub, position+1, endPos+1);
        result = std::make_unique<std::vector<double>>();
        hadError = !execute(sub, functions, localVariables, left, right, *result, errorReporter);
        position = endPos; // move position to the end
        return std::move(result);
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
        bool& hadError,
        IRuntimeErrorReporter* errorReporter)
{
    switch (operation.id)
    {
    case TokenIdFunction:
        {
            auto result = std::make_unique<std::vector<double>>();
            hadError = !execute(functions[operation.str].body, functions, localVariables, leftOfOperator, rightOfOperator, *result, errorReporter);
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
    default:break;
    }

    hadError = true;
    if(errorReporter)
        errorReporter->report(RuntimeErrorTypeNotAnOperation);

    return std::make_unique<std::vector<double>>();
}
