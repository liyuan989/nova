#include "error.h"

#include <iostream>

#include "scanner.h"
#include "parser.h"

namespace nova 
{

void errorToken(const std::string& message)
{
    std::cerr << "Token Error: " << message << std::endl;
    Scanner::setErrorFlag(true);
}

void errorSyntax(const std::string& message)
{
    std::cerr << "Syntax Error: " << message << std::endl;
    Parser::setErrorFlag(true);
}
    
} // namespace nova
