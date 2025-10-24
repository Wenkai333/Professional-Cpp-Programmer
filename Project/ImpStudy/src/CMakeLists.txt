# Main executable
add_executable(main main.cpp)

# Add the RAII practice executable
add_executable(raii_practice raii_practice.cpp)

# Add the smart pointer practice executable
add_executable(smart_pointer_practice smartPointer_practice.cpp)

# Add the allocators practice executable
add_executable(allocators_practice allocators_practice.cpp)

# Link threading library
target_link_libraries(main PRIVATE Threads::Threads)
