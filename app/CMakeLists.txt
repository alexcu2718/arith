# Declare an executable target
add_executable(cpp-pointer_app)

# Add source files to the executable target
target_sources(cpp-pointer_app
  PRIVATE
    cpp-pointer_app.cpp
)

# Link the executable against the library
target_link_libraries(cpp-pointer_app
  PRIVATE
    cpp-pointer
)
