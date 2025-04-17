#pragma once

#include "../core/formula.hpp"
#include <vector>
#include <memory>

namespace stalmarck {

class Solver {
public:
    Solver();
    ~Solver();

    // Core algorithm methods
    bool solve(const Formula& formula);
    bool apply_simple_rules(const std::vector<std::tuple<int, int, int>>& formula_triplets, const Formula& formula);
    bool branch_and_solve(int variable, bool value);
    
    // State management
    bool has_contradiction() const;
    bool has_complete_assignment() const;
    bool verify_assignment();
    bool eval_literal(int literal);
    void reset();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace stalmarck 