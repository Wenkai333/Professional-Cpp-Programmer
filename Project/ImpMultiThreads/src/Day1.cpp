
#define EXERCISE1
#ifdef EXERCISE1

// basic_threads.cpp
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// TODO: Implement a function that prints numbers
void print_numbers(int id, int count) {
    // Print numbers from 1 to count
    // Include thread id in output
    // Add small delay between prints to see interleaving
    using namespace std::literals::chrono_literals;
    for (int i = 1; i <= count; i++) {
        std::cout << "ThreadId: " << id << ", count: " << i << std::endl;
        std::this_thread::sleep_for(1s);
    }
}

// TODO: Implement a function that takes parameters by reference
void increment_counter(int& counter, int iterations) {
    // Increment counter 'iterations' times
    // WARNING: This will have a race condition!
    for (int i = 0; i < iterations; i++) {
        counter++;
    }
}

int main() {
    std::cout << "Hardware concurrency: " << std::thread::hardware_concurrency() << "\n\n";

    // TODO 1: Create a single thread running print_numbers
    // - Join it
    // - Observe output
    std::thread t1(print_numbers, 1, 10);
    t1.join();

    // TODO 2: Create 3 threads running print_numbers concurrently
    // - Store in std::vector<std::thread>
    // - Join all of them
    // - Observe interleaved output
    std::vector<std::thread> ths;
    ths.reserve(3);
    ths.emplace_back(std::thread(print_numbers, 1, 10));
    ths.emplace_back(std::thread(print_numbers, 2, 10));
    ths.emplace_back(std::thread(print_numbers, 3, 10));
    for (auto& t : ths) {
        t.join();
    }

    // TODO 3: Try NOT joining a thread (comment out join)
    // - Compile and run
    // - What happens? Why?
    // - Read the error message carefully
    std::thread t3(print_numbers, 1, 10);
    // t3.join(); // nothing happen, since the end of this main thread main destruct the thread,
    // then the sub-thread may be terminated.

    // TODO 4: Try detaching a thread instead
    // - What's the difference?
    // - When would you use detach?
    t3.detach();

    // TODO 5: Demonstrate the race condition
    // int counter = 0;
    // Create 10 threads, each incrementing counter 10000 times
    // Print final counter value
    // Run multiple times - is it always the same?
    //! Happened in TODO 2

    return 0;
}

#else

// thread_guard.cpp
#include <iostream>
#include <stdexcept>
#include <thread>

// TODO: Implement a RAII wrapper for std::thread
// This is analogous to your FileHandle from RAII EXERCISEs!
class thread_guard {
private:
    std::thread thread_;

public:
    // TODO: Constructor takes thread reference
    explicit thread_guard(std::thread& t) : thread_(std::move(t)) {
        std::cout << "Guard the ownership of the thread!" << std::endl;
    }

    // TODO: Destructor joins the thread if joinable
    ~thread_guard() {
        // If thread is joinable, join it
        // This prevents the std::terminate problem
        // Print message when joining
        if (thread_.joinable()) {
            thread_.join();
            std::cout << "thread is joined !" << std::endl;
        }
    }

    // TODO: Delete copy operations (why?)
    thread_guard(const thread_guard&) = delete;
    thread_guard& operator=(const thread_guard&) = delete;

    // TODO: BONUS - Implement move operations
    thread_guard(thread_guard&& other) : thread_(std::move(other.thread_)) {
        std::cout << "📦 thread_guard: Move constructor called\n";
    }
    thread_guard& operator=(thread_guard&& other) {
        if (this != &other) {
            // Join our current thread first
            if (thread_.joinable()) {
                thread_.join();
            }
            thread_ = std::move(other.thread_);
            std::cout << "📦 thread_guard: Move assignment called\n";
        }
        return *this;
    }

    std::thread* operator->() {
        return &thread_;
    }
};

void may_throw(int id) {
    if (id == 3) {
        throw std::runtime_error("Thread 3 throws!");
    }
    std::cout << "Thread " << id << " completed\n";
}

void long_running_task(int id) {
    using namespace std::literals::chrono_literals;
    for (int i = 0; i < 5; ++i) {
        std::cout << "Thread " << id << ": step " << i << "\n";
        std::this_thread::sleep_for(200ms);
    }
}

#endif