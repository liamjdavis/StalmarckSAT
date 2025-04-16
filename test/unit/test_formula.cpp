#include <gtest/gtest.h>
#include "core/formula.hpp"

namespace stalmarck {
namespace test {

// Basic functionality tests
TEST(FormulaTests, EmptyFormula) {
    Formula formula;
    EXPECT_EQ(formula.num_clauses(), 0);
    EXPECT_EQ(formula.num_variables(), 0);
}

TEST(FormulaTests, AddClause) {
    Formula formula;
    formula.add_clause({1, -2, 3});
    EXPECT_EQ(formula.num_clauses(), 1);
    EXPECT_EQ(formula.num_variables(), 3);

    formula.add_clause({-1, 2});
    EXPECT_EQ(formula.num_clauses(), 2);
    EXPECT_EQ(formula.num_variables(), 3);
}

TEST(FormulaTests, Normalization) {
    Formula formula;
    // Add clauses in unsorted order
    formula.add_clause({3, 1, -2});
    formula.add_clause({5, -1, 4});
    
    // Normalize the formula
    formula.normalize();
    
    // We can't directly test the normalization since the internal state is private
    // But we can verify that it doesn't change the number of variables or clauses
    EXPECT_EQ(5, formula.num_variables());
    EXPECT_EQ(2, formula.num_clauses());
}

TEST(FormulaTests, TranslateToNormalizedForm) {
    Formula formula;
    formula.add_clause({1, 2, 3});  // (1 ∨ 2 ∨ 3)
    formula.add_clause({-1, 4});    // (¬1 ∨ 4)
    
    formula.translate_to_normalized_form();
    
    // We can't directly test the internal state, but we can check that:
    // 1. The operation completes without errors
    // 2. The number of clauses might change in the implementation
    EXPECT_GE(formula.num_clauses(), 2);
}

// Test implication triplet encoding
TEST(FormulaTests, EncodeToImplicationTriplets) {
    Formula formula;
    formula.add_clause({1, 2});
    formula.add_clause({-2, 3});
    
    formula.translate_to_normalized_form();
    formula.encode_to_implication_triplets();
    
    // Similar to above, we're mainly testing that the operation completes
    // without errors since we can't directly inspect the triplets
}

// Test a more complex formula
TEST(FormulaTests, ComplexFormula) {
    Formula formula;
    
    // (x1 ∨ x2 ∨ x3) ∧ (¬x1 ∨ x4) ∧ (¬x2 ∨ ¬x4 ∨ x5)
    formula.add_clause({1, 2, 3});
    formula.add_clause({-1, 4});
    formula.add_clause({-2, -4, 5});
    
    EXPECT_EQ(5, formula.num_variables());
    EXPECT_EQ(3, formula.num_clauses());
    
    formula.normalize();
    formula.translate_to_normalized_form();
    formula.encode_to_implication_triplets();
    
    // Again, we're primarily testing that these operations complete without
    // throwing exceptions
}

} // namespace test
} // namespace stalmarck