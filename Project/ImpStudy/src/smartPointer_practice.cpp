/*
 * Smart Pointers Hands-On Exercise
 * Game Entity Management System
 * 
 * INSTRUCTIONS:
 * 1. Implement all methods marked with "TODO"
 * 2. Use appropriate smart pointers (unique_ptr, shared_ptr, weak_ptr)
 * 3. Ensure no memory leaks
 * 4. Follow RAII principles
 * 5. Run all tests to verify your implementation
 * 
 * Compile with: 
 * clang++ -std=c++20 -Wall -Wextra smart_pointers_exercise.cpp -o smart_ptr
 * 
 * DIFFICULTY LEVELS:
 * ⭐ Basic - unique_ptr fundamentals
 * ⭐⭐ Intermediate - shared_ptr and ownership
 * ⭐⭐⭐ Advanced - weak_ptr and circular references
 */

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cassert>

// =============================================================================
// Exercise 1: ⭐ Basic Entity with unique_ptr
// =============================================================================

class Entity {
protected:
    std::string name_;
    int health_;
    
public:
    Entity(const std::string& name, int health) 
        : name_(name), health_(health) {
        std::cout << "  🎮 Entity '" << name_ << "' created (HP: " << health_ << ")\n";
    }
    
    virtual ~Entity() {
        std::cout << "  💀 Entity '" << name_ << "' destroyed\n";
    }
    
    virtual void takeDamage(int damage) {
        health_ -= damage;
        std::cout << "  💥 " << name_ << " took " << damage 
                  << " damage! (HP: " << health_ << ")\n";
    }
    
    const std::string& getName() const { return name_; }
    int getHealth() const { return health_; }
    bool isAlive() const { return health_ > 0; }
};

class Player : public Entity {
    int score_;
    
public:
    Player(const std::string& name) 
        : Entity(name, 100), score_(0) {
        std::cout << "  👤 Player '" << name_ << "' joined the game!\n";
    }
    
    ~Player() override {
        std::cout << "  👋 Player '" << name_ << "' left the game (Score: " 
                  << score_ << ")\n";
    }
    
    void addScore(int points) {
        score_ += points;
        std::cout << "  ⭐ " << name_ << " scored " << points 
                  << " points! (Total: " << score_ << ")\n";
    }
    
    int getScore() const { return score_; }
};

class Enemy : public Entity {
    int damage_;
    
public:
    Enemy(const std::string& name, int health, int damage)
        : Entity(name, health), damage_(damage) {
        std::cout << "  👾 Enemy '" << name_ << "' spawned!\n";
    }
    
    ~Enemy() override {
        std::cout << "  ☠️ Enemy '" << name_ << "' defeated!\n";
    }
    
    int getDamage() const { return damage_; }
};

// TODO: ⭐ Exercise 1.1 - Create a factory function
// Create unique_ptr<Player> with the given name
std::unique_ptr<Player> createPlayer(const std::string& name) {
    // Your implementation here
    // Hint: Use std::make_unique
    
    return nullptr;  // Replace this
}

// TODO: ⭐ Exercise 1.2 - Create a factory function
// Create unique_ptr<Enemy> with name, health, and damage
std::unique_ptr<Enemy> createEnemy(const std::string& name, int health, int damage) {
    // Your implementation here
    
    return nullptr;  // Replace this
}

// TODO: ⭐ Exercise 1.3 - Transfer ownership
// This function should take ownership of the player and return it back
std::unique_ptr<Player> processPlayer(std::unique_ptr<Player> player) {
    // Your implementation here
    // 1. Check if player is valid (not nullptr)
    // 2. Add 10 points to the player's score
    // 3. Return the player (transfer ownership back)
    
    return nullptr;  // Replace this
}

// =============================================================================
// Exercise 2: ⭐⭐ Inventory System with shared_ptr
// =============================================================================

class Item {
    std::string name_;
    int value_;
    
public:
    Item(const std::string& name, int value) 
        : name_(name), value_(value) {
        std::cout << "  📦 Item '" << name_ << "' created (Value: " 
                  << value_ << ")\n";
    }
    
    ~Item() {
        std::cout << "  🗑️ Item '" << name_ << "' destroyed\n";
    }
    
    const std::string& getName() const { return name_; }
    int getValue() const { return value_; }
};

class Inventory {
    std::vector<std::shared_ptr<Item>> items_;
    
public:
    // TODO: ⭐⭐ Exercise 2.1 - Add item to inventory
    // Should accept shared_ptr and store it
    void addItem(std::shared_ptr<Item> item) {
        // Your implementation here
        // 1. Add item to items_ vector
        // 2. Print message: "  ➕ Added [item_name] to inventory"
        
    }
    
    // TODO: ⭐⭐ Exercise 2.2 - Remove item by name
    // Should find and remove the item, return true if found
    bool removeItem(const std::string& name) {
        // Your implementation here
        // Hints:
        // 1. Use std::find_if to find the item
        // 2. Use erase to remove it
        // 3. Print message if found: "  ➖ Removed [name] from inventory"
        // 4. Return true if found, false otherwise
        
        return false;  // Replace this
    }
    
    // TODO: ⭐⭐ Exercise 2.3 - Get item by name (returns shared ownership)
    // Should return shared_ptr to the item, or nullptr if not found
    std::shared_ptr<Item> getItem(const std::string& name) const {
        // Your implementation here
        // Hint: Find the item and return a copy of the shared_ptr
        
        return nullptr;  // Replace this
    }
    
    // TODO: ⭐⭐ Exercise 2.4 - Get total value of all items
    int getTotalValue() const {
        // Your implementation here
        // Hint: Sum up all item values
        
        return 0;  // Replace this
    }
    
    void listItems() const {
        std::cout << "  📋 Inventory:\n";
        for (const auto& item : items_) {
            std::cout << "    - " << item->getName() 
                      << " (Value: " << item->getValue() 
                      << ", Ref Count: " << item.use_count() << ")\n";
        }
    }
    
    size_t size() const { return items_.size(); }
};

// TODO: ⭐⭐ Exercise 2.5 - Create shared item
// Multiple inventories can share the same item
std::shared_ptr<Item> createSharedItem(const std::string& name, int value) {
    // Your implementation here
    // Hint: Use std::make_shared
    
    return nullptr;  // Replace this
}

// =============================================================================
// Exercise 3: ⭐⭐⭐ Team System with weak_ptr (Circular Reference Prevention)
// =============================================================================

class Team;  // Forward declaration

class TeamMember : public std::enable_shared_from_this<TeamMember> {
    std::string name_;
    std::weak_ptr<Team> team_;  // Weak reference to avoid circular dependency
    
public:
    explicit TeamMember(const std::string& name) : name_(name) {
        std::cout << "  👥 TeamMember '" << name_ << "' created\n";
    }
    
    ~TeamMember() {
        std::cout << "  👋 TeamMember '" << name_ << "' destroyed\n";
    }
    
    const std::string& getName() const { return name_; }
    
    // TODO: ⭐⭐⭐ Exercise 3.1 - Set team (weak reference)
    void setTeam(std::shared_ptr<Team> team) {
        // Your implementation here
        // Store team as weak_ptr to avoid circular reference
        
    }
    
    // TODO: ⭐⭐⭐ Exercise 3.2 - Get team name (using weak_ptr)
    std::string getTeamName() const {
        // Your implementation here
        // Hints:
        // 1. Lock the weak_ptr to get shared_ptr
        // 2. If valid, return team name
        // 3. If expired, return "No Team"
        
        return "No Team";  // Replace this
    }
    
    // TODO: ⭐⭐⭐ Exercise 3.3 - Check if still in team
    bool hasTeam() const {
        // Your implementation here
        // Hint: Check if weak_ptr is expired
        
        return false;  // Replace this
    }
    
    // Get shared_ptr to this object
    std::shared_ptr<TeamMember> getSharedPtr() {
        return shared_from_this();
    }
};

class Team {
    std::string name_;
    std::vector<std::shared_ptr<TeamMember>> members_;
    
public:
    explicit Team(const std::string& name) : name_(name) {
        std::cout << "  🏆 Team '" << name_ << "' created\n";
    }
    
    ~Team() {
        std::cout << "  🏁 Team '" << name_ << "' disbanded\n";
    }
    
    const std::string& getName() const { return name_; }
    
    // TODO: ⭐⭐⭐ Exercise 3.4 - Add member to team
    void addMember(std::shared_ptr<TeamMember> member) {
        // Your implementation here
        // 1. Add member to members_ vector
        // 2. Set this team for the member (pass shared_from_this equivalent)
        // 3. Print message: "  ➕ [member_name] joined team [team_name]"
        
    }
    
    // TODO: ⭐⭐⭐ Exercise 3.5 - Remove member by name
    bool removeMember(const std::string& name) {
        // Your implementation here
        // Similar to inventory removeItem
        
        return false;  // Replace this
    }
    
    // TODO: ⭐⭐⭐ Exercise 3.6 - Get member count
    size_t getMemberCount() const {
        // Your implementation here
        
        return 0;  // Replace this
    }
    
    void listMembers() const {
        std::cout << "  👥 Team '" << name_ << "' Members:\n";
        for (const auto& member : members_) {
            std::cout << "    - " << member->getName() 
                      << " (Ref Count: " << member.use_count() << ")\n";
        }
    }
};

// =============================================================================
// Exercise 4: ⭐⭐⭐ Resource Cache with weak_ptr
// =============================================================================

class Texture {
    std::string filename_;
    int width_, height_;
    
public:
    Texture(const std::string& filename, int width, int height)
        : filename_(filename), width_(width), height_(height) {
        std::cout << "  🖼️ Texture '" << filename_ << "' loaded (" 
                  << width_ << "x" << height_ << ")\n";
    }
    
    ~Texture() {
        std::cout << "  🗑️ Texture '" << filename_ << "' unloaded\n";
    }
    
    const std::string& getFilename() const { return filename_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
};

class TextureCache {
    std::map<std::string, std::weak_ptr<Texture>> cache_;
    
public:
    // TODO: ⭐⭐⭐ Exercise 4.1 - Load texture with caching
    std::shared_ptr<Texture> load(const std::string& filename, int width, int height) {
        // Your implementation here
        // 1. Check if texture exists in cache
        // 2. Try to lock the weak_ptr
        // 3. If valid, return existing texture (cache hit)
        // 4. If expired or not found, create new texture
        // 5. Store weak_ptr in cache
        // 6. Return shared_ptr to texture
        
        // Hints:
        // - Use cache_[filename].lock() to get shared_ptr
        // - Use std::make_shared to create new texture
        // - Print "  ✅ Cache HIT: [filename]" or "  ❌ Cache MISS: [filename]"
        
        return nullptr;  // Replace this
    }
    
    // TODO: ⭐⭐⭐ Exercise 4.2 - Clear expired entries
    void clearExpired() {
        // Your implementation here
        // Remove all expired weak_ptr entries from cache
        // Hint: Use erase_if (C++20) or manual loop with erase
        
    }
    
    // TODO: ⭐⭐⭐ Exercise 4.3 - Get cache statistics
    void printStats() const {
        // Your implementation here
        // Count total entries and how many are still valid
        // Print: "  📊 Cache: X entries (Y valid, Z expired)"
        
    }
};

// =============================================================================
// Test Functions
// =============================================================================

void test_exercise_1() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "TEST 1: ⭐ unique_ptr Basics\n";
    std::cout << std::string(60, '=') << "\n";
    
    {
        auto player = createPlayer("Alice");
        assert(player != nullptr);
        assert(player->getName() == "Alice");
        
        auto enemy = createEnemy("Goblin", 30, 5);
        assert(enemy != nullptr);
        
        player = processPlayer(std::move(player));
        assert(player != nullptr);
        assert(player->getScore() == 10);
        
        std::cout << "✅ Exercise 1 PASSED!\n";
    }
}

void test_exercise_2() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "TEST 2: ⭐⭐ shared_ptr and Inventory\n";
    std::cout << std::string(60, '=') << "\n";
    
    {
        Inventory inv1, inv2;
        
        auto sword = createSharedItem("Sword", 100);
        assert(sword != nullptr);
        
        inv1.addItem(sword);
        inv2.addItem(sword);  // Shared ownership
        
        std::cout << "  Sword ref count: " << sword.use_count() << "\n";
        assert(sword.use_count() == 3);  // sword + inv1 + inv2
        
        auto shield = createSharedItem("Shield", 50);
        inv1.addItem(shield);
        
        assert(inv1.size() == 2);
        assert(inv1.getTotalValue() == 150);
        
        inv1.listItems();
        
        bool removed = inv1.removeItem("Sword");
        assert(removed == true);
        assert(inv1.size() == 1);
        
        std::cout << "✅ Exercise 2 PASSED!\n";
    }
}

void test_exercise_3() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "TEST 3: ⭐⭐⭐ weak_ptr and Team System\n";
    std::cout << std::string(60, '=') << "\n";
    
    {
        auto team = std::make_shared<Team>("Heroes");
        auto member1 = std::make_shared<TeamMember>("Bob");
        auto member2 = std::make_shared<TeamMember>("Charlie");
        
        team->addMember(member1);
        team->addMember(member2);
        
        assert(member1->hasTeam() == true);
        assert(member1->getTeamName() == "Heroes");
        assert(team->getMemberCount() == 2);
        
        team->listMembers();
        
        team->removeMember("Bob");
        assert(team->getMemberCount() == 1);
        
        std::cout << "✅ Exercise 3 PASSED!\n";
    }
}

void test_exercise_4() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "TEST 4: ⭐⭐⭐ Resource Cache with weak_ptr\n";
    std::cout << std::string(60, '=') << "\n";
    
    {
        TextureCache cache;
        
        {
            auto tex1 = cache.load("player.png", 64, 64);
            auto tex2 = cache.load("player.png", 64, 64);  // Should be cache hit
            
            assert(tex1 == tex2);  // Same texture object
            assert(tex1.use_count() == 2);
            
            cache.printStats();
        }
        
        // Textures destroyed, but cache entries remain (expired)
        cache.printStats();
        
        cache.clearExpired();
        cache.printStats();
        
        std::cout << "✅ Exercise 4 PASSED!\n";
    }
}

// =============================================================================
// Bonus Exercise: Smart Pointer Best Practices
// =============================================================================

void bonus_exercise() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "BONUS: Smart Pointer Best Practices\n";
    std::cout << std::string(60, '=') << "\n";
    
    // TODO: BONUS 1 - Demonstrate passing smart pointers to functions
    // Write functions that:
    // a) Take unique_ptr by value (transfer ownership)
    // b) Take const unique_ptr& (observe without taking ownership)
    // c) Take shared_ptr by value (share ownership)
    // d) Take const shared_ptr& (observe without affecting ref count)
    
    // TODO: BONUS 2 - Convert between smart pointer types
    // Show how to:
    // a) Convert unique_ptr to shared_ptr
    // b) Get raw pointer from smart pointer (for observation)
    // c) Reset a smart pointer
    
    // TODO: BONUS 3 - Custom deleters
    // Create a unique_ptr with custom deleter for:
    // a) FILE* (use fclose)
    // b) Array (use delete[])
    
    std::cout << "💡 Implement bonus exercises for extra practice!\n";
}

// =============================================================================
// Main - Test Runner
// =============================================================================

int main() {
    std::cout << "🎮 Smart Pointers Hands-On Exercise\n";
    std::cout << "Implement all TODO methods to pass the tests!\n";
    
    try {
        // Uncomment tests as you complete each exercise:
        
        // test_exercise_1();  // ⭐ Basic unique_ptr
        // test_exercise_2();  // ⭐⭐ shared_ptr and Inventory
        // test_exercise_3();  // ⭐⭐⭐ weak_ptr and Teams
        // test_exercise_4();  // ⭐⭐⭐ Resource Cache
        // bonus_exercise();   // 🌟 Bonus challenges
        
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "❌ Uncomment tests as you complete each exercise!\n";
        std::cout << std::string(60, '=') << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "\n❌ TEST FAILED: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}

/*
 * 🎯 LEARNING CHECKLIST
 * 
 * After completing this exercise, you should be able to:
 * 
 * ✅ Create and use unique_ptr with make_unique
 * ✅ Transfer ownership with std::move
 * ✅ Understand when unique_ptr is appropriate
 * ✅ Create and use shared_ptr with make_shared
 * ✅ Share ownership between multiple objects
 * ✅ Check reference counts with use_count()
 * ✅ Use weak_ptr to avoid circular references
 * ✅ Lock weak_ptr to access the object safely
 * ✅ Check if weak_ptr is expired
 * ✅ Implement caching with weak_ptr
 * ✅ Understand the differences between all three smart pointer types
 * ✅ Follow smart pointer best practices
 * 
 * 📚 NEXT STEPS:
 * 1. Review your implementations with the solutions
 * 2. Try the bonus exercises
 * 3. Experiment with different scenarios
 * 4. Read about enable_shared_from_this in detail
 * 5. Practice with real-world projects
 */
