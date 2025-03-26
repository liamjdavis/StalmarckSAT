#pragma once

#include <vector>

namespace stalmarck {

class Formula::Impl {
public:
    std::vector<std::vector<int>> clauses;
    size_t num_vars = 0;
};

} // namespace stalmarck 