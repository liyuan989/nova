set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

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

add_executable(tiny tiny.cpp)
target_link_libraries(tiny nova)
