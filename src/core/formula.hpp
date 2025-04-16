#pragma once

#include <vector>
#include <string>
#include <memory>

namespace stalmarck {

class Formula {
public:
    Formula();
    ~Formula();
    
    // Delete copy operations
    Formula(const Formula&) = delete;
    Formula& operator=(const Formula&) = delete;
    
    // Add move operations
    Formula(Formula&&) noexcept = default;
    Formula& operator=(Formula&&) noexcept = default;

    // Formula manipulation
    void add_clause(const std::vector<int>& literals);
    void normalize();
    
    // Access methods
    size_t num_variables() const;
    size_t num_clauses() const;
    
    // Translation methods
    void translate_to_normalized_form();
    void encode_to_implication_triplets();

    // Get triplets
    const std::vector<std::tuple<int, int, int>>& get_triplets() const;

    // Get clauses
    const std::vector<std::vector<int>>& get_clauses() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace stalmarck 