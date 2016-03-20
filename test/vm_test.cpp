#include "parser.h"
#include "codegen.h"
#include "vm.h"

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
    nova::vm::VirtualMachine vm(code);
    vm.buildInstructions();
    vm.printInstructions();
    return 0;
}
