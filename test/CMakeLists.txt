set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/test)

add_executable(tokenizer_test tokenizer_test.cpp)
target_link_libraries(tokenizer_test nova)

add_executable(parser_test parser_test.cpp)
target_link_libraries(parser_test nova)

add_executable(analysis_test analysis_test.cpp)
target_link_libraries(analysis_test nova)

add_executable(codegen_test codegen_test.cpp)
target_link_libraries(codegen_test nova)

add_executable(vm_test vm_test.cpp)
target_link_libraries(vm_test nova)
