// race_conditions.cpp
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// TODO: Implement these functions with intentional race conditions
// Then you'll fix them in later exercises

// Race 1: Lost Updates
class BankAccount {
    std::mutex m;
    int balance_ = 1000;

public:
    // TODO: Implement deposit (has race condition!)
    void deposit(int amount) {
        m.lock();
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

        m.unlock();
    }

    // TODO: Implement withdraw (has race condition!)
    void withdraw(int amount) {
        m.lock();
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

        m.unlock();
    }

    int balance() const {
        return balance_;
    }
};

// Race 2: Use-After-Free with Threads
void demonstrate_use_after_free() {
    // int* ptr = new int(42);

    // // TODO: Create thread that uses ptr after 100ms delay
    // std::thread reader([ptr]() {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //     std::cout << "Reading: " << *ptr << "\n";  // Might be freed!
    // });

    // // TODO: Delete ptr immediately
    // delete ptr;

    //! correct
    auto ptr = std::make_shared<int>(42);

    std::thread reader([ptr]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Reading: " << *ptr << "\n";
    });

    reader.join();
    // What went wrong? How does TSan detect this?
}

// Race 3: Double-Checked Locking (Broken Pattern)
class Singleton {
    static Singleton* instance_;
    static std::once_flag flag_;  // ← Thread-safe initialization

    Singleton() = default;

public:
    // TODO: Implement getInstance with broken double-checked locking
    static Singleton* getInstance() {
        // if (instance_ == nullptr) {  // Check 1 (no lock)
        //     // TODO: What happens if two threads get here?
        //     instance_ = new Singleton();
        // }
        std::call_once(flag_, []() { instance_ = new Singleton(); });
        return instance_;
    }
};
Singleton* Singleton::instance_ = nullptr;
std::once_flag Singleton::flag_;

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
                std::cout << "Inconsistent! x=" << local.x << " y=" << local.y << "\n";
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
    {
        BankAccount b_acc;
        std::vector<std::thread> threads;
        for (int i = 0; i < 10; i++) {
            threads.emplace_back(&BankAccount::deposit, &b_acc, 100);
        }
        for (int i = 0; i < 10; i++) {
            threads.emplace_back(&BankAccount::withdraw, &b_acc, 50);
        }
        for (auto& t : threads) {
            t.join();
        }
        std::cout << "Test1: Balance: " << b_acc.balance() << std::endl;
    }

    // TODO 2: Test use-after-free
    demonstrate_use_after_free();
    // - Run with ASan
    // - Observe the thread safety warning

    // TODO 3: Test singleton race
    // - Create 100 threads calling getInstance()
    // - Print how many unique instances created
    // - Should be 1, might be more!
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < 100; i++) {
            threads.emplace_back(&Singleton::getInstance);
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    // TODO 4: Test torn reads
    // demonstrate_torn_write();
    // - On some systems you'll see inconsistent state
    // - On others, alignment might prevent it
    demonstrate_torn_write();

    return 0;
}