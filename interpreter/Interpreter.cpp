#include "Interpreter.h"
#include "../util/TokenSubArrayFinder.h"

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

        if(leftParameter->size() == 0){
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
            getOperatorOrFunctionParamerters(*operation, leftArg, rightArg, functions);
            if(rightArg && rightParameter->size() > 0){
                leftParameter = executeOperationOrFunction(*leftParameter, *rightParameter, *operation, left, right, functions, localVariables, hadError, errorReporter);
            }else if(leftArg && leftParameter->size() > 0){
                leftParameter = executeOperationOrFunction(*leftParameter, *rightParameter, *operation, left, right, functions, localVariables, hadError, errorReporter);
            }else{
                if(errorReporter)
                    errorReporter->report(RuntimeErrorTypeParameterError);
                return false;
            }
        }

        if(hadError)
            return false;

    }



}

    
std::unique_ptr<std::vector<double>> Interpreter::getNextArgument(
    int position,
    std::vector<Token*> &tokens,
    std::map<std::string, Function>& functions,
    std::map<std::string, std::vector<double>>& localVariables,
    std::vector<double>& left,
    std::vector<double>& right,
    bool& hadError,
    IRuntimeErrorReporter* errorReporter)
{
    std::unique_ptr<std::vector<double>> result;

    if(tokens[position]->id == TokenIdLiteral){
        result = std::make_unique<std::vector<double>>(tokens[position]->val);
        return std::move(result);
    }
    else if(tokens[position]->id == TokenIdFunction){
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

    /// TODO: Add checks for the rest of the tokens

    return false;
}



std::unique_ptr<std::vector<double>> Interpreter::executeOperationOrFunction(
    Token& operation,
    std::vector<double>& left,
    std::vector<double>& right,
    std::map<std::string, Function>& functions,
    std::map<std::string, std::vector<double>>& localVariables,
    bool& hadError,
    IRuntimeErrorReporter* errorReporter)
{
    /// TODO: Add a switch for the operations
}
