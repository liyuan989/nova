#include "scanner.h"
#include "parser.h"
#include "analysis.h"

int main(int argc, char* argv[]) {
    nova::Scanner scanner("test.tiny");
    nova::Parser parser(scanner);
    nova::Analysis analysis(parser.parse());
    analysis.buildSymbolTable();
    analysis.typeCheck();
    analysis.printSymbolTable();
    return 0;
}
