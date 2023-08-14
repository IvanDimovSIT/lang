#include "FunctionExtractor.h"
#include "../util/TokenSubArrayFinder.h"

bool FunctionExtractor::extractFunctions(std::vector<Token*> &tokens, std::vector<Token*> &dest)
{
    dest.clear();
    const int size = tokens.size();
    for(int i=0; i<size; i++){
        if(tokens[i]->id != TokenIdFunctionDeclaration){
            dest.push_back(tokens[i]);
        }else{
            while(tokens[i]->id != TokenIdOpenCurly && i<size)
                i++;
            i = TokenSubArrayFinder::findClosingCurly(tokens, i);
            if(i == TOKEN_INDEX_NOT_FOUND)
                return false;
        }
    }

    return true;
}