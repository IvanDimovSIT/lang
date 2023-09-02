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

bool LiteralParser::parse(const std::string& literal, Value& value){
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

bool LiteralParser::parseString(const std::string& literal, Value& value)
{
    value.clear();
    const int size = literal.size()-1;
    if(literal[size] != '"' || size < 2 || literal[0] != '"')
        return false;

    for(int i=1; i<size; i++){
        if(literal[i]!='\\'){
            value.push_back((double)literal[i]);
        }else{
            i++;
            switch (literal[i])
            {
            case 'n':
                value.push_back((double)'\n');
                break;
            case 't':
                value.push_back((double)'\t');
                break;
            case '0':
                value.push_back((double)'\0');
                break;
            default:
                value.push_back((double)literal[i]);
                break;
            }
        }
    }

    return true;
}
