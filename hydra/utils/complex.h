#ifndef HYDRA_COMPLEX_H_
#define HYDRA_COMPLEX_H_

#include <lila/all.h>
#include "typedefs.h"

namespace hydra {

  template <class coeff_t>
  coeff_t ForceReal(lila::real_t<coeff_t> number, bool warn, std::string msg)
  { 
    (void) warn;
    (void) msg;
    return (coeff_t)number;
  }

  template <class coeff_t>
  coeff_t ForceReal(lila::complex_t<coeff_t> number, bool warn, std::string msg) 
  { 
    if ((warn) && !lila::close(lila::imag(number), 0.))
      std::cerr << msg << std::endl;
    return lila::real(number);
  }

}

#endif