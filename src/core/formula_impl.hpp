#pragma once

#include <vector>
#include <unordered_set>

namespace stalmarck {

class Formula::Impl {
public:
    std::vector<std::vector<int>> clauses;
    std::unordered_set<int> negated_clauses;
    std::vector<std::tuple<int, int, int>> triplets; 
    size_t num_vars = 0;
};

} // namespace stalmarck 