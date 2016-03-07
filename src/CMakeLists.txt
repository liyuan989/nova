set(SRCS
 token.cpp
 scanner.cpp
 parser.cpp
 ast.cpp
 error.cpp
 )

add_library(nova ${SRCS})
