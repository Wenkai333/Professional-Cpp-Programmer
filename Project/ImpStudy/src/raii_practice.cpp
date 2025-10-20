/*
 * RAII Practice Exercises
 *
 * Instructions:
 * 1. Implement all the blank methods marked with "TODO"
 * 2. Follow RAII principles: Constructor acquires, Destructor releases
 * 3. Make sure resources are properly cleaned up
 * 4. Handle exceptions safely
 * 5. Test each class thoroughly
 *
 * Compile with: clang++ -std=c++20 -Wall -Wextra raii_practice.cpp -o raii_practice
 */

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

// =============================================================================
// Exercise 1: File Handle RAII Wrapper
// =============================================================================

class FileHandle {
private:
    std::fstream file_;
    std::string filename_;
    std::ios::openmode mode_;
    bool is_open_;

public:
    // TODO: Implement constructor that opens file
    // Should throw std::runtime_error if file cannot be opened
    explicit FileHandle(const std::string& filename,
                        std::ios::openmode mode = std::ios::in | std::ios::out)
        : file_(filename, mode), filename_(filename), mode_(mode), is_open_(false) {
        is_open_ = file_.is_open();
        if (!is_open_) {
            if (mode_ & std::ios::out) {
                std::ofstream creator(filename);
                creator.close();
            }
            file_.open(filename, mode);
            is_open_ = file_.is_open();
            if (!is_open_) {
                throw std::runtime_error("Failed to open: " + filename);
            }
        }
        std::cout << "📁 File opened: " << filename_ << "\n";
    }

    // TODO: Implement destructor that closes file
    ~FileHandle() {
        if (is_open_) {
            file_.close();
        }
    }

    // TODO: Implement copy constructor (should it be allowed?)
    FileHandle(const FileHandle& other) = delete;

    // TODO: Implement copy assignment (should it be allowed?)
    FileHandle& operator=(const FileHandle& other) = delete;

    // TODO: Implement move constructor
    FileHandle(FileHandle&& other) noexcept {
        file_ = std::move(other.file_);
        filename_ = other.filename_;
        mode_ = other.mode_;
        is_open_ = other.is_open_;

        other.is_open_ = false;
    }

    // TODO: Implement move assignment
    FileHandle& operator=(FileHandle&& other) noexcept {
        if (this != &other) {
            if (is_open_) {
                file_.close();
            }
            file_ = std::move(other.file_);
            filename_ = other.filename_;
            mode_ = other.mode_;
            is_open_ = other.is_open_;

            other.is_open_ = false;
        }
        return *this;
    }

    // TODO: Implement write method
    void write(const std::string& data) {
        if (!is_open_ && !(mode_ & std::ios::out)) {
            throw std::runtime_error("Failed to write: " + filename_);
        }

        file_ << data << "\n";
    }

    // TODO: Implement read method
    std::string readLine() {
        if (!is_open_ && !(mode_ & std::ios::in)) {
            throw std::runtime_error("Failed to read: " + filename_);
        }

        std::string data;
        file_ >> data;
        return data;
    }

    // TODO: Implement flush method
    void flush() {
        file_.flush();
    }

    bool isOpen() const {
        return is_open_;
    }
    const std::string& getFilename() const {
        return filename_;
    }
};

// =============================================================================
// Exercise 2: Dynamic Array RAII Wrapper
// =============================================================================

template <typename T>
class DynamicArray {
private:
    T* data_;
    size_t size_;
    size_t capacity_;

public:
    // TODO: Implement constructor with initial capacity
    explicit DynamicArray(size_t initial_capacity = 10)
        : data_(nullptr), size_(0), capacity_(initial_capacity) {
        data_ = new T[initial_capacity];
    }

    // TODO: Implement destructor
    ~DynamicArray() {
        if (data_) {
            delete[] data_;
        }
    }

    // TODO: Implement copy constructor
    DynamicArray(const DynamicArray& other) {
        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = new T[capacity_];
        for (size_t i = 0; i < size_; i++) {
            data_[i] = other.data_[i];
        }
    }

    // TODO: Implement copy assignment
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            data_ = new T[capacity_];
            for (size_t i = 0; i < size_; i++) {
                data_[i] = other.data_[i];
            }
        }
        return *this;
    }

    // TODO: Implement move constructor
    DynamicArray(DynamicArray&& other) noexcept
        : data_(other.data_),  // ✅ Initialize properly
          size_(other.size_),
          capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // TODO: Implement move assignment
    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            delete[] data_;

            size_ = other.size_;
            capacity_ = other.capacity_;
            data_ = other.data_;

            other.data_ = nullptr;
        }
        return *this;
    }

    // TODO: Implement push_back method
    void push_back(const T& value) {
        if (size_ >= capacity_) {
            resize(capacity_ * 2);
        }
        data_[size_++] = value;
    }

    // TODO: Implement operator[] for access
    T& operator[](size_t index) {
        if (size_ <= index) {
            throw std::out_of_range("Index " + std::to_string(index) +
                                    " out of range (size: " + std::to_string(size_) + ")");
        }
        return data_[index];
    }

    // TODO: Implement const version of operator[]
    const T& operator[](size_t index) const {
        if (size_ <= index) {
            throw std::out_of_range("Index " + std::to_string(index) +
                                    " out of range (size: " + std::to_string(size_) + ")");
        }
        return data_[index];
    }

    // TODO: Implement resize method
    void resize(size_t new_capacity) {
        capacity_ = new_capacity;
        T* new_arr = new T[capacity_];
        for (size_t i = 0; i < size_; i++) {
            new_arr[i] = data_[i];
        }
        delete[] data_;
        data_ = new_arr;
    }

    size_t size() const {
        return size_;
    }
    size_t capacity() const {
        return capacity_;
    }
    bool empty() const {
        return size_ == 0;
    }
};

// =============================================================================
// Exercise 3: Database Connection RAII Wrapper
// =============================================================================

class DatabaseConnection {
private:
    std::string connection_string_;
    bool is_connected_;
    int connection_id_;
    static int next_id_;

public:
    // TODO: Implement constructor that establishes connection
    explicit DatabaseConnection(const std::string& conn_str)
        : connection_string_(conn_str), is_connected_(false), connection_id_(0) {
        // Your implementation here
        // Hints:
        // - Simulate connection establishment
        // - Assign unique connection_id_
        // - Set is_connected_ to true
        // - Print connection message: "🔌 Database connected [ID: X]: " + conn_str
        // - Simulate potential connection failure (throw exception 10% of the time)

        auto gen_random = []() -> int {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(1, 100);
            return dis(gen);
        };
        const int rate_to_fail = gen_random();

        if (rate_to_fail <= 10) {
            throw std::runtime_error("Failed to connect Database: " + connection_string_);
        }

        connection_id_ = next_id_++;
        is_connected_ = true;

        std::cout << "🔌 Database connected [ID: X]: " + connection_string_ << std::endl;
    }

    // TODO: Implement destructor that closes connection
    ~DatabaseConnection() {
        // Your implementation here
        // Hints:
        // - Check if connected before closing
        // - Print disconnection message
        // - Set is_connected_ to false

        if (is_connected_) {
            std::cout << "🔌 Database disconnected [ID: X]: " + connection_string_ << std::endl;
            is_connected_ = false;
        }
    }

    // TODO: Should database connections be copyable? Implement accordingly
    DatabaseConnection(const DatabaseConnection& other) = delete;
    DatabaseConnection& operator=(const DatabaseConnection& other) = delete;

    // TODO: Implement move constructor
    DatabaseConnection(DatabaseConnection&& other) noexcept
        : connection_string_(std::move(other.connection_string_)),
          is_connected_(other.is_connected_),
          connection_id_(other.connection_id_) {
        // Reset the moved-from object
        other.is_connected_ = false;
        other.connection_id_ = 0;
        other.connection_string_.clear();

        if (is_connected_) {
            std::cout << "📦 Database connection moved [ID: " << connection_id_ << "]" << std::endl;
        }
    }

    // TODO: Implement move assignment
    DatabaseConnection& operator=(DatabaseConnection&& other) noexcept {
        if (this != &other) {
            // Close current connection if open
            if (is_connected_) {
                std::cout << "🔒 Database disconnected [ID: " << connection_id_
                          << "] (during move assignment)" << std::endl;
            }

            // Transfer ownership
            connection_string_ = std::move(other.connection_string_);
            is_connected_ = other.is_connected_;
            connection_id_ = other.connection_id_;

            // Reset the moved-from object
            other.is_connected_ = false;
            other.connection_id_ = 0;
            other.connection_string_.clear();

            if (is_connected_) {
                std::cout << "📦 Database connection move-assigned [ID: " << connection_id_ << "]"
                          << std::endl;
            }
        }
        return *this;
    }

    // TODO: Implement execute query method
    void executeQuery(const std::string& query) {
        // Your implementation here
        // Hints:
        // - Check if connected
        // - Print query execution message
        // - Simulate query failure for "DROP" commands
        if (!is_connected_) {
            throw std::runtime_error("Cannot execute query: Database not connected");
        }

        // Simulate query failure for dangerous operations
        if (query.find("DROP") != std::string::npos) {
            throw std::runtime_error("Query failed: DROP operations are not allowed");
        }

        std::cout << "🔍 Executing query [ID: " << connection_id_ << "]: " << query << std::endl;
    }

    // TODO: Implement transaction methods
    void beginTransaction() {
        if (!is_connected_) {
            throw std::runtime_error("Cannot begin transaction: Database not connected");
        }
        std::cout << "🚀 Transaction started [ID: " << connection_id_ << "]" << std::endl;
    }

    void commitTransaction() {
        if (!is_connected_) {
            throw std::runtime_error("Cannot commit transaction: Database not connected");
        }
        std::cout << "✅ Transaction committed [ID: " << connection_id_ << "]" << std::endl;
    }

    void rollbackTransaction() {
        if (!is_connected_) {
            throw std::runtime_error("Cannot rollback transaction: Database not connected");
        }
        std::cout << "↩️ Transaction rolled back [ID: " << connection_id_ << "]" << std::endl;
    }

    bool isConnected() const {
        return is_connected_;
    }
    int getConnectionId() const {
        return connection_id_;
    }
};

// Static member definition
int DatabaseConnection::next_id_ = 1;

// =============================================================================
// Exercise 4: Timer RAII Wrapper (Scope-based timing)
// =============================================================================

class ScopedTimer {
private:
    std::chrono::steady_clock::time_point start_time_;
    std::string operation_name_;
    bool stopped_;

public:
    // TODO: Implement constructor that starts timing
    explicit ScopedTimer(const std::string& operation_name)
        : operation_name_(operation_name), stopped_(false) {
        // Your implementation here
        // Hints:
        // - Record start time using std::chrono::steady_clock::now()
        // - Store operation name
        // - Print start message: "⏱️ Timer started: " + operation_name
        start_time_ = std::chrono::steady_clock::now();
        std::cout << "⏱️ Timer started: " + operation_name << std::endl;
    }

    // TODO: Implement destructor that prints elapsed time
    ~ScopedTimer() {
        // Your implementation here
        // Hints:
        // - Calculate elapsed time
        // - Print result: "⏰ Timer '" + name + "' finished: X ms"
        // - Use std::chrono::duration_cast<std::chrono::milliseconds>
        stop();
    }

    // TODO: Implement manual stop method
    void stop() {
        // Your implementation here
        // Hints:
        // - Only stop if not already stopped
        // - Print elapsed time manually
        // - Set stopped_ flag to prevent double-stopping
        if (!stopped_) {
            std::cout << "⏰ Timer '" + operation_name_ + "' finished:" << getElapsedMs() << " ms"
                      << std::endl;
            stopped_ = true;  // Mark as stopped
        }
    }

    // TODO: Implement elapsed time getter
    long long getElapsedMs() const {
        // Your implementation here
        // Return elapsed time in milliseconds
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_time_);
        return ms.count();
    }

    // Non-copyable, non-movable (timers are tied to specific scope)
    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;
    ScopedTimer(ScopedTimer&&) = delete;
    ScopedTimer& operator=(ScopedTimer&&) = delete;
};

// =============================================================================
// Exercise 5: Socket RAII Wrapper (Simplified simulation)
// =============================================================================

class Socket {
private:
    int socket_fd_;
    std::string address_;
    int port_;
    bool is_connected_;

public:
    // TODO: Implement constructor that creates and connects socket
    Socket(const std::string& address, int port) {
        // Your implementation here
        // Hints:
        // - Simulate socket creation (use random number as socket_fd_)
        // - Simulate connection process
        // - Print connection message
        // - Throw exception if port is invalid (< 1 or > 65535)
        port_ = -1;
    }

    // TODO: Implement destructor that closes socket
    ~Socket() {
        // Your implementation here
        // Hints:
        // - Check if connected before closing
        // - Print socket closure message
        // - Simulate socket cleanup
    }

    // TODO: Implement send method
    void send(const std::string& data) {
        // Your implementation here
        // Hints:
        // - Check if connected
        // - Print send message with data size
        // - Simulate network delay (optional)
    }

    // TODO: Implement receive method
    std::string receive() {
        // Your implementation here
        // Hints:
        // - Check if connected
        // - Simulate receiving data
        // - Return simulated response

        return "";
    }

    // TODO: Implement disconnect method
    void disconnect() {
        // Your implementation here
    }

    bool isConnected() const {
        return is_connected_;
    }
    int getSocketFd() const {
        return socket_fd_;
    }

    // Sockets typically shouldn't be copied, but can be moved
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    // TODO: Implement move constructor
    Socket(Socket&& other) noexcept {
        // Your implementation here
    }

    // TODO: Implement move assignment
    Socket& operator=(Socket&& other) noexcept {
        // Your implementation here

        return *this;
    }
};

// =============================================================================
// Test Functions (DO NOT MODIFY - Use these to test your implementations)
// =============================================================================

void test_file_handle() {
    std::cout << "\n=== Testing FileHandle ===\n";
    try {
        {
            FileHandle file("./temporary/test_output.txt");
            file.write("Hello, RAII!\n");
            file.write("Testing file wrapper.\n");
            file.flush();
        }  // File should be automatically closed here

        {
            FileHandle file("./temporary/test_output.txt");
            std::string line = file.readLine();
            while (!line.empty()) {
                std::cout << "Read: " << line;
                line = file.readLine();
            }
        }  // File should be automatically closed here

    } catch (const std::exception& e) {
        std::cout << "❌ FileHandle test failed: " << e.what() << std::endl;
    }
}

void test_dynamic_array() {
    std::cout << "\n=== Testing DynamicArray ===\n";
    try {
        {
            DynamicArray<int> arr(5);

            for (int i = 0; i < 10; ++i) {
                arr.push_back(i * i);
            }

            std::cout << "Array contents: ";
            for (size_t i = 0; i < arr.size(); ++i) {
                std::cout << arr[i] << " ";
            }
            std::cout << "\n";

            // Test copy
            DynamicArray<int> arr_copy = arr;
            std::cout << "Copy size: " << arr_copy.size() << "\n";

        }  // Arrays should be automatically cleaned up here

    } catch (const std::exception& e) {
        std::cout << "❌ DynamicArray test failed: " << e.what() << std::endl;
    }
}

void test_database_connection() {
    std::cout << "\n=== Testing DatabaseConnection ===\n";
    try {
        {
            DatabaseConnection db("postgresql://localhost:5432/testdb");
            db.executeQuery("SELECT * FROM users");
            db.beginTransaction();
            db.executeQuery("INSERT INTO users VALUES (1, 'Alice')");
            db.commitTransaction();
        }  // Connection should be automatically closed here

    } catch (const std::exception& e) {
        std::cout << "❌ DatabaseConnection test failed: " << e.what() << std::endl;
    }
}

void test_scoped_timer() {
    std::cout << "\n=== Testing ScopedTimer ===\n";
    try {
        {
            ScopedTimer timer("Heavy computation");

            // Simulate some work
            long long sum = 0;
            for (int i = 0; i < 1000000; ++i) {
                sum += i;
            }
            std::cout << "Computation result: " << sum << "\n";

        }  // Timer should automatically print elapsed time here

    } catch (const std::exception& e) {
        std::cout << "❌ ScopedTimer test failed: " << e.what() << std::endl;
    }
}

void test_socket() {
    std::cout << "\n=== Testing Socket ===\n";
    try {
        {
            Socket socket("192.168.1.100", 8080);
            socket.send("GET / HTTP/1.1\r\nHost: example.com\r\n\r\n");
            std::string response = socket.receive();
            std::cout << "Received: " << response << "\n";
        }  // Socket should be automatically closed here

    } catch (const std::exception& e) {
        std::cout << "❌ Socket test failed: " << e.what() << std::endl;
    }
}

// =============================================================================
// Main Function - Test Runner
// =============================================================================

int main() {
    std::cout << "🎯 RAII Practice Exercises\n";
    std::cout << "Implement all TODO methods and run tests!\n";

    // Uncomment these tests as you implement each class:

    test_file_handle();
    test_dynamic_array();
    test_database_connection();
    test_scoped_timer();
    // test_socket();

    std::cout << "\n=== Instructions ===\n";
    std::cout << "1. Implement all TODO methods in each class\n";
    std::cout << "2. Follow RAII principles: Constructor acquires, Destructor releases\n";
    std::cout << "3. Uncomment test functions one by one as you complete each class\n";
    std::cout << "4. Make sure all resources are properly cleaned up\n";
    std::cout << "5. Handle exceptions safely\n";
    std::cout << "\n💡 Remember: RAII means automatic cleanup when objects go out of scope!\n";

    return 0;
}
