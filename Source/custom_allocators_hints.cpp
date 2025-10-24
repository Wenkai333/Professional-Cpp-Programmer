/*
 * Custom Allocators Exercise - Solution Hints & Guidance
 * 
 * This file provides hints and partial implementations to help you
 * if you get stuck. Try to implement on your own first!
 */

#include <iostream>
#include <memory>
#include <new>
#include <cstddef>
#include <mutex>
#include <atomic>

// =============================================================================
// Exercise 1 Hints: Pool Allocator
// =============================================================================

/*
HINT 1: Block Union Structure
-----------------------------
The key insight is using a union to store either the object OR a pointer:

union Block {
    T element;       // When allocated: stores the actual object
    Block* next;     // When free: pointer to next free block
};

When free, the block stores a pointer to the next free block.
When allocated, the same memory holds the actual T object.
*/

/*
HINT 2: Free List Structure
---------------------------
Visual representation of free list:

Initial state (all free):
free_list_ -> [Block1] -> [Block2] -> [Block3] -> nullptr

After allocating one:
free_list_ -> [Block2] -> [Block3] -> nullptr
(Block1 is in use)

After deallocating Block1:
free_list_ -> [Block1] -> [Block2] -> [Block3] -> nullptr
*/

/*
HINT 3: Allocation Logic
------------------------
T* allocate(size_t n) {
    if (n != 1) {
        // Fall back for bulk allocations
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }
    
    if (!free_list_) {
        expand_pool();  // Need more memory
    }
    
    // Pop from free list
    Block* block = free_list_;
    free_list_ = free_list_->next;
    total_allocated_++;
    
    return reinterpret_cast<T*>(block);
}
*/

/*
HINT 4: Deallocation Logic
--------------------------
void deallocate(T* ptr, size_t n) {
    if (n != 1) {
        ::operator delete(ptr);
        return;
    }
    
    // Push back to free list
    Block* block = reinterpret_cast<Block*>(ptr);
    block->next = free_list_;
    free_list_ = block;
    total_deallocated_++;
}
*/

/*
HINT 5: Expanding the Pool
--------------------------
void expand_pool() {
    // Allocate new pool
    Pool* new_pool = static_cast<Pool*>(::operator new(sizeof(Pool)));
    
    // Calculate number of blocks
    constexpr size_t num_blocks = PoolSize / sizeof(Block);
    
    // Link all blocks to free list
    Block* blocks = reinterpret_cast<Block*>(new_pool->memory);
    for (size_t i = 0; i < num_blocks - 1; ++i) {
        blocks[i].next = &blocks[i + 1];
    }
    blocks[num_blocks - 1].next = free_list_;
    
    free_list_ = blocks;
    
    // Track pool for cleanup
    new_pool->next = pool_list_;
    pool_list_ = new_pool;
}
*/

// =============================================================================
// Exercise 2 Hints: Arena Allocator
// =============================================================================

/*
HINT 6: Arena Allocation with Alignment
---------------------------------------
The tricky part is handling alignment correctly:

void* allocate(size_t n, size_t alignment) {
    // Current position in buffer
    void* ptr = buffer_ + offset_;
    size_t space = size_ - offset_;
    
    // Align the pointer
    if (!std::align(alignment, n, ptr, space)) {
        throw std::bad_alloc();  // Not enough space
    }
    
    // Calculate how much we moved for alignment
    size_t aligned_offset = size_ - space;
    
    // Update offset past the allocation
    offset_ = aligned_offset + n;
    
    // Track peak usage
    peak_usage_ = std::max(peak_usage_, offset_);
    
    return ptr;
}
*/

/*
HINT 7: Arena Reset
------------------
void reset() {
    std::cout << "Arena reset (was using " << offset_ 
              << " / " << size_ << " bytes)\n";
    offset_ = 0;
    // Note: Don't reset peak_usage_ - it's cumulative
}
*/

/*
HINT 8: ArenaAllocator Comparison
---------------------------------
Two ArenaAllocator instances are equal if they point to the same Arena:

template<typename T, typename U>
bool operator==(const ArenaAllocator<T>& a, const ArenaAllocator<U>& b) {
    return a.get_arena() == b.get_arena();
}

This is important for container operations like swap!
*/

// =============================================================================
// Exercise 3 Hints: Thread-Safe Pool
// =============================================================================

/*
HINT 9: Simple Thread-Safety (Mutex)
------------------------------------
The simplest approach uses a mutex:

template<typename T, size_t PoolSize>
class ThreadSafePoolAllocator {
    std::mutex mutex_;
    Block* free_list_;
    // ... other members
    
public:
    T* allocate(size_t n) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Same logic as single-threaded version
        if (n != 1) return static_cast<T*>(::operator new(n * sizeof(T)));
        if (!free_list_) expand_pool();
        
        Block* block = free_list_;
        free_list_ = free_list_->next;
        return reinterpret_cast<T*>(block);
    }
    
    void deallocate(T* ptr, size_t n) {
        if (n != 1) {
            ::operator delete(ptr);
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        Block* block = reinterpret_cast<Block*>(ptr);
        block->next = free_list_;
        free_list_ = block;
    }
};
*/

/*
HINT 10: Lock-Free Thread-Safety (Advanced)
------------------------------------------
For better performance, use atomic CAS (Compare-And-Swap):

template<typename T, size_t PoolSize>
class LockFreePoolAllocator {
    std::atomic<Block*> free_list_;
    
public:
    T* allocate(size_t n) {
        if (n != 1) return static_cast<T*>(::operator new(n * sizeof(T)));
        
        Block* old_head = free_list_.load(std::memory_order_acquire);
        Block* new_head;
        
        do {
            if (!old_head) {
                // Need to expand - this requires synchronization
                // Fall back to regular allocation or use a mutex just for expansion
                return static_cast<T*>(::operator new(sizeof(T)));
            }
            new_head = old_head->next;
        } while (!free_list_.compare_exchange_weak(
            old_head, new_head,
            std::memory_order_release,
            std::memory_order_acquire
        ));
        
        return reinterpret_cast<T*>(old_head);
    }
    
    void deallocate(T* ptr, size_t n) {
        if (n != 1) {
            ::operator delete(ptr);
            return;
        }
        
        Block* block = reinterpret_cast<Block*>(ptr);
        Block* old_head = free_list_.load(std::memory_order_relaxed);
        
        do {
            block->next = old_head;
        } while (!free_list_.compare_exchange_weak(
            old_head, block,
            std::memory_order_release,
            std::memory_order_relaxed
        ));
    }
};

Note: This is simplified. Production lock-free code needs to handle
the ABA problem using hazard pointers or epoch-based reclamation.
*/

/*
HINT 11: Thread-Local Pools (Best Performance)
----------------------------------------------
Each thread gets its own pool - no synchronization needed!

template<typename T, size_t PoolSize>
class ThreadLocalPoolAllocator {
    struct ThreadLocalData {
        Block* free_list = nullptr;
        Pool* pools = nullptr;
    };
    
    static thread_local ThreadLocalData tls_data_;
    
public:
    T* allocate(size_t n) {
        // Use tls_data_.free_list
        // No synchronization needed!
    }
};

template<typename T, size_t PoolSize>
thread_local typename ThreadLocalPoolAllocator<T, PoolSize>::ThreadLocalData
    ThreadLocalPoolAllocator<T, PoolSize>::tls_data_;
*/

// =============================================================================
// Exercise 4 Hints: Tracking Allocator
// =============================================================================

/*
HINT 12: Tracking Statistics
----------------------------
template<typename T, typename Base = std::allocator<T>>
class TrackingAllocator {
    static inline std::atomic<size_t> total_allocated_{0};
    static inline std::atomic<size_t> total_freed_{0};
    static inline std::atomic<size_t> current_usage_{0};
    static inline std::atomic<size_t> peak_usage_{0};
    static inline std::atomic<size_t> alloc_count_{0};
    
    Base base_;
    
public:
    T* allocate(size_t n) {
        T* ptr = base_.allocate(n);
        
        size_t bytes = n * sizeof(T);
        total_allocated_.fetch_add(bytes, std::memory_order_relaxed);
        current_usage_.fetch_add(bytes, std::memory_order_relaxed);
        alloc_count_.fetch_add(1, std::memory_order_relaxed);
        
        // Update peak usage (need compare-exchange)
        size_t current = current_usage_.load(std::memory_order_relaxed);
        size_t peak = peak_usage_.load(std::memory_order_relaxed);
        while (current > peak && 
               !peak_usage_.compare_exchange_weak(peak, current)) {
            peak = peak_usage_.load(std::memory_order_relaxed);
        }
        
        return ptr;
    }
    
    void deallocate(T* ptr, size_t n) {
        base_.deallocate(ptr, n);
        
        size_t bytes = n * sizeof(T);
        total_freed_.fetch_add(bytes, std::memory_order_relaxed);
        current_usage_.fetch_sub(bytes, std::memory_order_relaxed);
    }
    
    static void print_stats() {
        size_t total_alloc = total_allocated_.load();
        size_t total_free = total_freed_.load();
        size_t current = current_usage_.load();
        size_t peak = peak_usage_.load();
        size_t count = alloc_count_.load();
        
        std::cout << "=== Allocation Statistics ===\n"
                  << "Total allocated: " << total_alloc << " bytes\n"
                  << "Total freed: " << total_free << " bytes\n"
                  << "Current usage: " << current << " bytes\n"
                  << "Peak usage: " << peak << " bytes\n"
                  << "Allocation count: " << count << "\n"
                  << "Average allocation: " << (count ? total_alloc/count : 0) << " bytes\n"
                  << "Potential leaks: " << (total_alloc - total_free) << " bytes\n";
    }
};
*/

// =============================================================================
// Common Pitfalls & Solutions
// =============================================================================

/*
PITFALL 1: Forgetting Alignment
-------------------------------
Problem:
    char buffer[1024];
    T* ptr = reinterpret_cast<T*>(buffer);  // Might be misaligned!

Solution:
    alignas(T) char buffer[1024];
    // or use std::align()
*/

/*
PITFALL 2: Memory Leaks in Destructor
-------------------------------------
Problem:
    ~PoolAllocator() {
        // Forgot to delete pools!
    }

Solution:
    ~PoolAllocator() {
        while (pool_list_) {
            Pool* next = pool_list_->next;
            ::operator delete(pool_list_);
            pool_list_ = next;
        }
        
        // Check for leaks
        if (total_allocated_ != total_deallocated_) {
            std::cerr << "WARNING: Memory leak detected! "
                      << (total_allocated_ - total_deallocated_)
                      << " objects not freed\n";
        }
    }
*/

/*
PITFALL 3: Rebind Type Mismatch
-------------------------------
Problem:
    // Container needs allocator for different type
    std::list<int> needs allocator<list_node<int>>

Solution:
    template<typename U>
    struct rebind {
        using other = PoolAllocator<U, PoolSize>;
    };
    
    // Copy constructor from different type
    template<typename U>
    PoolAllocator(const PoolAllocator<U>&) noexcept {
        // Initialize new instance - don't share pools!
    }
*/

/*
PITFALL 4: Bulk Allocation (n > 1)
---------------------------------
Problem:
    T* allocate(size_t n) {
        // Only handles n == 1, crashes on vector resize!
    }

Solution:
    T* allocate(size_t n) {
        if (n != 1) {
            // Fall back to default for bulk allocations
            return static_cast<T*>(::operator new(n * sizeof(T)));
        }
        // ... pool logic for single elements
    }
*/

// =============================================================================
// Testing Tips
// =============================================================================

/*
TIP 1: Use Address Sanitizer
----------------------------
Compile with:
    clang++ -std=c++20 -g -fsanitize=address custom_allocators.cpp

This will catch:
- Use after free
- Memory leaks
- Buffer overflows
- Alignment issues
*/

/*
TIP 2: Test with Valgrind
-------------------------
valgrind --leak-check=full ./allocators

Look for:
- Definite leaks
- Possible leaks
- Invalid reads/writes
*/

/*
TIP 3: Stress Test
-----------------
void stress_test() {
    std::list<int, PoolAllocator<int>> list;
    
    for (int i = 0; i < 100000; ++i) {
        list.push_back(i);
        
        if (i % 3 == 0) list.pop_front();
        if (i % 5 == 0) list.pop_back();
    }
}
*/

/*
TIP 4: Thread Safety Test
-------------------------
void thread_safety_test() {
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([]() {
            std::list<int, ThreadSafePoolAllocator<int>> list;
            for (int j = 0; j < 10000; ++j) {
                list.push_back(j);
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}
*/

// =============================================================================
// Performance Analysis
// =============================================================================

/*
EXPECTED RESULTS:
----------------
Pool Allocator:
- 5-10x faster than malloc/free for small objects
- Best for: linked lists, trees, frequent alloc/dealloc

Arena Allocator:
- 10-100x faster for batch deallocation
- Best for: per-frame allocations, request-scoped data

Thread-Safe Pool (Mutex):
- 2-5x faster than malloc under contention
- Some overhead from locking

Thread-Safe Pool (Lock-Free):
- 5-10x faster than malloc under contention
- No locking overhead

Thread-Local Pool:
- Same speed as single-threaded pool
- Zero synchronization overhead
*/

/*
WHEN TO USE EACH:
----------------
Pool Allocator:
✅ Game entities, particles
✅ Node-based containers (list, map, set)
✅ Object pools
❌ Large allocations
❌ Variable-sized allocations

Arena Allocator:
✅ Per-frame temp data
✅ Request/response processing
✅ Compilation/parsing
❌ Long-lived objects
❌ Need individual deallocation

Default Allocator:
✅ General purpose
✅ Variable sizes
✅ When performance is fine
✅ Rapid prototyping
*/

int main() {
    std::cout << "This file contains hints and solutions.\n";
    std::cout << "Refer to it when you get stuck!\n";
    std::cout << "\nKey concepts:\n";
    std::cout << "1. Pool allocators use free lists\n";
    std::cout << "2. Arena allocators bump a pointer\n";
    std::cout << "3. Thread safety requires synchronization\n";
    std::cout << "4. Always profile before optimizing!\n";
    
    return 0;
}
