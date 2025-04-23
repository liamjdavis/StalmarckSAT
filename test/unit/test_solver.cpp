#include <gtest/gtest.h>
#include "solver/solver.hpp"
#include "core/formula.hpp"

namespace stalmarck {
namespace test {

// Test initialization
TEST(SolverTests, Initialization) {
    Solver solver;
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_FALSE(solver.has_complete_assignment());
}

// Test Rule 1: (0,y,z) => y=1, z=0
TEST(SolverTests, rule1Basic) {
    Solver solver;
    Formula formula;

    formula.add_clause({0, 1, 2});
    formula.add_clause({-1, 2});

    const auto& triplets = formula.get_triplets();

    EXPECT_TRUE(solver.apply_simple_rules(triplets, formula));
    EXPECT_FALSE(solver.has_contradiction());
}

// Test Rule 2: (x,0,z) => x=1
TEST(SolverTests, rule2Basic) {
    Solver solver;
    Formula formula;

    formula.add_clause({1, 0, 2});
    formula.add_clause({-1, 2});

    const auto& triplets = formula.get_triplets();

    EXPECT_TRUE(solver.apply_simple_rules(triplets, formula));
    EXPECT_FALSE(solver.has_contradiction());
}

// Test Rule 3: (x,y,0) => x=-y
TEST(SolverTests, rule3Basic) {
    Solver solver;
    Formula formula;

    formula.add_clause({1, 2, 0});
    formula.add_clause({-1, 2});

    const auto& triplets = formula.get_triplets();

    EXPECT_TRUE(solver.apply_simple_rules(triplets, formula));
    EXPECT_FALSE(solver.has_contradiction());
}

// Test Rule 4: (x,y,y) => x=1
TEST(SolverTests, rule4Basic) {
    Solver solver;
    Formula formula;

    formula.add_clause({1, 2, 2});
    formula.add_clause({-1, 3});

    const auto& triplets = formula.get_triplets();

    EXPECT_TRUE(solver.apply_simple_rules(triplets, formula));
    EXPECT_FALSE(solver.has_contradiction());
}

// Test Rule 5: (x,y,1) => x=1
TEST(SolverTests, rule5Basic) {
    Solver solver;
    Formula formula;

    formula.add_clause({1, 2, 3});
    formula.add_clause({3});  // Force z=3 to be true

    const auto& triplets = formula.get_triplets();

    EXPECT_TRUE(solver.apply_simple_rules(triplets, formula));
    EXPECT_FALSE(solver.has_contradiction());
}

// Test Rule 6: (x,1,z) => x=z
TEST(SolverTests, rule6Basic) {
    Solver solver;
    Formula formula;

    formula.add_clause({1, 2, 3});
    formula.add_clause({2});  // Force y=2 to be true

    const auto& triplets = formula.get_triplets();

    EXPECT_TRUE(solver.apply_simple_rules(triplets, formula));
    EXPECT_FALSE(solver.has_contradiction());
}

// Test Rule 7: (x,x,z) => x=1, z=1
TEST(SolverTests, rule7Basic) {
    Solver solver;
    Formula formula;

    formula.add_clause({1, 1, 3});
    formula.add_clause({-1, 2});

    const auto& triplets = formula.get_triplets();

    EXPECT_TRUE(solver.apply_simple_rules(triplets, formula));
    EXPECT_FALSE(solver.has_contradiction());
}

// Test multiple rules combined
TEST(SolverTests, multipleRulesCombined) {
    Solver solver;
    Formula formula;

    // Add clauses that will create multiple triplets
    formula.add_clause({1, 2, 3});
    formula.add_clause({-1, 4, 5});
    formula.add_clause({-2, 6});
    formula.add_clause({-3, -4});

    const auto& triplets = formula.get_triplets();

    EXPECT_TRUE(solver.apply_simple_rules(triplets, formula));
    EXPECT_FALSE(solver.has_contradiction());
}

// Test direct contradiction
TEST(SolverTests, DirectContradiction) {
    Solver solver;
    
    // Create a formula with direct contradiction
    Formula formula;
    
    // Add contradictory clauses
    formula.add_clause({1});   // x1 must be true
    formula.add_clause({-1});  // NOT x1 must be true (x1 must be false)
    
    // Print out the triplets for debugging
    const auto& triplets = formula.get_triplets();
    std::cout << "Contradictory formula triplets:" << std::endl;
    for (const auto& triplet : triplets) {
        std::cout << "(" << std::get<0>(triplet) << ", " 
                  << std::get<1>(triplet) << ", " 
                  << std::get<2>(triplet) << ")" << std::endl;
    }
    
    // Solve should detect the contradiction
    bool result = solver.solve(formula);
    std::cout << "Result: " << result << std::endl;
    std::cout << "Has contradiction: " << solver.has_contradiction() << std::endl;
    
    EXPECT_FALSE(result) << "The solver should return false for a contradictory formula";
    EXPECT_TRUE(solver.has_contradiction()) << "The solver should set the contradiction flag";
}

// Test basic branching - unsatisfiable case (negation is a tautology)
TEST(SolverTests, BasicBranchingUnsatisfiable) {
    Solver solver;
    Formula formula;
    
    // Create an unsatisfiable formula: (x1) AND (NOT x1)
    // This represents the negation of a tautology x1 OR NOT x1
    formula.add_clause({1});   // x1
    formula.add_clause({-1});  // NOT x1
    
    // If the formula is unsatisfiable, its negation is a tautology
    EXPECT_FALSE(solver.solve(formula));
    EXPECT_TRUE(solver.has_contradiction());
}

// Test basic branching - satisfiable case (negation is not a tautology)
TEST(SolverTests, BasicBranchingSatisfiable) {
    Solver solver;
    Formula formula;
    
    // Create a satisfiable formula: (x1 OR x2) AND (NOT x1 OR x3)
    formula.add_clause({1, 2});    // x1 OR x2
    formula.add_clause({-1, 3});   // NOT x1 OR x3
    
    // A satisfiable formula should be solved successfully
    EXPECT_TRUE(solver.solve(formula));
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// Test with a logical law: Law of Excluded Middle (p OR NOT p)
TEST(SolverTests, LawOfExcludedMiddle) {
    Solver solver;
    Formula formula;
    
    // Negation of Law of Excluded Middle: NOT(p OR NOT p) = (NOT p AND p)
    formula.add_clause({-1});  // NOT p
    formula.add_clause({1});   // p
    
    // Should be unsatisfiable since (p OR NOT p) is a tautology
    EXPECT_FALSE(solver.solve(formula));
    EXPECT_TRUE(solver.has_contradiction());
}

// Test nested branching with multiple solutions
TEST(SolverTests, NestedBranchingMultipleSolutions) {
    Solver solver;
    Formula formula;
    
    // Create formula that requires multiple levels of branching
    // (a OR b) AND (c OR d) AND (NOT a OR NOT c) AND (NOT b OR NOT d)
    formula.add_clause({1, 2});    // a OR b
    formula.add_clause({3, 4});    // c OR d
    formula.add_clause({-1, -3});  // NOT a OR NOT c
    formula.add_clause({-2, -4});  // NOT b OR NOT d
    
    EXPECT_TRUE(solver.solve(formula));
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// Test large number of variables
TEST(SolverTests, LargeVariableSet) {
    Solver solver;
    Formula formula;
    
    // Create a chain of OR clauses with 20 variables
    for (int i = 1; i < 20; i++) {
        formula.add_clause({i, i + 1});  // vi OR v(i+1)
    }
    
    // Add some constraints to make it more interesting
    formula.add_clause({-1});     // v1 is false
    formula.add_clause({-10});    // v10 is false
    formula.add_clause({20});     // v20 is true
    
    EXPECT_TRUE(solver.solve(formula));
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// Test multiple satisfiable assignments
TEST(SolverTests, MultipleSatisfiableAssignments) {
    Solver solver;
    Formula formula;
    
    // Create formula: (a OR b) AND (b OR c) AND (c OR d)
    // This has multiple satisfiable assignments
    formula.add_clause({1, 2});   // a OR b
    formula.add_clause({2, 3});   // b OR c
    formula.add_clause({3, 4});   // c OR d
    
    EXPECT_TRUE(solver.solve(formula));
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// Test with a single literal and its negation
TEST(SolverTests, SingleLiteralContradiction) {
    Solver solver;
    Formula formula;

    // Add a single literal and its negation
    formula.add_clause({1});   // x1 must be true
    formula.add_clause({-1});  // x1 must be false

    EXPECT_FALSE(solver.solve(formula));
    EXPECT_TRUE(solver.has_contradiction());
}


// Test with a tautology clause
TEST(SolverTests, TautologyClause) {
    Solver solver;
    Formula formula;

    // Add a tautology clause (always true)
    formula.add_clause({1, -1}); // x1 OR NOT x1

    EXPECT_TRUE(solver.solve(formula));
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// Test with all negative literals
TEST(SolverTests, AllNegativeLiterals) {
    Solver solver;
    Formula formula;

    // Add clauses with only negative literals
    formula.add_clause({-1, -2});
    formula.add_clause({-2, -3});
    formula.add_clause({-3, -4});

    EXPECT_TRUE(solver.solve(formula));
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// Test with all positive literals
TEST(SolverTests, AllPositiveLiterals) {
    Solver solver;
    Formula formula;

    // Add clauses with only positive literals
    formula.add_clause({1, 2});
    formula.add_clause({2, 3});
    formula.add_clause({3, 4});

    EXPECT_TRUE(solver.solve(formula));
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// Test with a large contradictory formula
TEST(SolverTests, LargeContradictoryFormula) {
    Solver solver;
    Formula formula;

    // Create a large formula with a contradiction
    for (int i = 1; i <= 50; i++) {
        formula.add_clause({i, i + 1});  // vi OR v(i+1)
    }
    formula.add_clause({-25}); // Contradiction: v25 must be false
    formula.add_clause({25});  // Contradiction: v25 must be true

    EXPECT_FALSE(solver.solve(formula));
    EXPECT_TRUE(solver.has_contradiction());
}

// Test with a single variable appearing in multiple clauses
TEST(SolverTests, SingleVariableMultipleClauses) {
    Solver solver;
    Formula formula;

    // Add multiple clauses involving the same variable
    formula.add_clause({1});
    formula.add_clause({1, -1});
    formula.add_clause({-1});

    EXPECT_FALSE(solver.solve(formula));
    EXPECT_TRUE(solver.has_contradiction());
}

// Test with a chain of implications
TEST(SolverTests, ChainOfImplications) {
    Solver solver;
    Formula formula;

    // Create a chain of implications
    formula.add_clause({-1, 2}); // x1 → x2
    formula.add_clause({-2, 3}); // x2 → x3
    formula.add_clause({-3, 4}); // x3 → x4
    formula.add_clause({-4, 1}); // x4 → x1 (circular dependency)

    EXPECT_TRUE(solver.solve(formula));
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

TEST(SolverTests, RedundantClauses) {
    Solver solver;
    Formula formula;

    // Add redundant clauses
    formula.add_clause({1, 2});
    formula.add_clause({1, 2}); // Duplicate clause
    formula.add_clause({2, 1}); // Permutation of the first clause

    EXPECT_TRUE(solver.solve(formula));
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// Test with a single clause containing all variables
TEST(SolverTests, SingleClauseAllVariables) {
    Solver solver;
    Formula formula;

    // Add a single clause with all variables
    formula.add_clause({1, 2, 3, 4, 5});

    EXPECT_TRUE(solver.solve(formula)) << "A single clause with all variables should be satisfiable.";
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// // Test with mutually exclusive clauses
TEST(SolverTests, MutuallyExclusiveClauses) {
    Solver solver;
    Formula formula;

    // Add mutually exclusive clauses
    formula.add_clause({1, 2});   // x1 OR x2
    formula.add_clause({-1, -2}); // NOT x1 OR NOT x2

    EXPECT_TRUE(solver.solve(formula)) << "Mutually exclusive clauses should be satisfiable if no direct contradiction exists.";
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// // Test with a large number of variables and no constraints
TEST(SolverTests, LargeVariableSetNoConstraints) {
    Solver solver;
    Formula formula;

    // Add a large number of variables with no constraints
    for (int i = 1; i <= 100; i++) {
        formula.add_clause({i}); // Each variable is independent
    }

    EXPECT_TRUE(solver.solve(formula)) << "A formula with independent variables should be satisfiable.";
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// // Test with a formula that requires backtracking
TEST(SolverTests, RequiresBacktracking) {
    Solver solver;
    Formula formula;

    // Create a formula that requires backtracking to solve
    formula.add_clause({1, 2});    // x1 OR x2
    formula.add_clause({-1, 3});   // NOT x1 OR x3
    formula.add_clause({-2, -3});  // NOT x2 OR NOT x3

    EXPECT_TRUE(solver.solve(formula)) << "A formula requiring backtracking should be satisfiable if no contradictions exist.";
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// Test with a formula containing only unit clauses
TEST(SolverTests, OnlyUnitClauses) {
    Solver solver;
    Formula formula;

    // Add only unit clauses
    formula.add_clause({1});  // x1 must be true
    formula.add_clause({-2}); // x2 must be false
    formula.add_clause({3});  // x3 must be true

    EXPECT_TRUE(solver.solve(formula)) << "A formula with only unit clauses should be satisfiable if no contradictions exist.";
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// // Test with a formula containing redundant unit clauses
TEST(SolverTests, RedundantUnitClauses) {
    Solver solver;
    Formula formula;

    // Add redundant unit clauses
    formula.add_clause({1});  // x1 must be true
    formula.add_clause({1});  // Redundant clause
    formula.add_clause({-2}); // x2 must be false
    formula.add_clause({-2}); // Redundant clause

    EXPECT_TRUE(solver.solve(formula)) << "A formula with redundant unit clauses should still be satisfiable.";
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// // Test with a formula containing a mix of tautologies and contradictions
TEST(SolverTests, MixedTautologiesAndContradictions) {
    Solver solver;
    Formula formula;

    // Add tautologies
    formula.add_clause({1, -1}); // x1 OR NOT x1
    formula.add_clause({2, -2}); // x2 OR NOT x2

    // Add contradictions
    formula.add_clause({3});  // x3 must be true
    formula.add_clause({-3}); // x3 must be false

    EXPECT_FALSE(solver.solve(formula)) << "A formula with contradictions should be unsatisfiable, even with tautologies.";
    EXPECT_TRUE(solver.has_contradiction());
}

// // Test with a formula containing a circular dependency
TEST(SolverTests, CircularDependency) {
    Solver solver;
    Formula formula;

    // Create a circular dependency
    formula.add_clause({-1, 2}); // x1 → x2
    formula.add_clause({-2, 3}); // x2 → x3
    formula.add_clause({-3, 1}); // x3 → x1

    EXPECT_TRUE(solver.solve(formula)) << "A formula with a circular dependency should be satisfiable if no contradictions exist.";
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}

// ============================================================

//FAILING TESTS: 

// Test with a formula that has no solution
TEST(SolverTests, NoSolution) {  //This FAILS
    Solver solver;
    Formula formula;

    // Create a formula with no solution
    formula.add_clause({1, 2});    // x1 OR x2
    formula.add_clause({-1, 3});   // NOT x1 OR x3
    formula.add_clause({-2, -3});  // NOT x2 OR NOT x3
    formula.add_clause({-1, -2});  // NOT x1 OR NOT x2

    EXPECT_FALSE(solver.solve(formula)) << "A formula with no solution should be unsatisfiable.";
    EXPECT_TRUE(solver.has_contradiction());
}

//  Test with a large number of tautology clauses
TEST(SolverTests, LargeTautologyFormula) {  // this fails
    Solver solver;
    Formula formula;

    // Add a large number of tautology clauses
    for (int i = 1; i <= 15; i++) {
        formula.add_clause({i, -i}); // xi OR NOT xi
    }

    EXPECT_TRUE(solver.solve(formula));
    EXPECT_FALSE(solver.has_contradiction());
    EXPECT_TRUE(solver.has_complete_assignment());
}


} // namespace test
} // namespace stalmarck