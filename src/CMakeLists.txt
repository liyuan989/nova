set(SRCS
 token.cpp
 scanner.cpp
 parser.cpp
 ast.cpp
 error.cpp
 analysis.cpp
 symbol_table.cpp
 )

add_library(nova ${SRCS})
