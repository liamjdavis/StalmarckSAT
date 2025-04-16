#pragma once

#include "../core/formula.hpp"
#include <string>

namespace stalmarck {

class Parser {
public:
    Parser();
    ~Parser();

    // Parsing methods
    Formula parse_dimacs(const std::string& filename);
    
    // Error handling
    bool has_error() const;
    std::string get_error() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace stalmarck 