#include "FileReader.h"
#include <fstream>

bool FileReader::read(const std::string& filepath, std::string& contents)
{
    std::ifstream file(filepath);   
    if(!file.is_open())
        return false;

    contents = "";
    std::string line;
    while(getline(file, line)){
        contents += line;
        contents += '\n';
    }

    return true;
}
