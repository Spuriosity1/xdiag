#pragma once

#include <hydra/bitops/bitops.h>
#include <hydra/common.h>

#include <hydra/blocks/spinhalf/terms/apply_exchange.h>
#include <hydra/blocks/spinhalf/terms/apply_ising.h>
#include <hydra/blocks/spinhalf/terms/apply_scalar_chirality.h>
#include <hydra/blocks/spinhalf/terms/apply_spsm.h>
#include <hydra/blocks/spinhalf/terms/apply_sz.h>

namespace hydra::terms::spinhalf {

template <typename bit_t, typename coeff_t, bool symmetric, class IndexingIn,
          class IndexingOut, class Fill>
void apply_terms(BondList const &bonds, Couplings const &couplings,
                 IndexingIn const &indexing_in, IndexingOut const &indexing_out,
                 Fill &fill) {
  for (auto bond : bonds) {

    if (bond.type() == "EXCHANGE") {
      terms::spinhalf::apply_exchange<bit_t, coeff_t, symmetric>(
          bond, couplings, indexing_in, indexing_out, fill);
    } else if (bond.type() == "ISING") {
      terms::spinhalf::apply_ising<bit_t, coeff_t>(bond, couplings, indexing_in,
                                                   fill);
    } else if (bond.type() == "SZ") {
      terms::spinhalf::apply_sz<bit_t, coeff_t>(bond, couplings, indexing_in,
                                                fill);
    } else if (bond.type() == "S+") {
      terms::spinhalf::apply_spsm<bit_t, coeff_t, symmetric>(
          bond, couplings, indexing_in, indexing_out, fill, "S+");
    } else if (bond.type() == "S-") {
      terms::spinhalf::apply_spsm<bit_t, coeff_t, symmetric>(
          bond, couplings, indexing_in, indexing_out, fill, "S-");
    } else if (bond.type() == "SCALARCHIRALITY") {
      terms::spinhalf::apply_scalar_chirality<bit_t, coeff_t, symmetric>(
          bond, couplings, indexing_in, indexing_out, fill);
    } else {
      Log.err("Error in spinhalf::apply_terms: Unknown bond type {}",
              bond.type());
    }
  }
}

} // namespace hydra::terms::spinhalf