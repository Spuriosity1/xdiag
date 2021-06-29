#include "testcases_spinhalf.h"

namespace hydra::testcases::spinhalf {

std::tuple<BondList, Couplings> HBchain(int n_sites, double J) {

  BondList bondlist;
  Couplings couplings;
  couplings["J"] = J;
  for (int s = 0; s < n_sites; ++s) {
    bondlist << Bond("HB", "J", {s, (s + 1) % n_sites});
  }
  return std::make_tuple(bondlist, couplings);
}

std::tuple<BondList, Couplings, lila::Vector<double>>
HBchain_fullspectrum_nup(int L, int nup) {

  auto [bondlist, couplings] = HBchain(L, 1.0);

  lila::Vector<double> eigs;
  if (L == 2) {
    if ((nup == 0) || (nup == 2))
      eigs = {0.5};
    else if (nup == 1)
      eigs = {-1.5, 0.5};

  } else if (L == 3) {
    if ((nup == 0) || (nup == 3))
      eigs = {0.75};
    else if ((nup == 1) || (nup == 2))
      eigs = {-0.75, -0.75, 0.75};

  } else if (L == 4) {
    if (nup == 2)
      eigs = {-2.0, -1.0, 0.0, 0.0, 0.0, 1.0};
    else if ((nup == 1) || (nup == 3))
      eigs = {-1.0, 0.0, 0.0, 1.0};
    else if ((nup == 0) || (nup == 4))
      eigs = {1.0};

  } else if (L == 5) {
    if ((nup == 2) || (nup == 3)) {
      eigs = {-1.868033988749894,
              -1.868033988749894,
              -0.75,
              -0.559016994374947,
              -0.559016994374947,
              0.368033988749894,
              0.368033988749895,
              0.559016994374947,
              0.559016994374947,
              1.25};
    } else if ((nup == 1) || (nup == 4)) {
      eigs = {-0.559016994374948, -0.559016994374947, 0.559016994374947,
              0.559016994374947, 1.25};
    } else if ((nup == 0) || (nup == 5)) {
      eigs = {1.25};
    }

  } else if (L == 6) {
    if (nup == 3) {
      eigs = {-2.802775637731995e+00, -2.118033988749895e+00,
              -1.499999999999999e+00, -1.280776406404416e+00,
              -1.280776406404415e+00, -1.000000000000000e+00,
              -1.000000000000000e+00, -5.000000000000003e-01,
              -5.000000000000002e-01, -5.000000000000000e-01,
              -1.082836864502427e-16, 1.333450508964792e-16,
              1.180339887498945e-01,  4.999999999999998e-01,
              7.807764064044148e-01,  7.807764064044157e-01,
              8.027756377319950e-01,  1.000000000000001e+00,
              1.000000000000001e+00,  1.499999999999999e+00};
    } else if ((nup == 2) || (nup == 4)) {
      eigs = {-2.118033988749896e+00, -1.280776406404416e+00,
              -1.280776406404415e+00, -1.000000000000000e+00,
              -9.999999999999999e-01, -5.000000000000000e-01,
              -1.602135480420095e-16, -1.020727320943357e-16,
              1.180339887498948e-01,  4.999999999999999e-01,
              7.807764064044147e-01,  7.807764064044155e-01,
              9.999999999999997e-01,  1.000000000000000e+00,
              1.500000000000000e+00};
    } else if ((nup == 1) || (nup == 5)) {
      eigs = {-5.000000000000004e-01, 2.255027837597781e-17,
              3.556194393869664e-16,  9.999999999999998e-01,
              9.999999999999999e-01,  1.500000000000000e+00};
    } else if ((nup == 0) || (nup == 6)) {
      eigs = {1.5};
    }
  }
  return {bondlist, couplings, eigs};
}


std::tuple<BondList, Couplings> HB_alltoall(int n_sites)
{
  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0., 1.);

  BondList bondlist;
  Couplings couplings;
  for (int s1=0; s1<n_sites; ++s1)
    for (int s2=s1+1; s2<n_sites; ++s2)
      {
	std::stringstream ss;
	ss << "J" << s1 << "_" << s2;
	std::string name = ss.str();
        double value = distribution(generator);
	bondlist << Bond("HB", name, {s1, s2});
	couplings[name] = value;
      }
  return std::make_tuple(bondlist, couplings);
}
  
} // namespace hydra::testcases::spinhalf
