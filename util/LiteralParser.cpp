#include "LiteralParser.h"
#include <stdexcept>

bool LiteralParser::isValidChar(char c){
    return (
        (c>='0' && c<='9') ||
        (c == '-') ||
        (c == '.') ||
        (c == ',')
    );
}

bool LiteralParser::parseSingleValue(const std::string literal, double& value){
    if(literal.size() <= 0)
        return false;

    try{
        value = std::stod(literal);
    }catch(std::exception& e){
        return false;
    }

    return true;
}

bool LiteralParser::parse(const std::string& literal, std::vector<double>& value){
    value.clear();
    std::string curr = "";
    double val;
    for(const auto& i: literal){
        if(!isValidChar(i))
            return false;

        if(i == ','){
            if(!parseSingleValue(curr, val))
                return false;
            
            value.push_back(val);
            curr = "";
        }else{
            curr += i;
        }
    }

    if(!parseSingleValue(curr, val))
        return false;
            
    value.push_back(val);

    return true;
}
