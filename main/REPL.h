#pragma once
#include "../interpreter/Interpreter.h"
#include "../scanner/Scanner.h"
#include "../debug/ErrorPrinter.h"
#include "../interpreter/InterpreterIO.h"
#include "../interpreter/FunctionExtractor.h"

class REPL{
public:
    static void run();
private:
    static std::string readInput();
    static void countOpen(const std::string& line, int& openCurly, int& openSquare);
    static void copyFunctions(std::map<std::string, Function>& functions, ProgramState& programState);
    static void deleteFunction(Function& function);
    static void deleteProgramState(ProgramState& programState);
    static void printValue(Value& v);
    static bool isStringExit(const std::string& line);
    static void addFunctionNames(std::set<std::string>& functionNames, std::map<std::string, Function>& functions);
};
