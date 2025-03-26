#include "../core/stalmarck.hpp"
#include <iostream>
#include <string>
#include <cstring>

// Version information
#ifndef NDEBUG
#define BUILDTYPE "debug"
#else
#define BUILDTYPE "release"
#endif

#ifdef LOGGING
#define LOGGING_INFO " logging"
#else
#define LOGGING_INFO ""
#endif

#ifdef ENABLE_ASSERTIONS
#define ASSERTIONS_INFO " assertions"
#else
#define ASSERTIONS_INFO ""
#endif

#define VERSION "1.0.0"
#define COMPILER "g++"
#define BUILD (BUILDTYPE LOGGING_INFO ASSERTIONS_INFO)

void banner() {
    std::cout << "c StalmarckSAT SAT Solver Version " << VERSION << std::endl;
    std::cout << "c " << COMPILER << " " << BUILD << std::endl;
}

void usage(const char* name) {
    std::cout << "usage: " << name << " [<option> ...] [<input>]" << std::endl;
    std::cout << std::endl;
    std::cout << "where <option> is one of the following:" << std::endl;
    std::cout << std::endl;
    std::cout << "  -h | --help     print this command line summary" << std::endl;
    std::cout << "  -v | --verbose  enable verbose output" << std::endl;
    std::cout << "  --version       print version and exit" << std::endl;
    std::cout << std::endl;
    std::cout << "and <input> is either a DIMACS file or '-' for stdin" << std::endl;
}

int main(int argc, char* argv[]) {
    bool verbose = false;
    std::string filename;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        const std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            usage(argv[0]);
            return 0;
        } else if (arg == "--version") {
            banner();
            return 0;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg[0] == '-' && arg.length() > 1) {
            std::cerr << "error: invalid option '" << arg << "'" << std::endl;
            return 1;
        } else {
            if (!filename.empty()) {
                std::cerr << "error: multiple input files not supported" << std::endl;
                return 1;
            }
            filename = arg;
        }
    }

    // Print banner for non-quiet mode
    banner();

    try {
        stalmarck::StalmarckSolver solver;
        if (verbose) {
            solver.set_verbosity(1);
        }

        // Handle input
        if (filename.empty() || filename == "-") {
            std::cerr << "error: reading from stdin not implemented yet" << std::endl;
            return 1;
        }

        bool result = solver.solve(filename);
        
        // Print result in DIMACS format
        std::cout << "s " << (result ? "SATISFIABLE" : "UNSATISFIABLE") << std::endl;
        return result ? 10 : 20;  // Following SAT competition exit codes
        
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }
} 