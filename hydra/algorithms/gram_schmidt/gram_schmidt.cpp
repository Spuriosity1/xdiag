#include "gram_schmidt.h"

#include <hydra/algorithms/gram_schmidt/orthogonalize.h>
#include <hydra/utils/close.h>
#include <hydra/utils/logger.h>

namespace hydra {
template <typename coeff_t>
void gram_schmidt_inplace(arma::Mat<coeff_t> &M, int64_t max_col,
                          int iterations) {
  using namespace arma;

  if (max_col < 0) {
    max_col = M.n_cols;
  } else if (max_col > (int64_t)M.n_cols) {
    Log.err("Error in gram_schmidt_inplace: max_col larger than number of "
            "matrix columns");
  }

  if (max_col > 0) {
    double nrm = norm(M.col(0));
    if (close(nrm, 0.)) {
      Log.err(
          "Error in gram_schmidt_inplace: norm of columns found to be zero");
    } else {
      M.col(0) /= nrm;
    }

    for (int64_t col = 1; col < max_col; ++col) {

      orthogonalize_inplace(M.col(col), M, col, iterations);
      nrm = norm(M.col(col));
      if (close(nrm, 0.)) {
        Log.err(
            "Error in gram_schmidt_inplace: norm of columns found to be zero");
      } else {
        M.col(col) /= nrm;
      }
    }
  }
}

template void gram_schmidt_inplace(arma::Mat<double> &, int64_t, int);
template void gram_schmidt_inplace(arma::Mat<complex> &, int64_t, int);

template <typename coeff_t>
arma::Mat<coeff_t> gram_schmidt(arma::Mat<coeff_t> const &M, int64_t max_col,
                                int iterations) {
  arma::Mat<coeff_t> Q = M;
  gram_schmidt_inplace(Q, max_col, iterations);
  return Q;
}

template arma::Mat<double> gram_schmidt(arma::Mat<double> const &, int64_t,
                                        int);
template arma::Mat<complex> gram_schmidt(arma::Mat<complex> const &, int64_t,
                                         int);
} // namespace hydra
