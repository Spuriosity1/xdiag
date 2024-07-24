#include "../../catch.hpp"

#include <iostream>

#include "../electron/testcases_electron.hpp"
#include "../spinhalf/testcases_spinhalf.hpp"
#include <xdiag/algebra/algebra.hpp>
#include <xdiag/algebra/apply.hpp>
#include <xdiag/algebra/matrix.hpp>
#include <xdiag/algorithms/sparse_diag.hpp>
#include <xdiag/utils/close.hpp>

using namespace xdiag;

void test_spinhalf_symmetric_apply(OpSum ops, PermutationGroup space_group,
                                   std::vector<Representation> irreps) {
  int n_sites = space_group.n_sites();

  for (int nup = 0; nup <= n_sites; ++nup) {
    for (auto irrep : irreps) {
      auto block = Spinhalf(n_sites, nup, space_group, irrep);

      if (block.size() > 0) {
        auto H = matrixC(ops, block, block);
        REQUIRE(arma::norm(H - H.t()) < 1e-12);

        // Check whether apply gives the same as matrix multiplication
        arma::cx_vec v(block.size(), arma::fill::randn);
        arma::cx_vec w1 = H * v;
        arma::cx_vec w2(block.size(), arma::fill::zeros);
        apply(ops, block, v, block, w2);
        REQUIRE(close(w1, w2));

        // Compute eigenvalues and compare
        arma::vec evals_mat;
        arma::eig_sym(evals_mat, H);

        double e0_mat = evals_mat(0);
        double e0_app = eigval0(ops, block);
        // Log.out("e0_mat: {}, e0_app: {}", e0_mat, e0_app);
        REQUIRE(std::abs(e0_mat - e0_app) < 1e-7);

        // Compute eigenvalues with real arithmitic
        if (block.irrep().isreal() && ops.isreal()) {
          auto H_real = matrix(ops, block, block);
          arma::vec evals_mat_real;
          arma::eig_sym(evals_mat_real, H_real);

          REQUIRE(close(evals_mat_real, evals_mat));

          double e0_mat_real = evals_mat_real(0);
          double e0_app_real = eigval0(ops, block);
          REQUIRE(std::abs(e0_mat_real - e0_app_real) < 1e-7);
        }
      }
    }
  }
}

void test_spinhalf_symmetric_apply_no_sz(OpSum ops,
                                         PermutationGroup space_group,
                                         std::vector<Representation> irreps) {
  int n_sites = space_group.n_sites();

  for (auto irrep : irreps) {
    auto block = Spinhalf(n_sites, space_group, irrep);

    if (block.size() > 0) {
      auto H = matrixC(ops, block, block);
      REQUIRE(arma::norm(H - H.t()) < 1e-12);
      // Check whether apply gives the same as matrix multiplication
      arma::cx_vec v(block.size(), arma::fill::randn);
      arma::cx_vec w1 = H * v;
      arma::cx_vec w2(block.size(), arma::fill::zeros);
      apply(ops, block, v, block, w2);
      REQUIRE(close(w1, w2));
      // Compute eigenvalues and compare
      arma::vec evals_mat;
      arma::eig_sym(evals_mat, H);

      double e0_mat = evals_mat(0);
      double e0_app = eigval0(ops, block);

      // Log.out("e0_mat: {}, e0_app: {}", e0_mat, e0_app);
      REQUIRE(std::abs(e0_mat - e0_app) < 1e-7);

      // Compute eigenvalues with real arithmitic
      if (block.irrep().isreal() && ops.isreal()) {
        auto H_real = matrix(ops, block, block);
        arma::vec evals_mat_real;
        arma::eig_sym(evals_mat_real, H_real);

        REQUIRE(close(evals_mat_real, evals_mat));

        double e0_mat_real = evals_mat_real(0);
        double e0_app_real = eigval0(ops, block);
        REQUIRE(std::abs(e0_mat_real - e0_app_real) < 1e-7);
      }
    }
  }
}

void test_spinhalf_symmetric_apply_chains(int n_sites) {
  using namespace xdiag::testcases::spinhalf;
  using xdiag::testcases::electron::get_cyclic_group_irreps;
  Log.out("spinhalf_symmetric_apply: HB chain, N: {}", n_sites);
  auto [space_group, irreps] = get_cyclic_group_irreps(n_sites);
  auto ops = HBchain(n_sites, 1.0, 1.0);
  test_spinhalf_symmetric_apply(ops, space_group, irreps);
  test_spinhalf_symmetric_apply_no_sz(ops, space_group, irreps);
}

TEST_CASE("spinhalf_symmetric_apply", "[spinhalf]") {

  // Test linear Heisenberg chains
  for (int n_sites = 3; n_sites < 7; ++n_sites) {
    test_spinhalf_symmetric_apply_chains(n_sites);
  }

  // test a 3x3 triangular lattice
  Log.out("spinhalf_symmetric_apply: Triangular 3x3");
  std::string lfile = XDIAG_DIRECTORY
      "/misc/data/triangular.9.Jz1Jz2Jx1Jx2D1.sublattices.tsl.lat";

  auto ops = read_opsum(lfile);
  ops["Jz1"] = 1.00;
  ops["Jz2"] = 0.23;
  ops["Jx1"] = 0.76;
  ops["Jx2"] = 0.46;

  std::vector<std::pair<std::string, int>> rep_name_mult = {
      {"Gamma.D6.A1", 1}, {"Gamma.D6.A2", 1}, {"Gamma.D6.B1", 1},
      {"Gamma.D6.B2", 1}, {"Gamma.D6.E1", 2}, {"Gamma.D6.E2", 2},
      {"K.D3.A1", 2},     {"K.D3.A2", 2},     {"K.D3.E", 4},
      {"Y.D1.A", 6},      {"Y.D1.B", 6}};

  auto permutations = xdiag::read_permutations(lfile);
  auto space_group = PermutationGroup(permutations);

  std::vector<Representation> irreps;
  for (auto [name, mult] : rep_name_mult) {
    (void)mult;
    irreps.push_back(read_representation(lfile, name));
  }
  test_spinhalf_symmetric_apply(ops, space_group, irreps);
  test_spinhalf_symmetric_apply_no_sz(ops, space_group, irreps);

  // test J1-J2-Jchi triangular lattice
  {
    Log("spinhalf_symmetric_matrix: Triangular J1J2Jchi N=12");
    std::string lfile =
        XDIAG_DIRECTORY "/misc/data/triangular.j1j2jch/"
                        "triangular.12.j1j2jch.sublattices.fsl.lat";

    auto ops = read_opsum(lfile);
    ops["J1"] = 1.00;
    ops["J2"] = 0.15;
    ops["Jchi"] = -0.09;

    std::vector<std::pair<std::string, double>> rep_name_mult = {
        {"Gamma.C6.A", -6.9456000700824329641},
        {"Gamma.C6.B", -5.8410912437873072633},
        {"Gamma.C6.E1a", -3.8556417248355927541},
        {"Gamma.C6.E2a", -6.4157243358059030669},
        {"K.C3.A", -5.9197511811622431921},
        {"K.C3.Ea", -5.0281703836000861685},
        {"K.C3.Eb", -5.2045133473640809996},
        {"M.C2.A", -5.756684675081964464},
        {"M.C2.B", -5.7723510325561688816},
        {"X.C1.A", -5.9030627660522529965}};

    auto permutations = xdiag::read_permutations(lfile);
    auto space_group = PermutationGroup(permutations);

    int n_sites = 12;
    int n_up = 6;
    for (auto [name, energy] : rep_name_mult) {
      auto irrep = read_representation(lfile, name);
      auto spinhalf = Spinhalf(n_sites, n_up, space_group, irrep);
      auto e0 = eigval0(ops, spinhalf);
      Log("{} {:.12f} {:.12f}", name, e0, energy);

      REQUIRE(std::abs(e0 - energy) < 1e-10);
    }
  }
}
