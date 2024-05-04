#pragma once
#include "jlcxx/array.hpp"
#include "jlcxx/const_array.hpp"
#include "jlcxx/jlcxx.hpp"
#include "jlcxx/stl.hpp"
#include "jlcxx/tuple.hpp"

#include <xdiag/all.hpp>

#define JULIA_XDIAG_CALL_VOID(CMD)                                             \
  try {                                                                        \
    CMD;                                                                       \
  } catch (xdiag::Error const &e) {                                            \
    xdiag::error_trace(e);                                                     \
    throw(std::runtime_error("Error occurred in xdiag C++ core library"));     \
  }

#define JULIA_XDIAG_CALL_RETURN(CMD)                                           \
  try {                                                                        \
    return CMD;                                                                \
  } catch (xdiag::Error const &e) {                                            \
    xdiag::error_trace(e);                                                     \
    throw(std::runtime_error("Error occurred in xdiag C++ core library"));     \
    return decltype(CMD)();                                                    \
  }

#define JULIA_XDIAG_CALL_ASSIGN(LVALUE, CMD)                                   \
  try {                                                                        \
    LVALUE = CMD;                                                              \
  } catch (xdiag::Error const &e) {                                            \
    xdiag::error_trace(e);                                                     \
    throw(std::runtime_error("Error occurred in xdiag C++ core library"));     \
    return decltype(CMD)();                                                    \
  }
