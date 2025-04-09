#include "formula.hpp"
#include "formula_impl.hpp"
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

    std::vector<std::vector<int>> implication_representation;

     // Convert each disjunction of literals into implications (not A implies B)
    for (const auto& clause : impl_->clauses) {
        std::vector<int> clause_implications;
       
        for (size_t i = 0; i < clause.size(); i++) {
            int lit = clause[i];
            int next_lit = (i + 1 < clause.size()) ? clause[i + 1] : 0;

            if (next_lit != 0) {
                clause_implications.push_back(-lit); // Represent not A
                clause_implications.push_back(next_lit); // Represent B
            } else {
                // Handle the last literal in the clause
                clause_implications.push_back(-lit); // Represent not A
                clause_implications.push_back(0); // Represent false 
            }
        }

        // Add the clause implications to the 2D vector
        implication_representation.push_back(clause_implications);
    }


    // Convert the conjunction of clauses into implications
    for (size_t i = 0; i < implication_representation.size(); i++) {
        const auto& current_clause = implication_representation[i];
        if (i + 1 < implication_representation.size()) {
            const auto& next_clause = implication_representation[i + 1];

            // Add an implication: current_clause implies next_clause
            std::vector<int> conjunction_implication;
            conjunction_implication.push_back(-current_clause[0]); // Represent not (current_clause)
            conjunction_implication.push_back(next_clause[0]);     // Represent (next_clause)
            implication_representation.push_back(conjunction_implication);
        }
    }
    // update the clauses in the formula

    // The problem with the above methods is that we do not have a way to track the precedence of the clauses
    // currently, the assumption is that the resulting formula has the structure p -> (q -> (r -> (...)) as we add them left to right
    // Also, the implementation of the negation of clauses is sketchy, as we are just adding the minus in front of the negated clause (might need to actually recursively apply negation rules)
    impl_->clauses = implication_representation;
}

void Formula::encode_to_implication_triplets() {
    // TODO: Implement encoding to implication triplets
}

} // namespace stalmarck 