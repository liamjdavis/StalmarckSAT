#include <pybind11/pybind11.h>
#include "../src/core/stalmarck.hpp"
#include "../src/core/formula_impl.hpp"  // Include the formula implementation
#include <stdexcept>

namespace py = pybind11;

bool solve_file(const std::string& cnf_path) {
    try {
        stalmarck::StalmarckSolver solver;
        bool success = solver.solve(cnf_path);
        
        if (!success) {
            throw std::runtime_error("Error during solving");
        }
        
        return !solver.is_tautology(); // Return true for SAT, false for UNSAT
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Error: ") + e.what());
    }
}

PYBIND11_MODULE(_stalmarckpy, m) {
    m.doc() = "Python bindings for StalmarckSAT solver";
    
    m.def("solve_file", &solve_file, "Solve a CNF formula from a file",
          py::arg("cnf_path"));
}