
// #define EXERCISE1, EXERCISE2 or EXERCISE3
#define EXERCISE2

#ifdef EXERCISE1

// mutex_basics.cpp
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

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
        std::cout << "[" << std::this_thread::get_id() << "] " << message << "\n";
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
        threads.emplace_back([&account]() { account.deposit(100); });
        threads.emplace_back([&account]() { account.withdraw(50); });
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

#elif defined(EXERCISE2)

// lock_granularity.cpp
#include <chrono>
#include <iostream>
#include <mutex>
#include <ranges>
#include <thread>
#include <vector>

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
    explicit FineGrainedData(size_t size) : data_(size), mutexes_(size) {}

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

    size_t size() const {
        return mutexes_.size();
    }
};

// TODO: Benchmark both approaches
void benchmark_lock_granularity() {
    const int num_threads = 100;
    const std::vector<int> vec(std::views::iota(0, 101).begin(), std::views::iota(0, 101).end());

    // Version 1: Coarse-grained locking (one big lock)
    {
        CoarseGrainedData CGData;
        for (auto v : vec) {
            CGData.add(v);
        }
        auto start = std::chrono::steady_clock::now();
        {
            std::vector<std::thread> thrds;
            for (int i = 0; i < num_threads; i++) {
                thrds.emplace_back([&CGData]() { CGData.process(); });
            }
            for (auto& t : thrds) {
                t.join();
            }
        }
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Coarse-grained: " << duration.count() << "ms\n";
    }

    // Version 2: Fine-grained locking (multiple locks)
    {
        FineGrainedData FGData(vec.size());
        for (size_t i = 0; i < vec.size(); i++) {
            FGData.update(i, vec[i]);
        }
        auto start = std::chrono::steady_clock::now();
        {
            std::vector<std::thread> thrds;
            for (int j = 0; j < num_threads; j++) {
                thrds.emplace_back([&FGData]() {
                    for (size_t k = 0; k < FGData.size(); k++) {
                        FGData.process(k);
                    }
                });
            }
            for (auto& t : thrds) {
                t.join();
            }
        }
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Fine-grained: " << duration.count() << "ms\n";
    }
}

int main() {
    benchmark_lock_granularity();
}
// Questions to answer:
// 1. Which is faster with many threads?
//      - Coarse-grained: 169ms
//      - Fine-grained : 24ms
// 2. Which is simpler to implement correctly?
//.     - Coarse-grained
// 3. What if critical sections are very short?
// 4. What if critical sections are very long?
//      - The ratio matters!
//          - Lock overhead = Time to acquire + release lock (~100-1000ns)
//          - Work time = Time spent doing actual work inside lock

#elif defined(EXERCISE3)

// mutex_mistakes.cpp
// Each function has a mutex-related bug. Find and fix them!

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

// Mistake 1: Forgetting to lock
class Mistake1 {
    int data_ = 0;
    std::mutex mutex_;

public:
    void update() {
        std::lock_guard<std::mutex> lock(mutex_);  // fixed
        data_++;                                   // TODO: What's wrong? (No lock!)
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

    // fixed
    void update_data1() {
        std::lock_guard<std::mutex> lock(mutex1_);
        data1_++;
    }

    void update_data2() {
        std::lock_guard<std::mutex> lock(mutex2_);
        data2_++;
    }
};

// Mistake 3: Holding lock too long
class Mistake3 {
    int data_ = 0;
    std::mutex mutex_;

public:
    void update() {
        int local_copy;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            local_copy = ++data_;
        }

        // TODO: What's wrong with this?
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Updated data to " << local_copy << "\n";
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
#endif