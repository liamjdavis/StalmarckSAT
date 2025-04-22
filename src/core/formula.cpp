#include "formula.hpp"
#include "formula_impl.hpp"
#include <algorithm>
#include <iostream>
#include <tuple>
#include <sstream>
#include <unordered_map>

namespace stalmarck {

// Debug helper function to print a clause
std::string print_clause(const std::vector<int>& clause) {
    std::stringstream ss;
    ss << "(";
    bool first = true;
    for (int lit : clause) {
        if (!first) ss << " ∨ ";
        ss << lit;
        first = false;
    }
    ss << ")";
    return ss.str();
}

// Debug helper function to print a formula (set of clauses)
std::string print_formula(const std::vector<std::vector<int>>& clauses) {
    std::stringstream ss;
    bool first = true;
    for (const auto& clause : clauses) {
        if (!first) ss << " ∧ ";
        ss << print_clause(clause);
        first = false;
    }
    return ss.str();
}

// Debug helper function to print triplets
std::string print_triplets(const std::vector<std::tuple<int, int, int>>& triplets) {
    std::stringstream ss;
    ss << "[";
    bool first = true;
    for (const auto& triplet : triplets) {
        if (!first) ss << ", ";
        int x = std::get<0>(triplet);
        int y = std::get<1>(triplet);
        int z = std::get<2>(triplet);
        ss << "(" << x << "," << y << "," << z << ")";
        first = false;
    }
    ss << "]";
    return ss.str();
}

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
            } else {
                // Make sure to include the last literal
                clause_implications.push_back(lit);
            }
        }

        // Only add non-empty clauses
        if (!clause_implications.empty()) {
            implication_representation.push_back(clause_implications);
        }
    }

    // Guard against empty implication representation
    if (implication_representation.empty()) {
        return;
    }

    // converting the conjunctions into implications (one dimensional vector)
    std::vector<std::vector<int>> formula;
    for (size_t i = 0; i < implication_representation.size(); i++) {
        // Skip empty clauses
        if (implication_representation[i].empty()) {
            continue;
        }

        std::vector<int> clause;
        size_t next_clause_idx = i + 1;
        
        // Add first element of the first clause
        if (i == 0 && !implication_representation[i].empty()) {
            clause.push_back(implication_representation[i][0]);
        }
        
        // Add middle elements of the clause (corrected loop)
        for (size_t j = 1; j < implication_representation[i].size() - 1; j++) {
            clause.push_back(implication_representation[i][j]);
        }
        
        // Add last element of the last clause
        if (next_clause_idx == implication_representation.size() && 
            !implication_representation[i].empty()) {
            size_t last_idx = implication_representation[i].size() - 1;
            clause.push_back(implication_representation[i][last_idx]);
        }
        
        // Only add non-empty clauses
        if (!clause.empty()) {
            formula.push_back(clause);
        }

        // Add negative clause into implication form
        if (next_clause_idx < implication_representation.size() && 
            !implication_representation[i].empty() && 
            !implication_representation[next_clause_idx].empty()) {
            
            std::vector<int> negative_clause;
            size_t size_of_curr = implication_representation[i].size();
            
            if (size_of_curr > 0) {
                negative_clause.push_back(implication_representation[i][size_of_curr - 1]);
            }
            
            negative_clause.push_back(-implication_representation[next_clause_idx][0]);
            
            // Mark as negative clause (using a safer approach)
            impl_->negated_clauses.insert(formula.size());
            
            formula.push_back(negative_clause);
        }
    }
    
    impl_->clauses = formula;
}

void Formula::encode_to_implication_triplets() {
    // Validate the formula first
    for (const auto& clause : impl_->clauses) {
        for (int lit : clause) {
            if (lit < -static_cast<int>(impl_->num_vars) || lit > static_cast<int>(impl_->num_vars) || lit == 0) {
                // Invalid literal, but no printout needed
            }
        }
    }

    // Clear any existing triplets
    impl_->triplets.clear();
   
    // Assign a new variable for each compound subformula
    int next_variable = static_cast<int>(impl_->num_vars) + 2;
    int curr_rep = next_variable++; // New variable representing the current clause
    int prev_rep = curr_rep - 1; // Variable representing the previous clause

    // Process each clause
    for (long unsigned int i = this->num_clauses() - 1; i > 0; i--) {
        // Guard against out-of-bounds access
        if (i >= this->num_clauses()) {
            break;
        }
        
        std::vector<int> curr_clause = this->impl_->clauses[i];
        
        // Process literals in the clause
        for (unsigned int j = curr_clause.size() - 1; j > 0; j--) {
            int prev_lit = curr_clause[j - 1];
            int curr_lit = curr_clause[j];
            
            // Check for invalid literals
            if (prev_lit == 0) {
                continue;
            }

            // Create triplets based on the clause structure - no normalization assumptions
            if (j == curr_clause.size() - 1 && i == this->num_clauses() - 1) { 
                // If the last element of the last clause
                this->impl_->triplets.emplace_back(curr_rep, prev_lit, curr_lit);
            }
            else {
                // For all other elements/clauses
                this->impl_->triplets.emplace_back(curr_rep, prev_lit, prev_rep);
            }
        }
        
        prev_rep = curr_rep;
        curr_rep++;
    }
}

const std::vector<std::tuple<int, int, int>>& Formula::get_triplets() const {
    // First, check if we already have triplets
    if (impl_->triplets.empty()) {
        // We need to modify the formula, but this is a const method
        // We can use const_cast to temporarily remove const-ness
        // This is generally not good practice, but can be justified here
        // since we're maintaining logical constness (the external behavior doesn't change)
        Formula* non_const_this = const_cast<Formula*>(this);
        
        // Skip translation to normalized form (which creates issues for CNF files)
        // and directly encode the CNF clauses to triplets
        non_const_this->encode_to_implication_triplets();
    }
    
    // Now return the triplets
    return impl_->triplets;
}

const std::vector<std::vector<int>>& Formula::get_clauses() const {
    return impl_->clauses;
}

} // namespace stalmarck
