# Find the Catch2 package
find_package(Catch2 3 REQUIRED)

# Include Catch2's CMake helper functions (e.g., catch_discover_tests).
include(Catch)

# Create a single test binary that compiles all test files.
add_executable(cpp-pointer_tests)

# Add the implementation file
target_sources(cpp-pointer_tests
  PRIVATE
    cpp-pointer_t.cpp
)

# Link the test binary
target_link_libraries(cpp-pointer_tests
  PUBLIC
    cpp-pointer
    Catch2::Catch2WithMain
)

# allow user to run tests with `make test` or `ctest`
catch_discover_tests(cpp-pointer_tests
  TEST_PREFIX "cpp-pointer."
)
