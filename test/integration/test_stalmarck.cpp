#include <gtest/gtest.h>
#include "../../src/core/stalmarck.hpp"
#include "../../src/parser/parser.hpp"

using namespace stalmarck;

TEST(IntegrationTest, ParseAndSolve) {
    StalmarckSolver solver;
    Parser parser;
    
    // Test parsing and solving a simple formula
    std::string formula = "(p ∨ q) ∧ (¬p ∨ ¬q)";
    Formula parsed = parser.parse_formula(formula);
    EXPECT_FALSE(parser.has_error());
    
    bool result = solver.solve(formula);
    EXPECT_TRUE(result); // This formula should be satisfiable
}

TEST(IntegrationTest, DimacsInput) {
    StalmarckSolver solver;
    Parser parser;
    
    // Test parsing and solving a DIMACS file
    Formula formula = parser.parse_dimacs("test/sat/example.cnf");
    EXPECT_FALSE(parser.has_error());
    
    bool result = solver.solve(formula);
    EXPECT_TRUE(result);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 