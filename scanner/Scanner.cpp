#include "Scanner.h"
#include "../util/StringUtil.h"
#include "../util/LiteralParser.h"
#include "../util/TokenSubArrayFinder.h"

std::map<std::string, TokenId> Scanner::tokenMap = {
    {"+", TokenIdAdd},
    {"-", TokenIdSubtract},
    {"*", TokenIdMultiply},
    {"/", TokenIdDivide},
    {"%", TokenIdMod},
    {"**",TokenIdPower},
    {"<", TokenIdLessThan},
    {">", TokenIdGreaterThan},
    {"<=",TokenIdLessThanOrEquals},
    {">=",TokenIdGreaterThanOrEquals},
    {"==",TokenIdIsEquals},
    {"!=",TokenIdNotEquals},
    {"=", TokenIdEquals},
    {"!", TokenIdLogicalNot},
    {"|", TokenIdUnion},
    {":", TokenIdSelect},
    {"?", TokenIdRandom},
    {"^", TokenIdCeil},
    {"_", TokenIdFloor},
    {"~", TokenIdRound},
    {"$", TokenIdSort},
    {"@", TokenIdReverse},
    {"&", TokenIdMakeSet},
    {"#", TokenIdCount},
    {"#+",TokenIdSumAll},
    {"#*",TokenIdMultiplyAll},
    {"f", TokenIdFunctionDeclaration},
    {"a", TokenIdLeftParam},
    {"b", TokenIdRightParam},
    {"{", TokenIdOpenCurly},
    {"}", TokenIdCloseCurly},
    {"(", TokenIdOpenParenthesis},
    {")", TokenIdCloseParenthesis},
    {"\n",TokenIdEndLine},
    {";",TokenIdEndLine},
    {"if",TokenIdIf},
    {"do",TokenIdLoop},
    {"i", TokenIdIterate},
    {"r", TokenIdRead},
    {"w", TokenIdWrite},
    {"g", TokenIdReadText},
    {"t", TokenIdWriteText},
    {"\\", TokenIdApplyToEach}
};


bool Scanner::scan(
    const std::string& programSource,
    std::vector<Token>& tokens,
    std::map<std::string, Function>& functions,
    IScannerErrorReporter* errorReporter)
{
    tokens.clear();
    functions.clear();

    std::string source = removeComments(programSource);
    const int sourceLen = source.size();
    std::set<std::string> functionNames;
    int lineCounter = 1;
    std::string curr = "";
    bool hadError = !validateParenthesis(source, sourceLen, errorReporter);
    bool inComment = false;
    if(!findFunctionNames(source, functionNames, errorReporter)){
        hadError = true;
    }
    
    for(int i=0; i<sourceLen; i++){
        if(curr == "\""){
            const int stringLiteralEnd = StringUtil::findStringLiteralEndIndex(source, i-1);
            if(stringLiteralEnd == STRING_END_NOT_FOUND){
                hadError = true;
                if(errorReporter)
                    errorReporter->report(lineCounter, ScannerErrorTypeStringLiteralError);
            }
            curr += source.substr(i, stringLiteralEnd-i+1);
            if(!matchToken(curr, tokens, functionNames)){
                hadError = true;
                if(errorReporter)
                    errorReporter->report(lineCounter, ScannerErrorTypeStringLiteralError);
            }else{
                i = stringLiteralEnd;
            }

            curr = "";
            continue;
        }

        // special if token case
        if(curr == "" && source[i] == 'i' && (i+1<sourceLen) && source[i+1] == 'f' && matchToken("if", tokens, functionNames)){
            i++;
        }else if(isSingleCharToken(source[i])){
            if(!matchToken(curr, tokens, functionNames)){
                hadError = true;
                if(errorReporter)
                    errorReporter->report(lineCounter, ScannerErrorTypeUnrecognisedToken);
            }
            curr = "";
            if(!validateOperatorModifier(tokens, lineCounter, errorReporter))
                hadError = true;
            curr += source[i];
            if(!matchToken(curr, tokens, functionNames)){
                hadError = true;
                if(errorReporter)
                    errorReporter->report(lineCounter, ScannerErrorTypeUnrecognisedToken);
            }
            curr = "";
            if(!validateOperatorModifier(tokens, lineCounter, errorReporter))
                hadError = true;
        }else if(source[i] == ' ' || source[i] == '\t' /*|| (i+1<sourceLen && source[i+1] == '"')*/){
            if(!matchToken(curr, tokens, functionNames)){
                hadError = true;
                if(errorReporter)
                    errorReporter->report(lineCounter, ScannerErrorTypeUnrecognisedToken);
            }
            curr = "";
            if(!validateOperatorModifier(tokens, lineCounter, errorReporter))
                hadError = true;
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
    if(!validateOperatorModifier(tokens, lineCounter, errorReporter))
        hadError = true;

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

        if(source[i] == '"'){
            const int end = StringUtil::findStringLiteralEndIndex(source, i);
            if(end == STRING_END_NOT_FOUND){
                if(errorReporter)
                    errorReporter->report(lineCounter, ScannerErrorTypeStringLiteralError);
                foundError = true;
            }else{
                i = end;
            }
            continue;
        }

        if((source[i] == 'f' && i ==0) || (source[i] == 'f' && (i-1>=0) && source[i-1] != 'i')){ // extra checks for "if"
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
            
            if(source[i] == ' ' || source[i] == '{' || source[i] == '\n' || source[i] == ';'){
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
            f.body.clear();
            TokenSubArrayFinder::findSubArray(tokens, f.body, i+1, upTo-1);
            if(f.body.size() <= 0)
                return false;
            determineFunctionPrameters(f);
            functions[functionName] = f;
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
    }else if(LiteralParser::parse(token, t.val) || LiteralParser::parseString(token, t.val)){
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
    case 'r':
    case 'w':
    case 'g':
    case 't':
    case '{':
    case '}':
    case '(':
    case ')':
    case '\n':
    case ';':
    case '\\':
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

        if(source[i] == '"'){
            const int end = StringUtil::findStringLiteralEndIndex(source, i);
            if(end == STRING_END_NOT_FOUND){
                if(errorReporter)
                    errorReporter->report(lineCounter, ScannerErrorTypeStringLiteralError);
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


    bool isValid = (tokens[size-2].id != TokenIdApplyToEach) ||
        (tokens[size-1].id != TokenIdLiteral &&
        tokens[size-1].id != TokenIdVariable &&
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
        if(errorReporter)
            errorReporter->report(line, ScannerErrorTypeNoOperatorToModify);
        return false;
    }
}