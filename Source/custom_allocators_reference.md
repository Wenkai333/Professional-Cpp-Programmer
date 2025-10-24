# Custom Allocators Quick Reference Card

## 📋 Standard Allocator Interface

```cpp
template<typename T>
struct Allocator {
    using value_type = T;
    
    // REQUIRED: Allocate memory for n objects
    T* allocate(size_t n);
    
    // REQUIRED: Deallocate memory
    void deallocate(T* ptr, size_t n);
    
    // REQUIRED (C++17+): Equality comparison
    bool operator==(const Allocator& other) const;
    bool operator!=(const Allocator& other) const;
    
    // REQUIRED: Rebind for different types
    template<typename U>
    struct rebind {
        using other = Allocator<U>;
    };
    
    // OPTIONAL: Construction/destruction
    template<typename U, typename... Args>
    void construct(U* ptr, Args&&... args) {
        ::new(ptr) U(std::forward<Args>(args)...);
    }
    
    template<typename U>
    void destroy(U* ptr) {
        ptr->~U();
    }
};
```

## 🏊 Pool Allocator Pattern

**Use case**: Frequent allocation/deallocation of same-sized objects

### Key Data Structures
```cpp
union Block {
    T element;       // When allocated
    Block* next;     // When free (free list)
};
```

### Core Operations
```cpp
// Allocate - O(1)
Block* block = free_list_;
free_list_ = free_list_->next;
return reinterpret_cast<T*>(block);

// Deallocate - O(1)
Block* block = reinterpret_cast<Block*>(ptr);
block->next = free_list_;
free_list_ = block;
```

### Performance
- **Allocation**: O(1) - pop from free list
- **Deallocation**: O(1) - push to free list
- **Speedup**: 5-10x vs malloc/free
- **Memory**: Pre-allocated pools, low fragmentation

## 🏟️ Arena Allocator Pattern

**Use case**: Batch allocations with same lifetime

### Key Data Structure
```cpp
char* buffer_;      // Memory block
size_t offset_;     // Current position
size_t size_;       // Total size
```

### Core Operations
```cpp
// Allocate - bump pointer
void* ptr = buffer_ + offset_;
offset_ += n;
return ptr;

// Deallocate all - O(1)
offset_ = 0;
```

### Performance
- **Allocation**: O(1) - bump pointer
- **Individual deallocation**: Not supported
- **Batch deallocation**: O(1) - reset offset
- **Speedup**: 10-100x for batch deallocation
- **Memory**: Pre-allocated, sequential

## 🔐 Thread-Safety Strategies

### 1. Mutex-Based (Simple)
```cpp
std::mutex mutex_;

T* allocate(size_t n) {
    std::lock_guard<std::mutex> lock(mutex_);
    // ... allocation logic
}
```
**Pros**: Simple, safe  
**Cons**: Contention overhead

### 2. Lock-Free (Advanced)
```cpp
std::atomic<Block*> free_list_;

T* allocate(size_t n) {
    Block* old_head = free_list_.load();
    Block* new_head;
    do {
        new_head = old_head->next;
    } while (!free_list_.compare_exchange_weak(old_head, new_head));
    return reinterpret_cast<T*>(old_head);
}
```
**Pros**: No locking, scalable  
**Cons**: Complex, ABA problem

### 3. Thread-Local (Best)
```cpp
static thread_local Block* free_list_;

T* allocate(size_t n) {
    // No synchronization needed!
    Block* block = free_list_;
    free_list_ = free_list_->next;
    return reinterpret_cast<T*>(block);
}
```
**Pros**: Zero overhead, fastest  
**Cons**: Memory per thread

## 🎯 When to Use Each

| Allocator Type | Best For | Avoid For |
|---------------|----------|-----------|
| **Default** | General use, prototyping | Allocation hotspots |
| **Pool** | Small objects, nodes | Large/variable sizes |
| **Arena** | Frame data, parsing | Long-lived objects |
| **Thread-Safe Pool** | Concurrent access | Single-threaded |
| **Thread-Local** | Per-thread objects | Shared data |

## ⚠️ Common Pitfalls

### 1. Alignment Issues
```cpp
// ❌ WRONG
char buffer[1024];
T* ptr = reinterpret_cast<T*>(buffer);

// ✅ CORRECT
alignas(T) char buffer[1024];
T* ptr = reinterpret_cast<T*>(buffer);
```

### 2. Forgetting Bulk Allocations
```cpp
T* allocate(size_t n) {
    if (n != 1) {
        // MUST handle n > 1 for vectors!
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }
    // ... pool logic for n == 1
}
```

### 3. Memory Leaks
```cpp
~PoolAllocator() {
    // MUST clean up pools!
    while (pool_list_) {
        Pool* next = pool_list_->next;
        ::operator delete(pool_list_);
        pool_list_ = next;
    }
}
```

### 4. Rebind Issues
```cpp
// std::list<int> needs allocator<list_node<int>>
// Must support rebinding!

template<typename U>
struct rebind {
    using other = PoolAllocator<U, PoolSize>;
};

template<typename U>
PoolAllocator(const PoolAllocator<U>&) noexcept { }
```

## 📊 Performance Checklist

Before implementing custom allocator:
- [ ] Profile code - is allocation a bottleneck?
- [ ] Identify allocation patterns
- [ ] Measure current performance
- [ ] Choose appropriate allocator type

After implementing:
- [ ] Benchmark vs default allocator
- [ ] Test with AddressSanitizer
- [ ] Check for memory leaks (Valgrind)
- [ ] Verify thread safety (ThreadSanitizer)
- [ ] Document performance gains

## 🔧 Debugging Tools

```bash
# Address Sanitizer (catches memory errors)
clang++ -std=c++20 -g -fsanitize=address file.cpp

# Valgrind (memory leak detection)
valgrind --leak-check=full ./program

# Thread Sanitizer (data races)
clang++ -std=c++20 -g -fsanitize=thread file.cpp

# Memory profiling
valgrind --tool=massif ./program
ms_print massif.out.XXX
```

## 📈 Typical Performance Gains

| Operation | Default | Pool | Arena | Improvement |
|-----------|---------|------|-------|-------------|
| List insert | 100ns | 20ns | 15ns | 5-7x |
| List delete | 100ns | 15ns | 0ns* | 7-∞x |
| Vector resize | 500ns | 500ns | 400ns | 1.25x |
| Batch free | O(n) | O(n) | O(1) | 100-1000x |

\* Arena doesn't support individual deletion

## 🎓 Study Tips

1. **Start simple**: Implement pool allocator first
2. **Test thoroughly**: Use sanitizers and Valgrind
3. **Benchmark**: Measure actual improvements
4. **Read STL**: Study allocator_traits and pmr
5. **Practice**: Use in real projects

## 📚 Further Reading

- **C++17**: `<memory_resource>` polymorphic allocators
- **Book**: "The C++ Standard Library" - Josuttis
- **Video**: CppCon "Allocator is to Allocation what Vector is to Vexation"
- **Article**: Andrei Alexandrescu's "Policy-Based Design"

## 💡 Pro Tips

1. **Profile first** - Don't optimize without data
2. **Use pmr** - C++17 polymorphic allocators are easier
3. **Document** - Explain allocator requirements clearly
4. **Statistics** - Track allocations for debugging
5. **Thread-local** - Best for parallel performance

## 🎯 Decision Tree

```
Is allocation a bottleneck?
├─ No  → Use std::allocator
└─ Yes → What's the pattern?
    ├─ Small, fixed-size, frequent → Pool Allocator
    ├─ Batch with same lifetime → Arena Allocator
    ├─ Concurrent access → Thread-Safe Pool
    └─ Per-thread objects → Thread-Local Pool
```

## ⚡ Quick Implementation Checklist

Pool Allocator:
- [ ] Union Block (element/next)
- [ ] Free list pointer
- [ ] expand_pool() method
- [ ] Handle n != 1
- [ ] Cleanup in destructor

Arena Allocator:
- [ ] Buffer + offset
- [ ] std::align for alignment
- [ ] reset() method
- [ ] No individual deallocation
- [ ] Track peak usage

Thread-Safe:
- [ ] Choose synchronization strategy
- [ ] std::atomic for statistics
- [ ] Test with multiple threads
- [ ] Profile under contention

---

**Remember**: Custom allocators are powerful but complex. Always profile first, test thoroughly, and document clearly!
