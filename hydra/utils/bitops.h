#ifndef HYDRA_UTILS_BITOPS_
#define HYDRA_UTILS_BITOPS_

#include "typedefs.h"

namespace hydra { namespace utils {

    inline uint16 gbit(uint16 x, int n) { return (x>>n) & (1U); }
    inline uint32 gbit(uint32 x, int n) { return (x>>n) & (1U); }
    inline uint64 gbit(uint64 x, int n) { return (x>>n) & (1UL); }

    inline uint16 gbits(uint16 x, int m, int n) { return (x>>n)&((1U<<m)-1); }
    inline uint32 gbits(uint32 x, int m, int n) { return (x>>n)&((1U<<m)-1); }
    inline uint64 gbits(uint64 x, int m, int n) { return (x>>n)&((1UL<<m)-1); }

    inline int popcnt(const uint16& x) { return __builtin_popcount(x); }
    inline int popcnt(const uint32& x) { return __builtin_popcount(x); }
    inline int popcnt(const uint64& x) { return __builtin_popcount(x); }

  }  // namespace utils
}  // namespace hydra
#endif
