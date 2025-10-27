/*
 * Custom Allocators Hands-On Exercise
 * Performance-Critical Game Entity System
 *
 * INSTRUCTIONS:
 * 1. Implement all methods marked with "TODO"
 * 2. Follow the allocator interface requirements
 * 3. Pay attention to alignment and performance
 * 4. Run benchmarks to verify improvements
 * 5. Test with provided test cases
 *
 * Compile with:
 * clang++ -std=c++20 -Wall -Wextra -O2 custom_allocators_exercise.cpp -o allocators
 *
 * DIFFICULTY LEVELS:
 * ⭐ Basic - Simple pool allocator
 * ⭐⭐ Intermediate - Arena allocator with statistics
 * ⭐⭐⭐ Advanced - Multi-threaded pool allocator
 * 🌟 Expert - Polymorphic allocator wrapper
 */

#include <atomic>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <string>
#include <thread>
#include <vector>

// =============================================================================
// Exercise 1: ⭐ Basic Pool Allocator
// =============================================================================

/*
 * GOAL: Implement a pool allocator for fixed-size allocations
 *
 * Pool allocators are ideal for:
 * - Frequent allocations/deallocations of same-sized objects
 * - Game entities, particles, audio samples
 * - Linked lists, trees, graphs
 *
 * Performance target: 5-10x faster than default allocator
 */

template <typename T, size_t PoolSize = 1024>
class PoolAllocator {
private:
    // TODO: Define your data structures
    // Hints:
    // - Use a union for free list (stores either T or next pointer)
    // - Keep track of free blocks
    // - May need multiple pools if one fills up

    union Block {
        // TODO: Implement Block structure
        // When free: stores pointer to next free block
        // When allocated: stores actual T object
        T element;
        Block* next;
    };

    struct Pool {
        // TODO: Implement Pool structure
        // Should contain:
        // - Array of blocks
        // - Pointer to next vector<Block> blocks;
        static constexpr size_t blocks_per_pool = PoolSize / sizeof(Block);
        alignas(Block) char blocks[PoolSize];
        Pool* next;
    };

    struct PoolState {
        Block* free_list_ = nullptr;
        Pool* current_pool_ = nullptr;
        size_t total_allocated_ = 0;
        size_t total_deallocated_ = 0;

        ~PoolState() {
            while (current_pool_) {
                Pool* next = current_pool_->next;
                ::operator delete(current_pool_);
                current_pool_ = next;
            }

            // Print statistics
            std::cout << "🏊 PoolAllocator destroyed\n";
            std::cout << "   Allocated: " << total_allocated_ << "\n";
            std::cout << "   Deallocated: " << total_deallocated_ << "\n";

            // Check for leaks
            if (total_allocated_ != total_deallocated_) {
                std::cout << "⚠️  Memory leak: " << (total_allocated_ - total_deallocated_)
                          << " objects not freed!\n";
            }
        }
    };
    // TODO: Add member variables
    std::shared_ptr<PoolState> state_;

public:
    using value_type = T;

    // TODO: Implement constructor
    PoolAllocator() noexcept : state_(std::make_shared<PoolState>()) {
        std::cout << "🏊 PoolAllocator created for type: " << typeid(T).name() << "\n";
    }

    // TODO: Implement destructor
    ~PoolAllocator() = default;

    // TODO: Implement copy constructor (for rebinding)
    template <typename U>
    PoolAllocator(const PoolAllocator<U, PoolSize>&) noexcept
        : state_(std::make_shared<PoolState>()) {}

    // TODO: Implement allocate
    T* allocate(size_t n) {
        // Hints:
        // 1. If n != 1, fall back to ::operator new (pools are for single objects)
        // 2. If free_list_ is empty, create new pool (expand_pool())
        // 3. Pop a block from free list
        // 4. Update statistics
        // 5. Return pointer to block (cast appropriately)
        if (n != 1) {
            return static_cast<T*>(::operator new(n * sizeof(T)));
        }

        if (!state_->free_list_) {
            expand_pool();
        }

        Block* new_block = state_->free_list_;
        state_->free_list_ = state_->free_list_->next;
        state_->total_allocated_++;
        return reinterpret_cast<T*>(new_block);
    }

    // TODO: Implement deallocate
    void deallocate(T* ptr, size_t n) {
        // Hints:
        // 1. If n != 1, use ::operator delete
        // 2. Cast pointer to Block*
        // 3. Push block back onto free list
        // 4. Update statistics
        // 5. Don't actually free memory (reuse it!)

        if (n != 1) {
            ::operator delete(ptr);
            return;
        }
        Block* rm_block = reinterpret_cast<Block*>(ptr);
        state_->total_deallocated_++;
        rm_block->next = state_->free_list_;
        state_->free_list_ = rm_block;
    }

    // TODO: Implement rebind for different types
    template <typename U>
    struct rebind {
        using other = PoolAllocator<U, PoolSize>;
    };

    // TODO: Implement statistics methods
    size_t allocated_count() const {
        return state_->total_allocated_;
    }

    size_t deallocated_count() const {
        return state_->total_deallocated_;
    }

    size_t current_usage() const {
        return state_->total_allocated_ - state_->total_deallocated_;
    }

private:
    // TODO: Implement expand_pool helper
    void expand_pool() {
        // Hints:
        // 1. Allocate new Pool with ::operator new
        // 2. Link all blocks in pool to free list
        // 3. Link pool to existing pools (if tracking multiple)

        Pool* new_pool = static_cast<Pool*>(::operator new(sizeof(Pool)));
        new_pool->next = state_->current_pool_;
        state_->current_pool_ = new_pool;

        Block* blocks_ptr = reinterpret_cast<Block*>(state_->current_pool_->blocks);
        for (size_t i = 0; i < Pool::blocks_per_pool - 1; i++) {
            blocks_ptr[i].next = &blocks_ptr[i + 1];
        }
        blocks_ptr[Pool::blocks_per_pool - 1].next = state_->free_list_;
        state_->free_list_ = blocks_ptr;
    }
};

// TODO: Implement comparison operators (required for C++17+)
template <typename T, typename U, size_t PoolSize>
bool operator==(const PoolAllocator<T, PoolSize>&, const PoolAllocator<U, PoolSize>&) noexcept {
    return true;
}

template <typename T, typename U, size_t PoolSize>
bool operator!=(const PoolAllocator<T, PoolSize>&, const PoolAllocator<U, PoolSize>&) noexcept {
    return false;
}

// =============================================================================
// Exercise 2: ⭐⭐ Arena (Stack) Allocator
// =============================================================================

/*
 * GOAL: Implement an arena allocator for batch allocations
 *
 * Arena allocators are ideal for:
 * - Per-frame allocations in games
 * - Request-scoped allocations in servers
 * - Parsing/compilation temporary data
 *
 * Key feature: Deallocate everything at once (reset)
 */

class Arena {
private:
    // TODO: Add member variables
    // char* buffer_;        // Pointer to memory block
    // size_t size_;         // Total size
    // size_t offset_;       // Current allocation offset
    // size_t peak_usage_;   // Peak memory usage (statistics)

public:
    // TODO: Implement constructor
    explicit Arena(size_t size) {
        // Hints:
        // - Allocate buffer_ with new char[size]
        // - Initialize offset_ to 0
        // - Print creation message
    }

    // TODO: Implement destructor
    ~Arena() {
        // Hints:
        // - Delete buffer_
        // - Print destruction message with statistics
    }

    // Arena should not be copyable (it owns memory)
    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    // TODO: Implement move constructor and assignment if desired

    // TODO: Implement allocate with alignment
    void* allocate(size_t n, size_t alignment = alignof(std::max_align_t)) {
        // Hints:
        // 1. Calculate aligned offset using std::align
        // 2. Check if enough space available
        // 3. Update offset_
        // 4. Update peak_usage_
        // 5. Return pointer to allocated memory
        // 6. Throw std::bad_alloc if not enough space

        return nullptr;  // Replace this
    }

    // TODO: Implement reset (deallocate all at once)
    void reset() {
        // Hints:
        // - Set offset_ back to 0
        // - Print reset message with how much was used
    }

    // TODO: Implement statistics methods
    size_t used() const {
        // Return current offset_
        return 0;  // Replace this
    }

    size_t available() const {
        // Return size_ - offset_
        return 0;  // Replace this
    }

    size_t peak_usage() const {
        // Return peak_usage_
        return 0;  // Replace this
    }

    size_t total_size() const {
        // Return size_
        return 0;  // Replace this
    }
};

// TODO: Implement ArenaAllocator adapter (so it works with STL containers)
template <typename T>
class ArenaAllocator {
private:
    Arena* arena_;

public:
    using value_type = T;

    // TODO: Implement constructor
    explicit ArenaAllocator(Arena* arena) : arena_(arena) {
        // Just store the arena pointer
    }

    // TODO: Implement rebind constructor
    template <typename U>
    ArenaAllocator(const ArenaAllocator<U>& other) noexcept : arena_(other.get_arena()) {}

    // TODO: Implement allocate
    T* allocate(size_t n) {
        // Hints:
        // - Call arena_->allocate(n * sizeof(T), alignof(T))
        // - Cast result to T*

        return nullptr;  // Replace this
    }

    // TODO: Implement deallocate
    void deallocate(T*, size_t) noexcept {
        // Hint: No-op! Arena deallocates everything at once
    }

    // TODO: Implement rebind
    template <typename U>
    struct rebind {
        using other = ArenaAllocator<U>;
    };

    Arena* get_arena() const {
        return arena_;
    }

    // Make ArenaAllocator<U> a friend so it can access arena_
    template <typename U>
    friend class ArenaAllocator;
};

// TODO: Implement comparison operators
template <typename T, typename U>
bool operator==(const ArenaAllocator<T>& a, const ArenaAllocator<U>& b) noexcept {
    // Hint: Two arena allocators are equal if they use the same arena
    return false;  // Replace this
}

template <typename T, typename U>
bool operator!=(const ArenaAllocator<T>& a, const ArenaAllocator<U>& b) noexcept {
    return false;  // Replace this
}

// =============================================================================
// Exercise 3: ⭐⭐⭐ Thread-Safe Pool Allocator
// =============================================================================

/*
 * GOAL: Make the pool allocator thread-safe for concurrent allocations
 *
 * Requirements:
 * - Multiple threads can allocate/deallocate concurrently
 * - Use fine-grained locking or lock-free techniques
 * - Maintain performance under contention
 */

template <typename T, size_t PoolSize = 1024>
class ThreadSafePoolAllocator {
private:
    // TODO: Add thread-safety primitives
    // Options:
    // 1. std::mutex for simple locking
    // 2. std::atomic for lock-free free list
    // 3. Thread-local pools for best performance

    union Block {
        // TODO: Implement like PoolAllocator
    };

    struct Pool {
        // TODO: Implement like PoolAllocator
    };

    // TODO: Add member variables with synchronization
    // std::mutex mutex_;  // or std::atomic<Block*> for lock-free
    // Block* free_list_;
    // Pool* pools_;
    // std::atomic<size_t> allocated_;
    // std::atomic<size_t> deallocated_;

public:
    using value_type = T;

    // TODO: Implement thread-safe constructor
    ThreadSafePoolAllocator() noexcept {}

    // TODO: Implement thread-safe destructor
    ~ThreadSafePoolAllocator() {}

    // TODO: Implement thread-safe allocate
    T* allocate(size_t n) {
        // Hints:
        // Option 1 (Simple): Lock mutex, do allocation, unlock
        // Option 2 (Advanced): Use atomic CAS for lock-free allocation

        return nullptr;  // Replace this
    }

    // TODO: Implement thread-safe deallocate
    void deallocate(T* ptr, size_t n) {
        // Hints:
        // Option 1 (Simple): Lock mutex, return to free list, unlock
        // Option 2 (Advanced): Use atomic CAS for lock-free deallocation
    }

    template <typename U>
    struct rebind {
        using other = ThreadSafePoolAllocator<U, PoolSize>;
    };

private:
    // TODO: Implement thread-safe expand_pool
    void expand_pool() {
        // Hint: Must be called while holding lock (or use double-checked locking)
    }
};

// =============================================================================
// Exercise 4: 🌟 BONUS - Tracking Allocator (Debugging)
// =============================================================================

/*
 * GOAL: Create a wrapper allocator that tracks all allocations
 *
 * Useful for:
 * - Finding memory leaks
 * - Profiling memory usage
 * - Understanding allocation patterns
 */

template <typename T, typename BaseAllocator = std::allocator<T>>
class TrackingAllocator {
private:
    BaseAllocator base_;

    // TODO: Add static tracking variables
    // static inline std::atomic<size_t> total_allocated_{0};
    // static inline std::atomic<size_t> total_freed_{0};
    // static inline std::atomic<size_t> allocation_count_{0};
    // static inline std::atomic<size_t> deallocation_count_{0};
    // static inline std::atomic<size_t> current_usage_{0};
    // static inline std::atomic<size_t> peak_usage_{0};

public:
    using value_type = T;

    TrackingAllocator() noexcept = default;

    template <typename U>
    TrackingAllocator(const TrackingAllocator<U, BaseAllocator>&) noexcept {}

    // TODO: Implement allocate with tracking
    T* allocate(size_t n) {
        // Hints:
        // 1. Call base_.allocate(n)
        // 2. Update all statistics
        // 3. Print allocation info (optional, can be verbose)
        // 4. Return pointer

        return nullptr;  // Replace this
    }

    // TODO: Implement deallocate with tracking
    void deallocate(T* ptr, size_t n) {
        // Hints:
        // 1. Update statistics
        // 2. Print deallocation info (optional)
        // 3. Call base_.deallocate(ptr, n)
    }

    template <typename U>
    struct rebind {
        using other = TrackingAllocator<U, BaseAllocator>;
    };

    // TODO: Implement static method to print statistics
    static void print_stats() {
        // Print all tracking statistics:
        // - Total allocated
        // - Total freed
        // - Current usage
        // - Peak usage
        // - Allocation count
        // - Average allocation size
        // - Potential leaks
    }

    // TODO: Implement static method to reset statistics
    static void reset_stats() {}
};

// =============================================================================
// Test Data Structures
// =============================================================================

// Simple game entity for testing
struct Entity {
    int id;
    float x, y, z;
    float velocity_x, velocity_y;
    int health;

    Entity(int i = 0) : id(i), x(0), y(0), z(0), velocity_x(0), velocity_y(0), health(100) {}
};

// Particle for performance testing
struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float life;
    int color;
};

// =============================================================================
// Benchmark Functions
// =============================================================================

template <typename Allocator>
void benchmark_list_operations(const std::string& name, int iterations = 100000) {
    using namespace std::chrono;

    auto start = high_resolution_clock::now();

    {
        std::list<int, Allocator> my_list;

        // Allocations
        for (int i = 0; i < iterations; ++i) {
            my_list.push_back(i);
        }

        // Mixed operations
        for (int i = 0; i < iterations / 2; ++i) {
            my_list.pop_front();
        }

        for (int i = 0; i < iterations / 2; ++i) {
            my_list.push_back(i);
        }

        // Deallocations happen here when list is destroyed
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    std::cout << name << ": " << duration.count() << " ms\n";
}

template <typename Allocator>
void benchmark_vector_of_entities(const std::string& name, int count = 10000) {
    using namespace std::chrono;

    auto start = high_resolution_clock::now();

    {
        std::vector<Entity, Allocator> entities;
        entities.reserve(count);

        for (int i = 0; i < count; ++i) {
            entities.emplace_back(i);
        }

        // Simulate updates
        for (auto& e : entities) {
            e.x += e.velocity_x;
            e.y += e.velocity_y;
        }
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    std::cout << name << ": " << duration.count() << " μs\n";
}

/*
void benchmark_arena_pattern(int frames = 1000) {
    using namespace std::chrono;

    std::cout << "\n=== Arena Pattern Benchmark ===\n";

    // Without arena (default allocator)
    auto start = high_resolution_clock::now();
    {
        for (int frame = 0; frame < frames; ++frame) {
            std::vector<Particle> particles;
            particles.resize(10000);

            // Process particles...
            for (auto& p : particles) {
                p.x += p.vx;
                p.y += p.vy;
                p.z += p.vz;
                p.life -= 0.016f;
            }
            // Vector destroyed, memory freed
        }
    }
    auto mid = high_resolution_clock::now();

    // With arena
    {
        Arena arena(10000 * sizeof(Particle) * 2);  // Enough for all frames

        for (int frame = 0; frame < frames; ++frame) {
            std::vector<Particle, ArenaAllocator<Particle>> particles{&arena};
            particles.resize(10000);

            // Process particles...
            for (auto& p : particles) {
                p.x += p.vx;
                p.y += p.vy;
                p.z += p.vz;
                p.life -= 0.016f;
            }

            arena.reset();  // Fast deallocation!
        }
    }
    auto end = high_resolution_clock::now();

    auto default_time = duration_cast<milliseconds>(mid - start);
    auto arena_time = duration_cast<milliseconds>(end - mid);

    std::cout << "Default allocator: " << default_time.count() << " ms\n";
    std::cout << "Arena allocator: " << arena_time.count() << " ms\n";
    std::cout << "Speedup: " << (double)default_time.count() / arena_time.count() << "x\n";
}
*/

// =============================================================================
// Test Functions
// =============================================================================

void test_pool_allocator() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "TEST 1: ⭐ Pool Allocator\n";
    std::cout << std::string(60, '=') << "\n";

    // TODO: Uncomment when implemented
    {
        std::list<Entity, PoolAllocator<Entity>> entity_list;

        // Create entities
        for (int i = 0; i < 100; ++i) {
            entity_list.emplace_back(i);
        }

        std::cout << "Created 100 entities\n";

        // Remove some
        for (int i = 0; i < 50; ++i) {
            entity_list.pop_front();
        }

        std::cout << "Removed 50 entities\n";

        // Add more (should reuse freed memory)
        for (int i = 100; i < 150; ++i) {
            entity_list.emplace_back(i);
        }

        std::cout << "Added 50 more entities (reused memory)\n";

        // Print statistics
        PoolAllocator<Entity> alloc;
        std::cout << "Total allocated: " << alloc.allocated_count() << "\n";
        std::cout << "Total deallocated: " << alloc.deallocated_count() << "\n";
        std::cout << "Current usage: " << alloc.current_usage() << "\n";
    }

    std::cout << "\n✅ Pool Allocator test complete!\n";

    // std::cout << "❌ Implement PoolAllocator first!\n";
}

void test_arena_allocator() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "TEST 2: ⭐⭐ Arena Allocator\n";
    std::cout << std::string(60, '=') << "\n";

    // TODO: Uncomment when implemented
    /*
    {
        Arena arena(1024 * 1024);  // 1MB arena

        // First batch of allocations
        {
            std::vector<int, ArenaAllocator<int>> vec(&arena);
            vec.resize(1000);

            std::list<Entity, ArenaAllocator<Entity>> entities(&arena);
            for (int i = 0; i < 100; ++i) {
                entities.emplace_back(i);
            }

            std::cout << "Arena used: " << arena.used() << " bytes\n";
            std::cout << "Arena available: " << arena.available() << " bytes\n";
        }

        std::cout << "\nResetting arena...\n";
        arena.reset();

        // Second batch (reuses same memory)
        {
            std::vector<Particle, ArenaAllocator<Particle>> particles(&arena);
            particles.resize(10000);

            std::cout << "After reset, arena used: " << arena.used() << " bytes\n";
        }

        std::cout << "Peak usage: " << arena.peak_usage() << " bytes\n";
    }

    std::cout << "\n✅ Arena Allocator test complete!\n";
    */

    std::cout << "❌ Implement Arena and ArenaAllocator first!\n";
}

void test_thread_safety() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "TEST 3: ⭐⭐⭐ Thread-Safe Pool Allocator\n";
    std::cout << std::string(60, '=') << "\n";

    // TODO: Uncomment when implemented
    /*
    {
        const int num_threads = 4;
        const int ops_per_thread = 10000;

        std::vector<std::thread> threads;

        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([ops_per_thread]() {
                std::list<int, ThreadSafePoolAllocator<int>> local_list;

                for (int i = 0; i < ops_per_thread; ++i) {
                    local_list.push_back(i);

                    if (i % 100 == 0) {
                        local_list.pop_front();
                    }
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        std::cout << "All threads completed successfully!\n";
    }

    std::cout << "\n✅ Thread-safe allocator test complete!\n";
    */

    std::cout << "❌ Implement ThreadSafePoolAllocator first!\n";
}

void run_benchmarks() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "BENCHMARKS\n";
    std::cout << std::string(60, '=') << "\n";

    std::cout << "\n--- List Operations Benchmark ---\n";
    benchmark_list_operations<std::allocator<int>>("Default allocator");
    // TODO: Uncomment when implemented
    // benchmark_list_operations<PoolAllocator<int>>("Pool allocator");

    std::cout << "\n--- Vector of Entities Benchmark ---\n";
    benchmark_vector_of_entities<std::allocator<Entity>>("Default allocator");

    // TODO: Uncomment when implemented
    // benchmark_arena_pattern();

    std::cout << "\n";
}

// =============================================================================
// Main - Test Runner
// =============================================================================

int main() {
    std::cout << "🧠 Custom Allocators Hands-On Exercise\n";
    std::cout << "Implement TODO methods and uncomment tests!\n";

    try {
        // Run tests
        test_pool_allocator();
        test_arena_allocator();
        test_thread_safety();

        // Run performance benchmarks
        run_benchmarks();

        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "🎯 Exercise Instructions:\n";
        std::cout << "1. Implement PoolAllocator (⭐)\n";
        std::cout << "2. Implement Arena and ArenaAllocator (⭐⭐)\n";
        std::cout << "3. Implement ThreadSafePoolAllocator (⭐⭐⭐)\n";
        std::cout << "4. BONUS: Implement TrackingAllocator (🌟)\n";
        std::cout << "5. Uncomment tests as you complete each exercise\n";
        std::cout << "6. Compare benchmark results with default allocator\n";
        std::cout << std::string(60, '=') << "\n";

    } catch (const std::exception& e) {
        std::cout << "\n❌ EXCEPTION: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

/*
 * 🎯 LEARNING CHECKLIST
 *
 * After completing this exercise, you should understand:
 *
 * ✅ Why custom allocators improve performance
 * ✅ How to implement the standard allocator interface
 * ✅ Pool allocator pattern for fixed-size allocations
 * ✅ Arena allocator pattern for batch deallocation
 * ✅ Thread-safety considerations for allocators
 * ✅ How to benchmark allocator performance
 * ✅ When to use custom allocators vs std::allocator
 * ✅ Memory alignment requirements
 * ✅ Allocator rebinding for containers
 * ✅ Statistics tracking for profiling
 *
 * 📚 BONUS CHALLENGES:
 * 1. Add memory debugging (guards, canaries)
 * 2. Implement small object allocator (multiple size classes)
 * 3. Add memory defragmentation to pool allocator
 * 4. Profile with Valgrind/AddressSanitizer
 * 5. Compare with std::pmr allocators (C++17)
 *
 * 💡 PERFORMANCE TIPS:
 * - Pools eliminate malloc/free overhead
 * - Arenas enable O(1) batch deallocation
 * - Thread-local pools reduce contention
 * - Alignment matters for SIMD performance
 * - Profile before and after optimization!
 */
