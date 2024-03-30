#pragma once

#include <hydra/operators/bondlist.h>
#include <hydra/symmetries/operations/symmetry_operations.h>
#include <hydra/symmetries/permutation.h>
#include <hydra/symmetries/permutation_group.h>
#include <hydra/symmetries/representation.h>

#include <random>

namespace hydra::testcases::electron {

BondList get_linear_chain(int64_t n_sites, double t, double U);

BondList get_linear_chain_hb(int64_t n_sites, double J);

std::tuple<PermutationGroup, std::vector<Representation>>
get_cyclic_group_irreps(int64_t n_sites);

std::tuple<PermutationGroup, std::vector<Representation>, std::vector<int64_t>>
get_cyclic_group_irreps_mult(int64_t n_sites);

BondList heisenberg_triangle();

BondList heisenberg_alltoall(int64_t n_sites);

BondList heisenberg_kagome15();
BondList heisenberg_kagome39();

BondList freefermion_alltoall(int64_t n_sites);

BondList freefermion_alltoall_complex_updn(int64_t n_sites);

// BondList tJchain(int64_t n_sites, double t, double J);

std::tuple<BondList, arma::Col<double>> randomAlltoAll4NoU();
std::tuple<BondList, arma::Col<double>> randomAlltoAll4();

BondList randomAlltoAll3();

BondList square2x2(double t, double J);

BondList square3x3(double t, double J);

} // namespace hydra::testcases::electron