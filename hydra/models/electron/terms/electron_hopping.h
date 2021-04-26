#pragma once

#include <hydra/common.h>
#include <hydra/models/electron.h>
#include <hydra/operators/bondlist.h>
#include <hydra/operators/couplings.h>
#include <hydra/utils/bitops.h>

namespace hydra::electron {

template <class bit_t, class coeff_t, class Filler>
void do_hopping(BondList const &bonds, Couplings const &couplings,
                Electron<bit_t> const &block, Filler &&filler) {
  using utils::gbit;
  using utils::popcnt;

  auto hoppings = bonds.bonds_of_type("HOP");
  auto hoppings_up = bonds.bonds_of_type("HOPUP");
  auto hoppings_dn = bonds.bonds_of_type("HOPDN");
  for (auto hop : hoppings + hoppings_up + hoppings_dn) {

    if (hop.size() != 2)
      HydraLog.err("Error computing Electron hopping: "
                   "hoppings must have exactly two sites defined");

    std::string cpl = hop.coupling();
    if (couplings.defined(cpl) &&
        !lila::close(couplings[cpl], coeff_t(0.))) {

      int s1 = hop.site(0);
      int s2 = hop.site(1);
      bit_t flipmask = ((bit_t)1 << s1) | ((bit_t)1 << s2);
      int l = std::min(s1, s2);
      int u = std::max(s1, s2);
      bit_t spacemask = (((bit_t)1 << (u - l - 1)) - 1) << (l + 1);

      // Apply hoppings on upspins
      if ((hop.type() == "HOP") || (hop.type() == "HOPUP")) {
        idx_t idx_up = 0;
        for (auto up : Combinations(n_sites, n_up)) {
          if (popcnt(up & flipmask) == 1) {
            coeff_t t =
                (gbit(up, s1)) ? couplings[cpl] : lila::conj(couplings[cpl]);
            double fermi = popcnt(up & spacemask) & 1 ? -1. : 1.;
            bit_t up_flip = up ^ flipmask;
            idx_t idx_up_flip = block_out.index_up(up_flip);
            for (idx_t idx_dn = 0; idx_dn < size_dn; ++idx_dn) {
              idx_t idx_out = idx_up_flip * size_dn + idx_dn;
              idx_t idx_in = idx_up * size_dn + idx_dn;
              filler(idx_out, idx_in, t, fermi);
            }
          }
          ++idx_up;
        }
      }

      // Apply hoppings on dnspins
      if ((hop.type() == "HOP") || (hop.type() == "HOPDN")) {
        idx_t idx_dn = 0;
        for (auto dn : Combinations(n_sites, n_dn)) {
          if (popcnt(dn & flipmask) == 1) {
            coeff_t t =
                (gbit(dn, s1)) ? couplings[cpl] : lila::conj(couplings[cpl]);

            double fermi = popcnt(dn & spacemask) & 1 ? -1. : 1.;
            bit_t dn_flip = dn ^ flipmask;
            idx_t idx_dn_flip = block_out.index_dn(dn_flip);

            // TODO: replace this by lila slicing once available
            for (idx_t idx_up = 0; idx_up < size_up; ++idx_up) {
              idx_t idx_out = idx_up * size_dn + idx_dn_flip;
              idx_t idx_in = idx_up * size_dn + idx_dn;
              filler(idx_out, idx_in, t, fermi);
            }
          }
          ++idx_dn;
        }
      }
    }
  } // for (auto hop :...
}

} // namespace hydra::electron
