# Main executable
add_executable(main main.cpp)

# Daily practice executable
add_executable(day1 Day1.cpp)

# Link threading library
target_link_libraries(main PRIVATE Threads::Threads)