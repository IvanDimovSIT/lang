#include "Preprocessor.h"

void Preprocessor::process(std::string& source)
{
    std::string result = "";
    const int size = source.size();

    for(int i=0; i<size; i++){
        if(source[i] == '}'){
            result += "\n}\n";
        }else{
            result += source[i];
        }
    }
    result += '\n';

    source = result;
}