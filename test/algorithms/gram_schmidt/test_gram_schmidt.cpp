#include "../../catch.hpp"

#include <iostream>

#include <hydra/algorithms/gram_schmidt/gram_schmidt.h>
#include <hydra/utils/print_macro.h>

TEST_CASE("gram_schmidt", "[algorithms]") {
  using namespace hydra;
  using namespace arma;

  Log("testing gram_schmidt");
  for (int m = 1; m <= 1000; m *= 10) {

    {
      int n = m;
      mat A = randn(m, n);
      gram_schmidt_inplace(A);
      double ortholevel = norm(A.t() * A - eye(n, n));
      // Log("m: {}, n: {}, ortho: {}", m, n, ortholevel);
      REQUIRE(ortholevel < 1e-12);
    }

    {
      int n = m / 3;
      mat A = randn(m, n);
      gram_schmidt_inplace(A);
      double ortholevel = norm(A.t() * A - eye(n, n));
      // Log("m: {}, n: {}, ortho: {}", m, n, ortholevel);
      REQUIRE(ortholevel < 1e-12);
    }

    {
      int n = m;
      mat A = randn(m, n);
      auto Q = gram_schmidt(A);
      double ortholevel = norm(Q.t() * Q - eye(n, n));
      // Log("m: {}, n: {}, ortho: {}", m, n, ortholevel);
      REQUIRE(ortholevel < 1e-12);
    }

    {
      int n = m / 3;
      mat A = randn(m, n);
      auto Q = gram_schmidt(A);
      double ortholevel = norm(Q.t() * Q - eye(n, n));
      // Log("m: {}, n: {}, ortho: {}", m, n, ortholevel);
      REQUIRE(ortholevel < 1e-12);
    }
  }
}
