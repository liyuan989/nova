#include "codegen.h"
#include "parser.h"

#include <iostream>

int main(int argc, char* argv[])
{
    nova::Scanner scanner("test.tiny");
    nova::Parser parser(scanner);
    nova::AstPtr root = parser.parse();
    nova::Analysis analysis(root);
    analysis.buildSymbolTable();
    analysis.typeCheck();
    nova::CodeGenerator generator(analysis, root, "test.tiny", true);
    nova::CodeBuffer code =  generator.generateCode();
    std::cout << code;
    return 0;
}
