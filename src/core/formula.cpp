#include "core/formula.hpp"
#include "core/formula_impl.hpp"
#include <algorithm>

namespace stalmarck {

// Remove the redundant implementation class definition
// class Formula::Impl {
// public:
//     std::vector<std::vector<int>> clauses;
//     size_t num_vars = 0;
// };

Formula::Formula() : impl_(std::make_unique<Impl>()) {}
Formula::~Formula() = default;

void Formula::add_clause(const std::vector<int>& literals) {
    impl_->clauses.push_back(literals);
    for (int lit : literals) {
        impl_->num_vars = std::max(impl_->num_vars, static_cast<size_t>(std::abs(lit)));
    }
}

void Formula::normalize() {
    // Sort literals in each clause
    for (auto& clause : impl_->clauses) {
        std::sort(clause.begin(), clause.end());
    }
    // Sort clauses
    std::sort(impl_->clauses.begin(), impl_->clauses.end());
}

size_t Formula::num_variables() const {
    return impl_->num_vars;
}

size_t Formula::num_clauses() const {
    return impl_->clauses.size();
}

void Formula::translate_to_normalized_form() {
    // TODO: Implement translation to normalized form
}

void Formula::encode_to_implication_triplets() {
    // TODO: Implement encoding to implication triplets
}

} // namespace stalmarck 