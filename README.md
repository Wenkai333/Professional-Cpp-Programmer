# Professional C++ Programmer

**My structured journey to move beyond basic C++ usage and master professional development practices.**

## 🎯 Learning Goals

Becoming a professional C++ developer who can:
- Write efficient, maintainable, and safe C++ code
- Master memory management and resource handling  
- Use modern C++ features effectively
- Design scalable software architectures
- Debug and optimize performance-critical applications

Extra:
- Integrated **Claude AI** as a learning accelerator and development productivity tool

## 📁 Repository Structure

- **`Source/`** - Reference material for building the study
- **`Project/`** - Hands-on projects and implementations
- **`Notes/`** - Learning notes, summaries, and insights

## 📈 Current Progress

### Tier 1: Core Professional Competencies (In Progress)
- [x] Development environment setup (Explore Linux/macOS development environments)
- [x] Git/Github knowledge recall and environment setup
- [x] C++ project template creation
- [ ] **Memory Management & Resource Handling** ← `Current Focus`
  - [ ] RAII (Resource Acquisition Is Initialization)
  - [ ] Smart Pointers (unique_ptr, shared_ptr, weak_ptr)
  - [ ] Move Semantics & Perfect Forwarding
  - [ ] Custom Allocators
  - [ ] Memory Debugging Tools
- [ ] Modern C++ Standards (C++11/14/17/20/23)
- [ ] Object-Oriented Programming (Advanced)
- [ ] Template System (Essential)

*`To be continue...`*

## 🛠️ Development Environment

- **OS**: macOS (Apple Silicon)
- **Compiler**: LLVM Clang (C++20/23)
- **Build System**: CMake + Ninja
- **IDE**: VS Code with C++ extensions
- **Debugger**: LLDB
- **Version Control**: Git + GitHub
- **Package Manager**: Conan (when needed)

### Books

- *`<blank>`*

### Online Resources

- *`<blank>`*

## 🚀 Getting Started

### For New Study Projects Example
```bash
# Navigate to projects directory
cd Project/

# Create new study project (uses our custom template)
new-cpp-ng NewProject

# Work on studies...
cd NewProject
./scripts/build.sh
code .

# Save progress
cd ~/workspace/Professional-Cpp-Programmer/
git add .
git commit -m "Day 1: Task"
git push origin main