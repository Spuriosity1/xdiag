#pragma once
#ifdef HYDRA_USE_MPI

#include <mpi.h>

#include <iostream>
#include <string>

#define FMT_HEADER_ONLY
#include "extern/fmt/format.h"

#include <hydra/utils/error.h>

namespace hydra {

class LoggerMPI {
public:
  LoggerMPI() : verbosity_(0){};

  inline void set_verbosity(int verbosity) { verbosity_ = verbosity; }
  inline int verbosity() { return verbosity_; }

  template <typename... Args>
  inline void out(const std::string &format, const Args &...args) try {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0)
      std::cout << fmt::format(format, args...) << "\n";
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

  template <typename... Args>
  inline void warn(const std::string &format, const Args &...args) try {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0)
      std::cout << fmt::format(format, args...) << "\n" << std::flush;
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

  template <typename... Args>
  inline void err(const std::string &format, const Args &...args) try {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0)
      std::cerr << fmt::format(format, args...) << "\n" << std::flush;
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

  template <typename... Args>
  inline void out(int level, const std::string &format,
                  const Args &...args) try {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if ((rank == 0) && (level <= verbosity_))
      std::cout << fmt::format(format, args...) << "\n";
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

  template <typename... Args>
  inline void warn(int level, const std::string &format,
                   const Args &...args) try {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if ((rank == 0) && (level <= verbosity_))
      std::cout << fmt::format(format, args...) << "\n" << std::flush;
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

  template <typename... Args>
  inline void err(int level, const std::string &format,
                  const Args &...args) try {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if ((rank == 0) && (level <= verbosity_))
      std::cerr << fmt::format(format, args...) << "\n" << std::flush;
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

  template <typename... Args>
  inline void operator()(const std::string &format, const Args &...args) try {
    out(format, args...);
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

  template <typename... Args>
  inline void operator()(int level, const std::string &format,
                         const Args &...args) try {
    out(level, format, args...);
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

private:
  int verbosity_;
};

inline LoggerMPI LogMPI;

class LoggerMPIParallel {
public:
  LoggerMPIParallel() : verbosity_(0){};

  inline void set_verbosity(int verbosity) { verbosity_ = verbosity; }
  inline int verbosity() { return verbosity_; }

  template <typename... Args>
  inline void out(const std::string &format, const Args &...args) try {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    for (int r = 0; r < size; ++r) {
      if (r == rank) {
        std::cout << "[" << rank << "]: " << fmt::format(format, args...)
                  << "\n";
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

  template <typename... Args>
  inline void warn(const std::string &format, const Args &...args) try {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    for (int r = 0; r < size; ++r) {
      if (r == rank) {
        std::cout << "[" << rank << "]: " << fmt::format(format, args...)
                  << "\n"
                  << std::flush;
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

  template <typename... Args>
  inline void err(const std::string &format, const Args &...args) try {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    for (int r = 0; r < size; ++r) {
      if (r == rank) {
        std::cerr << "[" << rank << "]: " << fmt::format(format, args...)
                  << "\n"
                  << std::flush;
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

  template <typename... Args>
  inline void out(int level, const std::string &format,
                  const Args &...args) try {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    for (int r = 0; r < size; ++r) {
      if ((r == rank) && (level <= verbosity_)) {
        std::cout << "[" << rank << "]: " << fmt::format(format, args...)
                  << "\n";
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

  template <typename... Args>
  inline void warn(int level, const std::string &format,
                   const Args &...args) try {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    for (int r = 0; r < size; ++r) {
      if ((r == rank) && (level <= verbosity_)) {
        std::cout << "[" << rank << "]: " << fmt::format(format, args...)
                  << "\n"
                  << std::flush;
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

  template <typename... Args>
  inline void err(int level, const std::string &format,
                  const Args &...args) try {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    for (int r = 0; r < size; ++r) {
      if ((r == rank) && (level <= verbosity_)) {
        std::cerr << "[" << rank << "]: " << fmt::format(format, args...)
                  << "\n"
                  << std::flush;
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

  template <typename... Args>
  inline void operator()(const std::string &format, const Args &...args) try {
    out(format, args...);
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

  template <typename... Args>
  inline void operator()(int level, const std::string &format,
                         const Args &...args) try {
    out(level, format, args...);
  } catch (...) {
    HydraRethrow("Unable to print output using LoggerMPI");
  }

private:
  int verbosity_;
};

inline LoggerMPIParallel LogParallel;

} // namespace hydra

#endif
