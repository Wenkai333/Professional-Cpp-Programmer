# Main executable
add_executable(main main.cpp)

# Daily practice executable
#add_executable(day1 Day1.cpp)
#add_executable(day2 Day2.cpp)
add_executable(day3 Day3.cpp)

# Link threading library
target_link_libraries(main PRIVATE Threads::Threads)