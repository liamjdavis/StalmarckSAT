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
        return Formula{};  // Return empty formula
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == 'c' || line[0] == 'p') {
            continue; // Skip comments and problem line
        }
        
        std::vector<int> clause;
        std::istringstream iss(line);
        int lit;
        while (iss >> lit && lit != 0) {
            clause.push_back(lit);
        }
        if (!clause.empty()) {
            formula.add_clause(clause);
        }
    }
    
    return formula;  // Let the compiler handle the move
}

Formula Parser::parse_formula(const std::string& formula_str) {
    Formula formula;
    // TODO: Implement formula string parsing
    // This will need to handle the actual formula syntax
    (void)formula_str;  // Silence unused parameter warning
    return formula;  // Let the compiler handle the move
}

bool Parser::has_error() const {
    return impl_->has_error_flag;
}

std::string Parser::get_error() const {
    return impl_->error_message;
}

} // namespace stalmarck 