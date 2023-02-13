//
// Created by Luke Staszewski on 07.02.23.
//

#include "../../catch.hpp"
#include <hydra/all.h>

#include <complex>
#include <iostream>
using namespace std;
using namespace arma;

TEST_CASE("exp of identity", "[pade]") {

  auto I = eye(3, 3);
  cx_double i = {1, 0};
  Mat<cx_double> B = i * I;
  B.print();
  Mat<cx_double> E = hydra::expm(B);
  E.print();
}
