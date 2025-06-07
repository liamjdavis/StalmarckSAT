# StalmarckSAT

### The development of this project has been migrated to [github.com/Stalmarck-Satisfiability/StalmarckSAT](https://github.com/Stalmarck-Satisfiability/StalmarckSAT).

StalmarckSAT is an implementation of the Stålmarck's method for tautology checking in propositional logic, extended to also handle SAT solving.

## Project Overview

This project provides:
- A C++ library implementing Stålmarck's algorithm
- A command-line interface for solving SAT problems
- Support for DIMACS CNF format input

## Building from Source

### Prerequisites

- C++17 compatible compiler (GCC or Clang recommended)
- CMake 3.10 or higher
- GTest (for running tests)

### Building with CMake

```bash
# Create and enter build directory
mkdir -p build && cd build

# Configure
cmake ..

# Build
cmake --build .
```

### Building with configure script

Alternatively, you can use the provided configure script:

```bash
# Configure the build
./configure

# Build
cd build && make
```

Configuration options:
- `-g`: Enable debug mode
- `-l`: Enable logging
- `-c`: Enable assertion checking
- `-s`: Enable static compilation
- `--builddir=<dir>`: Specify build directory (default: 'build')

## Running the Solver

```bash
# Using a DIMACS CNF file
./build/stalmarck path/to/your/file.cnf

# With verbose output
./build/stalmarck -v path/to/your/file.cnf
```

Command line options:
- `-h, --help`: Display help information
- `-v, --verbose`: Enable verbose output
- `--version`: Display version information

## Using the C++ API

To use StalmarckSAT in your C++ project:

```cpp
#include "stalmarck.hpp"
#include <string>

int main() {
    // Create solver instance
    stalmarck::StalmarckSolver solver;
    
    // Configure solver (optional)
    solver.set_timeout(30.0);  // 30 seconds timeout
    solver.set_verbosity(1);   // Enable verbose output
    
    // Method 1: Solve using a formula string
    std::string formula = "(p ∨ q) ∧ (¬p ∨ ¬q)";
    bool parsed_successfully = solver.solve(formula);
    
    // Method 2: Solve using a pre-parsed Formula object
    stalmarck::Parser parser;
    stalmarck::Formula parsed_formula = parser.parse_dimacs("example.cnf");
    solver.solve(parsed_formula);
    
    // Check if the formula is a tautology
    bool is_taut = solver.is_tautology();
    
    return 0;
}
```

## Contributing

### Setting Up Development Environment

1. Fork and clone the repository:
   ```bash
   git clone https://github.com/yourusername/StalmarckSAT.git
   cd StalmarckSAT
   ```

2. Create a build directory:
   ```bash
   mkdir -p build && cd build
   ```

3. Configure with debug options:
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Debug ..
   ```

4. Build the project and tests:
   ```bash
   cmake --build .
   ```

5. Run the tests:
   ```bash
   ctest
   ```

### Project Structure

- `src/core/`: Core data structures and interfaces
- `src/solver/`: Implementation of Stålmarck's algorithm
- `src/parser/`: Input parsing (formula strings, DIMACS format)
- `src/cli/`: Command-line interface
- `test/unit/`: Unit tests
- `test/integration/`: Integration tests

### Coding Standards

- Use modern C++ features (C++17)
- Follow consistent naming and formatting conventions
- Write unit tests for new functionality
- Document public APIs

## License

This project is licensed under the MIT License - see the LICENSE file for details.
