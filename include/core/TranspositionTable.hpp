#pragma once

#include "core/Types.hpp"
#include "core/Move.hpp"
#include <vector>
#include <cstdint>
#include <cstddef>
#include <algorithm>

namespace chess {

enum class TTFlag : uint8_t {
    None = 0,
    Exact = 1,       // PV-Node: exact score
    LowerBound = 2,  // Fail-High (Beta-cutoff): true score >= score
    UpperBound = 3   // Fail-Low (Alpha-node): true score <= score
};

struct TTEntry {
    uint64_t key = 0;
    Move best_move{};
    int32_t score = 0;
    int16_t depth = 0;
    TTFlag flag = TTFlag::None;
    uint8_t age = 0;
};

class TranspositionTable {
private:
    std::vector<TTEntry> table_;
    size_t size_mask_ = 0;
    uint8_t current_age_ = 0;

public:
    TranspositionTable(size_t size_mb = 16) {
        resize(size_mb);
    }

    void resize(size_t size_mb) {
        if (size_mb == 0) size_mb = 1;
        size_t bytes = size_mb * 1024 * 1024;
        size_t num_entries = bytes / sizeof(TTEntry);
        // Round down to power of 2 for fast bitwise indexing
        size_t power_of_two = 1;
        while ((power_of_two << 1) <= num_entries) {
            power_of_two <<= 1;
        }
        table_.clear();
        table_.resize(power_of_two);
        size_mask_ = power_of_two - 1;
        current_age_ = 0;
    }

    void clear() {
        std::fill(table_.begin(), table_.end(), TTEntry{});
        current_age_ = 0;
    }

    void new_search() {
        current_age_++;
    }

    // Helper for mate score adjustment: store relative to root
    static int score_to_tt(int score, int ply) {
        if (score > 90000) return score + ply;
        if (score < -90000) return score - ply;
        return score;
    }

    static int score_from_tt(int score, int ply) {
        if (score > 90000) return score - ply;
        if (score < -90000) return score + ply;
        return score;
    }

    bool probe(uint64_t key, TTEntry& entry) const {
        if (table_.empty()) return false;
        size_t index = key & size_mask_;
        const TTEntry& e = table_[index];
        if (e.key == key && e.flag != TTFlag::None) {
            entry = e;
            return true;
        }
        return false;
    }

    void store(uint64_t key, int depth, int ply, int score, TTFlag flag, const Move& best_move) {
        if (table_.empty()) return;
        size_t index = key & size_mask_;
        TTEntry& e = table_[index];

        bool replace = (e.key != key) ||
                       (flag == TTFlag::Exact) ||
                       (depth >= e.depth) ||
                       (e.age != current_age_);

        if (replace) {
            e.key = key;
            e.score = score_to_tt(score, ply);
            e.depth = static_cast<int16_t>(depth);
            e.flag = flag;
            e.age = current_age_;
            if (best_move.is_valid() || e.key != key) {
                e.best_move = best_move;
            }
        } else if (best_move.is_valid() && e.key == key && !e.best_move.is_valid()) {
            e.best_move = best_move;
        }
    }

    int hashfull() const {
        if (table_.empty()) return 0;
        size_t sample_size = std::min<size_t>(table_.size(), 1000);
        size_t used = 0;
        for (size_t i = 0; i < sample_size; ++i) {
            if (table_[i].flag != TTFlag::None && table_[i].age == current_age_) {
                used++;
            }
        }
        return static_cast<int>((used * 1000) / sample_size);
    }
};

} // namespace chess
