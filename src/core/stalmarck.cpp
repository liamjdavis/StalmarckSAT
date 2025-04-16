#include "core/stalmarck.hpp"
#include "solver/solver.hpp"
#include "parser/parser.hpp"
#include <string>
#include <memory>

namespace stalmarck {

class StalmarckSolver::Impl {
public:
    Solver solver;
    Parser parser;
    bool is_tautology_result = false;
    double timeout = 0.0;
    int verbosity = 0;
};

StalmarckSolver::StalmarckSolver() : impl_(std::make_unique<Impl>()) {}
StalmarckSolver::~StalmarckSolver() = default;

bool StalmarckSolver::solve(const std::string& filename) {
    Formula parsed = impl_->parser.parse_dimacs(filename);
    if (impl_->parser.has_error()) {
        return false;
    }
    
    impl_->is_tautology_result = impl_->solver.solve(parsed);
    return true;
}

bool StalmarckSolver::solve(const Formula& formula) {
    impl_->is_tautology_result = impl_->solver.solve(formula);
    return true;
}

bool StalmarckSolver::is_tautology() const {
    return impl_->is_tautology_result;
}

void StalmarckSolver::set_timeout(double seconds) {
    impl_->timeout = seconds;
}

void StalmarckSolver::set_verbosity(int level) {
    impl_->verbosity = level;
}

} // namespace stalmarck