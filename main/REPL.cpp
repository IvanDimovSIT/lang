#include "REPL.h"
#include "../reporting/REPLPreprocessorErrorPrinter.h"
#include "../scanner/Preprocessor.h"
#include "../util/StringUtil.h"
#include "../token/OperatorsHelp.h"
#include "../token/TokenSyntax.h"
#include <iostream>
#include <cctype>

void REPL::run()
{
    ErrorPrinter errorPrinter;
    REPLPreprocessorErrorPrinter preprocessorErrorPrinter;
    InterpreterIO io;
    Interpreter interpreter(&errorPrinter, &io);
    std::vector<Token> tokens;
    std::vector<const Token*> tokensRef;
    std::unordered_map<std::string, Function> functions = {};
    std::unordered_set<std::string> functionNames;
    ProgramState programState;

    std::string source;
    do{
        bool isHelp = false, isExit = false;
        errorPrinter.resetErrors();
        functions = programState.functions;
        tokens.clear();
        tokensRef.clear();

        source = readInput(isHelp, isExit);
        if(isExit){
            deleteProgramState(programState);
            return;
        }
        if(isHelp){
            continue;
        }
        if(!Preprocessor::process(source, source, "", &preprocessorErrorPrinter)){
            continue;
        }
        if(!Scanner::scan(source, tokens, functions, functionNames, &errorPrinter)){
            continue;
        }
        if(!FunctionExtractor::extractFunctions(tokens, tokensRef)){
            std::cout << "Definition error" << std::endl;
            continue;
        }
        addFunctionNames(functionNames, functions);
        copyFunctions(functions, programState);
        Value result;
        if(!interpreter.execute(tokensRef, programState, result)){
            continue;
        }
        printValue(result);
    }while(true);
}

void REPL::countOpen(const std::string& line, int& openCurly, int& openSquare)
{
    for(const auto& i: Preprocessor::removeComments(line)){
        switch (i)
        {
        case '{': openCurly++; break;
        case '[': openSquare++; break;
        case '}': openCurly--; break;
        case ']': openSquare--; break;
        default:break;
        }
    }
}

std::string REPL::readInput(bool& isHelp, bool& isExit)
{
    std::string read, source = "";
    int openCurly = 0, openSquare = 0;
    do{
        if(openCurly > 0 || openSquare > 0)
            std::cout << "... ";
        else
            std::cout << ">>> ";
        getline(std::cin, read);
        if(openCurly == 0 && openSquare == 0 && isStringExit(read)){
            isExit = true;
            return "";
        }
        if(openCurly == 0 && openSquare == 0 && displayHelp(read)){
            isHelp = true;
            return "";
        }
        source += read;
        source += '\n';
        countOpen(read, openCurly, openSquare);
    }while(openCurly > 0 || openSquare > 0);

    return source;
}

void REPL::deleteFunction(Function& function)
{
    for(auto& i:function.body)
        delete i;
    
    function.body.clear();
}


void REPL::copyFunctions(std::unordered_map<std::string, Function>& functions, ProgramState& programState)
{
    for(auto& i: functions){
        Function functionCopy = i.second;
        if(programState.functions.find(i.first) != programState.functions.end()){
            deleteFunction(programState.functions[i.first]);
        }
        for(int i=0; i<functionCopy.body.size(); i++)
            functionCopy.body[i] = new Token(*functionCopy.body[i]);

        programState.functions[i.first] = functionCopy;
    }
}

void REPL::deleteProgramState(ProgramState& programState)
{
    for(auto& i: programState.functions){
        deleteFunction(i.second);
    }
}

void REPL::printValue(Value& value)
{
    if(value.size() == 0){
        return;
    }

    std::string valueString = "";
    StringUtil::convertValueToString(value, valueString);
   
   if(value.size() == 1){
        std::cout << valueString << std::endl;
   }else{
        std::cout << '[' << valueString << ']' << std::endl;
   }
}

bool REPL::isStringExit(const std::string& line)
{
    const int size = line.size();
    for(int i=0; i<size-3; i++){
        if(std::isspace(line[i]))
            continue;
        
        std::string exitString = line.substr(i, 4);

        return exitString == "exit" || exitString == "quit"; 
    }

    return false;
}

bool REPL::displayHelp(const std::string& line)
{
    const int size = line.size();
    for(int i=0; i<size-3; i++){
        if(std::isspace(line[i]))
            continue;
        
        std::string helpString = line.substr(i, 4);
        if(helpString == "help"){
            std::string toCheck = line.substr(i+4);
            displayHelpFor(toCheck);
            return true;
        }else
            return false;
    }

    return false;
}

void REPL::displayHelpFor(const std::string& operatorString)
{
    const int size = operatorString.size();
    std::string token = "";
    for(int i = 0; i<size; i++){
        if(std::isspace(operatorString[i]) && token.size() > 0)
            break;
        else if(std::isspace(operatorString[i]))
            continue;
        
        token += operatorString[i];
    }

    displayDescription(token);
}

void REPL::displayDescription(const std::string& token)
{
    if(token == ""){
        std::cout << "No operator entered (eg. use \"help +\" to display help about the \"+\" operator, or \"help all\" to display all operators)" << std::endl;
        return;
    }

    if(token == "all"){
        for(const auto& i: OperatorsHelp::descriptions){
            std::cout << i.second.description << std::endl;
        }
        return;
    }

    if(!TokenSyntax::isValidToken(token)){
        std::cout << '"' << token << '"' << " is not a recognised operator" << std::endl;
        return;
    }

    const auto description = OperatorsHelp::descriptions.find(TokenSyntax::getToken(token));
    if(description == OperatorsHelp::descriptions.end()){
        std::cout << '"' << token << '"' << " is not a recognised operator" << std::endl;
        return;
    }

    std::cout << description->second.description << "\nExamples:" << std::endl;
    for(const auto& i: description->second.examples){
        std::cout << i << std::endl;
    }
}

void REPL::addFunctionNames(std::unordered_set<std::string>& functionNames, std::unordered_map<std::string, Function>& functions){
    for(auto& i:functions)
        functionNames.insert(i.first);
}
