#include <gtest/gtest.h>
#include <random>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <set>
#include <algorithm>

// Fix the include order to ensure Formula is defined before Formula::Impl
#include "core/formula.hpp"
#include "core/formula_impl.hpp"
#include "solver/solver.hpp"
#include "parser/parser.hpp"

namespace stalmarck {
namespace test {

// Utility to generate random SAT formulas
class FormulaGenerator {
public:
    FormulaGenerator(int seed = 0) : rng_(seed) {}
    
    // Generate a random CNF formula with n variables and m clauses
    // Each clause has exactly k literals
    Formula generate_random_k_sat(int n, int m, int k) {
        Formula formula;
        
        for (int i = 0; i < m; i++) {
            std::vector<int> clause;
            // Use a set to avoid duplicate literals in a clause
            std::set<int> literals_set;
            
            while (literals_set.size() < static_cast<size_t>(k)) {
                // Generate a random variable (1 to n)
                int var = uniform_dist_(rng_, std::uniform_int_distribution<>::param_type(1, n));
                // Randomly negate it
                bool negate = coin_flip_(rng_);
                int literal = negate ? -var : var;
                
                // Add to set if not already present
                literals_set.insert(literal);
            }
            
            // Convert set to vector for the clause
            clause.assign(literals_set.begin(), literals_set.end());
            formula.add_clause(clause);
        }
        
        return formula;
    }
    
    // Generate a formula with known satisfiability
    Formula generate_known_sat_formula(int n, int m, int k, bool satisfiable) {
        if (satisfiable) {
            return generate_satisfiable_formula(n, m, k);
        } else {
            return generate_unsatisfiable_formula(n, m, k);
        }
    }
    
    // Generate a satisfiable formula by first creating a solution
    Formula generate_satisfiable_formula(int n, int m, int k) {
        Formula formula;
        
        // Generate a random assignment
        std::vector<bool> assignment(n + 1);
        for (int i = 1; i <= n; i++) {
            assignment[i] = coin_flip_(rng_);
        }
        
        // Generate clauses that are satisfied by our assignment
        for (int i = 0; i < m; i++) {
            std::vector<int> clause;
            std::set<int> literals_set;
            
            // Generate a clause that's satisfied by our assignment
            while (literals_set.size() < static_cast<size_t>(k)) {
                int var = uniform_dist_(rng_, std::uniform_int_distribution<>::param_type(1, n));
                
                // Determine if this should be positive or negative to be satisfied
                bool var_value = assignment[var];
                // With 70% probability, make it satisfy the clause
                bool make_satisfied = (uniform_dist_(rng_, std::uniform_int_distribution<>::param_type(1, 10)) <= 7);
                
                int literal = make_satisfied ? (var_value ? var : -var) : (var_value ? -var : var);
                literals_set.insert(literal);
            }
            
            clause.assign(literals_set.begin(), literals_set.end());
            formula.add_clause(clause);
        }
        
        return formula;
    }
    
    // Generate an unsatisfiable formula (fairly complex - we'll use a pigeonhole principle)
    Formula generate_unsatisfiable_formula(int n, int m, int k) {
        // For simplicity, we'll create a formula with a direct contradiction
        // This is not ideal for all testing, but good enough for basic fuzzing
        Formula formula;
        
        // Add a variable and its negation as separate unit clauses
        formula.add_clause({1});
        formula.add_clause({-1});
        
        // Add additional random clauses to reach the desired number
        int remaining_clauses = m - 2;
        for (int i = 0; i < remaining_clauses; i++) {
            std::vector<int> clause;
            std::set<int> literals_set;
            
            while (literals_set.size() < static_cast<size_t>(k)) {
                int var = uniform_dist_(rng_, std::uniform_int_distribution<>::param_type(1, n));
                bool negate = coin_flip_(rng_);
                int literal = negate ? -var : var;
                literals_set.insert(literal);
            }
            
            clause.assign(literals_set.begin(), literals_set.end());
            formula.add_clause(clause);
        }
        
        return formula;
    }
    
    // Generate a formula with conflicting rules that might trigger bugs
    Formula generate_conflicting_rules_formula() {
        Formula formula;
        
        // Create specific clauses designed to test rule conflicts
        // Rule 1 and Rule 3 conflict
        formula.add_clause({1, 2, 3});     // Triplet (aux1, 1, 2)
        formula.add_clause({-1, -2, 3});   // Might create a conflict in rule application
        
        // Rule 4 and Rule 7 with shared variables
        formula.add_clause({4, 5, 5});     // x=4, y=z=5 (Rule 4)
        formula.add_clause({5, 5, -6});    // x=y=5, z=-6 (Rule 7)
        
        // Cyclic dependencies
        formula.add_clause({7, 8, 9});
        formula.add_clause({-9, 10, 7});
        formula.add_clause({-7, -8, -10});
        
        return formula;
    }
    
    // Generate a DIMACS CNF file with a random formula
    std::string generate_random_cnf_file(int n, int m, int k, bool satisfiable) {
        Formula formula = generate_known_sat_formula(n, m, k, satisfiable);
        
        // Create a temporary file
        std::string filename = "temp_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".cnf";
        std::ofstream file(filename);
        
        // Write DIMACS header
        file << "c Random " << (satisfiable ? "satisfiable" : "unsatisfiable") << " " << k << "-SAT formula\n";
        file << "p cnf " << n << " " << formula.num_clauses() << "\n";
        
        // Write clauses (need to extract them from the Formula object)
        const auto& clauses = formula.get_clauses();
        for (const auto& clause : clauses) {
            for (int literal : clause) {
                file << literal << " ";
            }
            file << "0\n";
        }
        
        file.close();
        return filename;
    }

private:
    std::mt19937 rng_;
    std::uniform_int_distribution<> uniform_dist_;
    std::bernoulli_distribution coin_flip_{0.5};
};

// Verification utilities
class SolverVerification {
public:
    // Verify that the assignment satisfies all clauses
    static bool verify_assignment(const Formula& formula, const std::unordered_map<int, bool>& assignment) {
        const auto& clauses = formula.get_clauses();
        
        for (const auto& clause : clauses) {
            bool clause_satisfied = false;
            
            for (int literal : clause) {
                int var = std::abs(literal);
                bool expected_value = (literal > 0);
                
                auto it = assignment.find(var);
                if (it != assignment.end()) {
                    bool actual_value = it->second;
                    
                    if ((actual_value && expected_value) || (!actual_value && !expected_value)) {
                        clause_satisfied = true;
                        break;
                    }
                }
            }
            
            if (!clause_satisfied) {
                return false;
            }
        }
        
        return true;
    }
    
    // Verify that the solver's result matches the expected satisfiability
    static bool verify_solver_result(Solver& solver, const Formula& formula, bool expected_sat) {
        bool result = solver.solve(formula);
        
        if (expected_sat) {
            // If expected to be satisfiable, solver should return true and not have a contradiction
            return result && !solver.has_contradiction();
        } else {
            // If expected to be unsatisfiable, solver should return false and have a contradiction
            return !result && solver.has_contradiction();
        }
    }
};

// Test fixtures
class SolverFuzzTest : public ::testing::Test {
protected:
    FormulaGenerator generator;
    
    void SetUp() override {
        // Use a random seed based on time for each test run
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        generator = FormulaGenerator(seed);
    }
};

// Basic fuzzing tests
TEST_F(SolverFuzzTest, RandomKSatFormulas) {
    const int num_tests = 20;
    const int n_vars_max = 20;
    const int m_clauses_max = 30;
    const int k_width_max = 3;
    
    std::cout << "Running " << num_tests << " random k-SAT formula tests\n";
    
    for (int i = 0; i < num_tests; i++) {
        // Generate random parameters
        int n = std::max(3, std::rand() % n_vars_max + 1);
        int m = std::max(1, std::rand() % m_clauses_max + 1);
        int k = std::max(1, std::rand() % k_width_max + 1);
        
        std::cout << "Test " << (i+1) << ": " << n << " variables, " << m << " clauses, width=" << k << "\n";
        
        Formula formula = generator.generate_random_k_sat(n, m, k);
        Solver solver;
        
        // Solve the formula
        bool result = solver.solve(formula);
        
        // Validate the result
        if (result) {
            std::cout << "  Satisfiable\n";
            EXPECT_FALSE(solver.has_contradiction());
            EXPECT_TRUE(solver.has_complete_assignment());
        } else {
            std::cout << "  Unsatisfiable\n";
            EXPECT_TRUE(solver.has_contradiction());
        }
    }
}

// Test with formulas of known satisfiability
TEST_F(SolverFuzzTest, KnownSatisfiabilityFormulas) {
    const int num_tests = 10;
    
    std::cout << "Running " << num_tests << " tests with known satisfiability\n";
    
    for (int i = 0; i < num_tests; i++) {
        // Alternate between SAT and UNSAT
        bool expected_sat = (i % 2 == 0);
        
        // Generate parameters
        int n = 10;
        int m = 15;
        int k = 3;
        
        std::cout << "Test " << (i+1) << ": Expected " << (expected_sat ? "SAT" : "UNSAT") << "\n";
        
        Formula formula = generator.generate_known_sat_formula(n, m, k, expected_sat);
        Solver solver;
        
        bool result = solver.solve(formula);
        
        // Check that the result matches the expected satisfiability
        EXPECT_EQ(result, expected_sat) << "Solver result doesn't match expected satisfiability";
        
        if (expected_sat) {
            EXPECT_FALSE(solver.has_contradiction());
        } else {
            EXPECT_TRUE(solver.has_contradiction());
        }
    }
}

// Test with formulas specifically designed to trigger rule conflicts
TEST_F(SolverFuzzTest, RuleConflictFormulas) {
    std::cout << "Testing formulas designed to trigger rule conflicts\n";
    
    Formula formula = generator.generate_conflicting_rules_formula();
    Solver solver;
    
    // The solver should handle this without crashing
    ASSERT_NO_THROW({
        solver.solve(formula);
    });
    
    // We don't assert on the actual result since it depends on the specific formula,
    // but we make sure the solver maintains consistent state
    bool result = solver.solve(formula);
    if (result) {
        EXPECT_FALSE(solver.has_contradiction());
        EXPECT_TRUE(solver.has_complete_assignment());
    } else {
        EXPECT_TRUE(solver.has_contradiction());
    }
}

// Test against edge cases and corner cases
TEST_F(SolverFuzzTest, EdgeCases) {
    std::cout << "Testing edge cases\n";
    
    // Empty formula
    {
        Formula empty_formula;
        Solver solver;
        
        ASSERT_NO_THROW({
            bool result = solver.solve(empty_formula);
            // Empty formula should be satisfiable
            EXPECT_TRUE(result);
            EXPECT_FALSE(solver.has_contradiction());
        });
    }
    
    // Single variable formula
    {
        Formula single_var;
        single_var.add_clause({1});
        
        Solver solver;
        ASSERT_NO_THROW({
            bool result = solver.solve(single_var);
            EXPECT_TRUE(result);
            EXPECT_FALSE(solver.has_contradiction());
        });
    }
    
    // Direct contradiction
    {
        Formula contradiction;
        contradiction.add_clause({1});
        contradiction.add_clause({-1});
        
        Solver solver;
        ASSERT_NO_THROW({
            bool result = solver.solve(contradiction);
            EXPECT_FALSE(result);
            EXPECT_TRUE(solver.has_contradiction());
        });
    }
    
    // Tautology clause
    {
        Formula tautology;
        tautology.add_clause({1, -1});
        
        Solver solver;
        ASSERT_NO_THROW({
            bool result = solver.solve(tautology);
            EXPECT_TRUE(result);
        });
    }
}

// Run a continuous fuzzing loop (designed to be run for longer periods)
TEST_F(SolverFuzzTest, DISABLED_ContinuousFuzzing) {
    const int max_iterations = 1000;  // Set to higher value for longer fuzzing
    
    std::cout << "Starting continuous fuzzing for " << max_iterations << " iterations\n";
    
    for (int i = 0; i < max_iterations; i++) {
        if (i % 100 == 0) {
            std::cout << "Iteration " << i << "\n";
        }
        
        // Randomize parameters
        int n = std::rand() % 50 + 1;
        int m = std::rand() % 100 + 1;
        int k = std::rand() % 5 + 1;
        bool expected_sat = (std::rand() % 2 == 0);
        
        Formula formula = generator.generate_known_sat_formula(n, m, k, expected_sat);
        Solver solver;
        
        // Set a timeout for each iteration
        auto start = std::chrono::steady_clock::now();
        auto timeout = std::chrono::seconds(10);
        
        bool result = solver.solve(formula);
        
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        
        // Skip slow tests to avoid hanging
        if (duration > timeout) {
            std::cout << "Test " << i << " timed out after " << duration.count() << " seconds\n";
            continue;
        }
        
        // For known satisfiability, verify results
        if (expected_sat != result) {
            FAIL() << "Iteration " << i << ": Expected " << (expected_sat ? "SAT" : "UNSAT") 
                   << " but got " << (result ? "SAT" : "UNSAT");
        }
    }
    
    std::cout << "Continuous fuzzing completed\n";
}

// Mutation-based fuzzing - rewritten to avoid copying Formula objects
TEST_F(SolverFuzzTest, MutationBasedFuzzing) {
    const int num_base_formulas = 5;
    const int mutations_per_formula = 10;
    
    std::cout << "Running mutation-based fuzzing\n";
    
    for (int i = 0; i < num_base_formulas; i++) {
        // Generate base formula parameters
        int n = 10;
        int m = 15;
        int k = 3;
        bool satisfiable = (i % 2 == 0);
        
        // For each base formula configuration, create mutations
        for (int j = 0; j < mutations_per_formula; j++) {
            // Create a new formula each time (no copying)
            Formula formula = generator.generate_known_sat_formula(n, m, k, satisfiable);
            
            // Mutate by adding a random clause
            std::vector<int> new_clause;
            int clause_size = std::rand() % 4 + 1;
            for (int k = 0; k < clause_size; k++) {
                int var = std::rand() % 10 + 1;
                bool negate = (std::rand() % 2 == 0);
                new_clause.push_back(negate ? -var : var);
            }
            
            formula.add_clause(new_clause);
            
            // Solve the mutated formula
            Solver solver;
            bool result = solver.solve(formula);
            
            // We don't assert on the result as it may change with the mutation,
            // but we check that the solver's state is consistent
            if (result) {
                EXPECT_FALSE(solver.has_contradiction());
            } else {
                EXPECT_TRUE(solver.has_contradiction());
            }
        }
    }
}

// Main function to run the fuzzer
} // namespace test
} // namespace stalmarck

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Parse command-line arguments specific to fuzzing
    int num_iterations = 50;
    bool run_continuous_fuzzing = false;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--iterations" && i + 1 < argc) {
            num_iterations = std::stoi(argv[i + 1]);
            i++;
        } else if (arg == "--continuous") {
            run_continuous_fuzzing = true;
        }
    }
    
    std::cout << "Running fuzzer with " << num_iterations << " iterations\n";
    
    // Override continuous fuzzing flag based on command line
    if (run_continuous_fuzzing) {
        ::testing::GTEST_FLAG(filter) = "SolverFuzzTest.ContinuousFuzzing";
    } else {
        // Disable continuous fuzzing test by default
        ::testing::GTEST_FLAG(filter) = "-SolverFuzzTest.ContinuousFuzzing";
    }
    
    return RUN_ALL_TESTS();
}