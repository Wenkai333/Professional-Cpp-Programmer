# Week 1-2: C++ Threading Fundamentals & Race Conditions
## Complete Study Plan with Resources (3-4 hours/day, 14 days)

---

## 📋 Table of Contents

- [Overview](#overview)
- [Setup & Prerequisites](#setup--prerequisites)
- [Day 1: Thread Creation & Lifecycle](#day-1-thread-creation--lifecycle)
- [Day 2: Understanding Race Conditions](#day-2-understanding-race-conditions)
- [Day 3: Mutex Basics & Critical Sections](#day-3-mutex-basics--critical-sections)
- [Day 4: Advanced Locking & unique_lock](#day-4-advanced-locking--unique_lock)
- [Day 5: Thread-Safe Data Structures](#day-5-thread-safe-data-structures)
- [Day 6-7: Weekend Project](#day-6-7-weekend-project)
- [Days 8-14: Week 2 Preview](#days-8-14-week-2-preview)
- [Essential Resources Library](#essential-resources-library)
- [Progress Tracking](#progress-tracking)

---

## 📊 Overview

**Duration:** 14 days (3-4 hours each)  
**Split:** 40% Theory, 60% Hands-on Coding  
**Prerequisites:** RAII, Smart Pointers, and Memory Management (your completed work)  
**Tools:** ThreadSanitizer (TSan), GDB/LLDB, VS Code setup

### Learning Objectives

By the end of Week 1-2, you will:
- ✅ Create and manage threads safely
- ✅ Understand what causes race conditions and data races
- ✅ Use mutex and lock_guard correctly
- ✅ Apply RAII principles to thread synchronization
- ✅ Debug concurrent code with ThreadSanitizer
- ✅ Know when NOT to use multithreading

---

## 🛠️ Setup & Prerequisites

### Setup Checklist

Before starting Day 1:

```bash
# 1. Verify ThreadSanitizer works
cat > test_tsan.cpp << 'EOF'
#include <thread>
int x = 0;
int main() {
    std::thread t1([](){ x++; });
    std::thread t2([](){ x++; });
    t1.join(); t2.join();
}
EOF

clang++ -std=c++20 -g -fsanitize=thread test_tsan.cpp -o test_tsan
./test_tsan
# Should report data race!

# 2. Create project structure
mkdir -p ~/concurrency-study/week1-2/{day1,day2,day3,day4,day5,weekend}
cd ~/concurrency-study/week1-2

# 3. Create build script
cat > build.sh << 'EOF'
#!/bin/bash
FILE=$1
clang++ -std=c++20 -Wall -Wextra -g \
    -fsanitize=thread \
    -pthread \
    ${FILE} -o ${FILE%.cpp}
EOF
chmod +x build.sh

# 4. Usage
./build.sh day1/basic_threads.cpp
./day1/basic_threads
```

### Essential Tools Documentation

**ThreadSanitizer:**
- Official Docs: https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual
- Clang Docs: https://clang.llvm.org/docs/ThreadSanitizer.html
- Usage Guide: https://developer.android.com/ndk/guides/tsan

**GDB/LLDB for Threading:**
- GDB Thread Commands: https://sourceware.org/gdb/current/onlinedocs/gdb/Threads.html
- LLDB Tutorial: https://lldb.llvm.org/use/tutorial.html

---

## Day 1: Thread Creation & Lifecycle (3.5 hours)

### 📚 Theory (1.5 hours)

#### Core Concepts

```cpp
// What is a thread?
// - Separate execution path within a process
// - Shares memory with other threads (this is both power and danger)
// - Has its own stack, but shares heap
// - Managed by OS scheduler

// Thread States:
// Created → Ready → Running → Blocked → Terminated
```

**Key Questions to Answer:**
1. Why do we need threads? (parallelism vs concurrency)
2. What's the difference between process and thread?
3. What resources do threads share vs not share?
4. What happens if we don't join or detach?

#### Required Reading

1. **CppReference - std::thread** (20 mins)
   - https://en.cppreference.com/w/cpp/thread/thread
   - Focus on: Constructor, join, detach, joinable

2. **"C++ Threading Tutorial"** (30 mins)
   - https://www.modernescpp.com/index.php/c-core-guidelines-taking-care-of-your-child/
   - Comprehensive thread basics

3. **Alternative Tutorial** (20 mins)
   - https://www.bogotobogo.com/cplusplus/multithreaded4_cplusplus11.php
   - Practical examples

#### Video Resources

1. **🎥 CppCon 2017: "C++ Concurrency in Action" by Anthony Williams** (25 mins)
   - https://www.youtube.com/watch?v=Xz5x8IwmFj4
   - Focus: First 25 minutes on thread basics
   - **Why:** Author of the definitive book on C++ concurrency

2. **🎥 "C++ Threading #1: Introduction" by The Cherno** (20 mins)
   - https://www.youtube.com/watch?v=wXBcwQwctlY
   - **Why:** Clear, visual explanation with simple examples

#### Optional Deep Dive

3. **🎥 CppCon 2020: "Back to Basics: Concurrency" by Arthur O'Dwyer** (60 mins)
   - https://www.youtube.com/watch?v=F6Ipn7gCOsY
   - **Why:** Best "Back to Basics" talk on concurrency

#### Quick References

- **std::thread cheat sheet**: https://github.com/AnthonyCalandra/modern-cpp-features#stdthread
- **Hardware concurrency**: https://en.cppreference.com/w/cpp/thread/thread/hardware_concurrency

---

### 💻 Hands-On (2 hours)

#### Exercise 1.1: Basic Thread Creation

```cpp
// basic_threads.cpp
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>

// TODO: Implement a function that prints numbers
void print_numbers(int id, int count) {
    // Print numbers from 1 to count
    // Include thread id in output
    // Add small delay between prints to see interleaving
}

// TODO: Implement a function that takes parameters by reference
void increment_counter(int& counter, int iterations) {
    // Increment counter 'iterations' times
    // WARNING: This will have a race condition!
}

int main() {
    std::cout << "Hardware concurrency: " 
              << std::thread::hardware_concurrency() << "\n\n";
    
    // TODO 1: Create a single thread running print_numbers
    // - Join it
    // - Observe output
    
    // TODO 2: Create 3 threads running print_numbers concurrently
    // - Store in std::vector<std::thread>
    // - Join all of them
    // - Observe interleaved output
    
    // TODO 3: Try NOT joining a thread (comment out join)
    // - Compile and run
    // - What happens? Why?
    // - Read the error message carefully
    
    // TODO 4: Try detaching a thread instead
    // - What's the difference?
    // - When would you use detach?
    
    // TODO 5: Demonstrate the race condition
    int counter = 0;
    // Create 10 threads, each incrementing counter 10000 times
    // Print final counter value
    // Run multiple times - is it always the same?
    
    return 0;
}
```

#### Exercise 1.2: Thread RAII Wrapper

```cpp
// thread_guard.cpp
#include <iostream>
#include <thread>
#include <stdexcept>

// TODO: Implement a RAII wrapper for std::thread
// This is analogous to your FileHandle from RAII exercises!
class thread_guard {
private:
    std::thread& thread_;
    
public:
    // TODO: Constructor takes thread reference
    explicit thread_guard(std::thread& t) : thread_(t) {
        // Store the thread reference
    }
    
    // TODO: Destructor joins the thread if joinable
    ~thread_guard() {
        // If thread is joinable, join it
        // This prevents the std::terminate problem
        // Print message when joining
    }
    
    // TODO: Delete copy operations (why?)
    thread_guard(const thread_guard&) = delete;
    thread_guard& operator=(const thread_guard&) = delete;
    
    // TODO: BONUS - Implement move operations
};

void may_throw(int id) {
    if (id == 3) {
        throw std::runtime_error("Thread 3 throws!");
    }
    std::cout << "Thread " << id << " completed\n";
}

int main() {
    // TODO: Test thread_guard with normal execution
    
    // TODO: Test thread_guard with exception
    // Without thread_guard, exception causes std::terminate
    // With thread_guard, thread is properly joined via RAII
    
    return 0;
}
```

#### Compile and Test

```bash
# Normal compilation
clang++ -std=c++20 -Wall -Wextra -g basic_threads.cpp -o basic_threads
./basic_threads

# With ThreadSanitizer (you'll see race condition detected!)
clang++ -std=c++20 -g -fsanitize=thread basic_threads.cpp -o basic_threads_tsan
./basic_threads_tsan
```

**Expected Observations:**
1. Thread output is interleaved (non-deterministic)
2. Counter race condition gives wrong results
3. TSan reports the data race with line numbers
4. thread_guard prevents std::terminate

---

### ✅ Daily Checkpoint

- [ ] Can create and join threads
- [ ] Understand thread lifecycle
- [ ] Implemented thread_guard RAII wrapper
- [ ] Witnessed your first data race!
- [ ] Used ThreadSanitizer successfully

**Time spent:** ___ hours  
**Difficulties:**  
**Questions:**

---

## Day 2: Understanding Race Conditions (3.5 hours)

### 📚 Theory (1.5 hours)

#### Critical Concepts

```cpp
// Data Race vs Race Condition
// 
// Data Race: Two threads access same memory, at least one writes, no synchronization
// Result: UNDEFINED BEHAVIOR
//
// Race Condition: Outcome depends on timing/ordering of events
// Result: Incorrect behavior, but DEFINED
//
// All data races are bugs. Not all race conditions are data races.

// Example of Data Race (undefined behavior):
int counter = 0;
// Thread 1: counter++;  (read-modify-write)
// Thread 2: counter++;  (read-modify-write)
// Result: UB! Could be 1 or 2, could crash, could format your hard drive

// Example of Race Condition without Data Race:
std::mutex m;
bool ready = false;
// Thread 1: { lock(m); ready = true; unlock(m); }
// Thread 2: { lock(m); if(ready) { ... } unlock(m); }
// Result: Defined behavior, but outcome depends on which thread runs first
```

**Key Questions:**
1. Why is `counter++` not atomic?
2. What does "undefined behavior" really mean in concurrent code?
3. How does compiler optimization make races worse?
4. What is "memory visibility" between threads?

#### Required Reading

1. **CppReference - Memory Model** (20 mins)
   - https://en.cppreference.com/w/cpp/language/memory_model
   - Focus: Data races section

2. **"What Every Programmer Should Know About Memory"** (25 mins)
   - https://www.akkadia.org/drepper/cpumemory.pdf (Section 6)
   - Or simplified: https://lwn.net/Articles/250967/
   - Focus: Cache coherency and why races are bad

3. **Microsoft Docs: "Data Races and Race Conditions"** (15 mins)
   - https://docs.microsoft.com/en-us/cpp/parallel/concurrency-runtime/best-practices-in-the-concurrency-runtime#data-races
   - Clear definitions with examples

#### Video Resources

1. **🎥 CppCon 2016: "The Speed of Concurrency" by Fedor Pikus** (30 mins)
   - https://www.youtube.com/watch?v=9hJkWwHDDxs
   - Focus: First 30 minutes on data races
   - **Why:** Shows EXACTLY what happens at hardware level

2. **🎥 "C++ Threading #2: Race Condition and Mutex" by The Cherno** (18 mins)
   - https://www.youtube.com/watch?v=3ZxZPeXPaM4
   - **Why:** Visual demonstration of race conditions

#### Optional Deep Dive

3. **🎥 CppCon 2017: "Is Parallel Programming Still Hard?" by Paul McKenney** (60 mins)
   - https://www.youtube.com/watch?v=YM8Xy6oKVQg
   - **Why:** Deep understanding of why concurrent programming is hard

#### Interactive Resources

- **Race Condition Visualizer**: https://software.intel.com/content/www/us/en/develop/articles/use-thread-checker-to-find-threading-errors.html
- **Compiler Explorer with TSan**: https://godbolt.org/ (use `-fsanitize=thread`)

---

### 💻 Hands-On (2 hours)

#### Exercise 2.1: Race Condition Hunting

```cpp
// race_conditions.cpp
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

// TODO: Implement these functions with intentional race conditions
// Then you'll fix them in later exercises

// Race 1: Lost Updates
class BankAccount {
    int balance_ = 1000;
    
public:
    // TODO: Implement deposit (has race condition!)
    void deposit(int amount) {
        // Read balance
        int temp = balance_;
        // Add amount
        temp += amount;
        // Simulate some processing
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        // Write back
        balance_ = temp;
        // Print transaction
        std::cout << "Deposited " << amount << ", balance: " << balance_ << "\n";
    }
    
    // TODO: Implement withdraw (has race condition!)
    void withdraw(int amount) {
        // Read balance
        int temp = balance_;
        // Subtract amount
        temp -= amount;
        // Simulate some processing
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        // Write back
        balance_ = temp;
        // Print transaction
        std::cout << "Withdrew " << amount << ", balance: " << balance_ << "\n";
    }
    
    int balance() const { return balance_; }
};

// Race 2: Use-After-Free with Threads
void demonstrate_use_after_free() {
    int* ptr = new int(42);
    
    // TODO: Create thread that uses ptr after 100ms delay
    std::thread reader([ptr]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Reading: " << *ptr << "\n";  // Might be freed!
    });
    
    // TODO: Delete ptr immediately
    delete ptr;
    
    reader.join();
    // What went wrong? How does TSan detect this?
}

// Race 3: Double-Checked Locking (Broken Pattern)
class Singleton {
    static Singleton* instance_;
    
    Singleton() = default;
    
public:
    // TODO: Implement getInstance with broken double-checked locking
    static Singleton* getInstance() {
        if (instance_ == nullptr) {  // Check 1 (no lock)
            // TODO: What happens if two threads get here?
            instance_ = new Singleton();
        }
        return instance_;
    }
};
Singleton* Singleton::instance_ = nullptr;

// Race 4: Torn Reads/Writes
struct Point {
    double x, y;  // Each double is 8 bytes
};

void demonstrate_torn_write() {
    Point p{0.0, 0.0};
    
    // TODO: Writer thread - continuously writes {1.0, 1.0}
    std::thread writer([&p]() {
        for (int i = 0; i < 100000; ++i) {
            p = Point{1.0, 1.0};
        }
    });
    
    // TODO: Reader thread - reads and checks consistency
    std::thread reader([&p]() {
        for (int i = 0; i < 100000; ++i) {
            Point local = p;
            if (local.x != local.y) {
                // Torn read! Saw partial update
                std::cout << "Inconsistent! x=" << local.x 
                         << " y=" << local.y << "\n";
            }
        }
    });
    
    writer.join();
    reader.join();
}

int main() {
    std::cout << "=== Race Condition Examples ===\n\n";
    
    // TODO 1: Test BankAccount race
    // - Create account with 1000 balance
    // - Spawn 10 threads, each deposits 100
    // - Spawn 10 threads, each withdraws 50
    // - Expected final balance: 1000 + 1000 - 500 = 1500
    // - Actual balance: varies! (race condition)
    // - Run with TSan
    
    // TODO 2: Test use-after-free
    // demonstrate_use_after_free();
    // - Run with TSan
    // - Observe the thread safety warning
    
    // TODO 3: Test singleton race
    // - Create 100 threads calling getInstance()
    // - Print how many unique instances created
    // - Should be 1, might be more!
    
    // TODO 4: Test torn reads
    // demonstrate_torn_write();
    // - On some systems you'll see inconsistent state
    // - On others, alignment might prevent it
    
    return 0;
}
```

#### Exercise 2.2: Race Condition Analysis

```cpp
// analyze_races.cpp
// For each code snippet, identify:
// 1. Is there a data race?
// 2. Is there a race condition?
// 3. What could go wrong?
// 4. How would you fix it? (you'll implement fixes later)

// Snippet 1:
std::vector<int> data;
void thread1() { data.push_back(1); }
void thread2() { data.push_back(2); }
// Analysis: ?

// Snippet 2:
int x = 0;
void thread1() { x = 1; }
void thread2() { int y = x; }
// Analysis: ?

// Snippet 3:
std::atomic<bool> flag{false};
int value = 0;
void thread1() { value = 42; flag.store(true); }
void thread2() { if (flag.load()) { int x = value; } }
// Analysis: ? (Tricky!)

// TODO: Write your analysis in comments
// TODO: Run with TSan to verify your analysis
// TODO: Document what TSan reports
```

#### Debugging Session

```bash
# Compile with TSan
clang++ -std=c++20 -g -O1 -fsanitize=thread race_conditions.cpp -o races

# Run multiple times
for i in {1..10}; do
    echo "Run $i:"
    ./races
    echo "---"
done

# TSan will report:
# - Exact line numbers of conflicting accesses
# - Stack traces for both threads
# - Whether it's a read-write or write-write conflict
```

---

### ✅ Daily Checkpoint

- [ ] Understand difference between data race and race condition
- [ ] Can identify races by reading code
- [ ] Have witnessed multiple types of races
- [ ] Comfortable using ThreadSanitizer
- [ ] Understand why races cause undefined behavior

**Time spent:** ___ hours  
**Difficulties:**  
**Questions:**

---

## Day 3: Mutex Basics & Critical Sections (4 hours)

### 📚 Theory (1.5 hours)

#### Core Concepts

```cpp
// Mutex = Mutual Exclusion
// Purpose: Protect shared data from simultaneous access
// Mechanism: Only one thread can "own" the mutex at a time

// Critical Section = Code that accesses shared data
// Rule: Only ONE thread can be in a critical section at a time

// RAII for Mutexes:
// std::mutex + std::lock_guard = Perfect match!
// Just like your FileHandle + RAII

// Mental Model:
// Mutex is like a bathroom key
// - Only one person can use the bathroom (hold the lock)
// - Others wait until key is available
// - RAII ensures you always return the key (even with exceptions)
```

**Key Concepts:**
1. What is mutual exclusion?
2. What is a critical section?
3. Why RAII is critical for locks
4. Performance cost of locking
5. Lock contention and scalability

#### Required Reading

1. **CppReference - std::mutex** (15 mins)
   - https://en.cppreference.com/w/cpp/thread/mutex

2. **CppReference - std::lock_guard** (10 mins)
   - https://en.cppreference.com/w/cpp/thread/lock_guard

3. **"RAII and Locking" by Herb Sutter** (20 mins)
   - https://herbsutter.com/2013/05/13/gotw-91-solution-smart-pointer-parameters/
   - Related RAII principles you already know!

4. **ModernEscpp: "The Risks of Mutexes"** (20 mins)
   - https://www.modernescpp.com/index.php/the-risks-of-mutexes/
   - Common mistakes and how to avoid them

#### Video Resources

1. **🎥 CppCon 2016: "The Exception Situation" by Patrice Roy** (20 mins)
   - https://www.youtube.com/watch?v=Oy-VTqz1_58
   - Focus: Exception safety with mutexes (30 mins mark)
   - **Why:** Shows why RAII is critical for locks

2. **🎥 "C++ Threading #3: Locking and Deadlock" by Bo Qian** (15 mins)
   - https://www.youtube.com/watch?v=3ZxZPeXPaM4
   - **Why:** Clear explanation of mutex basics

3. **🎥 CppCon 2015: "Thinking Asynchronously" by Tony Van Eerd** (25 mins)
   - https://www.youtube.com/watch?v=5S7tTcBkMY0
   - Focus: First 25 mins on synchronization primitives
   - **Why:** Great mental models for thinking about synchronization

#### Interactive Learning

- **Deadlock Empire Game**: https://deadlockempire.github.io/
  - Interactive game to understand mutexes
  - Play levels 1-3 today

---

### 💻 Hands-On (2.5 hours)

#### Exercise 3.1: Fixing Race Conditions with Mutex

```cpp
// mutex_basics.cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

// TODO: Fix the BankAccount from Day 2
class ThreadSafeBankAccount {
private:
    int balance_;
    mutable std::mutex mutex_;  // Add mutex to protect balance_
    
public:
    explicit ThreadSafeBankAccount(int initial) : balance_(initial) {}
    
    // TODO: Implement thread-safe deposit
    void deposit(int amount) {
        std::lock_guard<std::mutex> lock(mutex_);
        balance_ += amount;
        std::cout << "Deposited " << amount << ", balance: " << balance_ << "\n";
    }
    
    // TODO: Implement thread-safe withdraw
    void withdraw(int amount) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (balance_ >= amount) {
            balance_ -= amount;
            std::cout << "Withdrew " << amount << ", balance: " << balance_ << "\n";
        }
    }
    
    // TODO: Implement thread-safe balance getter
    int balance() const {
        // Need to lock even for reading!
        // But mutex_ is const? Need mutable!
        std::lock_guard<std::mutex> lock(mutex_);
        return balance_;
    }
};

// TODO: Implement a thread-safe counter
class Counter {
private:
    int count_ = 0;
    mutable std::mutex mutex_;
    
public:
    // TODO: Implement increment
    void increment() {
        std::lock_guard<std::mutex> lock(mutex_);
        ++count_;
        // lock automatically released when lock_guard destructor runs
    }
    
    // TODO: Implement decrement
    void decrement() {
        std::lock_guard<std::mutex> lock(mutex_);
        --count_;
    }
    
    // TODO: Implement get
    int get() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_;
    }
};

// TODO: Implement a thread-safe logger
class Logger {
private:
    mutable std::mutex mutex_;
    
public:
    // TODO: Thread-safe log function
    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "[" << std::this_thread::get_id() << "] " 
                  << message << "\n";
        // Without mutex, output gets interleaved/garbled
    }
};

// Demonstrate correct usage
void test_thread_safe_account() {
    ThreadSafeBankAccount account(1000);
    std::vector<std::thread> threads;
    
    // TODO: Create threads that deposit and withdraw
    // - Should get correct final balance now
    // - Run with TSan - no warnings!
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&account]() {
            account.deposit(100);
        });
        threads.emplace_back([&account]() {
            account.withdraw(50);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Final balance: " << account.balance() << "\n";
}

int main() {
    // TODO: Test all implementations
    // TODO: Verify no TSan warnings
    // TODO: Compare performance with unsafe version
    
    test_thread_safe_account();
    
    return 0;
}
```

#### Exercise 3.2: Lock Granularity Experiments

```cpp
// lock_granularity.cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

// Version 1: Coarse-grained locking (one big lock)
class CoarseGrainedData {
    std::vector<int> data_;
    mutable std::mutex mutex_;
    
public:
    // TODO: Lock for entire operation
    void add(int value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.push_back(value);
    }
    
    void process() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& val : data_) {
            val *= 2;  // Expensive work
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.size();
    }
};

// Version 2: Fine-grained locking (multiple locks)
class FineGrainedData {
    std::vector<int> data_;
    std::vector<std::mutex> mutexes_;  // One mutex per element!
    
public:
    explicit FineGrainedData(size_t size) 
        : data_(size), mutexes_(size) {}
    
    // TODO: Lock only specific element
    void update(size_t index, int value) {
        if (index < mutexes_.size()) {
            std::lock_guard<std::mutex> lock(mutexes_[index]);
            data_[index] = value;
        }
    }
    
    void process(size_t index) {
        if (index < mutexes_.size()) {
            std::lock_guard<std::mutex> lock(mutexes_[index]);
            data_[index] *= 2;
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    }
};

// TODO: Benchmark both approaches
// Questions to answer:
// 1. Which is faster with many threads?
// 2. Which is simpler to implement correctly?
// 3. What if critical sections are very short?
// 4. What if critical sections are very long?
```

#### Exercise 3.3: Common Mutex Mistakes

```cpp
// mutex_mistakes.cpp
// Each function has a mutex-related bug. Find and fix them!

// Mistake 1: Forgetting to lock
class Mistake1 {
    int data_ = 0;
    std::mutex mutex_;
    
public:
    void update() {
        data_++;  // TODO: What's wrong? (No lock!)
    }
};

// Mistake 2: Locking wrong mutex
class Mistake2 {
    int data1_ = 0;
    int data2_ = 0;
    std::mutex mutex1_;
    std::mutex mutex2_;
    
public:
    void update1() {
        std::lock_guard<std::mutex> lock(mutex1_);
        data1_++;
        data2_++;  // TODO: What's wrong? (Wrong mutex!)
    }
};

// Mistake 3: Holding lock too long
class Mistake3 {
    int data_ = 0;
    std::mutex mutex_;
    
public:
    void update() {
        std::lock_guard<std::mutex> lock(mutex_);
        data_++;
        
        // TODO: What's wrong with this?
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Updated data to " << data_ << "\n";
    }
};

// Mistake 4: Returning reference to protected data
class Mistake4 {
    int data_ = 0;
    mutable std::mutex mutex_;
    
public:
    int& get() {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_;  // TODO: What's wrong? (Lock released, reference still live!)
    }
};

// Mistake 5: Exception safety
class Mistake5 {
    int data_ = 0;
    std::mutex mutex_;
    
public:
    void update() {
        mutex_.lock();  // TODO: What's wrong? (No RAII!)
        
        if (data_ > 100) {
            throw std::runtime_error("Too large!");  // Mutex never unlocked!
        }
        
        data_++;
        mutex_.unlock();
    }
};

// TODO: Fix all mistakes
// TODO: Write test cases that expose the bugs
// TODO: Explain why each is wrong in comments
```

---

### ✅ Daily Checkpoint

- [ ] Can use std::mutex and std::lock_guard correctly
- [ ] Understand RAII applies to locks
- [ ] Can identify protected vs unprotected data
- [ ] Fixed all race conditions from Day 2
- [ ] Understand lock granularity tradeoffs

**Time spent:** ___ hours  
**Difficulties:**  
**Questions:**

---

## Day 4: Advanced Locking & unique_lock (3.5 hours)

### 📚 Theory (1.5 hours)

#### Core Concepts

```cpp
// std::lock_guard vs std::unique_lock
//
// lock_guard:
// - Simple, lightweight
// - Locks on construction, unlocks on destruction
// - Cannot unlock early
// - Cannot transfer ownership
//
// unique_lock:
// - More flexible
// - Can lock/unlock multiple times
// - Can defer locking
// - Can transfer ownership (movable)
// - Slightly more overhead
//
// Rule of thumb:
// - Default to lock_guard (simpler, faster)
// - Use unique_lock when you need flexibility

// When to use unique_lock:
// 1. Need to unlock before end of scope
// 2. Condition variables (required!)
// 3. Need to move lock ownership
// 4. Deferred locking
```

**Key Questions:**
1. When do you need unique_lock over lock_guard?
2. What does "deferred locking" mean?
3. How does unique_lock enable condition variables?
4. What's the performance difference?

#### Required Reading

1. **CppReference - std::unique_lock** (20 mins)
   - https://en.cppreference.com/w/cpp/thread/unique_lock

2. **CppReference - std::lock** (10 mins)
   - https://en.cppreference.com/w/cpp/thread/lock

3. **CppReference - std::scoped_lock (C++17)** (10 mins)
   - https://en.cppreference.com/w/cpp/thread/scoped_lock

4. **"Avoid Deadlocks"** (20 mins)
   - https://www.modernescpp.com/index.php/avoid-deadlocks/

5. **"unique_lock vs lock_guard"** (15 mins)
   - https://stackoverflow.com/questions/20516773/stdunique-lockstdmutex-or-stdlock-guardstdmutex
   - Read top 3 answers thoroughly

#### Video Resources

1. **🎥 CppCon 2017: "Postmodern Immutable Data Structures"** (25 mins)
   - https://www.youtube.com/watch?v=sPhpelUfu8Q
   - Focus: Locking strategies section (around 20 min mark)
   - **Why:** Shows when you DON'T need complex locking

2. **🎥 "Deadlock Explained with Dining Philosophers"** (10 mins)
   - https://www.youtube.com/watch?v=FYQlvpao8fc
   - **Why:** Classic visualization of deadlock problem

3. **🎥 CppCon 2015: "Live Lock-Free or Deadlock" by Fedor Pikus** (30 mins)
   - https://www.youtube.com/watch?v=lVBvHbJsg5Y
   - Focus: Deadlock prevention strategies (first 30 mins)
   - **Why:** Practical deadlock prevention techniques

#### Interactive Learning

- **Deadlock Empire Game**: https://deadlockempire.github.io/
  - Play levels 4-8 to understand deadlocks
  - Time: 20 mins

---

### 💻 Hands-On (2 hours)

#### Exercise 4.1: unique_lock Basics

```cpp
// unique_lock_basics.cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

class Database {
private:
    std::mutex mutex_;
    int query_count_ = 0;
    
public:
    // TODO: Implement with unique_lock allowing early unlock
    void execute_query() {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // Fast operation under lock
        query_count_++;
        int local_count = query_count_;
        
        // Unlock early for expensive work
        lock.unlock();
        
        // Expensive operation WITHOUT holding lock
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Query " << local_count << " completed\n";
        
        // Can relock if needed
        lock.lock();
        // Do more work under lock
        std::cout << "Finalizing query " << local_count << "\n";
    }
    
    // TODO: Implement deferred locking
    void conditional_update(bool should_update) {
        std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
        
        // Do some work without lock
        std::cout << "Preparing update...\n";
        
        if (should_update) {
            lock.lock();  // Lock only if needed
            query_count_++;
            std::cout << "Updated count to " << query_count_ << "\n";
        }
        
        // lock automatically unlocks in destructor (if locked)
    }
    
    // TODO: Implement with try_lock
    bool try_execute_query() {
        std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);
        
        if (lock.owns_lock()) {
            // Got the lock!
            query_count_++;
            std::cout << "Acquired lock, count: " << query_count_ << "\n";
            return true;
        } else {
            // Couldn't get lock, do something else
            std::cout << "Could not acquire lock\n";
            return false;
        }
    }
};

// TODO: Demonstrate unique_lock's movability
std::unique_lock<std::mutex> get_lock(std::mutex& m) {
    std::unique_lock<std::mutex> lock(m);
    std::cout << "Lock acquired in function\n";
    // Do some work
    return lock;  // Move lock ownership out
}

int main() {
    Database db;
    
    // TODO: Test all Database methods
    std::thread t1([&db]() { db.execute_query(); });
    std::thread t2([&db]() { db.conditional_update(true); });
    std::thread t3([&db]() { db.try_execute_query(); });
    
    t1.join();
    t2.join();
    t3.join();
    
    // TODO: Demonstrate lock transfer
    std::mutex m;
    auto lock = get_lock(m);
    std::cout << "Lock transferred to main\n";
    
    return 0;
}
```

#### Exercise 4.2: Locking Multiple Mutexes

```cpp
// multiple_mutexes.cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

class Account {
private:
    int balance_;
    std::mutex mutex_;
    
public:
    explicit Account(int balance) : balance_(balance) {}
    
    std::mutex& get_mutex() { return mutex_; }
    
    void withdraw(int amount) { balance_ -= amount; }
    void deposit(int amount) { balance_ += amount; }
    int balance() const { return balance_; }
};

// TODO: Implement WRONG transfer (causes deadlock!)
void transfer_deadlock_version(Account& from, Account& to, int amount) {
    std::lock_guard<std::mutex> lock1(from.get_mutex());
    std::this_thread::sleep_for(std::chrono::milliseconds(1));  // Increase chance of deadlock
    std::lock_guard<std::mutex> lock2(to.get_mutex());
    
    from.withdraw(amount);
    to.deposit(amount);
    
    // Deadlock scenario:
    // Thread 1: transfer(A, B, 100) - locks A, waits for B
    // Thread 2: transfer(B, A, 50)  - locks B, waits for A
    // Both wait forever!
}

// TODO: Implement CORRECT transfer using std::lock
void transfer_correct_version(Account& from, Account& to, int amount) {
    // std::lock locks multiple mutexes atomically
    // (all or nothing - prevents deadlock)
    std::lock(from.get_mutex(), to.get_mutex());
    
    // Now adopt the locks into lock_guards for RAII
    std::lock_guard<std::mutex> lock1(from.get_mutex(), std::adopt_lock);
    std::lock_guard<std::mutex> lock2(to.get_mutex(), std::adopt_lock);
    
    from.withdraw(amount);
    to.deposit(amount);
    
    std::cout << "Transferred " << amount << " from account to account\n";
}

// TODO: C++17 version using scoped_lock (simplest!)
void transfer_cpp17_version(Account& from, Account& to, int amount) {
    // std::scoped_lock locks multiple mutexes AND provides RAII
    // One-liner solution!
    std::scoped_lock lock(from.get_mutex(), to.get_mutex());
    
    from.withdraw(amount);
    to.deposit(amount);
    
    std::cout << "Transferred " << amount << " (C++17 style)\n";
}

// TODO: Test deadlock scenario
void test_deadlock() {
    std::cout << "Testing deadlock scenario (might hang!)...\n";
    Account alice(1000);
    Account bob(1000);
    
    std::vector<std::thread> threads;
    
    // Create transfers in both directions
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            transfer_deadlock_version(alice, bob, 10);
        });
        threads.emplace_back([&]() {
            transfer_deadlock_version(bob, alice, 10);
        });
    }
    
    for (auto& t : threads) {
        t.join();  // This might never complete! (deadlock)
    }
}

void test_correct_version() {
    std::cout << "\nTesting correct version...\n";
    Account alice(1000);
    Account bob(1000);
    
    std::vector<std::thread> threads;
    
    // Create transfers in both directions
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            transfer_correct_version(alice, bob, 10);
        });
        threads.emplace_back([&]() {
            transfer_correct_version(bob, alice, 10);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Alice balance: " << alice.balance() << "\n";
    std::cout << "Bob balance: " << bob.balance() << "\n";
}

int main() {
    // TODO: Try test_deadlock() - it might hang!
    // test_deadlock();
    
    // TODO: Then test correct version
    test_correct_version();
    
    return 0;
}
```

#### Exercise 4.3: Lock Timing and try_lock

```cpp
// lock_timing.cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>

class Resource {
private:
    std::mutex mutex_;
    int usage_count_ = 0;
    
public:
    // TODO: Try to acquire lock, give up if takes too long
    bool try_use_with_timeout() {
        std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
        
        // Try to lock for up to 100ms
        if (lock.try_lock_for(std::chrono::milliseconds(100))) {
            usage_count_++;
            std::cout << "Acquired lock, usage: " << usage_count_ << "\n";
            
            // Simulate work
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            return true;
        } else {
            std::cout << "Timeout waiting for lock\n";
            return false;
        }
    }
    
    // TODO: Try to acquire lock until specific time
    bool try_use_until(std::chrono::steady_clock::time_point deadline) {
        std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
        
        if (lock.try_lock_until(deadline)) {
            usage_count_++;
            std::cout << "Acquired lock before deadline\n";
            return true;
        } else {
            std::cout << "Deadline passed\n";
            return false;
        }
    }
    
    // TODO: Non-blocking attempt
    bool try_use_nowait() {
        std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);
        
        if (lock.owns_lock()) {
            usage_count_++;
            std::cout << "Immediately acquired lock\n";
            return true;
        } else {
            std::cout << "Lock busy, skipping\n";
            return false;
        }
    }
};

int main() {
    Resource resource;
    std::vector<std::thread> threads;
    
    // TODO: Test different timing strategies
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&resource]() {
            resource.try_use_with_timeout();
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
```

---

### ✅ Daily Checkpoint

- [ ] Understand when to use unique_lock vs lock_guard
- [ ] Can use deferred locking and early unlock
- [ ] Know how to lock multiple mutexes safely
- [ ] Understand deadlock causes and prevention
- [ ] Can use try_lock family of functions

**Time spent:** ___ hours  
**Difficulties:**  
**Questions:**

---

## Day 5: Thread-Safe Data Structures (4 hours)

### 📚 Theory (1.5 hours)

#### Core Concepts

```cpp
// Thread-Safe Data Structure Design Principles:
//
// 1. Identify critical sections
//    - What data is shared?
//    - What operations modify it?
//
// 2. Minimize lock scope
//    - Hold locks only as long as necessary
//    - Unlock before expensive operations
//
// 3. Const correctness matters!
//    - Even const methods need locks if data is mutable
//
// 4. Exception safety
//    - Use RAII for locks
//    - Leave data in valid state
//
// 5. Avoid returning references/pointers to protected data
//    - Lock doesn't protect after return!
```

**Key Questions:**
1. What makes a data structure thread-safe?
2. How to design interface for thread safety?
3. What are the performance tradeoffs?
4. When is lock-free better? (Spoiler: rarely for beginners!)

#### Required Reading

1. **"Thread-Safe Data Structures" by Anthony Williams** (20 mins)
   - https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html

2. **"The Problem with Locks"** (15 mins)
   - https://www.modernescpp.com/index.php/the-problem-with-locks/

3. **CppReference - std::stack/queue** (10 mins)
   - Review interface design
   - https://en.cppreference.com/w/cpp/container/stack

4. **"Exception Safety and Concurrency"** (20 mins)
   - https://www.boost.org/doc/libs/1_76_0/libs/exception/doc/exception_safety.html
   - Focus on concurrent contexts

#### Video Resources

1. **🎥 CppCon 2014: "Lock-Free Programming Part 2"** (20 mins)
   - https://www.youtube.com/watch?v=CmxkPChOcvw
   - Focus: First 20 mins showing why lock-based is often better
   - **Why:** Appreciate simplicity of lock-based structures

2. **🎥 "Producer Consumer Problem" by Neso Academy** (15 mins)
   - https://www.youtube.com/watch?v=Qx3P2wazwI0
   - **Why:** Classic concurrency pattern explained clearly

3. **🎥 CppCon 2016: "A Lock-Free Concurrency Toolkit"** (15 mins)
   - https://www.youtube.com/watch?v=FhzV_5ZdB0w
   - Just introduction - to see what advanced looks like!

#### Case Studies

- **Boost.Lockfree Documentation**: https://www.boost.org/doc/libs/1_76_0/doc/html/lockfree.html
  - Read "Rationale" section
  - Time: 15 mins

---

### 💻 Hands-On (2.5 hours)

#### Exercise 5.1: Thread-Safe Stack

```cpp
// threadsafe_stack.cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <stack>
#include <stdexcept>
#include <memory>
#include <vector>

// TODO: Implement a thread-safe stack
template<typename T>
class ThreadSafeStack {
private:
    std::stack<T> data_;
    mutable std::mutex mutex_;  // Mutable so const methods can lock
    
public:
    ThreadSafeStack() = default;
    
    // TODO: Copy constructor (need to lock source!)
    ThreadSafeStack(const ThreadSafeStack& other) {
        std::lock_guard<std::mutex> lock(other.mutex_);
        data_ = other.data_;
    }
    
    // Disable assignment (complex to implement correctly)
    ThreadSafeStack& operator=(const ThreadSafeStack&) = delete;
    
    // TODO: Implement push
    void push(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.push(std::move(value));
    }
    
    // TODO: Implement pop that returns value
    // Why not return T directly? (Exception safety!)
    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (data_.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        
        // Make shared_ptr to return value before modifying stack
        auto result = std::make_shared<T>(std::move(data_.top()));
        data_.pop();
        return result;
    }
    
    // TODO: Implement pop into reference
    void pop(T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (data_.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        
        value = std::move(data_.top());
        data_.pop();
    }
    
    // TODO: Implement empty check
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.empty();
    }
    
    // TODO: Implement size
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.size();
    }
};

// TODO: Test the thread-safe stack
void test_threadsafe_stack() {
    ThreadSafeStack<int> stack;
    std::vector<std::thread> threads;
    
    // TODO: Create producer threads
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&stack, i]() {
            for (int j = 0; j < 100; ++j) {
                stack.push(i * 100 + j);
            }
        });
    }
    
    // TODO: Create consumer threads
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&stack]() {
            for (int j = 0; j < 100; ++j) {
                try {
                    auto value = stack.pop();
                    // Process value
                } catch (const std::runtime_error&) {
                    // Stack was empty
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Final stack size: " << stack.size() << "\n";
}

int main() {
    test_threadsafe_stack();
    return 0;
}
```

#### Exercise 5.2: Thread-Safe Queue

```cpp
// threadsafe_queue.cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <memory>
#include <vector>
#include <atomic>

// TODO: Implement a thread-safe queue
template<typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex mutex_;
    std::queue<T> data_;
    
public:
    ThreadSafeQueue() = default;
    
    // TODO: Implement push
    void push(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.push(std::move(value));
    }
    
    // TODO: Implement try_pop (non-blocking)
    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (data_.empty()) {
            return std::shared_ptr<T>();  // nullptr
        }
        
        auto result = std::make_shared<T>(std::move(data_.front()));
        data_.pop();
        return result;
    }
    
    // TODO: Implement try_pop into reference
    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (data_.empty()) {
            return false;
        }
        
        value = std::move(data_.front());
        data_.pop();
        return true;
    }
    
    // TODO: Implement empty
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.empty();
    }
    
    // TODO: Implement size
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.size();
    }
};

// TODO: Test producer-consumer pattern
void test_producer_consumer() {
    ThreadSafeQueue<int> queue;
    std::atomic<bool> done{false};
    
    // Producer threads
    std::vector<std::thread> producers;
    for (int i = 0; i < 3; ++i) {
        producers.emplace_back([&queue, i]() {
            for (int j = 0; j < 100; ++j) {
                queue.push(i * 100 + j);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }
    
    // Consumer threads
    std::vector<std::thread> consumers;
    for (int i = 0; i < 3; ++i) {
        consumers.emplace_back([&queue, &done]() {
            while (!done || !queue.empty()) {
                int value;
                if (queue.try_pop(value)) {
                    // Process value
                    std::cout << "Consumed: " << value << "\n";
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        });
    }
    
    // Wait for producers
    for (auto& t : producers) {
        t.join();
    }
    
    // Drain queue
    while (!queue.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    done = true;
    
    // Wait for consumers
    for (auto& t : consumers) {
        t.join();
    }
}

int main() {
    test_producer_consumer();
    return 0;
}
```

#### Exercise 5.3: Interface Design Discussion

```cpp
// interface_problems.cpp

// Problem 1: Why is this interface problematic?
class BadThreadSafeVector {
private:
    std::vector<int> data_;
    mutable std::mutex mutex_;
    
public:
    // Returns reference to internal data - NOT SAFE!
    int& operator[](size_t index) {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_[index];  // Lock released, but reference still live!
    }
    
    // Returns iterator to internal data - NOT SAFE!
    auto begin() {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.begin();  // Same problem!
    }
};

// Problem 2: TOCTOU (Time-of-check Time-of-use)
class BadThreadSafeMap {
private:
    std::map<int, std::string> data_;
    mutable std::mutex mutex_;
    
public:
    bool contains(int key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.find(key) != data_.end();
    }
    
    std::string get(int key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.at(key);  // Might throw if key doesn't exist
    }
    
    // Problem: User checks contains(), then calls get()
    // Between the two calls, another thread might erase the key!
};

// TODO: Fix both classes
// TODO: Design better interfaces
// TODO: Document the problems in comments

// Better approach for Problem 2:
class GoodThreadSafeMap {
private:
    std::map<int, std::string> data_;
    mutable std::mutex mutex_;
    
public:
    // Atomic check-and-get operation
    std::optional<std::string> try_get(int key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = data_.find(key);
        if (it != data_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};
```

---

### ✅ Daily Checkpoint

- [ ] Can implement thread-safe stack
- [ ] Can implement thread-safe queue
- [ ] Understand interface design for thread safety
- [ ] Know why returning references is dangerous
- [ ] Understand TOCTOU bugs

**Time spent:** ___ hours  
**Difficulties:**  
**Questions:**

---

## Day 6-7: Weekend Project (6-8 hours total)

### 📚 Project Resources

#### Task Queue Systems

1. **"Thread Pools Explained"** (20 mins)
   - https://www.modernescpp.com/index.php/threadpools/

2. **"Work-Stealing Queue"** (30 mins)
   - https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/

3. **CppReference - std::priority_queue** (10 mins)
   - https://en.cppreference.com/w/cpp/container/priority_queue

#### Video Resources

1. **🎥 CppCon 2016: "A Work-Stealing Job System" by Sean Parent** (60 mins)
   - https://www.youtube.com/watch?v=zULU6Hhp42w
   - Full walkthrough of professional task system
   - **Why:** Industry-standard design patterns

2. **🎥 "Task Scheduling in Games" by GDC** (40 mins)
   - https://www.youtube.com/watch?v=1KZghhYP6OI
   - **Why:** Real-world requirements and solutions

3. **🎥 CppCon 2015: "How to make your data structures wait-free"** (60 mins)
   - https://www.youtube.com/watch?v=tSqvOz1m9-Q
   - Optional - shows advanced techniques

#### Reference Implementations

- **Intel TBB Task Scheduler**: https://spec.oneapi.io/versions/latest/elements/oneTBB/source/task_scheduler.html
  - Study the design
  - Time: 30 mins

---

### 💻 Project: Multi-threaded Task Queue System

**Goal:** Build a realistic concurrent system combining all Week 1 concepts

#### Requirements

```cpp
// task_queue_system.cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <vector>
#include <atomic>
#include <chrono>

// 1. Task representation
struct Task {
    int id;
    std::function<void()> work;
    int priority;  // Higher = more important
    
    // For priority queue - lower priority value = higher priority
    bool operator<(const Task& other) const {
        return priority < other.priority;
    }
};

// 2. Thread-safe priority queue
template<typename T>
class ThreadSafePriorityQueue {
private:
    std::priority_queue<T> queue_;
    mutable std::mutex mutex_;
    
public:
    // TODO: Implement push
    void push(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(value));
    }
    
    // TODO: Implement try_pop
    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        value = std::move(const_cast<T&>(queue_.top()));
        queue_.pop();
        return true;
    }
    
    // TODO: Implement empty
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    
    // TODO: Implement size
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
};

// 3. Worker thread pool
class TaskQueue {
private:
    ThreadSafePriorityQueue<Task> tasks_;
    std::vector<std::thread> workers_;
    std::atomic<bool> stop_{false};
    std::atomic<int> completed_tasks_{0};
    std::atomic<int> total_tasks_{0};
    
public:
    // TODO: Constructor spawns N worker threads
    explicit TaskQueue(size_t num_threads) {
        std::cout << "Creating task queue with " << num_threads << " workers\n";
        
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this, i]() {
                worker_thread(i);
            });
        }
    }
    
    // TODO: Destructor stops workers gracefully
    ~TaskQueue() {
        std::cout << "Shutting down task queue...\n";
        stop_ = true;
        
        for (auto& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        
        std::cout << "Task queue shutdown complete\n";
        std::cout << "Total tasks: " << total_tasks_ << "\n";
        std::cout << "Completed: " << completed_tasks_ << "\n";
    }
    
    // TODO: Add task to queue
    void submit(Task task) {
        tasks_.push(std::move(task));
        total_tasks_++;
    }
    
    // TODO: Get statistics
    int completed_count() const {
        return completed_tasks_;
    }
    
    int pending_count() const {
        return tasks_.size();
    }
    
private:
    // TODO: Worker thread function
    void worker_thread(size_t worker_id) {
        std::cout << "Worker " << worker_id << " started\n";
        
        while (!stop_) {
            Task task;
            if (tasks_.try_pop(task)) {
                std::cout << "Worker " << worker_id 
                         << " executing task " << task.id 
                         << " (priority " << task.priority << ")\n";
                
                try {
                    task.work();
                    completed_tasks_++;
                } catch (const std::exception& e) {
                    std::cerr << "Task " << task.id << " threw exception: " 
                             << e.what() << "\n";
                }
            } else {
                // No tasks available, sleep briefly
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        
        std::cout << "Worker " << worker_id << " stopped\n";
    }
};

// 4. Test with realistic scenarios
void test_task_queue() {
    TaskQueue queue(4);  // 4 worker threads
    
    std::cout << "\n=== Submitting tasks ===\n";
    
    // TODO: Submit 20 tasks with random priorities
    for (int i = 0; i < 20; ++i) {
        int priority = rand() % 10;
        
        Task task;
        task.id = i;
        task.priority = priority;
        task.work = [i, priority]() {
            // Simulate work
            std::this_thread::sleep_for(std::chrono::milliseconds(50 + rand() % 100));
            std::cout << "  Task " << i << " (priority " << priority << ") completed\n";
        };
        
        queue.submit(std::move(task));
    }
    
    std::cout << "\n=== Waiting for completion ===\n";
    
    // Wait for all tasks to complete
    while (queue.pending_count() > 0 || queue.completed_count() < 20) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Pending: " << queue.pending_count() 
                 << ", Completed: " << queue.completed_count() << "\n";
    }
    
    std::cout << "\n=== All tasks completed ===\n";
}

int main() {
    std::cout << "🎯 Multi-threaded Task Queue System\n\n";
    
    test_task_queue();
    
    std::cout << "\n✅ Test complete!\n";
    
    return 0;
}

// 5. BONUS: Add cancellation support
// 6. BONUS: Add task dependencies (task B waits for task A)
// 7. BONUS: Implement work stealing between threads
```

---

### 📝 Implementation Plan

#### Day 6 (3-4 hours)

1. **Implement ThreadSafePriorityQueue (1 hour)**
   - Start with basic queue operations
   - Add priority comparison
   - Test with simple tasks

2. **Basic TaskQueue with simple threads (1.5 hours)**
   - Create worker threads
   - Implement submit and worker_thread
   - Test with non-priority tasks

3. **Test with simple tasks (30 mins)**
   - Create 10-20 simple tasks
   - Verify they all execute
   - Check for race conditions with TSan

4. **Debug with TSan (30 mins)**
   - Fix any issues found
   - Verify clean TSan output

#### Day 7 (3-4 hours)

1. **Add priority handling (1 hour)**
   - Verify high-priority tasks execute first
   - Test with mixed priorities

2. **Add statistics tracking (1 hour)**
   - Track completed vs total tasks
   - Add timing statistics
   - Print summary at end

3. **Implement graceful shutdown (1 hour)**
   - Finish current tasks before stopping
   - Don't accept new tasks during shutdown
   - Join all threads properly

4. **Performance tuning (1 hour)**
   - Measure throughput
   - Experiment with thread count
   - Profile with different task types

5. **BONUS features (if time permits)**
   - Task cancellation support
   - Task dependencies (task B waits for task A)
   - Work stealing between threads

---

### ✅ Success Criteria

- [ ] All workers process tasks correctly
- [ ] No race conditions (TSan clean)
- [ ] Proper shutdown (no leaked threads)
- [ ] Higher priority tasks execute first
- [ ] Performance scales with thread count
- [ ] Handles exceptions in tasks gracefully

**Day 6 Time spent:** ___ hours  
**Day 7 Time spent:** ___ hours  
**Total project time:** ___ hours  
**Difficulties:**  
**Questions:**

---

## Days 8-14: Week 2 Preview

### Day 8-9: Atomic Operations Basics
- std::atomic<T> fundamentals
- Lock-free counters
- Compare-and-swap
- Basic memory ordering

### Day 10-11: Condition Variables
- Producer-consumer with blocking
- Wait, notify_one, notify_all
- Spurious wakeups
- Timeouts

### Day 12-13: Memory Ordering Introduction
- Sequential consistency
- Acquire-release semantics
- Understanding the memory model

### Day 14: Week 1-2 Comprehensive Project
- Build something using everything learned
- Performance benchmarking
- Code review and optimization

---

## 📚 Essential Resources Library

### 🎥 Complete Video Series

#### Must-Watch Series

1. **"C++ Concurrency in Action" by Bo Qian**
   - https://www.youtube.com/playlist?list=PL5jc9xFGsL8E12so1wlMS0r0hTQoJL74M
   - 17 videos, ~30 mins each
   - Watch 1-2 per day alongside exercises

2. **"C++ Weekly" by Jason Turner - Threading Episodes**
   - https://www.youtube.com/c/lefticus1/search?query=thread
   - Short 5-15 min episodes
   - Great for quick clarifications

3. **CppCon "Back to Basics" Series**
   - https://www.youtube.com/results?search_query=cppcon+back+to+basics+concurrency
   - Multiple years of foundational talks

### 📖 Books (Priority Order)

#### Essential

1. **"C++ Concurrency in Action" 2nd Edition - Anthony Williams**
   - THE definitive book
   - Chapters 1-4 during Week 1-2
   - https://www.manning.com/books/c-plus-plus-concurrency-in-action-second-edition

#### Supplementary

2. **"C++ High Performance" 2nd Edition - Andrist & Sehr**
   - Chapter 11 on Concurrency
   - Practical performance-oriented

3. **"The Art of Multiprocessor Programming" - Herlihy & Shavit**
   - Not C++-specific but excellent theory
   - Understanding universal principles

### 🔗 Interactive Learning

#### Visualizations

1. **ThreadSanitizer Online**: https://godbolt.org/
   - Use flags: `-fsanitize=thread -O1`
   - See TSan output immediately

2. **Deadlock Empire**: https://deadlockempire.github.io/
   - Game to learn mutexes and deadlocks
   - Play all levels during Week 1-2

3. **C++ Insights**: https://cppinsights.io/
   - See compiler-generated code

#### Documentation

1. **CppReference Concurrency**: https://en.cppreference.com/w/cpp/thread
   - Bookmark this!
   - Best C++ reference

2. **C++ Core Guidelines - Concurrency**:
   - https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-concurrency
   - Industry best practices

### 📱 YouTube Channels to Subscribe

1. **CppCon**: https://www.youtube.com/user/CppCon
2. **C++ Weekly**: https://www.youtube.com/c/lefticus1
3. **The Cherno**: https://www.youtube.com/c/TheChernoProject
4. **Bo Qian**: https://www.youtube.com/user/BoQianTheProgrammer
5. **Arthur O'Dwyer**: https://www.youtube.com/c/ArthurODwyer

---

## 📊 Progress Tracking

### Daily Log Template

Create `concurrency_progress.md`:

```markdown
# Concurrency Learning Progress

## Week 1-2: Fundamentals

### Day 1: Thread Basics (Date: ___)
**Resources Used:**
- [x] CppReference std::thread
- [x] CppCon 2017: Anthony Williams
- [x] The Cherno Threading #1
- [ ] Optional deep dive

**Understanding Check:**
- What happens if thread not joined? ✅ / ❓
- Why RAII for threads? ✅ / ❓
- Hardware concurrency meaning? ✅ / ❓

**Exercises Completed:**
- [x] basic_threads.cpp
- [x] thread_guard.cpp

**Time Spent:** ___ hours (Goal: 3-4)

**Key Takeaways:**
1. 
2. 
3. 

**Questions for Next Session:**
1. 
2. 

**TSan Findings:**
- Race condition in counter increment
- [Screenshot or description]

---

### Day 2: Race Conditions (Date: ___)
[Same format]

---

[Continue for each day]
```

### Weekly Review Template

```markdown
## Week 1 Review (Days 1-7)

**Overall Progress:**
- Completed: X/7 days
- Total time: XX hours
- Exercises finished: XX/XX

**Strongest Areas:**
1. 
2. 

**Areas Needing Review:**
1. 
2. 

**Next Week Focus:**
1. 
2. 

**Questions Remaining:**
1. 
2. 
```

---

## 💡 Learning Strategy Tips

### Video Watching Strategy

1. **First Pass (1.25x speed):**
   - Get overall understanding
   - Don't pause to code yet

2. **Second Pass (1x speed):**
   - Pause and take notes
   - Screenshot important slides
   - Write down questions

3. **Third Pass (while coding):**
   - Reference specific sections
   - Verify implementation

### Reading Strategy

1. **Skim first** (5 mins)
   - Structure and main points

2. **Deep read** (15-20 mins)
   - Take notes in own words
   - Draw diagrams

3. **Code examples** (10 mins)
   - Type out (don't copy-paste!)
   - Modify and experiment

### Daily Routine

**Morning (Theory - 1.5 hours):**
1. CppReference (15-20 mins)
2. One article (20-25 mins)
3. Video at 1.25-1.5x (25-30 mins)
4. Take notes (15-20 mins)

**Afternoon (Hands-On - 2-2.5 hours):**
1. First exercise (45-60 mins)
2. Second exercise (45-60 mins)
3. Experiment (30 mins)

**Evening (Optional - 30 mins):**
1. Supplementary video
2. Additional articles
3. Bonus exercises

---

## 🆘 When Resources Conflict

If different resources give different advice:

1. **Check CppReference** (source of truth)
2. **Check publication date** (newer often better)
3. **Check C++ version** (pre-C++11 is outdated)
4. **Ask me** with specific examples

**Example:**
> "Video X says to use pthread_create, but article Y uses std::thread. Which?"
> Answer: std::thread (C++11+), pthread is C API

---

## 🎓 Quality Resource Indicators

### ✅ High Quality
- CppCon/C++Now talks
- CppReference documentation
- "C++ Concurrency in Action" book
- Core Guidelines
- Anthony Williams, Herb Sutter, Scott Meyers

### ⚠️ Use with Caution
- Random blog posts (verify)
- StackOverflow (read multiple answers)
- Old content (pre-C++11)

### ❌ Avoid
- pthread directly (use std::thread)
- Windows-specific APIs (use standard)
- Pre-C++11 approaches

---

## ✨ Final Notes

### Remember

- **Theory + Practice**: 40/60 split
- **TSan is your friend**: Run it always
- **RAII everything**: Applies to locks too
- **Start simple**: Don't jump to lock-free
- **Ask questions**: Understanding > completion

### When You Need Help

Contact me when you:
1. Don't understand WHY (not just HOW)
2. Can't interpret TSan output
3. Hit a deadlock you can't debug
4. Have design questions
5. Want to discuss tradeoffs

### Daily Affirmation

> "Concurrent programming is hard. Every step forward is progress. Every bug found is learning. I'm building expertise systematically."

---

**Ready to start Day 1?** 🚀

Good luck with your concurrent programming journey!
