cmake_minimum_required(VERSION 3.23)

# Set a name and a version number for your project:
project(cpp-pointer
  VERSION 0.0.1
  LANGUAGES CXX
)

# Initialize some default paths
include(GNUInstallDirs)

# Define build options
option(cpp-pointer_BUILD_TESTING "Enable building of tests" OFF)

if(PROJECT_IS_TOP_LEVEL)
  option(cpp-pointer_BUILD_DOCS "Enable building of documentation" ON)
else()
  option(cpp-pointer_BUILD_DOCS "Enable building of documentation" OFF)
endif()

# Add an interface target for the header-only library
add_library(cpp-pointer INTERFACE)

# Add the public headers that belong to this library
target_sources(cpp-pointer
  INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
      include/cpp-pointer/cpp-pointer.hpp
)

# Request a minimum C++ standard for this target
target_compile_features(cpp-pointer
  INTERFACE
    cxx_std_23
)

if(PROJECT_IS_TOP_LEVEL)
  # Compile the application executable
  add_subdirectory(app)
endif()

if(PROJECT_IS_TOP_LEVEL)
  # Compile the tests
  include(CTest)
  if(cpp-pointer_BUILD_TESTING)
    add_subdirectory(tests)
  endif()
endif()

# Add the documentation
if(PROJECT_IS_TOP_LEVEL AND cpp-pointer_BUILD_DOCS)
  add_subdirectory(doc)
endif()

# Add an alias target for use if this project is included as a subproject in another project
add_library(cpp-pointer::cpp-pointer ALIAS cpp-pointer)

# Install the target and headers
install(
  TARGETS cpp-pointer
  EXPORT cpp-pointerTargets
  FILE_SET HEADERS
)

# Install the exported targets (this creates cpp-pointerTargets.cmake)
install(
  EXPORT cpp-pointerTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp-pointer
  NAMESPACE cpp-pointer::
)

# Include helpers for calling configure_package_config_file
include(CMakePackageConfigHelpers)

# Generate the cpp-pointerConfig.cmake file
configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/cpp-pointerConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cpp-pointerConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp-pointer
)

# Install the generated Config file
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-pointerConfig.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp-pointer
)

# Generate a version file
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/cpp-pointerConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# Install the version file
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-pointerConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp-pointer
)

# This prints a summary of found dependencies
include(FeatureSummary)
feature_summary(WHAT ALL)
