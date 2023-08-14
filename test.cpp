#include <iostream>
#include <cassert>
#include "scanner/Scanner.h"
#include "util/LiteralParser.h"
#include "util/StringUtil.h"
#include "util/TokenSubArrayFinder.h"
#include "debug/DebugPrinter.h"
#include "debug/ErrorPrinter.h"
#include "interpreter/Interpreter.h"
#include "interpreter/FunctionExtractor.h"

ErrorPrinter errorPrinter;

void testLiteralParser(){
    std::vector<double> lit;
    assert(!LiteralParser::parse("sssa",lit));
    assert(LiteralParser::parse("123",lit));
    assert(lit.size() == 1);
    assert(lit[0] == 123);
    
    assert(LiteralParser::parse("-0.12,3.1,0.0",lit));
    assert(lit.size() == 3);
    assert(lit[0] == -0.12);
    assert(lit[1] == 3.1);
    assert(lit[2] == 0.0);
}

void testStringUtil(){
    assert(StringUtil::isValidIdentifierName("MYVAR"));
    assert(StringUtil::isValidIdentifierName("MYFUNC1"));
    assert(!StringUtil::isValidIdentifierName("MYVaR"));
    assert(!StringUtil::isValidIdentifierName("0MYVR"));
    assert(!StringUtil::isValidIdentifierName("ABC.A"));
}



void testScanner(){
    std::string source = "f FUNC { a + 1,2,3 }\nA = 2,1 FUNC ";
    std::vector<Token> tokens;
    std::map<std::string, Function> functions;
    assert(Scanner::scan(source, tokens, functions, &errorPrinter));
    assert(functions.size() == 1);
    assert(functions["FUNC"].body.size() == 3);
    assert(functions["FUNC"].left);
    assert(!functions["FUNC"].right);

    assert(tokens[0].id == TokenIdFunctionDeclaration);
    assert(tokens[1].id == TokenIdFunction);
    assert(tokens[2].id == TokenIdOpenCurly);
    assert(tokens[3].id == TokenIdLeftParam);
    assert(tokens[4].id == TokenIdAdd);
    assert(tokens[5].id == TokenIdLiteral);
    assert(tokens[5].val.size() == 3);
    assert(tokens[6].id == TokenIdCloseCurly);
    assert(tokens[7].id == TokenIdEndLine);
    assert(tokens[8].id == TokenIdVariable);
    assert(tokens[9].id == TokenIdEquals);
    assert(tokens[10].id == TokenIdLiteral);
    assert(tokens[10].val.size() == 2);
    assert(tokens[11].id == TokenIdFunction);

    std::vector<Token*> exec;
    FunctionExtractor::extractFunctions(tokens, exec);
    assert(exec.size() == 5);

    std::vector<double> result;
    assert(Interpreter::execute(exec, functions, result, (IRuntimeErrorReporter*)&errorPrinter));

}

int main(){
    testLiteralParser();
    testStringUtil();
    testScanner();

    std::cout << "ALL TESTS PASSED!" << std::endl;
    return 0;
}