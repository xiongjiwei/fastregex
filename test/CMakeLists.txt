add_executable(Catch_tests_run parser_test.cpp restring_test.cpp ast_test.cpp vm_test.cpp rex_test.cpp program_test.cpp re_test.cpp)
target_link_libraries(Catch_tests_run regex_lib)