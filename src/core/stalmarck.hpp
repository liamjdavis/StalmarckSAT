#pragma once

#include <vector>
#include <memory>
#include <string>
#include "formula.hpp"

namespace stalmarck {

class StalmarckSolver {
public:
    StalmarckSolver();
    ~StalmarckSolver();

    // Main interface methods
    bool solve(const std::string& filename); // Changed from formula to filename
    bool solve(const Formula& formula);
    bool is_tautology() const;
    
    // Configuration methods
    void set_timeout(double seconds);
    void set_verbosity(int level);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace stalmarck