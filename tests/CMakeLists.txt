add_executable(cpp-pointer_tests)

target_sources(cpp-pointer_tests
  PRIVATE
    cpp-pointer_t.cpp
)

target_link_libraries(cpp-pointer_tests
  PUBLIC
    cpp-pointer
)

add_test(NAME cpp-pointer.tests COMMAND cpp-pointer_tests)
