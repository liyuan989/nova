#include <iostream>

#include "scanner.h"
#include "parser.h"
#include "analysis.h"
#include "codegen.h"
#include "vm.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Useage: " << argv[0] << " [filename]" << std::endl;
        return 0;
    }
    nova::Scanner scanner(argv[1]);
    if (!scanner.isFileOpened()) {
        std::cerr << "Can not touch the file " << argv[1] << std::endl;
        return 0;
    }

    nova::Parser parser(scanner);
    nova::AstPtr root = parser.parse();
    nova::Analysis analysis(root);
    analysis.buildSymbolTable();
    analysis.typeCheck();
    if (nova::Scanner::getErrorFlag() ||
        nova::Parser::getErrorFlag()) {
        return 0;      
    }
    nova::CodeGenerator generator(analysis, root, argv[1], true);
    nova::CodeBuffer code = generator.generateCode();
    if (nova::CodeGenerator::getErrorFlag()) {
        return 0;   
    }

    nova::vm::VirtualMachine vm(code);
    vm.buildInstructions();
    if (nova::vm::Scanner::getErrorFlag() ||
        nova::vm::VirtualMachine::getErrorFlag()) {
        return 0;   
    }
    vm.run();
    return 0;
}

