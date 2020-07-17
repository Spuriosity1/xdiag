#ifndef HYDRA_INDEXING_LINTABLE_
#define HYDRA_INDEXING_LINTABLE_

#include <vector>

#include <hydra/utils/typedefs.h>

#include <hydra/utils/bitops.h>

namespace hydra { namespace indexing {

    using utils::gbits;

    template <class hilbertspace_type, class index_type=uint64>
    class LinTable {
    public:
      using hilbertspace_t = hilbertspace_type;
      using index_t = index_type;
      using state_t = typename hilbertspace_t::state_t;
    
      LinTable() = default;
      LinTable(const hilbertspace_t& hilbertspace);

      index_t index(const state_t& state) const;
      inline state_t state(const index_t& index) const
      { return states_[index]; }  

      inline index_t size() const
      { return total_size_; }

    private:
      index_t total_size_;
      index_t right_size_;
      index_t left_size_;
      int site_divider_;
      std::vector<index_t> right_indices_;
      std::vector<index_t> left_indices_;
      int num_of_right_sites;
      int num_of_left_sites;
      std::vector<state_t> states_;
    };

  }  // namespace indexing
}  // namespace hydra

#endif