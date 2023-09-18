#include "Preprocessor.h"
#include "../util/StringUtil.h"
#include "../util/FileReader.h"

bool Preprocessor::process(std::string source, std::string& dest, const std::string& sourceFilepath, IPreprocessorErrorReporter* errorReporter)
{
    source = removeComments(source);
    dest = "";
    
    int line = 1;
    bool isSearchingForInclude = true;
    for(int i=0; i<source.size(); i++){
        if(source[i] == '\n')
            line++;
        
        if(!isSearchingForInclude || source[i] != '\''){
            dest += source[i];
            if(source[i] == '\n'){
                isSearchingForInclude = true;
            }else if(!StringUtil::isSeparator(source[i])){
                isSearchingForInclude = false;
            }
            continue;
        }

        if(source[i] == '\''){
            isSearchingForInclude = true;
            std::string includedSource;
            if(!includeSource(i, line, source, includedSource, sourceFilepath, errorReporter))
                return false;
            dest += includedSource + '\n';
        }
    }

    return true;
}

bool Preprocessor::includeSource(int& position, int line, std::string source, std::string& dest, const std::string& sourceFilepath, IPreprocessorErrorReporter* errorReporter)
{
    const int size = source.size();
    std::string include = "";
    std::string relativeFilepath = "";

    for(int i = position+1; i<size; i++){
        if(source[i] == '\n'){
            if(errorReporter)
                errorReporter->reportIncorrectIncludeSyntax(sourceFilepath, line);
            return false;
        }else if(source[i] == '\''){
            std::string rawContents;
            std::string fullIncludedFilepath = getDirectoryPath(sourceFilepath)+relativeFilepath;
            if(!FileReader::read(fullIncludedFilepath, rawContents)){
                if(errorReporter)
                    errorReporter->reportFilepathError(sourceFilepath, line, fullIncludedFilepath);
                
                return false;
            }
            if(!process(rawContents, include, fullIncludedFilepath, errorReporter))
                return false;
            
            dest = include;
            position = i;
            return true;
        }else{
            relativeFilepath += source[i];
        }
    }

    if(errorReporter)
        errorReporter->reportIncorrectIncludeSyntax(sourceFilepath, line);

    return false;
}
    

std::string Preprocessor::removeComments(const std::string& source){
    std::string program = "";
    bool inComment = false;
    const int sourceLen = source.size();
    
    for(int i=0; i<sourceLen; i++){
        if(inComment && source[i]=='\n'){
            inComment = false;
            program += '\n';
        }else if((!inComment) && source[i] == '/' && (i+1<sourceLen) && source[i+1] == '/'){
            inComment = true;
        }else if(!inComment){
            program += source[i];
        }
    }

    return program;
}

std::string Preprocessor::getDirectoryPath(const std::string& filepath)
{
    std::string path = "";
    bool pathStart = false;
    for(int i=filepath.size()-1; i>=0; i--){
        if(pathStart){
            path = filepath[i] + path;
        }else if(filepath[i] == '/'){
            pathStart = true;
            path = "/";
        }
    }

    return path;
}

