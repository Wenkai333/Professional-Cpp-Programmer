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
#include <stdexcept>
#include <string>
#include <vector>

// =============================================================================
// Exercise 1: File Handle RAII Wrapper
// =============================================================================

class FileHandle {
private:
    FILE* file_;
    std::string filename_;
    bool is_open_;

public:
    // TODO: Implement constructor that opens file
    // Should throw std::runtime_error if file cannot be opened
    explicit FileHandle(const std::string& filename, const char* mode = "r") {
        // Your implementation here
        // Hints:
        // - Use fopen() to open the file
        // - Store filename for error messages
        // - Set is_open_ appropriately
        // - Print success message: "📁 File opened: " + filename
    }

    // TODO: Implement destructor that closes file
    ~FileHandle() {
        // Your implementation here
        // Hints:
        // - Check if file is open before closing
        // - Use fclose() to close the file
        // - Print cleanup message: "🔒 File closed: " + filename
    }

    // TODO: Implement copy constructor (should it be allowed?)
    FileHandle(const FileHandle& other) {
        // Your implementation here
        // Think: Should files be copyable? What are the implications?
    }

    // TODO: Implement copy assignment (should it be allowed?)
    FileHandle& operator=(const FileHandle& other) {
        // Your implementation here
    }

    // TODO: Implement move constructor
    FileHandle(FileHandle&& other) noexcept {
        // Your implementation here
        // Hints:
        // - Transfer ownership of file handle
        // - Leave 'other' in valid but unspecified state
    }

    // TODO: Implement move assignment
    FileHandle& operator=(FileHandle&& other) noexcept {
        // Your implementation here
    }

    // TODO: Implement write method
    void write(const std::string& data) {
        // Your implementation here
        // Hints:
        // - Check if file is open
        // - Use fprintf() or fwrite()
        // - Throw exception if file is not open
    }

    // TODO: Implement read method
    std::string readLine() {
        // Your implementation here
        // Hints:
        // - Check if file is open
        // - Use fgets() to read a line
        // - Return empty string if EOF or error

        return "";
    }

    // TODO: Implement flush method
    void flush() {
        // Your implementation here
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
    explicit DynamicArray(size_t initial_capacity = 10) {
        // Your implementation here
        // Hints:
        // - Allocate memory using new T[capacity]
        // - Initialize size_ and capacity_
        // - Print allocation message
    }

    // TODO: Implement destructor
    ~DynamicArray() {
        // Your implementation here
        // Hints:
        // - Use delete[] to free the array
        // - Print deallocation message
    }

    // TODO: Implement copy constructor
    DynamicArray(const DynamicArray& other) {
        // Your implementation here
        // Hints:
        // - Allocate new memory
        // - Copy all elements from other
        // - Deep copy, not shallow copy!
    }

    // TODO: Implement copy assignment
    DynamicArray& operator=(const DynamicArray& other) {
        // Your implementation here
        // Hints:
        // - Check for self-assignment
        // - Clean up current resources
        // - Allocate new memory and copy

        return *this;
    }

    // TODO: Implement move constructor
    DynamicArray(DynamicArray&& other) noexcept {
        // Your implementation here
        // Hints:
        // - Transfer ownership of data
        // - Reset other's pointers to nullptr
    }

    // TODO: Implement move assignment
    DynamicArray& operator=(DynamicArray&& other) noexcept {
        // Your implementation here

        return *this;
    }

    // TODO: Implement push_back method
    void push_back(const T& value) {
        // Your implementation here
        // Hints:
        // - Check if resize is needed
        // - Double capacity when full
        // - Use placement new or copy assignment
    }

    // TODO: Implement operator[] for access
    T& operator[](size_t index) {
        // Your implementation here
        // Hints:
        // - Check bounds (throw std::out_of_range if invalid)
        // - Return reference to element

        static T dummy{};  // Remove this line in your implementation
        return dummy;
    }

    // TODO: Implement const version of operator[]
    const T& operator[](size_t index) const {
        // Your implementation here

        static T dummy{};  // Remove this line in your implementation
        return dummy;
    }

    // TODO: Implement resize method
    void resize(size_t new_capacity) {
        // Your implementation here
        // Hints:
        // - Allocate new memory
        // - Copy existing elements
        // - Delete old memory
        // - Update pointers and capacity
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
    explicit DatabaseConnection(const std::string& conn_str) {
        // Your implementation here
        // Hints:
        // - Simulate connection establishment
        // - Assign unique connection_id_
        // - Set is_connected_ to true
        // - Print connection message: "🔌 Database connected [ID: X]: " + conn_str
        // - Simulate potential connection failure (throw exception 10% of the time)
    }

    // TODO: Implement destructor that closes connection
    ~DatabaseConnection() {
        // Your implementation here
        // Hints:
        // - Check if connected before closing
        // - Print disconnection message
        // - Set is_connected_ to false
    }

    // TODO: Should database connections be copyable? Implement accordingly
    DatabaseConnection(const DatabaseConnection& other) = delete;
    DatabaseConnection& operator=(const DatabaseConnection& other) = delete;

    // TODO: Implement move constructor
    DatabaseConnection(DatabaseConnection&& other) noexcept {
        // Your implementation here
    }

    // TODO: Implement move assignment
    DatabaseConnection& operator=(DatabaseConnection&& other) noexcept {
        // Your implementation here

        return *this;
    }

    // TODO: Implement execute query method
    void executeQuery(const std::string& query) {
        // Your implementation here
        // Hints:
        // - Check if connected
        // - Print query execution message
        // - Simulate query failure for "DROP" commands
    }

    // TODO: Implement transaction methods
    void beginTransaction() {
        // Your implementation here
    }

    void commitTransaction() {
        // Your implementation here
    }

    void rollbackTransaction() {
        // Your implementation here
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
    explicit ScopedTimer(const std::string& operation_name) {
        // Your implementation here
        // Hints:
        // - Record start time using std::chrono::steady_clock::now()
        // - Store operation name
        // - Print start message: "⏱️ Timer started: " + operation_name
    }

    // TODO: Implement destructor that prints elapsed time
    ~ScopedTimer() {
        // Your implementation here
        // Hints:
        // - Calculate elapsed time
        // - Print result: "⏰ Timer '" + name + "' finished: X ms"
        // - Use std::chrono::duration_cast<std::chrono::milliseconds>
    }

    // TODO: Implement manual stop method
    void stop() {
        // Your implementation here
        // Hints:
        // - Only stop if not already stopped
        // - Print elapsed time manually
        // - Set stopped_ flag to prevent double-stopping
    }

    // TODO: Implement elapsed time getter
    long long getElapsedMs() const {
        // Your implementation here
        // Return elapsed time in milliseconds

        return 0;
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
            FileHandle file("test_output.txt", "w");
            file.write("Hello, RAII!\n");
            file.write("Testing file wrapper.\n");
            file.flush();
        }  // File should be automatically closed here

        {
            FileHandle file("test_output.txt", "r");
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
            int sum = 0;
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

    // test_file_handle();
    // test_dynamic_array();
    // test_database_connection();
    // test_scoped_timer();
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