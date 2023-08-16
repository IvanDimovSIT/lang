#include "Scanner.h"
#include "../util/StringUtil.h"
#include "../util/LiteralParser.h"
#include "../util/TokenSubArrayFinder.h"

std::map<std::string, TokenId> Scanner::tokenMap = {
    {"+", TokenIdAdd},
    {"-", TokenIdSubtract},
    {"*", TokenIdMultiply},
    {"/", TokenIdDivide},
    {"=", TokenIdEquals},
    {"f", TokenIdFunctionDeclaration},
    {"i", TokenIdIterate},
    {"a", TokenIdLeftParam},
    {"b", TokenIdRightParam},
    {"{", TokenIdOpenCurly},
    {"}", TokenIdCloseCurly},
    {"(", TokenIdOpenParenthesis},
    {")", TokenIdCloseParenthesis},
    {"\n", TokenIdEndLine},
    {"?", TokenIdIf},
    {"loop", TokenIdLoop}
};


bool Scanner::scan(
    const std::string& source,
    std::vector<Token>& tokens,
    std::map<std::string, Function>& functions,
    IScannerErrorReporter* errorReporter)
{
    tokens.clear();
    functions.clear();

    const int sourceLen = source.size();
    std::set<std::string> functionNames;
    int lineCounter = 1;
    std::string curr = "";
    bool hadError = !validateParenthesis(source, sourceLen, errorReporter);
    if(!findFunctionNames(source, functionNames, errorReporter)){
        hadError = true;
    }
    
    for(int i=0; i<sourceLen; i++){
        if(isSingleCharToken(source[i])){
            if(!matchToken(curr, tokens, functionNames)){
                hadError = true;
                if(errorReporter)
                    errorReporter->report(lineCounter, ScannerErrorTypeUnrecognisedToken);
            }
            curr = "";
            curr += source[i];
            if(!matchToken(curr, tokens, functionNames)){
                hadError = true;
                if(errorReporter)
                    errorReporter->report(lineCounter, ScannerErrorTypeUnrecognisedToken);
            }
            curr = "";
        }else if(source[i] == ' ' || source[i] == '\t'){
            if(!matchToken(curr, tokens, functionNames)){
                hadError = true;
                if(errorReporter)
                    errorReporter->report(lineCounter, ScannerErrorTypeUnrecognisedToken);
            }
            curr = "";
        }else{
            curr += source[i];
        }

        if(source[i] == '\n')
            lineCounter++;
    }

    if (!matchToken(curr, tokens, functionNames)) {
        hadError = true;
        if (errorReporter)
            errorReporter->report(lineCounter, ScannerErrorTypeUnrecognisedToken);
    }

    if(!extractFunctions(tokens, functions)){
        hadError = true;
        if(errorReporter)
            errorReporter->report(lineCounter, ScannerErrorTypeFunctionDefinitionError);
    }

    return !hadError;
}

bool Scanner::findFunctionNames(const std::string& source, std::set<std::string>& functionNames, IScannerErrorReporter* errorReporter)
{
    functionNames.clear();
    const int stringLen = source.size();
    std::string curr = "";
    bool foundError = false;
    bool scanningFunctionName = false;
    int lineCounter = 1;

    for(int i=0; i<stringLen; i++){
        if(source[i] == '\n')
            lineCounter++;

        if(source[i] == 'f'){
            if(!scanningFunctionName){
                scanningFunctionName = true;
                continue;
            }else{
                foundError = true;
                if(errorReporter)
                    errorReporter->report(lineCounter, ScannerErrorTypeFunctionDefinitionError);
            }
        }
        else if(scanningFunctionName){
            if(source[i] == ' ' && curr.size() <= 0)
                continue;
            
            if(source[i] == ' ' || source[i] == '{' || source[i] == '\n'){
                scanningFunctionName = false;
                if(functionNames.count(curr) != 0){
                    foundError = true;
                    if(errorReporter)
                        errorReporter->report(lineCounter, ScannerErrorTypeFunctionRedefinition);
                }
                if(!StringUtil::isValidIdentifierName(curr)){
                    foundError = true;
                    curr = "";
                    if(errorReporter)
                        errorReporter->report(lineCounter, ScannerErrorTypeFunctionDefinitionError);
                }else{
                    functionNames.insert(curr);
                    curr = "";
                }
            }else if(source[i] >= 'A' && source[i] <= 'Z'){
                curr += source[i];
            }else{
                foundError = true;
                if(errorReporter)
                    errorReporter->report(lineCounter, ScannerErrorTypeFunctionNameDefinitionError);
            }
        }
    }

    if(scanningFunctionName){
        foundError = true;
        if(errorReporter)
            errorReporter->report(lineCounter, ScannerErrorTypeFunctionDefinitionError);
    }

    return !foundError;
}

void Scanner::determineFunctionPrameters(Function& function)
{
    function.left = false;
    function.right = false;
    const int size = function.body.size();
    for(int i=0; i<size; i++){
        if(function.body[i]->id == TokenIdLeftParam)
            function.left = true;
        else if(function.body[i]->id == TokenIdRightParam)
            function.right = true;
    }
}

bool Scanner::extractFunctions(std::vector<Token>& tokens, std::map<std::string, Function>& functions)
{
    const int size = tokens.size();
    Function f;
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
            if(upTo == TOKEN_INDEX_NOT_FOUND){
                return false;
            }
            TokenSubArrayFinder::findSubArray(tokens, f.body, i+1, upTo-1);
            if(f.body.size() <= 0)
                return false;
            determineFunctionPrameters(f);
            functions[functionName] = f;
            functionName = "";
            findOpen = false;
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

bool Scanner::matchToken(const std::string& token, std::vector<Token>& tokens, std::set<std::string>& functionNames)
{
    if(token.size() <= 0 || token[0] == ' ')
        return true;

    Token t;
    t.str = token;
    
    if(tokenMap.count(token) >= 1){
        t.id = tokenMap[token];
        tokens.push_back(t);
        return true;
    }else if(LiteralParser::parse(token, t.val)){
        t.id = TokenIdLiteral;
        tokens.push_back(t);
        return true;
    }else if(functionNames.count(token) != 0){
        t.id = TokenIdFunction;
        tokens.push_back(t);
        return true;
    }else if(StringUtil::isValidIdentifierName(token)){
        t.id = TokenIdVariable;
        tokens.push_back(t);
        return true;
    }

    return false;
}

bool Scanner::isSingleCharToken(char token)
{
    switch (token)
    {
    case 'i':
    case 'f':
    case 'a':
    case 'b':
    case '{':
    case '}':
    case '(':
    case ')':
    case '\n':
        return true;
    default:
        return false;
    }
}

bool Scanner::validateParenthesis(const std::string& source, const int sourceLen, IScannerErrorReporter* errorReporter)
{
    int openP = 0;
    int openCurly = 0;
    int lineCounter = 1;
    bool hadError = false;

    for(int i=0; i<sourceLen; i++){
        if(source[i] == '\n')
            lineCounter++;
        
        if(source[i] == '('){
            openP++;
        }else if(source[i] == ')'){
            openP--;
        }else if(source[i] == '{'){
            openCurly++;
        }else if(source[i] == '}'){
            openCurly--;
        }

        if(openP < 0){
            hadError = true;
            if(errorReporter)
                errorReporter->report(lineCounter, ScannerErrorTypeMoreClosingParenthesis);
        }
        if(openCurly < 0){
            hadError = true;
            if(errorReporter)
                errorReporter->report(lineCounter, ScannerErrorTypeMoreClosingCurlyBrackets);
        }

    }

    if(openP > 0){
        hadError = true;
        if(errorReporter)
            errorReporter->report(lineCounter, ScannerErrorTypeMoreOpenParenthesis);
    }

    if(openCurly > 0){
        hadError = true;
        if(errorReporter)
            errorReporter->report(lineCounter, ScannerErrorTypeMoreOpenCurlyBrackets);
    }
    
    return !hadError;
}
