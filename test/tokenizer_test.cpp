#include <iostream>
#include <string>

#include "scanner.h"

int main(int argc, char *argv[])
{
    nova::Scanner scanner("tokenizer_test.txt");
    nova::Token toc = scanner.getNextToken();
    
    while (toc.getTokenType() != nova::TokenType::kEndOfFile && !scanner.getErrorFlag()) 
    {
        std::cout << toc.getTokenLocation().toString() << " name = " << toc.getTokenName() 
                  << ", TokenType = " << toc.getTokenTypeDescription()  << std::endl;
        toc = scanner.getNextToken();
    }
    
    return 0;
}
