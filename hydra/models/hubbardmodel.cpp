#include <lila/all.h>

#include <hydra/hilbertspaces/spinhalf.h>
#include <hydra/indexing/indextable.h>
#include <hydra/indexing/indexhubbard.h>
#include <hydra/utils/range.h>
#include <hydra/utils/bitops.h>

#include "hubbardmodel.h"
#include "hubbardmodeldetail.h"


namespace hydra { namespace models {
    
    template <class coeff_t>
    HubbardModel<coeff_t>::HubbardModel
    (BondList bondlist, Couplings couplings, hilbertspaces::hubbard_qn qn)
      : n_sites_(bondlist.n_sites()),
	qn_(qn)
    {
      hilbertspaces::Hubbard<uint32> hs(n_sites_, qn_);
      dim_ = hs.size();
      
      hubbardmodeldetail::set_hubbard_terms<coeff_t>
      (bondlist, couplings, hoppings_, hopping_amplitudes_,
       currents_, current_amplitudes_, interactions_, interaction_strengths_,
       onsites_, onsite_potentials_, U_);
    }

    template <class coeff_t>
    void HubbardModel<coeff_t>::set_qn(hilbertspaces::hubbard_qn qn) 
    { 
      qn_ = qn; 
      hilbertspaces::Hubbard<uint32> hs(n_sites_, qn_);
      dim_ = hs.size();
    }

    template <class coeff_t>
    lila::Matrix<coeff_t> HubbardModel<coeff_t>::matrix() const
    {
      using hilbertspaces::Spinhalf;
      using hilbertspaces::Hubbard;
      using indexing::IndexTable;
      using indexing::IndexHubbard;
      using utils::range;
      using utils::gbit;
      using utils::gbits;
      using utils::popcnt;
	
      Hubbard<uint32> hs(n_sites_, qn_);
      IndexHubbard<IndexTable<Spinhalf<uint32>, uint32>> indexing(hs);
      int dim = indexing.size();
      lila::Matrix<coeff_t> hamilton(dim, dim);
      lila::Zeros(hamilton);

      // Apply Hubbard U term
      if (std::abs(U_) > 1e-14)
	{
	  for (int idx : range<>(indexing.size()))
	    {
	      auto state = indexing.state(idx);
	      hamilton(idx,idx) += U_*(double)popcnt(state.upspins & 
						     state.downspins);
	    }
	}

      // Apply Hubbard V term
      int interaction_idx=0;
      for (auto pair : interactions_)
	{
	  const int s1 = pair.first; 
	  const int s2 = pair.second;
	  const double V = interaction_strengths_[interaction_idx];
	  if (std::abs(V) > 1e-14)
	    {
	      for (int idx : range<>(indexing.size()))
		{
		  auto state = indexing.state(idx);
		  hamilton(idx,idx) += 
		    V * (double)((gbit(state.upspins, s1) + 
				  gbit(state.downspins, s1))*
				 (gbit(state.upspins, s2) + 
				  gbit(state.downspins, s2)));
		}
	    }
	  ++interaction_idx;
	}

      // Apply onsite chemical potential
      int onsite_idx=0;
      for (auto site : onsites_)
	{
	  const double mu = onsite_potentials_[onsite_idx];
	  if (std::abs(mu) > 1e-14)
	    {
	      for (int idx : range<>(indexing.size()))
		{
		  auto state = indexing.state(idx);
		  hamilton(idx,idx) -= 
		    mu * (double)((gbit(state.upspins, site) + 
				   gbit(state.downspins, site)));
		}
	    }
	  ++onsite_idx;
	}
      
      // Apply hopping terms
      int hopping_idx=0;
      for (auto pair : hoppings_)
	{
	  const int s1 = std::min(pair.first, pair.second); 
	  const int s2 = std::max(pair.first, pair.second);
	  const coeff_t t = hopping_amplitudes_[hopping_idx];
	  const uint32 flipmask = ((uint32)1 << s1) | ((uint32)1 << s2);

	  if (std::abs(t) > 1e-14)
	    {
	      for (int idx : range<>(indexing.size()))
		{
		  auto state = indexing.state(idx);
		  const uint32& upspins = state.upspins;
		  const uint32& downspins = state.downspins;
		
		  // upspins hopping
		  if (((upspins & flipmask) != 0) && 
		      ((upspins & flipmask) != flipmask))
		    {
		      const double fermi = 
			popcnt(gbits(upspins, s2-s1-1, s1+1)) % 2==0 ? 1. : -1.;
		      // printf("fermi %f\n", fermi);
		      auto new_state = state;
		      new_state.upspins = upspins ^ flipmask;
		      int new_idx = indexing.index(new_state);
		      hamilton(new_idx, idx) -= fermi*t;
		    }

		  // downspins hopping
		  if (((downspins & flipmask) != 0) && 
		      ((downspins & flipmask) != flipmask))
		    {
		      const double fermi = 
			popcnt(gbits(downspins, s2-s1-1, s1+1)) % 2==0 ? 1.:-1.;
		      // printf("fermi %f\n", fermi);
		      auto new_state = state;
		      new_state.downspins = downspins ^ flipmask;
		      int new_idx = indexing.index(new_state);
		      hamilton(new_idx, idx) -= fermi*t;
		    }
		}
	    }
	  ++hopping_idx;
	}  // loop over hoppings

      return hamilton;
    }

    template <class coeff_t>
    void HubbardModel<coeff_t>::apply_hamiltonian
    (const lila::Vector<coeff_t>& in_vec, lila::Vector<coeff_t>& out_vec) const
    {
      using hilbertspaces::Spinhalf;
      using hilbertspaces::Hubbard;
      using indexing::IndexTable;
      using indexing::IndexHubbard;
      using utils::range;
      using utils::gbit;
      using utils::gbits;
      using utils::popcnt;
	
      Hubbard<uint32> hs(n_sites_, qn_);
      IndexHubbard<IndexTable<Spinhalf<uint32>, uint32>> indexing(hs);
      int64 dim = indexing.size();
      assert(in_vec.size() == dim);
      assert(out_vec.size() == dim);
      lila::Zeros(out_vec);

      // Apply Hubbard U term
      if (std::abs(U_) > 1e-14)
	{
	  for (int idx : range<>(indexing.size()))
	    {
	      auto state = indexing.state(idx);
	      auto coeff =  U_*(double)popcnt(state.upspins & state.downspins);
	      out_vec(idx) += coeff * in_vec(idx);
	    }
	}

      // Apply Hubbard V term
      int interaction_idx=0;
      for (auto pair : interactions_)
	{
	  const int s1 = pair.first; 
	  const int s2 = pair.second;
	  const double V = interaction_strengths_[interaction_idx];
	  if (std::abs(V) > 1e-14)
	    {
	      for (int idx : range<>(indexing.size()))
		{
		  auto state = indexing.state(idx);
		  auto coeff = 	
		    V * (double)((gbit(state.upspins, s1) + 
				  gbit(state.downspins, s1))*
				 (gbit(state.upspins, s2) + 
				  gbit(state.downspins, s2)));
		  out_vec(idx) += coeff * in_vec(idx); 

		}
	    }
	  ++interaction_idx;
	}

      // Apply onsite chemical potential
      int onsite_idx=0;
      for (auto site : onsites_)
	{
	  const double mu = onsite_potentials_[onsite_idx];
	  if (std::abs(mu) > 1e-14)
	    {
	      for (int idx : range<>(indexing.size()))
		{
		  auto state = indexing.state(idx);
		  auto coeff = 
		    mu * (double)((gbit(state.upspins, site) + 
				   gbit(state.downspins, site)));
		  out_vec(idx) -= coeff * in_vec(idx); 
		}
	    }
	  ++onsite_idx;
	}
      
      // Apply hopping terms
      int hopping_idx=0;
      for (auto pair : hoppings_)
	{
	  const int s1 = std::min(pair.first, pair.second); 
	  const int s2 = std::max(pair.first, pair.second);
	  const coeff_t t = hopping_amplitudes_[hopping_idx];
	  const uint32 flipmask = ((uint32)1 << s1) | ((uint32)1 << s2);
	  for (int idx : range<>(indexing.size()))
	    {
	      auto state = indexing.state(idx);
	      const uint32& upspins = state.upspins;
	      const uint32& downspins = state.downspins;
		
	      // upspins hopping
	      if (((upspins & flipmask) != 0) && 
		  ((upspins & flipmask) != flipmask))
		{
		  const double fermi = 
		    popcnt(gbits(upspins, s2-s1-1, s1+1)) % 2==0 ? 1. : -1.;
		  auto new_state = state;
		  new_state.upspins = upspins ^ flipmask;
		  int new_idx = indexing.index(new_state);
		  out_vec(new_idx) -= fermi*t*in_vec(idx);
		}

	      // downspins hopping
	      if (((downspins & flipmask) != 0) && 
		  ((downspins & flipmask) != flipmask))
		{
		  const double fermi = 
		    popcnt(gbits(downspins, s2-s1-1, s1+1)) % 2==0 ? 1. : -1.;
		  auto new_state = state;
		  new_state.downspins = downspins ^ flipmask;
		  int new_idx = indexing.index(new_state);
		  out_vec(new_idx) -= fermi*t*in_vec(idx);
		}
	    }
	  ++hopping_idx;
	}  // loop over hoppings
    }

    template <class coeff_t>
    hilbertspaces::hubbard_qn HubbardModel<coeff_t>::apply_fermion
    (const lila::Vector<coeff_t>& state_before, 
     lila::Vector<coeff_t>& state_after, 
     std::string type, int site) 
      const
    {
      using hilbertspaces::Spinhalf;
      using hilbertspaces::Hubbard;
      using indexing::IndexTable;
      using indexing::IndexHubbard;
      using utils::range;
      using utils::gbit;
      using utils::gbits;
      using utils::popcnt;
	
      Hubbard<uint32> hs_before(n_sites_, qn_);
      IndexHubbard<IndexTable<Spinhalf<uint32>, uint32>> indexing_before(hs_before);

      auto qn_after = qn_;
      if (type == "cdagup") ++qn_after.n_upspins;
      else if (type == "cup") --qn_after.n_upspins;
      else if (type == "cdagdn") ++qn_after.n_downspins;
      else if (type == "cdn") --qn_after.n_downspins;
      else 
	{
	  std::cerr << "Error in apply_fermion: Invalid fermion type!" << std::endl;
	  exit(EXIT_FAILURE);
	}

      Hubbard<uint32> hs_after(n_sites_, qn_after);
      IndexHubbard<IndexTable<Spinhalf<uint32>, uint32>> indexing_after(hs_after);
      int64 dim_after = indexing_after.size();

      state_after.resize(dim_after);

      const uint32 sitemask = (1 << site);
      const uint32 antisitemask = ~(1 << site);
      if (type == "cdagup")
	{
	  for (int idx : range<>(indexing_before.size()))
	    {
	      auto config = indexing_before.state(idx);
	      const uint32& upspins = config.upspins;

	      // raise local site val if 0
	      if (gbit(upspins, site) == 0) 
		{
		  auto new_config = config;
		  new_config.upspins |= sitemask;
		  int new_idx = indexing_after.index(new_config);
		  double fermi = 
		    popcnt(gbits(upspins, site, 0)) % 2==0 ? 1. : -1.;
		  state_after(new_idx) += fermi * state_before(idx);
		}
	    }
	} 
      else if (type == "cup")
	{
	  for (int idx : range<>(indexing_before.size()))
	    {
	      auto config = indexing_before.state(idx);
	      const uint32& upspins = config.upspins;
	
	      // lower local site val if 1
	      if (gbit(upspins, site) == 1) 
		{
		  auto new_config = config;
		  new_config.upspins &= antisitemask;
		  int new_idx = indexing_after.index(new_config);
		  double fermi = 
		    popcnt(gbits(upspins, site, 0)) % 2==0 ? 1. : -1.;
		  state_after(new_idx) += fermi * state_before(idx);
		}
	    }
	}
      else if (type == "cdagdn")
	{
	  for (int idx : range<>(indexing_before.size()))
	    {
	      auto config = indexing_before.state(idx);
	      const uint32& downspins = config.downspins;
	
	      // raise local site val if 0
	      if (gbit(downspins, site) == 0) 
		{
		  auto new_config = config;
		  new_config.downspins |= sitemask;
		  int new_idx = indexing_after.index(new_config);
		  double fermi = 
		    popcnt(gbits(downspins, site, 0)) % 2==0 ? 1. : -1.;
		  state_after(new_idx) += fermi * state_before(idx);
		}
	    }
	} 
      else if (type == "cdn")
	{
	  for (int idx : range<>(indexing_before.size()))
	    {
	      auto config = indexing_before.state(idx);
	      const uint32& downspins = config.downspins;
	
	      // lower local site val if 1
	      if (gbit(downspins, site) == 1) 
		{
		  auto new_config = config;
		  new_config.downspins &= antisitemask;
		  int new_idx = indexing_after.index(new_config);
		  double fermi = 
		    popcnt(gbits(downspins, site, 0)) % 2==0 ? 1. : -1.;
		  state_after(new_idx) += fermi * state_before(idx);
		}
	    }
	}
      return qn_after;
    }
    
    template class HubbardModel<double>;
    template class HubbardModel<complex>;    
  }
}