#pragma once
#include <vector>
#include <cstdint>
#include <algorithm>
#include <limits>

// Structure-of-Arrays (SoA) layout for cache efficiency
struct OrderBook {
    std::vector<double>   prices;
    std::vector<uint32_t> qtys;
    std::vector<uint64_t> ids;

    void reserve(size_t n) {
        prices.reserve(n);
        qtys.reserve(n);
        ids.reserve(n);
    }

    inline void add(uint64_t id, uint32_t qty, double px) noexcept {
        prices.push_back(px);
        qtys.push_back(qty);
        ids.push_back(id);
    }

    inline void clear() noexcept {
        prices.clear();
        qtys.clear();
        ids.clear();
    }

    size_t size() const noexcept { return prices.size(); }
};
