#include "Scanner.h"
#include "../util/StringUtil.h"
#include "../util/LiteralParser.h"
#include "../util/TokenSubArrayFinder.h"
#include "../token/TokenSyntax.h"

bool Scanner::scan(
    const std::string& programSource,
    std::vector<Token>& tokens,
    std::unordered_map<std::string, Function>& functions,
    IScannerErrorReporter* errorReporter)
{
    std::unordered_set<std::string> empty;
    return scanREPL(programSource, tokens, functions, empty, errorReporter);
}

bool Scanner::scanREPL(
    const std::string& programSource,
    std::vector<Token>& tokens,
    std::unordered_map<std::string, Function>& functions,
    std::unordered_set<std::string>& previousFunctions,
    IScannerErrorReporter* errorReporter)
{
    tokens.clear();
    functions.clear();

    std::string source = removeComments(programSource);
    const int sourceLen = source.size();
    std::unordered_set<std::string> functionNames;
    int lineCounter = 1;
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
                report(errorReporter, lineCounter, ScannerErrorTypeStringLiteralError);
            }
            curr += source.substr(i, stringLiteralEnd-i+1);
            if(!matchToken(curr, tokens, functionNames, previousFunctions, lineCounter)){
                hadError = true;
                report(errorReporter, lineCounter, ScannerErrorTypeStringLiteralError);
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
            matchToken("if", tokens, functionNames, previousFunctions, lineCounter)){
            i++;
        }else if(TokenSyntax::isSingleCharToken(source[i])){ // End current token and add next token
            if(!matchToken(curr, tokens, functionNames, previousFunctions, lineCounter)){
                hadError = true;
                report(errorReporter, lineCounter, ScannerErrorTypeUnrecognisedToken);
            }
            curr = "";
            if(!validateOperatorModifier(tokens, lineCounter, errorReporter))
                hadError = true;
            curr += source[i];
            if(!matchToken(curr, tokens, functionNames, previousFunctions, lineCounter)){
                hadError = true;
                report(errorReporter, lineCounter, ScannerErrorTypeUnrecognisedToken);
            }
            curr = "";
            if(!validateOperatorModifier(tokens, lineCounter, errorReporter))
                hadError = true;
        }else if(isSeparator(source[i])){ // End token scanning and add it
            if(!matchToken(curr, tokens, functionNames, previousFunctions, lineCounter)){
                hadError = true;
                report(errorReporter, lineCounter, ScannerErrorTypeUnrecognisedToken);
            }
            curr = "";
            if(!validateOperatorModifier(tokens, lineCounter, errorReporter))
                hadError = true;
        }else{ // Add character
            curr += source[i];
            if((i+1<sourceLen) &&
                TokenSyntax::isValidToken(curr+source[i+1]) &&
                matchToken(curr+source[i+1], tokens, functionNames, previousFunctions, lineCounter)){
                i++;
                curr = "";
            }
        }

        if(source[i] == '\n')
            lineCounter++;
    }

    if (!matchToken(curr, tokens, functionNames, previousFunctions, lineCounter)) {
        hadError = true;
        report(errorReporter, lineCounter, ScannerErrorTypeUnrecognisedToken);
    }
    if(!validateOperatorModifier(tokens, lineCounter, errorReporter))
        hadError = true;

    if(!extractFunctions(tokens, functions)){
        hadError = true;
        report(errorReporter, lineCounter, ScannerErrorTypeFunctionDefinitionError);
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
                report(errorReporter, lineCounter, ScannerErrorTypeStringLiteralError);
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
                report(errorReporter, lineCounter, ScannerErrorTypeFunctionDefinitionError);
            }
        }
        else if(isScanningFunctionName){
            if(isSeparator(source[i]) && curr.size() <= 0)
                continue;
            
            if(isSeparator(source[i]) || source[i] == '{' || source[i] == '\n' || source[i] == ';'){
                isScanningFunctionName = false;
                if(functionNames.count(curr) != 0){
                    foundError = true;
                    report(errorReporter, lineCounter, ScannerErrorTypeFunctionRedefinition);
                }
                if(!StringUtil::isValidIdentifierName(curr)){
                    foundError = true;
                    curr = "";
                    report(errorReporter, lineCounter, ScannerErrorTypeFunctionDefinitionError);
                }else{
                    functionNames.insert(curr);
                    curr = "";
                }
            }else if(source[i] >= 'A' && source[i] <= 'Z' || (isAfterFirstFunctionChar && source[i] >= '0' && source[i] <= '9')){
                curr += source[i];
                isAfterFirstFunctionChar = true;
            }else{
                foundError = true;
                report(errorReporter, lineCounter, ScannerErrorTypeFunctionNameDefinitionError);
            }
        }
    }

    if(isScanningFunctionName){
        foundError = true;
        report(errorReporter, lineCounter, ScannerErrorTypeFunctionDefinitionError);
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

bool Scanner::extractFunctions(std::vector<Token>& tokens, std::unordered_map<std::string, Function>& functions)
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
    std::unordered_set<std::string>& previousFunctions,
    int line)
{
    if(tokenString.size() <= 0 || isSeparator(tokenString[0]))
        return true;

    Token token;
    token.str = tokenString;
    
    if(TokenSyntax::isValidToken(tokenString)){
        token.id = TokenSyntax::getToken(tokenString);
        if(token.id == TokenIdEndLine)
            token.val.push_back(line);
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
                report(errorReporter, lineCounter, ScannerErrorTypeStringLiteralError);
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
                report(errorReporter, lineCounter, ScannerErrorTypeMissingAsyncEnd);
            }else{
                asyncStart = true;
            }
        }else if(source[i] == ']'){
            if(asyncStart){
                asyncStart = false;
            }else{
                hadError = true;
                report(errorReporter, lineCounter, ScannerErrorTypeMissingAsyncStart);
            }
        }

        if(openP < 0){
            hadError = true;
            report(errorReporter, lineCounter, ScannerErrorTypeMoreClosingParenthesis);
        }
        if(openCurly < 0){
            hadError = true;
            report(errorReporter, lineCounter, ScannerErrorTypeMoreClosingCurlyBrackets);
        }

    }

    if(openP > 0){
        hadError = true;
        if(errorReporter)
            errorReporter->report(lineCounter, ScannerErrorTypeMoreOpenParenthesis);
    }

    if(openCurly > 0){
        hadError = true;
        report(errorReporter, lineCounter, ScannerErrorTypeMoreOpenCurlyBrackets);
    }
    
    return !hadError;
}

std::string Scanner::removeComments(const std::string& source)
{
    std::string program = "";
    bool inComment = false;
    const int sourceLen = source.size();
    
    for(int i=0; i<sourceLen; i++){
        if(inComment && source[i]=='\n'){
            inComment = false;
            program += '\n';
        }else if((!inComment) && source[i] == '/' && (i+1<sourceLen) && source[i+1] == '/'){
            inComment = true;
        }else if(!inComment){
            program += source[i];
        }
    }

    return program;
}

bool Scanner::validateOperatorModifier(std::vector<Token>& tokens, int line, IScannerErrorReporter* errorReporter)
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
        report(errorReporter, line, ScannerErrorTypeNoOperatorToModify);
        return false;
    }
}

void Scanner::report(IScannerErrorReporter* errorReporter, int line, ScannerErrorType error)
{
    if(errorReporter)
        errorReporter->report(line, error);
}

inline bool Scanner::isSeparator(char c)
{
    return c == ' ' || c == '\t';
}
