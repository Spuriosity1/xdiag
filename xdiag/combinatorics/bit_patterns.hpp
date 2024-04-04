#pragma once

#include <xdiag/bits/bitops.hpp>
#include <xdiag/common.hpp>

namespace xdiag::combinatorics {

template <typename bit_t> constexpr bit_t get_next_pattern(bit_t v) noexcept {
  // Bit twiddling Hack from
  // http://graphics.stanford.edu/~seander/bithacks.html
  // #NextBitPermutation

  // Fast version (needs __builtin_ctz(v)) (some problem with 0)
  bit_t t = v | (v - 1); // t gets v's least significant 0
  return ((t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1)));

  // // Fast version (needs __builtin_ctz(v)) (some problem with 0)
  // int_t t = v | (v - 1); // t gets v's least significant 0
  // return v == 0 ? ~v :((t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) +
  // 1)));

  // // Slow version (should work everywhere)
  // int_t t = (v | (v - 1)) + 1;
  // return v == 0 ? ~v : t | ((((t & -t) / (v & -v)) >> 1) - 1);
}

template <typename bit_t>
bit_t get_nth_pattern(int64_t n, int64_t n_sites, int64_t n_upspins);

template <typename bit_t>
int64_t get_n_for_pattern(bit_t pattern, int64_t n_sites, int64_t n_upspins);

} // namespace xdiag::combinatorics
