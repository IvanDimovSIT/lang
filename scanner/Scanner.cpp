#include "Scanner.h"
#include "../util/StringUtil.h"
#include "../util/LiteralParser.h"
#include "../util/TokenSubArrayFinder.h"
#include "../token/TokenSyntax.h"

bool Scanner::scan(
    const std::string& source,
    std::vector<Token>& tokens,
    std::unordered_map<std::string, Function>& functions,
    IScannerErrorReporter* errorReporter)
{
    std::unordered_set<std::string> empty;
    return scan(source, tokens, functions, empty, errorReporter);
}

bool Scanner::scan(
    const std::string& source,
    std::vector<Token>& tokens,
    std::unordered_map<std::string, Function>& functions,
    std::unordered_set<std::string>& previousFunctions,
    IScannerErrorReporter* errorReporter)
{
    tokens.clear();
    functions.clear();

    const int sourceLen = source.size();
    std::unordered_set<std::string> functionNames;
    std::string curr = "";
    bool hadError = !validateParenthesis(source, sourceLen, errorReporter);
    if(!findFunctionNames(source, functionNames, errorReporter)){
        hadError = true;
    }
    
    for(int i=0; i<sourceLen; i++){
        if(curr == "\""){
            const int stringLiteralEnd = StringUtil::findStringLiteralEndIndex(source, i-1);
            if(stringLiteralEnd == StringUtil::STRING_END_NOT_FOUND){
                hadError = true;
                report(errorReporter, StringUtil::getLine(source, i), ScannerErrorTypeStringLiteralError);
            }
            curr += source.substr(i, stringLiteralEnd-i+1);
            if(!matchToken(curr, tokens, functionNames, previousFunctions)){
                hadError = true;
                report(errorReporter, StringUtil::getLine(source, i), ScannerErrorTypeStringLiteralError);
            }else{
                i = stringLiteralEnd;
            }

            curr = "";
            continue;
        }

        // special if token case
        if( curr == "" &&
            source[i] == 'i' &&
            (i+1<sourceLen) &&
            source[i+1] == 'f' &&
            matchToken("if", tokens, functionNames, previousFunctions)){
            i++;
        }else if(TokenSyntax::isSingleCharToken(source[i])){ // End current token and add next token
            addNextToken(curr, tokens, functionNames, previousFunctions, hadError, errorReporter);
            curr = "";
            if(!validateOperatorModifier(tokens)){
                report(errorReporter, StringUtil::getLine(source, i) ,ScannerErrorTypeNoOperatorToModify);
                hadError = true;
            }

            curr += source[i];
            addNextToken(curr, tokens, functionNames, previousFunctions, hadError, errorReporter);
            curr = "";
            if(!validateOperatorModifier(tokens)){
                report(errorReporter, StringUtil::getLine(source, i) ,ScannerErrorTypeNoOperatorToModify);
                hadError = true;
            }

        }else if(StringUtil::isSeparator(source[i])){ // End token scanning and add it
            addNextToken(curr, tokens, functionNames, previousFunctions, hadError, errorReporter);
            curr = "";
            if(!validateOperatorModifier(tokens)){
                report(errorReporter, StringUtil::getLine(source, i) ,ScannerErrorTypeNoOperatorToModify);
                hadError = true;
            }

        }else{ // Add character
            curr += source[i];
            earlyTokenMatch(i, source, curr, tokens, functionNames, previousFunctions, hadError, errorReporter);
        }
    }

    addNextToken(curr, tokens, functionNames, previousFunctions, hadError, errorReporter);
    if(!validateOperatorModifier(tokens)){
        report(errorReporter, StringUtil::getLine(source, sourceLen-1) ,ScannerErrorTypeNoOperatorToModify);
        hadError = true;
    }

    if(!extractFunctions(tokens, functions)){
        hadError = true;
        report(errorReporter, StringUtil::getLine(source, 0), ScannerErrorTypeFunctionDefinitionError);
    }

    return !hadError;
}

bool Scanner::findFunctionNames(const std::string& source, std::unordered_set<std::string>& functionNames, IScannerErrorReporter* errorReporter)
{
    functionNames.clear();
    const int stringLen = source.size();
    std::string curr = "";
    bool foundError = false;
    bool isScanningFunctionName = false;
    bool isAfterFirstFunctionChar = false;
    int lineCounter = 1;

    for(int i=0; i<stringLen; i++){
        if(source[i] == '\n')
            lineCounter++;

        if(source[i] == '"'){
            const int end = StringUtil::findStringLiteralEndIndex(source, i);
            if(end == StringUtil::STRING_END_NOT_FOUND){
                report(errorReporter, StringUtil::getLine(source, i), ScannerErrorTypeStringLiteralError);
                foundError = true;
            }else{
                i = end;
            }
            continue;
        }

        if((source[i] == 'f' && i ==0) || (source[i] == 'f' && (i-1>=0) && source[i-1] != 'i')){ // extra checks for "if"
            if(!isScanningFunctionName){
                isScanningFunctionName = true;
                isAfterFirstFunctionChar = false;
                continue;
            }else{
                foundError = true;
                report(errorReporter, StringUtil::getLine(source, i), ScannerErrorTypeFunctionDefinitionError);
            }
        }
        else if(isScanningFunctionName){
            if(StringUtil::isSeparator(source[i]) && curr.size() <= 0)
                continue;
            
            if(StringUtil::isSeparator(source[i]) || source[i] == '{' || source[i] == '\n' || source[i] == ';'){
                isScanningFunctionName = false;
                if(functionNames.count(curr) != 0){
                    foundError = true;
                    report(errorReporter, StringUtil::getLine(source, i), ScannerErrorTypeFunctionRedefinition);
                }
                if(!StringUtil::isValidIdentifierName(curr)){
                    foundError = true;
                    curr = "";
                    report(errorReporter, StringUtil::getLine(source, i), ScannerErrorTypeFunctionDefinitionError);
                }else{
                    functionNames.insert(curr);
                    curr = "";
                }
            }else if(std::isupper(source[i]) || (isAfterFirstFunctionChar && std::isdigit(source[i]))){
                curr += source[i];
                isAfterFirstFunctionChar = true;
            }else{
                foundError = true;
                report(errorReporter, StringUtil::getLine(source, i), ScannerErrorTypeFunctionNameDefinitionError);
            }
        }
    }

    if(isScanningFunctionName){
        foundError = true;
        report(errorReporter, StringUtil::getLine(source, stringLen-1), ScannerErrorTypeFunctionDefinitionError);
    }

    return !foundError;
}

void Scanner::determineFunctionPrameters(Function& function)
{
    function.hasLeft = false;
    function.hasRight = false;
    const int size = function.body.size();
    for(int i=0; i<size; i++){
        if(function.body[i]->id == TokenIdLeftParam)
            function.hasLeft = true;
        else if(function.body[i]->id == TokenIdRightParam)
            function.hasRight = true;
        
        if(function.hasLeft && function.hasRight)
            return;
    }
}

bool Scanner::extractFunctions(const std::vector<Token>& tokens, std::unordered_map<std::string, Function>& functions)
{
    const int size = tokens.size();
    Function function;
    std::string functionName = "";

    bool findOpen = false;

    for(int i=0; i<size; i++)
    {
        if(findOpen){
            if(tokens[i].id == TokenIdEndLine)
                continue;
            else if(tokens[i].id != TokenIdOpenCurly)
                return false;
            
            int upTo = TokenSubArrayFinder::findClosingCurly(tokens, i);
            if(upTo == TokenSubArrayFinder::TOKEN_INDEX_NOT_FOUND){
                return false;
            }
            function.body.clear();
            TokenSubArrayFinder::findSubArray(tokens, function.body, i+1, upTo-1);
            if(function.body.size() <= 0)
                return false;
            determineFunctionPrameters(function);
            functions[functionName] = function;
            functionName = "";
            findOpen = false;
            i = upTo;
            continue;
        }

        if(tokens[i].id == TokenIdFunctionDeclaration && (i+1<size) && tokens[i+1].id == TokenIdFunction){
            findOpen = true;
            functionName = tokens[i+1].str;
            i++;
            continue;
        }
    }
    
    return true;
}

bool Scanner::matchToken(
    const std::string& tokenString,
    std::vector<Token>& tokens,
    std::unordered_set<std::string>& functionNames,
    std::unordered_set<std::string>& previousFunctions)
{
    if(tokenString.size() <= 0 || StringUtil::isSeparator(tokenString[0]))
        return true;

    Token token;
    token.str = tokenString;
    
    if(TokenSyntax::isValidToken(tokenString)){
        token.id = TokenSyntax::getToken(tokenString);
        tokens.push_back(token);
        return true;
    }else if(LiteralParser::parse(tokenString, token.val) || LiteralParser::parseString(tokenString, token.val)){
        token.id = TokenIdLiteral;
        tokens.push_back(token);
        return true;
    }else if(functionNames.count(tokenString) != 0 || previousFunctions.count(tokenString) != 0){
        token.id = TokenIdFunction;
        tokens.push_back(token);
        return true;
    }else if(StringUtil::isValidIdentifierName(tokenString) && previousFunctions.count(tokenString) == 0){
        token.id = TokenIdVariable;
        tokens.push_back(token);
        return true;
    }

    return false;
}

void Scanner::addNextToken(
    const std::string& tokenString,
    std::vector<Token>& tokens,
    std::unordered_set<std::string>& functionNames,
    std::unordered_set<std::string>& previousFunctions,
    bool& hadError,
    IScannerErrorReporter* errorReporter)
{
    if (!matchToken(tokenString, tokens, functionNames, previousFunctions)) {
        hadError = true;
        report(errorReporter, tokenString, ScannerErrorTypeUnrecognisedToken);
    }
}

inline void Scanner::earlyTokenMatch(
    int& position,
    const std::string& source,
    std::string& currentToken,
    std::vector<Token>& tokens,
    std::unordered_set<std::string>& functionNames,
    std::unordered_set<std::string>& previousFunctions,
    bool& hadError,
    IScannerErrorReporter* errorReporter)
{
    const int sourceLen = source.size();
    if((position+1<sourceLen) &&
        TokenSyntax::isValidToken(currentToken+source[position+1]) &&
        matchToken(currentToken+source[position+1], tokens, functionNames, previousFunctions)){
        position++;
        currentToken = "";
    }else if(source[position] != '"' && isIdentifierNextToSymbol(source, position)){
        addNextToken(currentToken, tokens, functionNames, previousFunctions, hadError, errorReporter);
        currentToken = "";
    }
}

bool Scanner::isIdentifierNextToSymbol(const std::string& source, int position)
{
    const int size = source.size();
    return 
        (position+1<size) &&
        (
            (std::isupper(source[position]) && ((!std::isupper(source[position+1])) && (!std::isdigit(source[position+1])))) ||
            (std::isupper(source[position+1]) && ((!std::isupper(source[position])) && (!std::isdigit(source[position]))))
        );
}

bool Scanner::validateParenthesis(const std::string& source, const int sourceLen, IScannerErrorReporter* errorReporter)
{
    int openP = 0;
    int openCurly = 0;
    int lineCounter = 1;
    bool hadError = false;
    bool asyncStart = false;

    for(int i=0; i<sourceLen; i++){
        if(source[i] == '\n'){
            lineCounter++;
        }

        if(source[i] == '"'){
            const int end = StringUtil::findStringLiteralEndIndex(source, i);
            if(end == StringUtil::STRING_END_NOT_FOUND){
                report(errorReporter, StringUtil::getLine(source, i), ScannerErrorTypeStringLiteralError);
                hadError = true;
            }else{
                i = end;
            }
            continue;
        }
        
        if(source[i] == '('){
            openP++;
        }else if(source[i] == ')'){
            openP--;
        }else if(source[i] == '{'){
            openCurly++;
        }else if(source[i] == '}'){
            openCurly--;
        }else if(source[i] == '['){
            if(asyncStart){
                hadError = true;
                report(errorReporter, StringUtil::getLine(source, i), ScannerErrorTypeMissingAsyncEnd);
            }else{
                asyncStart = true;
            }
        }else if(source[i] == ']'){
            if(asyncStart){
                asyncStart = false;
            }else{
                hadError = true;
                report(errorReporter, StringUtil::getLine(source, i), ScannerErrorTypeMissingAsyncStart);
            }
        }

        if(openP < 0){
            hadError = true;
            report(errorReporter, StringUtil::getLine(source, i), ScannerErrorTypeMoreClosingParenthesis);
        }
        if(openCurly < 0){
            hadError = true;
            report(errorReporter, StringUtil::getLine(source, i), ScannerErrorTypeMoreClosingCurlyBrackets);
        }

    }

    if(openP > 0){
        hadError = true;
        if(errorReporter)
            errorReporter->report(StringUtil::getLine(source, sourceLen-1), ScannerErrorTypeMoreOpenParenthesis);
    }

    if(openCurly > 0){
        hadError = true;
        report(errorReporter, StringUtil::getLine(source, sourceLen-1), ScannerErrorTypeMoreOpenCurlyBrackets);
    }
    
    return !hadError;
}

bool Scanner::validateOperatorModifier(const std::vector<Token>& tokens)
{
    const int size = tokens.size();
    if(size < 2)
        return true;


    const bool isValid = (tokens[size-2].id != TokenIdApplyToEach) ||
        (tokens[size-1].id != TokenIdLiteral &&
        tokens[size-1].id != TokenIdVariable &&
        tokens[size-1].id != TokenIdAsyncStart &&
        tokens[size-1].id != TokenIdAsyncEnd &&
        tokens[size-1].id != TokenIdAsyncJoin &&
        tokens[size-1].id != TokenIdEquals &&
        tokens[size-1].id != TokenIdWrite &&
        tokens[size-1].id != TokenIdWriteText &&
        tokens[size-1].id != TokenIdRead &&
        tokens[size-1].id != TokenIdReadText &&
        tokens[size-1].id != TokenIdOpenParenthesis &&
        tokens[size-1].id != TokenIdOpenCurly &&
        tokens[size-1].id != TokenIdCloseParenthesis &&
        tokens[size-1].id != TokenIdCloseCurly);

    if(isValid){
        return true;
    }else{
        return false;
    }
}

void Scanner::report(IScannerErrorReporter* errorReporter, const std::string& line, ScannerErrorType error)
{
    if(errorReporter)
        errorReporter->report(line, error);
}
