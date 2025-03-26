#include <gtest/gtest.h>
#include "../../src/solver/solver.hpp"
#include "../../src/core/formula.hpp"

using namespace stalmarck;

TEST(SolverTest, SimpleTautology) {
    Solver solver;
    Formula formula;
    // Add a simple tautology: (p ∨ ¬p)
    formula.add_clause({1, -1});
    EXPECT_TRUE(solver.solve(formula));
}

TEST(SolverTest, SimpleContradiction) {
    Solver solver;
    Formula formula;
    // Add a contradiction: (p ∧ ¬p)
    formula.add_clause({1});
    formula.add_clause({-1});
    EXPECT_FALSE(solver.solve(formula));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 