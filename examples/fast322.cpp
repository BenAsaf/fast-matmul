/**
   Copyright (c) 2014-2015, Sandia Corporation
   All rights reserved.

   This file is part of fast-matmul and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause.
*/

#include "linalg.hpp"
#include "fast322_11_50.hpp"
#include "timing.hpp"

#include <iostream>

int main(int argc, char **argv) {
  int m = 900;
  int k = 200;
  int n = 200;
  int numsteps = 1;

  Matrix<double> A = RandomMatrix<double>(m, k);
  Matrix<double> B = RandomMatrix<double>(k, n);
  Matrix<double> C1(m, n), C2(m, n);

  Time([&] { MatMul(A, B, C1); }, "Classical gemm");
  Time([&] { grey322_11_50::FastMatmul(A, B, C2, numsteps); },
       "Fast (3, 2, 2)");

  // Test for correctness.
  std::cout << "Maximum relative difference: " << MaxRelativeDiff(C1, C2)
            << std::endl;

  return 0;
}
