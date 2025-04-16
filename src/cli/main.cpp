#include "../core/stalmarck.hpp"
#include "../parser/parser.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <cnf-file>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    
    try {
        stalmarck::Parser parser;
        stalmarck::Formula formula = parser.parse_dimacs(filename);
        
        if (parser.has_error()) {
            std::cerr << "Error parsing file: " << parser.get_error() << std::endl;
            return 1;
        }

        stalmarck::StalmarckSolver solver;
        bool success = solver.solve(formula);
        
        if (!success) {
            std::cerr << "Error during solving" << std::endl;
            return 1;
        }

        // Print result
        std::cout << (solver.is_tautology() ? "SAT" : "UNSAT") << std::endl;
        return solver.is_tautology() ? 10 : 20;  // Standard SAT solver exit codes

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}