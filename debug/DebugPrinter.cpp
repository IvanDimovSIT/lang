#include "DebugPrinter.h"
#include <iostream>

void DebugPrinter::printTokens(const std::vector<Token>& tokens)
{
    std::vector<const Token*> t;
    for(const auto& i: tokens){
        t.push_back(&i);
    }
    printTokens(t);
}

void DebugPrinter::printTokens(const std::vector<const Token*>& tokens)
{
    for(const auto& i: tokens){
        std::cout << "(id:"<< i->id <<")" << "(str:\"" << i->str << "\")" << "(val(count):\"" << i->val.size() << "\")" << std::endl;
    }
}
