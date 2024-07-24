#include "random_state.hpp"

#include <xdiag/algebra/algebra.hpp>
#include <xdiag/random/hash.hpp>
#include <xdiag/random/hash_functions.hpp>
#include <xdiag/random/random_utils.hpp>

namespace xdiag {

RandomState::RandomState(int64_t seed, bool normalized)
    : seed_(seed), normalized_(normalized) {}
int64_t RandomState::seed() const { return seed_; }
bool RandomState::normalized() const { return normalized_; }

void fill(State &state, RandomState const &rstate, int64_t col) try {
  int64_t seed = rstate.seed();
  int64_t seed_modified =
      random::hash_combine(seed, random::hash(state.block()));

  if (state.isreal()) {
    auto v = state.vector(col, false);
    random::fill_random_normal_vector(v, seed_modified);
  } else {
    auto v = state.vectorC(col, false);
    random::fill_random_normal_vector(v, seed_modified);
  }
  if (rstate.normalized()) {
    double nrm = norm(state);
    state /= nrm;
  }
} catch (Error const &e) {
  XDIAG_RETHROW(e);
}

State rand(Block const &block, bool real, int64_t seed, bool normalized) {
  return std::visit(
      [&](auto &&block) { return rand(block, real, seed, normalized); }, block);
}

template <typename block_t>
State rand(block_t const &block, bool real, int64_t seed, bool normalized) {
  auto state = State(block, real);
  auto rstate = RandomState(seed, normalized);
  fill(state, rstate);
  return state;
}
template State rand(Spinhalf const &, bool, int64_t, bool);
template State rand(tJ const &, bool, int64_t, bool);
template State rand(Electron const &, bool, int64_t, bool);
#ifdef XDIAG_USE_MPI
template State rand(SpinhalfDistributed const &, bool, int64_t, bool);
template State rand(tJDistributed const &, bool, int64_t, bool);
#endif

std::ostream &operator<<(std::ostream &out, RandomState const &state) {
  out << "  RandomState, seed  : " << state.seed() << "\n";
  return out;
}
std::string to_string(RandomState const &state) {
  return to_string_generic(state);
}

} // namespace xdiag
