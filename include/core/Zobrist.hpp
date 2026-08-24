#pragma once

#include "core/Types.hpp"
#include <cstdint>
#include <cstddef>

namespace chess {

class Zobrist {
private:
    static uint64_t splitmix64(uint64_t& state) {
        uint64_t z = (state += 0x9e3779b97f4a7c15ULL);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }

public:
    static inline uint64_t piece_keys[13][64];
    static inline uint64_t side_key;
    static inline uint64_t castling_keys[16];
    static inline uint64_t en_passant_keys[8];
    static inline bool initialized = false;

    static void init() {
        if (initialized) return;
        uint64_t seed = 1070372ULL; // Fixed seed for reproducible keys
        for (int p = 0; p < 13; ++p) {
            for (int sq = 0; sq < 64; ++sq) {
                piece_keys[p][sq] = splitmix64(seed);
            }
        }
        side_key = splitmix64(seed);
        for (int i = 0; i < 16; ++i) {
            castling_keys[i] = splitmix64(seed);
        }
        for (int i = 0; i < 8; ++i) {
            en_passant_keys[i] = splitmix64(seed);
        }
        initialized = true;
    }
};

} // namespace chess
