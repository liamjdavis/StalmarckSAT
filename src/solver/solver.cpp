#include "solver/solver.hpp"
#include "core/formula.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream> // For debug output
#include <sstream>  // For string formatting

namespace stalmarck {

// Debug helper function to print assignments
std::string print_assignments(const std::unordered_map<int, bool>& assignments) {
    std::stringstream ss;
    ss << "{";
    bool first = true;
    for (const auto& [var, val] : assignments) {
        if (!first) ss << ", ";
        ss << var << ":" << (val ? "T" : "F");
        first = false;
    }
    ss << "}";
    return ss.str();
}

// Debug helper to print a triplet
std::string triplet_to_string(const std::tuple<int, int, int>& triplet) {
    int x = std::get<0>(triplet);
    int y = std::get<1>(triplet);
    int z = std::get<2>(triplet);
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
}

class Solver::Impl {
public:
    std::unordered_map<int, bool> assignments;
    bool has_contradiction_flag = false;
    bool has_complete_assignment_flag = false;
    std::vector<std::tuple<int, int, int>> current_triplets;
    size_t current_num_variables = 0;
};

Solver::Solver() : impl_(std::make_unique<Impl>()) {}
Solver::~Solver() = default;

bool Solver::solve(const Formula& formula) {
    std::cout << "\n[SOLVER] Starting to solve formula with " << formula.num_variables() << " variables" << std::endl;
    
    // Reset state at the beginning
    reset();
    
    // Check for direct contradictions in unit clauses
    const auto& clauses = formula.get_clauses(); // You'll need to add this accessor method to your Formula class
    
    std::cout << "[SOLVER] Checking " << clauses.size() << " clauses for unit contradictions" << std::endl;
    
    // Build a set of unit clauses for quick contradiction checking
    std::unordered_set<int> unit_clauses;
    for (const auto& clause : clauses) {
        if (clause.size() == 1) {
            int lit = clause[0];
            std::cout << "[SOLVER] Found unit clause: " << lit << std::endl;
            // Check if this contradicts an existing unit clause
            if (unit_clauses.find(-lit) != unit_clauses.end()) {
                std::cout << "[SOLVER] Contradiction found in unit clauses: " << lit << " and " << -lit << std::endl;
                impl_->has_contradiction_flag = true;
                return false;
            }
            unit_clauses.insert(lit);
        }
    }
    
    std::vector<std::tuple<int, int, int>> triplets = formula.get_triplets();
    
    std::cout << "[SOLVER] Formula has " << triplets.size() << " triplets" << std::endl;
    
    // Store the triplets and formula size for branching
    impl_->current_triplets = triplets;
    impl_->current_num_variables = formula.num_variables();
    
    // First try simple rules
    std::cout << "[SOLVER] Applying simple rules" << std::endl;
    if (!apply_simple_rules(triplets, formula)) {
        // A contradiction was detected during simple rule application
        std::cout << "[SOLVER] Contradiction detected during simple rule application" << std::endl;
        impl_->has_contradiction_flag = true;
        return false;
    }
    
    // If we have a complete assignment without contradiction, we're done
    if (has_complete_assignment()) {
        std::cout << "[SOLVER] Found complete assignment: " << print_assignments(impl_->assignments) << std::endl;
        return !has_contradiction();
    }
    
    // If we already have a contradiction, we're done
    if (has_contradiction()) {
        std::cout << "[SOLVER] Contradiction detected, formula is unsatisfiable" << std::endl;
        return false;
    }
    
    std::cout << "[SOLVER] Simple rules insufficient, starting branching" << std::endl;
    
    // Choose an unassigned variable and try both values
    for (size_t i = 1; i <= formula.num_variables(); ++i) {
        if (impl_->assignments.find(i) == impl_->assignments.end()) {
            std::cout << "[SOLVER] Branching on variable " << i << std::endl;
            
            // Try p = true
            std::cout << "[SOLVER] Trying " << i << " = TRUE" << std::endl;
            if (branch_and_solve(i, true)) {
                std::cout << "[SOLVER] Found satisfying assignment with " << i << " = TRUE" << std::endl;
                return true;
            }
            
            // Try p = false
            std::cout << "[SOLVER] Trying " << i << " = FALSE" << std::endl;
            if (branch_and_solve(i, false)) {
                std::cout << "[SOLVER] Found satisfying assignment with " << i << " = FALSE" << std::endl;
                return true;
            }
            
            // If both branches lead to contradiction, formula is unsatisfiable
            std::cout << "[SOLVER] Both branches for variable " << i << " lead to contradiction" << std::endl;
            impl_->has_contradiction_flag = true;
            return false;
        }
    }
    
    bool result = !has_contradiction();
    std::cout << "[SOLVER] Final result: " << (result ? "Satisfiable" : "Unsatisfiable") << std::endl;
    return result;
}

bool Solver::apply_simple_rules(const std::vector<std::tuple<int, int, int>>& formula_triplets, const Formula& formula) {
    std::cout << "[APPLY_RULES] Starting application of simple rules" << std::endl;
    std::cout << "[APPLY_RULES] Current assignments: " << print_assignments(impl_->assignments) << std::endl;
    
    bool changed = true;
    int iteration = 0;
    
    // Keep applying rules until no more changes are made
    while (changed) {
        changed = false;
        iteration++;
        std::cout << "[APPLY_RULES] Iteration " << iteration << std::endl;
        
        // Iterate through all triplets
        for (const auto& triplet : formula_triplets) {
            int x = std::get<0>(triplet);
            int y = std::get<1>(triplet);
            int z = std::get<2>(triplet);
            
            std::cout << "[APPLY_RULES] Processing triplet " << triplet_to_string(triplet) << std::endl;
            
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
            
            std::cout << "[APPLY_RULES] Variables: "
                      << "x(" << x << ")=" << (x_assigned ? (x_val ? "1" : "0") : "?") << ", "
                      << "y(" << y << ")=" << (y_assigned ? (y_val ? "1" : "0") : "?") << ", "
                      << "z(" << z << ")=" << (z_assigned ? (z_val ? "1" : "0") : "?") << std::endl;
            
            // Rule 1: (0,y,z) => y=1, z=0
            if (x_assigned && !x_val) {
                std::cout << "[APPLY_RULES] Rule 1 applicable: (0,y,z) => y=1, z=0" << std::endl;
                if (!y_assigned) {
                    impl_->assignments[std::abs(y)] = (y > 0);
                    std::cout << "[APPLY_RULES] Setting |" << y << "| = " << (y > 0) << std::endl;
                    changed = true;
                } else if (y_val != (y > 0)) {
                    std::cout << "[APPLY_RULES] Contradiction in Rule 1: y=" << y_val << " but should be " << (y > 0) << std::endl;
                    impl_->has_contradiction_flag = true;
                    return false;
                }
                
                if (!z_assigned) {
                    impl_->assignments[std::abs(z)] = !(z > 0);
                    std::cout << "[APPLY_RULES] Setting |" << z << "| = " << !(z > 0) << std::endl;
                    changed = true;
                } else if (z_val == (z > 0)) {
                    std::cout << "[APPLY_RULES] Contradiction in Rule 1: z=" << z_val << " but should be " << !(z > 0) << std::endl;
                    impl_->has_contradiction_flag = true;
                    return false;
                }
            }
            
            // Rule 2: (x,0,z) => x=1
            if (y_assigned && !y_val) {
                std::cout << "[APPLY_RULES] Rule 2 applicable: (x,0,z) => x=1" << std::endl;
                if (!x_assigned) {
                    impl_->assignments[std::abs(x)] = (x > 0);
                    std::cout << "[APPLY_RULES] Setting |" << x << "| = " << (x > 0) << std::endl;
                    changed = true;
                } else if (x_val != (x > 0)) {
                    std::cout << "[APPLY_RULES] Contradiction in Rule 2: x=" << x_val << " but should be " << (x > 0) << std::endl;
                    impl_->has_contradiction_flag = true;
                    return false;
                }
            }
            
            // Rule 3: (x,y,0) => x=-y (x is the negation of y)
            if (z_assigned && !z_val) {
                std::cout << "[APPLY_RULES] Rule 3 applicable: (x,y,0) => x=-y" << std::endl;
                if (!x_assigned && !y_assigned) {
                    std::cout << "[APPLY_RULES] Cannot determine values yet for Rule 3" << std::endl;
                    // Cannot determine values yet
                } else if (x_assigned && !y_assigned) {
                    impl_->assignments[std::abs(y)] = !((y > 0) == x_val);
                    std::cout << "[APPLY_RULES] Setting |" << y << "| = " << !((y > 0) == x_val) << std::endl;
                    changed = true;
                } else if (!x_assigned && y_assigned) {
                    impl_->assignments[std::abs(x)] = !((x > 0) == y_val);
                    std::cout << "[APPLY_RULES] Setting |" << x << "| = " << !((x > 0) == y_val) << std::endl;
                    changed = true;
                } else if (x_val == y_val) {
                    std::cout << "[APPLY_RULES] Contradiction in Rule 3: x=" << x_val << " and y=" << y_val << " must be opposites" << std::endl;
                    impl_->has_contradiction_flag = true;
                    return false;
                }
            }
            
            // Rule 4: (x,y,y) => x=1
            if (std::abs(y) == std::abs(z) && 
                ((y > 0 && z > 0) || (y < 0 && z < 0))) {
                std::cout << "[APPLY_RULES] Rule 4 applicable: (x,y,y) => x=1" << std::endl;
                if (!x_assigned) {
                    impl_->assignments[std::abs(x)] = (x > 0);
                    std::cout << "[APPLY_RULES] Setting |" << x << "| = " << (x > 0) << std::endl;
                    changed = true;
                } else if (x_val != (x > 0)) {
                    std::cout << "[APPLY_RULES] Contradiction in Rule 4: x=" << x_val << " but should be " << (x > 0) << std::endl;
                    impl_->has_contradiction_flag = true;
                    return false;
                }
            }
            
            // Rule 5: (x,y,1) => x=1
            if (z_assigned && z_val) {
                std::cout << "[APPLY_RULES] Rule 5 applicable: (x,y,1) => x=1" << std::endl;
                if (!x_assigned) {
                    impl_->assignments[std::abs(x)] = (x > 0);
                    std::cout << "[APPLY_RULES] Setting |" << x << "| = " << (x > 0) << std::endl;
                    changed = true;
                } else if (x_val != (x > 0)) {
                    std::cout << "[APPLY_RULES] Contradiction in Rule 5: x=" << x_val << " but should be " << (x > 0) << std::endl;
                    impl_->has_contradiction_flag = true;
                    return false;
                }
            }
            
            // Rule 6: (x,1,z) => x=z
            if (y_assigned && y_val) {
                std::cout << "[APPLY_RULES] Rule 6 applicable: (x,1,z) => x=z" << std::endl;
                if (!x_assigned && !z_assigned) {
                    std::cout << "[APPLY_RULES] Cannot determine values yet for Rule 6" << std::endl;
                    // Cannot determine values yet
                } else if (x_assigned && !z_assigned) {
                    impl_->assignments[std::abs(z)] = ((z > 0) == x_val);
                    std::cout << "[APPLY_RULES] Setting |" << z << "| = " << ((z > 0) == x_val) << std::endl;
                    changed = true;
                } else if (!x_assigned && z_assigned) {
                    impl_->assignments[std::abs(x)] = ((x > 0) == z_val);
                    std::cout << "[APPLY_RULES] Setting |" << x << "| = " << ((x > 0) == z_val) << std::endl;
                    changed = true;
                } else if (x_val != z_val) {
                    std::cout << "[APPLY_RULES] Contradiction in Rule 6: x=" << x_val << " but should equal z=" << z_val << std::endl;
                    impl_->has_contradiction_flag = true;
                    return false;
                }
            }
            
            // Rule 7: (x,x,z) => x=1, z=1
            if (std::abs(x) == std::abs(y) && 
                ((x > 0 && y > 0) || (x < 0 && y < 0))) {
                std::cout << "[APPLY_RULES] Rule 7 applicable: (x,x,z) => x=1, z=1" << std::endl;
                if (!x_assigned) {
                    impl_->assignments[std::abs(x)] = (x > 0);
                    std::cout << "[APPLY_RULES] Setting |" << x << "| = " << (x > 0) << std::endl;
                    changed = true;
                } else if (x_val != (x > 0)) {
                    std::cout << "[APPLY_RULES] Contradiction in Rule 7: x=" << x_val << " but should be " << (x > 0) << std::endl;
                    impl_->has_contradiction_flag = true;
                    return false;
                }
                
                if (!z_assigned) {
                    impl_->assignments[std::abs(z)] = (z > 0);
                    std::cout << "[APPLY_RULES] Setting |" << z << "| = " << (z > 0) << std::endl;
                    changed = true;
                } else if (z_val != (z > 0)) {
                    std::cout << "[APPLY_RULES] Contradiction in Rule 7: z=" << z_val << " but should be " << (z > 0) << std::endl;
                    impl_->has_contradiction_flag = true;
                    return false;
                }
            }
        }
        
        std::cout << "[APPLY_RULES] After iteration " << iteration << ": " << print_assignments(impl_->assignments) << std::endl;
        
        // Check if we now have a complete assignment
        if (impl_->assignments.size() == formula.num_variables()) {
            std::cout << "[APPLY_RULES] Found complete assignment" << std::endl;
            impl_->has_complete_assignment_flag = true;
            return true;
        }
    }
    
    std::cout << "[APPLY_RULES] No more changes possible. Contradiction: " << (impl_->has_contradiction_flag ? "Yes" : "No") << std::endl;
    return !impl_->has_contradiction_flag;
}

bool Solver::branch_and_solve(int variable, bool value) {
    std::cout << "[BRANCH] Branching on variable " << variable << " = " << (value ? "TRUE" : "FALSE") << std::endl;
    
    // Save the current state before branching
    auto saved_assignments = impl_->assignments;
    bool saved_contradiction = impl_->has_contradiction_flag;
    bool saved_complete_assignment = impl_->has_complete_assignment_flag;
    
    // Set the variable to the given value
    impl_->assignments[variable] = value;
    std::cout << "[BRANCH] Set variable " << variable << " = " << value << ", now have " 
              << print_assignments(impl_->assignments) << std::endl;
    
    // Create a temporary formula to pass to apply_simple_rules
    Formula temp_formula;
    
    // Apply simple rules with the new assignment
    std::cout << "[BRANCH] Applying simple rules after assignment" << std::endl;
    if (!apply_simple_rules(impl_->current_triplets, temp_formula)) {
        // This branch leads to a contradiction
        std::cout << "[BRANCH] Contradiction found after applying rules" << std::endl;
        // Restore the state before returning
        impl_->assignments = saved_assignments;
        impl_->has_contradiction_flag = saved_contradiction;
        impl_->has_complete_assignment_flag = saved_complete_assignment;
        return false;
    }
    
    // Check if we now have a complete assignment without contradiction
    if (has_complete_assignment() && !has_contradiction()) {
        // Verify this assignment actually satisfies the formula
        bool satisfies = verify_assignment();
        if (satisfies) {
            std::cout << "[BRANCH] Found complete satisfying assignment: " << print_assignments(impl_->assignments) << std::endl;
            return true;
        } else {
            std::cout << "[BRANCH] Complete assignment found but doesn't satisfy formula" << std::endl;
            impl_->has_contradiction_flag = true;
            impl_->assignments = saved_assignments;
            impl_->has_contradiction_flag = saved_contradiction;
            impl_->has_complete_assignment_flag = saved_complete_assignment;
            return false;
        }
    }
    
    // Need to continue branching on other variables
    for (size_t i = 1; i <= impl_->current_num_variables; ++i) {
        if (i != variable && impl_->assignments.find(i) == impl_->assignments.end()) {
            std::cout << "[BRANCH] Found unassigned variable " << i << ", continuing branching" << std::endl;
            
            // Try TRUE branch
            std::cout << "[BRANCH] Trying nested branch " << i << " = TRUE" << std::endl;
            bool true_branch = branch_and_solve(i, true);
            if (true_branch) {
                std::cout << "[BRANCH] Nested branch " << i << " = TRUE successful" << std::endl;
                return true;
            }
            
            // Try FALSE branch 
            std::cout << "[BRANCH] Trying nested branch " << i << " = FALSE" << std::endl;
            bool false_branch = branch_and_solve(i, false);
            if (false_branch) {
                std::cout << "[BRANCH] Nested branch " << i << " = FALSE successful" << std::endl;
                return true;
            }
            
            // If both branches failed, this path is unsatisfiable
            std::cout << "[BRANCH] Both nested branches for " << i << " failed" << std::endl;
            impl_->has_contradiction_flag = true;
            // Restore state before returning
            impl_->assignments = saved_assignments;
            impl_->has_contradiction_flag = saved_contradiction;
            impl_->has_complete_assignment_flag = saved_complete_assignment;
            return false;
        }
    }
    
    // If no unassigned variables found and we get here, we have a complete assignment
    if (impl_->assignments.size() >= impl_->current_num_variables && !has_contradiction()) {
        // Verify this assignment actually satisfies the formula
        bool satisfies = verify_assignment();
        if (satisfies) {
            std::cout << "[BRANCH] No more unassigned variables, complete assignment found: " 
                    << print_assignments(impl_->assignments) << std::endl;
            impl_->has_complete_assignment_flag = true;
            return true;
        } else {
            std::cout << "[BRANCH] Complete assignment found but doesn't satisfy formula" << std::endl;
            impl_->has_contradiction_flag = true;
            impl_->assignments = saved_assignments;
            impl_->has_contradiction_flag = saved_contradiction;
            impl_->has_complete_assignment_flag = saved_complete_assignment;
            return false;
        }
    }
    
    // Otherwise, restore state and return false
    std::cout << "[BRANCH] Branch failed, restoring state" << std::endl;
    impl_->assignments = saved_assignments;
    impl_->has_contradiction_flag = saved_contradiction;
    impl_->has_complete_assignment_flag = saved_complete_assignment;
    return false;
}

bool Solver::has_contradiction() const {
    return impl_->has_contradiction_flag;
}

bool Solver::has_complete_assignment() const {
    return impl_->has_complete_assignment_flag;
}

void Solver::reset() {
    std::cout << "[SOLVER] Resetting solver state" << std::endl;
    impl_->assignments.clear();
    impl_->has_contradiction_flag = false;
    impl_->has_complete_assignment_flag = false;
}

bool Solver::verify_assignment() {
    std::cout << "[VERIFY] Verifying assignment: " << print_assignments(impl_->assignments) << std::endl;

    // Check each triplet to ensure it's satisfied
    for (const auto& triplet : impl_->current_triplets) {
        int x = std::get<0>(triplet);
        int y = std::get<1>(triplet);
        int z = std::get<2>(triplet);
        
        // Get the actual Boolean values for each variable
        bool x_val = eval_literal(x);
        bool y_val = eval_literal(y);
        bool z_val = eval_literal(z);
        
        // A triplet encodes (x ↔ (y ∧ z)), which is satisfied if x = (y && z)
        bool triplet_satisfied = (x_val == (y_val && z_val));
        
        std::cout << "[VERIFY] Triplet " << triplet_to_string(triplet) << ": "
                  << "x=" << x_val << ", y=" << y_val << ", z=" << z_val 
                  << " => " << (triplet_satisfied ? "Satisfied" : "Not satisfied") << std::endl;
        
        if (!triplet_satisfied) {
            std::cout << "[VERIFY] Assignment doesn't satisfy all triplets" << std::endl;
            return false;
        }
    }
    
    std::cout << "[VERIFY] Assignment satisfies all triplets" << std::endl;
    return true;
}

bool Solver::eval_literal(int literal) {
    // Get the variable's assignment, respecting the sign
    int var = std::abs(literal);
    bool var_value = impl_->assignments[var];
    
    // If the literal is negative, negate the value
    return (literal > 0) ? var_value : !var_value;
}

} // namespace stalmarck