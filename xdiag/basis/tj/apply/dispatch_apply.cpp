#include "dispatch_apply.hpp"

#include <xdiag/algebra/fill.hpp>
#include <xdiag/basis/tj/apply/dispatch.hpp>

namespace xdiag::basis::tj {

template <typename coeff_t>
void dispatch_apply(OpSum const &ops, tJ const &block_in,
                    arma::Col<coeff_t> const &vec_in, tJ const &block_out,
                    arma::Col<coeff_t> &vec_out) try {
  auto fill = [&](int64_t idx_in, int64_t idx_out, coeff_t val) {
    return fill_apply(vec_in, vec_out, idx_in, idx_out, val);
  };
  dispatch<coeff_t>(ops, block_in, block_out, fill);
} catch (Error const &error) {
  XDIAG_RETHROW(error);
}

template void dispatch_apply(OpSum const &, tJ const &, arma::vec const &,
                             tJ const &block, arma::vec &);
template void dispatch_apply(OpSum const &, tJ const &, arma::cx_vec const &,
                             tJ const &block, arma::cx_vec &);

} // namespace xdiag::basis::tj
