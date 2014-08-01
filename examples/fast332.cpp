#include "linalg.hpp"
#include "fast332_15_103.hpp"
#include "timing.hpp"

#include <iostream>

int main(int argc, char **argv) {
    int m = 6000;
    int k = 6000;
    int n = 2000;
    int numsteps = 1;

    Matrix<double> A = RandomMatrix<double>(m, k);
    Matrix<double> B = RandomMatrix<double>(k, n);
    Matrix<double> C1(m, n), C2(m, n);

    Time([&] { Gemm(A, B, C1); }, "Classical gemm");
	Time([&] { grey332_15_103::FastMatmul(A, B, C2, numsteps); },
		 "Fast (3, 3, 2)");

    // Test for correctness.
    std::cout << "Maximum relative difference: " << MaxRelativeDiff(C1, C2) << std::endl;

    return 0;
}
