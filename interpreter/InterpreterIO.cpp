#include "InterpreterIO.h"
#include <iostream>

std::unique_ptr<std::vector<double>> InterpreterIO::read()
{
    std::string input, cur="";
    double converted;
    auto result = std::make_unique<std::vector<double>>();
    std::cout << '>';
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
    
void InterpreterIO::write(std::vector<double>& value)
{
    const int size = value.size();
    for(int i=0; i<size-1; i++){
        std::cout << value[i] << ", ";
    }
    std::cout << value[size-1] << std::endl;
}