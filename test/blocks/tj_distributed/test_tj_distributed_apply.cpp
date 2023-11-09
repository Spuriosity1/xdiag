#include "../../catch.hpp"

#include <iostream>

#include "../spinhalf/testcases_spinhalf.h"
#include "../tj/testcases_tj.h"

#include <hydra/algorithms/sparse_diag.h>
#include <hydra/blocks/spinhalf/spinhalf_matrix.h>
#include <hydra/blocks/tj/tj_apply.h>
#include <hydra/blocks/tj/tj_matrix.h>
#include <hydra/blocks/tj_distributed/tj_distributed_apply.h>
#include <hydra/utils/close.h>
#include <hydra/utils/print_macro.h>

using namespace hydra;

void test_tjdistributed_e0_real(BondList bonds, int nup, int ndn, double e0) {
  int n_sites = bonds.n_sites();
  auto block = tJDistributed(n_sites, nup, ndn);
  double e0c = eigval0(bonds, block);
  REQUIRE(std::abs(e0 - e0c) < 1e-6);
}

TEST_CASE("tj_distributed_apply", "[tj_distributed]") try {
  using namespace hydra::testcases::tj;

  Log("tj_distributed_apply test");
  int N = 4;

  // Play-around test
  for (int nup = 0; nup <= N; ++nup) {
    for (int ndn = 0; ndn <= N - nup; ++ndn) {
      auto block = tJDistributed(N, nup, ndn);
      BondList bonds;
      for (int i = 0; i < N; ++i) {
        bonds << Bond("ISING", "Jz", {i, (i + 1) % N});
        bonds << Bond("EXCHANGE", "Jx", {i, (i + 1) % N});
        bonds << Bond("HOPDN", "TDN", {i, (i + 1) % N});
        bonds << Bond("HOPUP", "TUP", {i, (i + 1) % N});
      }
      bonds["Jz"] = 1.32;
      bonds["Jx"] = complex(.432, .576);
      bonds["TDN"] = complex(-0.1432, .3576);
      bonds["TUP"] = complex(-0.4321, .5763); // 2.104;

      double e0 = eigval0(bonds, block);
      auto block2 = tJ(N, nup, ndn);
      double e02 = eigval0(bonds, block2);
      // Log("{} {} {:.12f} {:.12f}", nup, ndn, e0, e02);

      REQUIRE(close(e0, e02));
    }
  }

  Log("tj_distributed: HB all-to-all comparison");
  for (int n_sites = 2; n_sites < 8; ++n_sites) {
    // Log("N: {}", n_sites);
    auto bonds = testcases::spinhalf::HB_alltoall(n_sites);
    // HydraPrint(bonds);
    for (int nup = 0; nup <= n_sites; ++nup) {
      auto block = Spinhalf(n_sites, nup);
      auto block_tJ = tJDistributed(n_sites, nup, n_sites - nup);
      double e0_spinhalf = eigval0(bonds, block);
      double e0 = eigval0(bonds, block_tJ);
      // Log("{} {}", e0_spinhalf, e0);
      REQUIRE(close(e0_spinhalf, e0));
    }
  }


  {
    Log("tj_distributed: TJModel: six-site chain test, t=1.0, J=1.0, N=6");
    auto bonds = tJchain(6, 1.0, 1.0);
    std::vector<std::tuple<int, int, double>> nup_ndn_e0 = {
        {0, 0, 0.0},         {0, 1, -2.0},        {0, 2, -2.96081311},
        {0, 3, -3.79610527}, {0, 4, -2.46081311}, {0, 5, -0.99999999},
        {0, 6, 1.500000000}, {1, 0, -2.0},        {1, 1, -3.61222054},
        {1, 2, -4.04537829}, {1, 3, -4.10768318}, {1, 4, -2.42705097},
        {1, 5, -0.49999999}, {2, 0, -2.96081311}, {2, 1, -4.04537829},
        {2, 2, -4.16447847}, {2, 3, -3.52922048}, {2, 4, -2.11803398},
        {3, 0, -3.79610527}, {3, 1, -4.10768318}, {3, 2, -3.52922048},
        {3, 3, -2.80277563}, {4, 0, -2.46081311}, {4, 1, -2.42705097},
        {4, 2, -2.11803398}, {5, 0, -0.99999999}, {5, 1, -0.49999999},
        {6, 0, 1.500000000}};
    for (auto [nup, ndn, e0] : nup_ndn_e0) {
      test_tjdistributed_e0_real(bonds, nup, ndn, e0);
    }
  }

  {
    Log("tj_distributed: TJModel: six-site chain test, t=1.0, J=0.0, N=6");
    auto bonds = tJchain(6, 1.0, 0.0);
    std::vector<std::tuple<int, int, double>> nup_ndn_e0 = {
        {0, 0, 0.0},         {0, 1, -2.0},        {0, 2, -3.00000000},
        {0, 3, -4.00000000}, {0, 4, -2.99999999}, {0, 5, -2.00000000},
        {0, 6, 0.000000000}, {1, 0, -2.0},        {1, 1, -3.46410161},
        {1, 2, -3.99999999}, {1, 3, -3.46410161}, {1, 4, -1.99999999},
        {1, 5, 0.000000000}, {2, 0, -3.00000000}, {2, 1, -3.99999999},
        {2, 2, -3.46410161}, {2, 3, -1.99999999}, {2, 4, 0.000000000},
        {3, 0, -4.00000000}, {3, 1, -3.46410161}, {3, 2, -1.99999999},
        {3, 3, 0.000000000}, {4, 0, -2.99999999}, {4, 1, -1.99999999},
        {4, 2, 0.000000000}, {5, 0, -2.00000000}, {5, 1, 0.000000000},
        {6, 0, 0.000000000}};
    for (auto [nup, ndn, e0] : nup_ndn_e0) {
      test_tjdistributed_e0_real(bonds, nup, ndn, e0);
    }
  }

  {
    for (int n_sites = 1; n_sites < 8; ++n_sites) {
      Log("tj_distributed: tj_alltoall random (real) N={}", n_sites);
      auto bonds = tj_alltoall(n_sites);
      for (int nup = 0; nup <= n_sites; ++nup) {
        for (int ndn = 0; ndn <= n_sites - nup; ++ndn) {
          auto block = tJ(n_sites, nup, ndn);
          auto block2 = tJDistributed(n_sites, nup, ndn);

          double e0 = eigval0(bonds, block);
          double e02 = eigval0(bonds, block2);
          REQUIRE(close(e0, e02));
        }
      }
    }
  }
  {
    for (int n_sites = 1; n_sites < 8; ++n_sites) {
      Log("tj_distributed: tj_alltoall random (complex) N={}", n_sites);
      auto bonds = tj_alltoall_complex(n_sites);
      for (int nup = 0; nup <= n_sites; ++nup) {
        for (int ndn = 0; ndn <= n_sites - nup; ++ndn) {

          auto block = tJ(n_sites, nup, ndn);
          auto block2 = tJDistributed(n_sites, nup, ndn);

          double e0 = eigval0(bonds, block);
          double e02 = eigval0(bonds, block2);
          REQUIRE(close(e0, e02));
        }
      }
    }
  }

  
  Log.out("tj_distributed: HB triangular N=12 complex exchange");
  int n_sites = 12;
  std::vector<double> etas = {0.00, 0.01, 0.02,
                              0.03, 0.04, 0.05}; // dont change etas :-)
  for (auto eta : etas) {
    Log("eta: {}", eta);
    for (int nup = 0; nup <= n_sites; ++nup) {
      auto [bonds, e00] = testcases::spinhalf::triangular_12_complex(nup, eta);

      auto block = Spinhalf(n_sites, nup);
      auto block_tJ = tJDistributed(n_sites, nup, n_sites - nup);
      double e0_spinhalf = eigval0(bonds, block);
      double e0 = eigval0(bonds, block_tJ);
      // Log("{} {} {} {}", nup, e0_spinhalf, e0, e00);
      REQUIRE(close(e0_spinhalf, e0));
      if (nup == 6) {
        REQUIRE(std::abs(e0 - e00) < 1e-8);
      }
    }
  }
} catch (std::exception const &e) {
  traceback(e);
}
