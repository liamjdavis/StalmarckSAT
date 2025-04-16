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

} // namespace test
} // namespace stalmarck