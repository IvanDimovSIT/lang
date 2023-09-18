#pragma once
#include <string>

class IPreprocessorErrorReporter{
public:
    virtual void reportFilepathError(const std::string& containingFile, int line, std::string& filePath) = 0;
    
    virtual void reportIncorrectIncludeSyntax(const std::string& containingFile, int line) = 0;

    virtual void reportIncludeRecursionLimitReached() = 0;
};

class Preprocessor{
public:
    static bool process(std::string source, std::string& dest, const std::string& sourceFilepath, IPreprocessorErrorReporter* errorReporter);

private:
    static bool process(
        int recursionLimit,
        std::string source,
        std::string& dest,
        const std::string& sourceFilepath,
        IPreprocessorErrorReporter* errorReporter);

    static bool includeSource(
        int recursionLimit,
        int& position,
        int line,
        std::string source,
        std::string& dest,
        const std::string& sourceFilepath,
        IPreprocessorErrorReporter* errorReporter);

    static std::string removeComments(const std::string& source);

    static std::string getDirectoryPath(const std::string& filepath);

    static void addSemicolonsToCodeBlocks(std::string& source);

    static const int MAX_RECURSION = 20;
};