# Main executable
add_executable(main main.cpp)

# Link threading library
target_link_libraries(main PRIVATE Threads::Threads)
