#include <gtest/gtest.h>
#include <filesystem>
#include "core/stalmarck.hpp"
#include "parser/parser.hpp"

namespace stalmarck {
namespace test {

class IntegrationTests : public ::testing::Test {
protected:
    std::string getTestCasesPath() {
        // Get the directory containing the test source file
        std::filesystem::path source_dir = __FILE__;
        source_dir = source_dir.parent_path();
        return (source_dir / "test_cases").string();
    }

    std::vector<std::string> getCNFFiles() {
        std::vector<std::string> files;
        std::string test_cases_path = getTestCasesPath();
        
        // Create directory if it doesn't exist
        if (!std::filesystem::exists(test_cases_path)) {
            std::filesystem::create_directory(test_cases_path);
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(test_cases_path)) {
            if (entry.path().extension() == ".cnf") {
                files.push_back(entry.path().filename().string());
            }
        }
        return files;
    }

    bool expectedResult(const std::string& filename) {
        if (filename.substr(0, 3) == "sat") return true;
        if (filename.substr(0, 5) == "unsat") return false;
        
        ADD_FAILURE() << "Invalid test case filename: " << filename 
                     << " - must start with 'sat' or 'unsat'";
        return false;
    }
};

TEST_F(IntegrationTests, SolveAllCNFs) {
    auto files = getCNFFiles();
    std::cout << "Found " << files.size() << " CNF files to test\n";
    
    int passed = 0;
    for (const auto& filename : files) {
        std::cout << "\nTesting " << filename << "...\n";
        std::string filepath = getTestCasesPath() + "/" + filename;
        
        // Parse the CNF file
        Parser parser;
        Formula formula = parser.parse_dimacs(filepath);
        if (parser.has_error()) {
            ADD_FAILURE() << "Failed to parse " << filename 
                         << ": " << parser.get_error();
            continue;
        }
        
        // Solve the formula
        StalmarckSolver solver;
        bool success = solver.solve(formula);
        if (!success) {
            ADD_FAILURE() << "Solver failed on " << filename;
            continue;
        }
        
        // Check if result matches filename prefix
        bool is_sat = solver.is_tautology();
        bool expected_sat = expectedResult(filename);
        if (is_sat == expected_sat) {
            passed++;
            std::cout << filename << ": " 
                      << (is_sat ? "SAT" : "UNSAT") 
                      << " (correct)" << std::endl;
        } else {
            ADD_FAILURE() << "Wrong result for " << filename 
                         << ": expected " << (expected_sat ? "SAT" : "UNSAT")
                         << " but got " << (is_sat ? "SAT" : "UNSAT");
        }
    }
    
    std::cout << "\nPassed " << passed << " out of " << files.size() << " tests\n";
}

} // namespace test
} // namespace stalmarck