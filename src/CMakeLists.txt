set(SRCS
 token.cpp
 scanner.cpp
 parser.cpp
 ast.cpp
 error.cpp
 analysis.cpp
 symbol_table.cpp
 codegen.cpp
 vm.cpp
 )

add_library(nova ${SRCS})
