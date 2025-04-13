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
                clause_implications.push_back(-lit); // Represent not A  (not doing anything to B as it stays according to rules)
            }
        }

        // Add the clause implications to the 2D vector
        implication_representation.push_back(clause_implications);
    }

    // converting the conjunctions into implications (one dimensional vector)
    std::vector<std::vector<int>> formula;
    for (long unsigned int i = 0; i < implication_representation.size(); i++){
        std::vector<int> clause;
        // copy all elements into implication formula
        long unsigned int next_clause_idx = i + 1; 
        if (i == 0) clause.push_back(implication_representation[i][0]); // copy the first element of the iff it is the first clause
        for (int j = 1; i < implication_representation[i].size()-1; j++){ // not adding the first element
            clause.push_back(implication_representation[i][j]);
        }
        if (next_clause_idx == implication_representation.size()){
            clause.push_back(implication_representation[i][implication_representation.size() - 1]); // add the last element
        }
        formula.push_back(clause); // add clause to the formula

        // add negative clause into implication form using last element of current and first elem of next clause
        // this eliminates the ANDs in the formula
        if (next_clause_idx < implication_representation.size()){
            std::vector<int> negative_clause;
            int size_of_curr = implication_representation[i].size();
            negative_clause.push_back(implication_representation[i][size_of_curr - 1]);
            negative_clause.push_back(-implication_representation[next_clause_idx][0]);
            negative_clause.push_back(formula.size()); // flag the negative clause by index
            formula.push_back(negative_clause);
        }
    }
    // every clause of size 2 must be interpreted as negated one (unless we have a way to track them)
    impl_->clauses = formula;
}

void Formula::encode_to_implication_triplets() {
   
    // Assign a new variable for each compound subformula
    int next_variable = static_cast<int>(impl_->num_vars) + 2;
    int curr_rep = next_variable++; // New variable representing the current clause
    int prev_rep = curr_rep - 1; // Variable representing the previous clause

    for (int i = this->num_clauses() - 1; i > 0; i++) {
        std::vector<int> curr_clause = this->impl_->clauses[i];



        for (int j = curr_clause.size() - 1; j > 0; j--) {

            unsigned long int prev_lit = (j - 1 >= 0) ? curr_clause[j - 1] : 0;
            unsigned long int curr_lit = curr_clause[j];

            if (prev_lit != 0 && j == curr_clause.size() - 1 && i == this->num_clauses() - 1) { // if the last element
                this->impl_->triplets.emplace_back(curr_rep, prev_lit, curr_lit);
            }
            else if (prev_lit != 0) { // if intermediate element of arbitrary clause
                this->impl_->triplets.emplace_back(curr_rep, prev_lit, prev_rep);
            }
            else if (i + 1 < this->num_clauses() && this->impl_->clauses[i].size() == 2 && this->impl_->negated_clauses.find(i) != this->impl_->negated_clauses.end()){ // handling negative clauses
                this->impl_->triplets.emplace_back(curr_rep, prev_lit, -prev_rep);
            }
            
        }
        prev_rep = curr_rep;
        curr_rep++;
    }
      
}

} // namespace stalmarck

} // namespace stalmarck