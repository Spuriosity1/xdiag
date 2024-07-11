#include "basis_sz.hpp"

#include <xdiag/combinatorics/binomial.hpp>
#include <xdiag/combinatorics/combinations.hpp>
#include <xdiag/combinatorics/subsets.hpp>
#include <xdiag/parallel/mpi/allreduce.hpp>

namespace xdiag::basis::spinhalf_distributed {

template <typename bit_t, class process_f>
static int64_t
fill_tables(int64_t n_sites, int64_t n_up, int64_t n_prefix_bits,
            process_f rank, std::vector<bit_t> &prefixes,
            std::unordered_map<bit_t, int64_t> &prefix_begin,
            std::vector<combinatorics::LinTable<bit_t>> &postfix_lintables,
            std::vector<std::vector<bit_t>> &postfix_states) {

  using combinatorics::binomial;
  using combinatorics::LinTable;

  int mpi_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

  int64_t n_postfix_bits = n_sites - n_prefix_bits;

  // Determine the valid prefixes that belong to my process
  int64_t size_local = 0;
  for (bit_t prefix : combinatorics::Subsets<bit_t>(n_prefix_bits)) {
    int n_up_prefix = bits::popcnt(prefix);
    int n_up_postfix = n_up - n_up_prefix;

    // Ignore impossible prefix configurations
    if ((n_up_postfix < 0) || (n_up_postfix > n_postfix_bits)) {
      continue;
    }

    // only keep "random" prefixes
    if (rank(prefix) != mpi_rank) {
      continue;
    }

    // Register a prefix
    prefix_begin[prefix] = size_local;
    size_local += binomial(n_postfix_bits, n_up_postfix);
    prefixes.push_back(prefix);
  }

  // Create the lintables for postfix lookup
  postfix_states.resize(n_postfix_bits + 1);
  for (int n_up_postfix = 0; n_up_postfix <= n_postfix_bits; ++n_up_postfix) {
    postfix_lintables.push_back(LinTable<bit_t>(n_postfix_bits, n_up_postfix));

    // Precompute posfix configurations for given n_up_postfix
    std::vector<bit_t> postfixes_nup(binomial(n_postfix_bits, n_up_postfix), 0);
    int64_t pf_idx = 0;
    for (bit_t pf :
         combinatorics::Combinations<bit_t>(n_postfix_bits, n_up_postfix)) {
      postfixes_nup[pf_idx++] = pf;
    }
    postfix_states[n_up_postfix] = postfixes_nup;
  }
  return size_local;
}

template <typename bit_t>
BasisSz<bit_t>::BasisSz(int64_t n_sites, int64_t n_up)
    : n_sites_(n_sites), n_up_(n_up), n_prefix_bits_(n_sites / 2),
      n_postfix_bits_(n_sites - n_prefix_bits_) {
  using namespace combinatorics;

  if (n_sites < 0) {
    XDIAG_THROW("n_sites < 0");
  } else if (n_up < 0) {
    XDIAG_THROW("nup < 0");
  } else if (n_up > n_sites) {
    XDIAG_THROW("n_up > n_sites");
  }

  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank_);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size_);

  size_ = binomial(n_sites, n_up);
  size_local_ = fill_tables(
      n_sites, n_up, n_prefix_bits_,
      [this, &n_sites](bit_t spins) {
        Log("rank {} {} {}", BSTR(spins), this->rank(spins));
        return rank(spins);
      },
      prefixes_, prefix_begin_, postfix_lintables_, postfix_states_);

  size_local_transpose_ = fill_tables(
      n_sites, n_up, n_postfix_bits_,
      [this](bit_t spins) { return rank(spins); }, postfixes_,
      postfix_begin_, prefix_lintables_, prefix_states_);

  // Compute max/min number of states stored locally
  int64_t size_max;
  int64_t size_max_transpose;
  mpi::Allreduce(&size_local_, &size_max, 1, MPI_MAX, MPI_COMM_WORLD);
  mpi::Allreduce(&size_local_transpose_, &size_max_transpose, 1, MPI_MAX,
                 MPI_COMM_WORLD);
  size_max_ = std::max(size_max, size_max_transpose);

  int64_t size_min;
  int64_t size_min_transpose;
  mpi::Allreduce(&size_local_, &size_min, 1, MPI_MIN, MPI_COMM_WORLD);
  mpi::Allreduce(&size_local_transpose_, &size_min_transpose, 1, MPI_MIN,
                 MPI_COMM_WORLD);

  // Check local sizes sum up to the actual dimension
  int64_t size;
  mpi::Allreduce(&size_local_, &size, 1, MPI_SUM, MPI_COMM_WORLD);
  assert(size == size_);

  int64_t size_transpose;
  mpi::Allreduce(&size_local_transpose_, &size_transpose, 1, MPI_SUM,
                 MPI_COMM_WORLD);
  assert(size_transpose == size_);
}

template <typename bit_t> int64_t BasisSz<bit_t>::n_sites() const {
  return n_sites_;
}
template <typename bit_t> int64_t BasisSz<bit_t>::n_up() const { return n_up_; }
template <typename bit_t> int64_t BasisSz<bit_t>::n_prefix_bits() const {
  return n_prefix_bits_;
};
template <typename bit_t> int64_t BasisSz<bit_t>::n_postfix_bits() const {
  return n_postfix_bits_;
}

template <typename bit_t> int64_t BasisSz<bit_t>::size() const { return size_; }
template <typename bit_t> int64_t BasisSz<bit_t>::size_local() const {
  return size_local_;
}
template <typename bit_t> int64_t BasisSz<bit_t>::size_local_transpose() const {
  return size_local_transpose_;
}
template <typename bit_t> int64_t BasisSz<bit_t>::size_max() const {
  return size_max_;
}
template <typename bit_t> int64_t BasisSz<bit_t>::size_min() const {
  return size_min_;
}

template <typename bit_t>
std::vector<bit_t> const &BasisSz<bit_t>::prefixes() const {
  return prefixes_;
}
template <typename bit_t>
int64_t BasisSz<bit_t>::prefix_begin(bit_t prefix) const {
  return prefix_begin_.at(prefix);
}
template <typename bit_t>
combinatorics::LinTable<bit_t> const &
BasisSz<bit_t>::postfix_lintable(bit_t prefix) const {
  int n_up_prefix = bits::popcnt(prefix);
  int n_up_postfix = n_up_ - n_up_prefix;
  return postfix_lintables_[n_up_postfix];
}
template <typename bit_t>
std::vector<bit_t> const &BasisSz<bit_t>::postfix_states(bit_t prefix) const {
  int n_up_prefix = bits::popcnt(prefix);
  int n_up_postfix = n_up_ - n_up_prefix;
  return postfix_states_[n_up_postfix];
}

template <typename bit_t>
std::vector<bit_t> const &BasisSz<bit_t>::postfixes() const {
  return postfixes_;
}
template <typename bit_t>
int64_t BasisSz<bit_t>::postfix_begin(bit_t postfix) const {
  return postfix_begin_.at(postfix);
}
template <typename bit_t>
combinatorics::LinTable<bit_t> const &
BasisSz<bit_t>::prefix_lintable(bit_t postfix) const {
  int n_up_postfix = bits::popcnt(postfix);
  int n_up_prefix = n_up_ - n_up_postfix;
  return prefix_lintables_[n_up_prefix];
}
template <typename bit_t>
std::vector<bit_t> const &BasisSz<bit_t>::prefix_states(bit_t postfix) const {
  int n_up_postfix = bits::popcnt(postfix);
  int n_up_prefix = n_up_ - n_up_postfix;
  return prefix_states_[n_up_prefix];
}

template class BasisSz<uint32_t>;
template class BasisSz<uint64_t>;

} // namespace xdiag::basis::spinhalf_distributed
