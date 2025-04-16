#include "solver/solver.hpp"
#include "core/formula.hpp"
#include <vector>
#include <unordered_map>

namespace stalmarck {

class Solver::Impl {
public:
    std::unordered_map<int, bool> assignments;
    bool has_contradiction_flag = false;
    bool has_complete_assignment_flag = false;
};

Solver::Solver() : impl_(std::make_unique<Impl>()) {}
Solver::~Solver() = default;

bool Solver::solve(const Formula& formula) {
    reset();

    std::vector<std::tuple<int, int, int>> triplets = formula.get_triplets();
    
    // First try simple rules
    if (!apply_simple_rules(triplets, formula)) {
        return false;
    }
    
    // If we have a complete assignment without contradiction, we're done
    if (has_complete_assignment()) {
        return !has_contradiction();
    }
    
    // If we have a contradiction, we're done
    if (has_contradiction()) {
        return false;
    }
    
    // Choose an unassigned variable and try both values
    for (size_t i = 1; i <= formula.num_variables(); ++i) {
        if (impl_->assignments.find(i) == impl_->assignments.end()) {
            // Try p = true
            if (branch_and_solve(i, true)) {
                return true;
            }
            
            // Try p = false
            if (branch_and_solve(i, false)) {
                return true;
            }
            
            // If both branches lead to contradiction, formula is unsatisfiable
            return false;
        }
    }
    
    return true;
}

bool Solver::apply_simple_rules(const std::vector<std::tuple<int, int, int>>& formula_triplets, const Formula& formula) {
    bool changed = true;
    
    // Keep applying rules until no more changes are made
    while (changed) {
        changed = false;
        
        // Iterate through all triplets
        for (const auto& triplet : formula_triplets) {
            int x = std::get<0>(triplet);
            int y = std::get<1>(triplet);
            int z = std::get<2>(triplet);
            
            // Get current assignments (if they exist)
            auto x_it = impl_->assignments.find(std::abs(x));
            auto y_it = impl_->assignments.find(std::abs(y));
            auto z_it = impl_->assignments.find(std::abs(z));
            
            bool x_assigned = x_it != impl_->assignments.end();
            bool y_assigned = y_it != impl_->assignments.end();
            bool z_assigned = z_it != impl_->assignments.end();
            
            // Get the actual values (accounting for negation)
            bool x_val = x_assigned ? ((x > 0) == x_it->second) : false;
            bool y_val = y_assigned ? ((y > 0) == y_it->second) : false;
            bool z_val = z_assigned ? ((z > 0) == z_it->second) : false;
            
            // Rule 1: (0,y,z) => y=1, z=0
            if (x_assigned && !x_val) {
                if (!y_assigned) {
                    impl_->assignments[std::abs(y)] = (y > 0);
                    changed = true;
                } else if (y_val != (y > 0)) {
                    impl_->has_contradiction_flag = true;
                    return false;
                }
                
                if (!z_assigned) {
                    impl_->assignments[std::abs(z)] = !(z > 0);
                    changed = true;
                } else if (z_val == (z > 0)) {
                    impl_->has_contradiction_flag = true;
                    return false;
                }
            }
            
            // Rule 2: (x,0,z) => x=1
            if (y_assigned && !y_val) {
                if (!x_assigned) {
                    impl_->assignments[std::abs(x)] = (x > 0);
                    changed = true;
                } else if (x_val != (x > 0)) {
                    impl_->has_contradiction_flag = true;
                    return false;
                }
            }
            
            // Rule 3: (x,y,0) => x=-y (x is the negation of y)
            if (z_assigned && !z_val) {
                if (!x_assigned && !y_assigned) {
                    // Cannot determine values yet
                } else if (x_assigned && !y_assigned) {
                    impl_->assignments[std::abs(y)] = !((y > 0) == x_val);
                    changed = true;
                } else if (!x_assigned && y_assigned) {
                    impl_->assignments[std::abs(x)] = !((x > 0) == y_val);
                    changed = true;
                } else if (x_val == y_val) {
                    impl_->has_contradiction_flag = true;
                    return false;
                }
            }
            
            // Rule 4: (x,y,y) => x=1
            if (std::abs(y) == std::abs(z) && 
                ((y > 0 && z > 0) || (y < 0 && z < 0))) {
                if (!x_assigned) {
                    impl_->assignments[std::abs(x)] = (x > 0);
                    changed = true;
                } else if (x_val != (x > 0)) {
                    impl_->has_contradiction_flag = true;
                    return false;
                }
            }
            
            // Rule 5: (x,y,1) => x=1
            if (z_assigned && z_val) {
                if (!x_assigned) {
                    impl_->assignments[std::abs(x)] = (x > 0);
                    changed = true;
                } else if (x_val != (x > 0)) {
                    impl_->has_contradiction_flag = true;
                    return false;
                }
            }
            
            // Rule 6: (x,1,z) => x=z
            if (y_assigned && y_val) {
                if (!x_assigned && !z_assigned) {
                    // Cannot determine values yet
                } else if (x_assigned && !z_assigned) {
                    impl_->assignments[std::abs(z)] = ((z > 0) == x_val);
                    changed = true;
                } else if (!x_assigned && z_assigned) {
                    impl_->assignments[std::abs(x)] = ((x > 0) == z_val);
                    changed = true;
                } else if (x_val != z_val) {
                    impl_->has_contradiction_flag = true;
                    return false;
                }
            }
            
            // Rule 7: (x,x,z) => x=1, z=1
            if (std::abs(x) == std::abs(y) && 
                ((x > 0 && y > 0) || (x < 0 && y < 0))) {
                if (!x_assigned) {
                    impl_->assignments[std::abs(x)] = (x > 0);
                    changed = true;
                } else if (x_val != (x > 0)) {
                    impl_->has_contradiction_flag = true;
                    return false;
                }
                
                if (!z_assigned) {
                    impl_->assignments[std::abs(z)] = (z > 0);
                    changed = true;
                } else if (z_val != (z > 0)) {
                    impl_->has_contradiction_flag = true;
                    return false;
                }
            }
        }
        
        // Check if we now have a complete assignment
        if (impl_->assignments.size() == formula.num_variables()) {
            impl_->has_complete_assignment_flag = true;
            return true;
        }
    }
    
    return !impl_->has_contradiction_flag;
}

bool Solver::branch_and_solve(int variable, bool value) {
    // TODO: Implement branching and recursive solving
    // Silence unused parameter warnings until implementation is complete
    (void)variable;
    (void)value;
    return true;
}

bool Solver::has_contradiction() const {
    return impl_->has_contradiction_flag;
}

bool Solver::has_complete_assignment() const {
    return impl_->has_complete_assignment_flag;
}

void Solver::reset() {
    impl_->assignments.clear();
    impl_->has_contradiction_flag = false;
    impl_->has_complete_assignment_flag = false;
}

} // namespace stalmarck 