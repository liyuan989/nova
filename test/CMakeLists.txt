set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/test)

add_executable(tokenizer_test tokenizer_test.cpp)
target_link_libraries(tokenizer_test nova)

add_executable(parser_test parser_test.cpp)
target_link_libraries(parser_test nova)
