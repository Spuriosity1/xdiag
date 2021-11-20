#pragma once

#include <hydra/common.h>

#include <hydra/bitops/bitops.h>

namespace hydra::combinatorics {

template <class bit_t = std_bit_t>
bit_t down_hole_to_up(bit_t downspins, bit_t holes);
template <class bit_t = std_bit_t>
bit_t up_hole_to_down(bit_t upspins, bit_t holes);
template <class bit_t = std_bit_t>
bit_t up_down_to_hole(bit_t upspins, bit_t downspins);
template <class bit_t = std_bit_t>
bit_t down_up_to_hole(bit_t downspins, bit_t upspins);

} // namespace hydra::combinatorics