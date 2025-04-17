#include "parser/parser.hpp"
#include "core/formula.hpp"
#include "core/formula_impl.hpp"
#include <fstream>
#include <sstream>
#include <string>

namespace stalmarck {

class Parser::Impl {
public:
    std::string error_message;
    bool has_error_flag = false;
};

Parser::Parser() : impl_(std::make_unique<Impl>()) {}
Parser::~Parser() = default;

Formula Parser::parse_dimacs(const std::string& filename) {
    Formula formula;
    std::ifstream file(filename);
    if (!file.is_open()) {
        impl_->error_message = "Could not open file: " + filename;
        impl_->has_error_flag = true;
        return Formula{};
    }
    
    // Track maximum variable number for validation
    int num_vars = 0;
    std::string line;
    
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == 'c') {
            continue;
        }
        
        // Parse problem line
        if (line[0] == 'p') {
            std::istringstream iss(line);
            std::string p, cnf;
            iss >> p >> cnf >> num_vars;
            if (p != "p" || cnf != "cnf") {
                impl_->error_message = "Invalid problem line format";
                impl_->has_error_flag = true;
                return Formula{};
            }
            continue;
        }
        
        // Parse clause
        std::vector<int> clause;
        std::istringstream iss(line);
        int lit;
        while (iss >> lit && lit != 0) {
            // Validate literal is within bounds
            if (std::abs(lit) > num_vars) {
                impl_->error_message = "Variable number exceeds declared maximum";
                impl_->has_error_flag = true;
                return Formula{};
            }
            clause.push_back(lit);
        }
        
        if (!clause.empty()) {
            formula.add_clause(clause);
        }
    }
    
    return formula;
}

bool Parser::has_error() const {
    return impl_->has_error_flag;
}

std::string Parser::get_error() const {
    return impl_->error_message;
}

} // namespace stalmarck