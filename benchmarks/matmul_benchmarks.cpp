/**
   Copyright (c) 2014-2015, Sandia Corporation
   All rights reserved.

   This file is part of fast-matmul and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause.
*/

#include "all_algorithms.hpp"
#include "common.hpp"
#ifdef __INTEL_MKL__
# include "mkl.h"
#endif

#include <algorithm>
#include <stdexcept>
#include <vector>

// Run a single benchmark for multiplying m x k x n with num_steps of recursion.
// To just call GEMM, set num_steps to zero.
// The median of five trials is printed to std::cout.
// If run_check is true, then it also
void SingleBenchmark(int m, int k, int n, int num_steps, int algorithm) {
  // Run a set number of trials and pick the median time.
  int num_trials = 5;
  std::vector<double> times(num_trials);
  for (int trial = 0; trial < num_trials; ++trial) {
    Matrix<double> A = RandomMatrix<double>(m, k);
    Matrix<double> B = RandomMatrix<double>(k, n);
    Matrix<double> C1(m, n);
    times[trial] = RunAlgorithm(algorithm, A, B, C1, num_steps);
  }

  // Spit out the median time
  std::sort(times.begin(), times.end());
  size_t ind = num_trials / 2;
  std::cout << " " << m << " " << k << " " << n << " "
            << num_steps << " " << times[ind] << " "
            << "; ";
}


// Runs a set of benchmarks.
void BenchmarkSet(std::vector<int>& m_vals, std::vector<int>& k_vals,
                  std::vector<int>& n_vals, std::vector<int>& num_steps,
                  int algorithm) {

  assert(m_vals.size() == k_vals.size() && k_vals.size() == n_vals.size());

  for (int curr_num_steps : num_steps) {
    std::cout << Alg2Str(algorithm) << "_" << curr_num_steps << " = [";
    for (int i = 0; i < m_vals.size(); ++i) {
      SingleBenchmark(m_vals[i], k_vals[i], n_vals[i], curr_num_steps, algorithm);
    }
    std::cout << "];" << std::endl;
  }
  std::cout << std::endl << std::endl;
}


void SquareTest() {
  std::vector<int> m_vals;
  for (int i = 1000; i <= 7500; i += 500) {
    m_vals.push_back(i);
  }
  std::vector<int> num_levels = {0};
  BenchmarkSet(m_vals, m_vals, m_vals, num_levels, MKL);
  num_levels = {1, 2, 3};
  BenchmarkSet(m_vals, m_vals, m_vals, num_levels, STRASSEN);
  return;
}


void SquareTestPar() {
  std::vector<int> m_vals;
  for (int i = 1000; i <= 15000; i += 500) {
    m_vals.push_back(i);
  }

  std::vector<int> num_levels = {0};
#if defined(_PARALLEL_) && (_PARALLEL_ == _DFS_PAR_)
  BenchmarkSet(m_vals, m_vals, m_vals, num_levels, MKL);
#endif

  num_levels = {1, 2, 3};
  BenchmarkSet(m_vals, m_vals, m_vals, num_levels, STRASSEN);
}


void OuterTestPar() {
  std::vector<int> m_vals;
  for (int i = 3000; i <= 18000; i += 500) {
    m_vals.push_back(i);
  }
  std::vector<int> k_vals(m_vals.size(), 2800);

  std::vector<int> num_levels = {0};
#if defined(_PARALLEL_) && (_PARALLEL_ == _DFS_PAR_)
  BenchmarkSet(m_vals, k_vals, m_vals, num_levels, MKL);
#endif
  num_levels = {1, 2};
  BenchmarkSet(m_vals, k_vals, m_vals, num_levels, FAST424_26_257);
}


void TSSquareTestPar() {
  std::vector<int> m_vals;
  for (int i = 3000; i <= 20000; i += 500) {
    m_vals.push_back(i);
  }
  std::vector<int> k_vals(m_vals.size(), 3000);
  std::vector<int> num_levels = {0};

#if defined(_PARALLEL_) && (_PARALLEL_ == _DFS_PAR_)
  BenchmarkSet(m_vals, k_vals, k_vals, num_levels, MKL);
#endif
  num_levels = {1, 2};
  BenchmarkSet(m_vals, k_vals, k_vals, num_levels, FAST433_29_234);
}


void SquareBenchmark(int which) {
#ifdef _PARALLEL_
  std::vector<int> m_vals;
  for (int i = 9000; i <= 13000; i += 500) {
    m_vals.push_back(i);
  }
#else
  std::vector<int> m_vals;
  for (int i = 1000; i <= 7500; i += 500) {
    m_vals.push_back(i);
  }
#endif
  std::vector<int> num_levels_MKL = {0};
  std::vector<int> num_levels = {1, 2};

  switch (which) {
  case 0:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels_MKL, MKL);
    break;
  case 1:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, BINI322_10_52_APPROX);
    break;
  case 2:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, STRASSEN);
    break;
  case 3:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST322_11_50);
    break;
  case 4:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST422_14_84);
    break;
  case 5:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST323_15_103);
    break;
  case 6:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST332_15_103);
    break;
  case 7:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST522_18_99);
    break;
  case 8:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST252_18_99);
    break;
  case 9:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST234_20_144);
    break;
  case 10:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST324_20_144);
    break;
  case 11:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST423_20_144);
    break;
  case 12:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST342_20_144);
    break;
  case 13:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST333_23_152);
    break;
  case 14:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST424_26_257);
    break;
  case 15:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST442_26_257);
    break;
  case 16:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST433_29_234);
    break;
  case 17:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, FAST343_29_234);
    break;
  case 18:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, SMIRNOV336_40_960);
    break;
  case 19:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, SMIRNOV363_40_960);
    break;
  case 20:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, SMIRNOV633_40_960);
    break;
  case 21:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, SCHONHAGE333_21_117_APPROX);
    break;
  case 22:
    BenchmarkSet(m_vals, m_vals, m_vals, num_levels, CLASSICAL423);
    break;
  default:
    throw std::logic_error("Unknown algorithm");
  }
  
  return;
}


// (N, k, N) for fixed k ~ 2000
void OuterProductBenchmark(int which) {
  std::vector<int> m_vals;
#ifdef _PARALLEL_
  for (int i = 3000; i <= 18000; i += 500) {
    m_vals.push_back(i);
  }
  std::vector<int> k_vals(m_vals.size(), 2800);
#else
  for (int i = 2000; i <= 12000; i += 500) {
    m_vals.push_back(i);
  }
  std::vector<int> k_vals(m_vals.size(), 1600);
#endif

  std::vector<int> num_levels_MKL = {0};
  std::vector<int> num_levels = {1, 2};

  switch (which) {
  case 0:
    BenchmarkSet(m_vals, k_vals, m_vals, num_levels_MKL, MKL);
    break;
  case 1:
    BenchmarkSet(m_vals, k_vals, m_vals, num_levels, FAST424_26_257);
    break;
  case 2:
    BenchmarkSet(m_vals, k_vals, m_vals, num_levels, FAST433_29_234);
    break;
  case 3:
    BenchmarkSet(m_vals, k_vals, m_vals, num_levels, FAST323_15_103);
    break;
  case 4:
    BenchmarkSet(m_vals, k_vals, m_vals, num_levels, FAST522_18_99);
    break;
  case 5:
    BenchmarkSet(m_vals, k_vals, m_vals, num_levels, FAST423_20_144);
    break;
  case 6:
    BenchmarkSet(m_vals, k_vals, m_vals, num_levels, STRASSEN);
    break;
  case 7:
    BenchmarkSet(m_vals, k_vals, m_vals, num_levels, BINI322_10_52_APPROX);
    break;
  case 8:
    BenchmarkSet(m_vals, k_vals, m_vals, num_levels, SCHONHAGE333_21_117_APPROX);
    break;
  case 9:
    BenchmarkSet(m_vals, k_vals, m_vals, num_levels, SMIRNOV633_40_960);
    break;
  case 10:
    BenchmarkSet(m_vals, k_vals, m_vals, num_levels, CLASSICAL423);
    break;
  default:
    throw std::logic_error("Unknown algorithm");
  }
}


// (N, k, k) for fixed k ~ 2000
void TSSquareBenchmark(int which) {
  std::vector<int> m_vals;
#ifdef _PARALLEL_
  for (int i = 3000; i <= 20000; i += 500) {
    m_vals.push_back(i);
  }
  std::vector<int> k_vals(m_vals.size(), 3000);
#else
  for (int i = 10000; i <= 18000; i += 500) {
    m_vals.push_back(i);
  }
  std::vector<int> k_vals(m_vals.size(), 2400);
#endif

  std::vector<int> num_levels_MKL = {0};
  std::vector<int> num_levels = {1, 2};
  switch (which) {
  case 0:
    BenchmarkSet(m_vals, k_vals, k_vals, num_levels_MKL, MKL);
    break;
  case 1:
    BenchmarkSet(m_vals, k_vals, k_vals, num_levels, FAST424_26_257);
    break;
  case 2:
    BenchmarkSet(m_vals, k_vals, k_vals, num_levels, FAST433_29_234);
    break;
  case 3:
    BenchmarkSet(m_vals, k_vals, k_vals, num_levels, FAST323_15_103);
    break;
  case 4:
    BenchmarkSet(m_vals, k_vals, k_vals, num_levels, FAST522_18_99);
    break;
  case 5:
    BenchmarkSet(m_vals, k_vals, k_vals, num_levels, FAST423_20_144);
    break;
  case 6:
    BenchmarkSet(m_vals, k_vals, k_vals, num_levels, STRASSEN);
    break;
  case 7:
    BenchmarkSet(m_vals, k_vals, k_vals, num_levels, BINI322_10_52_APPROX);
    break;
  case 8:
    BenchmarkSet(m_vals, k_vals, k_vals, num_levels, SCHONHAGE333_21_117_APPROX);
    break;
  case 9:
    BenchmarkSet(m_vals, k_vals, k_vals, num_levels, SMIRNOV633_40_960);
    break;
  case 10:
    BenchmarkSet(m_vals, k_vals, k_vals, num_levels, CLASSICAL423);
    break;
  }
}


int main(int argc, char **argv) {
  auto opts = GetOpts(argc, argv);

  // Run all <N, N, N> benchmarks
  if (OptExists(opts, "square_all")) {
    for (int i = 0; i <= 21; ++i) {
      SquareBenchmark(i);
    }
  }

  // Run a single <N, N, N> benchmark
  if (OptExists(opts, "square")) {
    int which = GetIntOpt(opts, "square");
    SquareBenchmark(which);
  }

  // Run <N, k, N> benchmark for fixed k
  if (OptExists(opts, "outer_prod_like")) {
    int which = GetIntOpt(opts, "outer_prod_like");
    OuterProductBenchmark(which);
  }

  // Run <N, k, k> benchmark for fixed k
  if (OptExists(opts, "ts_square_like")) {
    int which = GetIntOpt(opts, "ts_square_like");
    TSSquareBenchmark(which);
  }


  // Functions for testing
  if (OptExists(opts, "square_test")) {
    SquareTest();
  }
  if (OptExists(opts, "square_test_par")) {
    SquareTestPar();
  }
  if (OptExists(opts, "outer_test_par")) {
    OuterTestPar();
  }
  if (OptExists(opts, "ts_square_test_par")) {
    TSSquareTestPar();
  }

  return 0;
}
