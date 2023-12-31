#include "InterpreterIO.h"
#include <iostream> 
#include "../util/StringUtil.h"

std::unique_ptr<Value> InterpreterIO::read()
{
    std::string input, cur="";
    double converted;
    auto result = std::make_unique<Value>();
    std::cout << "numbers>";
    getline(std::cin, input);
    const int size = input.size();

    for(int i=0; i<size; i++){
        if(input[i] == ' ' || input[i] == ',' || input[i] == '\t'){
            try{    
                converted = std::stod(cur);
                result->push_back(converted);
            }catch(std::exception& e){}
            cur = "";
        }else{
            cur += input[i];
        }
    }

    try{    
        converted = std::stod(cur);
        result->push_back(converted);
    }catch(std::exception& e){}

    if(result->size() == 0)
        result->push_back(0.0);

    return std::move(result);
}
    
void InterpreterIO::write(const Value& value)
{
    std::string converted = "";
    StringUtil::convertValueToString(value, converted);
    std::cout << converted << std::endl;
}

std::unique_ptr<Value> InterpreterIO::readText()
{
    std::string input;
    auto result = std::make_unique<Value>();
    std::cout << "text>";
    getline(std::cin, input);
    
    for(const auto& i: input){
        if(i < CHARS_START || i > CHARS_END)
            continue;

        result->push_back((double)i);
    }

    
    if(result->size() == 0)
        result->push_back(0.0);

    return std::move(result);
}
    
void InterpreterIO::writeText(const Value& value)
{
    for(const auto& i: value){
        if(i<CHARS_START || i>CHARS_END)
            continue;
        
        std::cout << ((char)i);
    }
    std::cout << std::endl;
}
