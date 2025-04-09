#include "formula.hpp"
#include "formula_impl.hpp"
#include <algorithm>
#include <iostream>
#include <tuple>

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

     // we undertand the formula using the right associativity rule of implications
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

            // need to somehow convert the conjunction of clauses into implications

        }
    }
    // update the clauses in the formula

    // The problem with the above methods is that we do not have a way to track the precedence of the clauses
    // currently, the assumption is that the resulting formula has the structure p -> q -> r -> ... as we add them left to right
    impl_->clauses = implication_representation;
}

void Formula::encode_to_implication_triplets() {
    std::vector<std::tuple<int, int, int>> triplets; // Store the triplets (x, y, z)

    // Assign a new variable for each compound subformula
    int next_variable = static_cast<int>(impl_->num_vars) + 1;

    for (const auto& clause : impl_->clauses) {
        int current_rep = next_variable++; // New variable representing the current clause

        // Process literals in reverse order (from last to first)
        for (int i = static_cast<int>(clause.size()) - 1; i >= 0; i--) {
            int lit = clause[i];
            int prev_lit = (i - 1 >= 0) ? clause[i - 1] : 0;

            if (prev_lit != 0) {
                // Add triplet for x ↔ (y → z), where x = current_rep, y = prev_lit, z = lit
                triplets.emplace_back(current_rep, prev_lit, lit);
            } else {
                // Add triplet for x ↔ (y → ⊥), where x = current_rep, y = lit, z = 0 (false)
                triplets.emplace_back(current_rep, lit, 0);
            }
        }

    // might need to add more logic to combine triples
      
    }

}

} // namespace stalmarck