#include <gtest/gtest.h>
#include <fstream>
#include "parser/parser.hpp"
#include "core/formula.hpp"

namespace stalmarck {
namespace test {
class ParserTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test files before each test
        createTestFiles();
    }

    void TearDown() override {
        // Clean up test files after each test
        removeTestFiles();
    }

private:
    void createTestFiles() {
        // Valid CNF file
        std::ofstream valid("valid.cnf");
        valid << "c Example CNF file\n"
              << "p cnf 3 2\n"
              << "1 2 0\n"
              << "-2 3 0\n";
        valid.close();

        // Invalid format file
        std::ofstream invalid_format("invalid_format.cnf");
        invalid_format << "c Example CNF file\n"
                      << "p sat 3 2\n"
                      << "1 2 0\n";
        invalid_format.close();

        // Invalid variable file
        std::ofstream invalid_var("invalid_var.cnf");
        invalid_var << "c Example CNF file\n"
                   << "p cnf 2 1\n"
                   << "1 4 0\n";
        invalid_var.close();
    }

    void removeTestFiles() {
        std::remove("valid.cnf");
        std::remove("invalid_format.cnf");
        std::remove("invalid_var.cnf");
    }
};

TEST_F(ParserTests, ValidCNFParsing) {
    Parser parser;
    Formula formula = parser.parse_dimacs("valid.cnf");
    
    EXPECT_FALSE(parser.has_error());
    EXPECT_EQ(formula.num_variables(), 3);
    EXPECT_EQ(formula.num_clauses(), 2);
}

TEST_F(ParserTests, NonexistentFile) {
    Parser parser;
    Formula formula = parser.parse_dimacs("nonexistent.cnf");
    
    EXPECT_TRUE(parser.has_error());
    EXPECT_EQ(parser.get_error(), "Could not open file: nonexistent.cnf");
    EXPECT_EQ(formula.num_clauses(), 0);
}

TEST_F(ParserTests, InvalidFormatLine) {
    Parser parser;
    Formula formula = parser.parse_dimacs("invalid_format.cnf");
    
    EXPECT_TRUE(parser.has_error());
    EXPECT_EQ(parser.get_error(), "Invalid problem line format");
    EXPECT_EQ(formula.num_clauses(), 0);
}

TEST_F(ParserTests, InvalidVariableNumber) {
    Parser parser;
    Formula formula = parser.parse_dimacs("invalid_var.cnf");
    
    EXPECT_TRUE(parser.has_error());
    EXPECT_EQ(parser.get_error(), "Variable number exceeds declared maximum");
    EXPECT_EQ(formula.num_clauses(), 0);
}

TEST_F(ParserTests, EmptyFile) {
    // Create empty file
    std::ofstream empty("empty.cnf");
    empty.close();
    
    Parser parser;
    Formula formula = parser.parse_dimacs("empty.cnf");
    
    EXPECT_FALSE(parser.has_error());
    EXPECT_EQ(formula.num_clauses(), 0);
    
    std::remove("empty.cnf");
}

TEST_F(ParserTests, CommentsOnly) {
    // Create file with only comments
    std::ofstream comments("comments.cnf");
    comments << "c This is a comment\n"
            << "c Another comment\n";
    comments.close();
    
    Parser parser;
    Formula formula = parser.parse_dimacs("comments.cnf");
    
    EXPECT_FALSE(parser.has_error());
    EXPECT_EQ(formula.num_clauses(), 0);
    
    std::remove("comments.cnf");
}

} // namespace test
} // namespace stalmarck