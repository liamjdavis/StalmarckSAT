#include <gtest/gtest.h>
#include <filesystem>
#include "core/stalmarck.hpp"
#include "parser/parser.hpp"

namespace stalmarck {
namespace test {

class IntegrationTests : public ::testing::Test {
protected:
    std::vector<std::string> getCNFFiles() {
        std::vector<std::string> files;
        std::string regress_path = (std::filesystem::current_path() / "regress").string();
        
        for (const auto& entry : std::filesystem::directory_iterator(regress_path)) {
            if (entry.path().extension() == ".cnf") {
                files.push_back(entry.path().filename().string());
            }
        }
        return files;
    }

    std::string getRegressPath() {
        return (std::filesystem::current_path() / "regress").string();
    }
};

TEST_F(IntegrationTests, SolveAllCNFs) {
    for (const auto& filename : getCNFFiles()) {
        std::string filepath = getRegressPath() + "/" + filename;
        
        // Parse the CNF file
        Parser parser;
        Formula formula = parser.parse_dimacs(filepath);
        ASSERT_FALSE(parser.has_error()) 
            << "Failed to parse " << filename 
            << ": " << parser.get_error();
        
        // Solve the formula
        StalmarckSolver solver;
        bool success = solver.solve(formula);
        ASSERT_TRUE(success) 
            << "Solver failed on " << filename;
        
        // Just log the result without checking correctness
        std::cout << filename << ": " 
                  << (solver.is_tautology() ? "SAT" : "UNSAT") 
                  << std::endl;
    }
}

} // namespace test
} // namespace stalmarck