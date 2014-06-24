#include "fast333.hpp"

#include <stdlib.h>
#include <time.h>

#include <chrono>  // note: C++11
#include <random>

// Build with: g++ main.cpp -std=c++0x -O3 -lblas -o test
// Use with:
//    ./test n numsteps run_classical run_fast
// n is the size of the matrix (default 2187)
// numsteps is the number of recursive steps (default 1)
// run_classical is 0/1 on whether or not to run the classical algorithm (default 0)
// run_fast is 0/1 on whether or not to run the fast algorithm (default 1)
int main(int argc, char **argv) {
    int n = 2187;
    int numsteps = 1;
    int run_classical = 0;
    int run_fast = 1;
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    if (argc > 2) {
        numsteps = atoi(argv[2]);
    }
    if (argc > 3) {
        run_classical = atoi(argv[3]);
    }
    if (argc > 4) {
        run_fast = atoi(argv[4]);
    }

    std::cout << "n is: " << n << std::endl;
    std::cout << "number of recursive steps is: " << numsteps << std::endl;

    srand (time(NULL));

    Matrix<double> A(n);
    Matrix<double> B(n);
    Matrix<double> C1(n), C2(n);
    for (int j = 0; j < n; ++j) {
        for (int i = 0; i < n; ++i) {
            A.data()[i + j * A.stride()] = ((double) rand() / RAND_MAX) * 1024;
            B.data()[i + j * B.stride()] = ((double) rand() / RAND_MAX) * 1024;
        }
    }

    if (run_classical) {
        std::cout << "Running classical gemm..." << std::endl;
        auto t1 = std::chrono::high_resolution_clock::now();
        Gemm(A, B, C1);
        auto t2 = std::chrono::high_resolution_clock::now();
        std::cout << "Classical gemm took "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
                  << " milliseconds"
                  << std::endl;
    }

    if (run_fast) {
        std::cout << "Running fast matmul..." << std::endl;
        auto t3 = std::chrono::high_resolution_clock::now();
        FastMatmul(A, B, C2, numsteps);
        auto t4 = std::chrono::high_resolution_clock::now();
        std::cout << "Fast matmul took "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(t4-t3).count()
                  << " milliseconds"
                  << std::endl;
    }

    if (!run_fast || !run_classical) {
	return 0;
    }

    // Test for correctness.
    std::cout << "Maximum relative difference: " << MaxRelativeDiff(C1, C2) << std::endl;
    double diff = FrobeniusDiff(C1, C2);
    std::cout << "Frobenius norm solution relative difference: "
              << diff / FrobeniusNorm(C1)
              << std::endl;

    std::cout << "Frobenius norm of A: " << FrobeniusNorm(A) << std::endl;
    std::cout << "Frobenius norm of B: " << FrobeniusNorm(B) << std::endl;

    // Component-wise differences
    int step = n / 3;
    Matrix<double> C11A(C1.data(), C1.stride(), step, step);
    Matrix<double> C21A(C1.data() + step, C1.stride(), step, step);
    Matrix<double> C31A(C1.data() + 2 * step, C1.stride(), step, step);
    Matrix<double> C12A(C1.data() + step * C1.stride(), C1.stride(), step, step);
    Matrix<double> C22A(C1.data() + step * C1.stride() + step, C1.stride(), step, step);
    Matrix<double> C32A(C1.data() + step * C1.stride() + 2 * step, C1.stride(), step, step);
    Matrix<double> C13A(C1.data() + 2 * step * C1.stride(), C1.stride(), step, step);
    Matrix<double> C23A(C1.data() + 2 * step * C1.stride() + step, C1.stride(), step, step);
    Matrix<double> C33A(C1.data() + 2 * step * C1.stride() + 2 * step, C1.stride(), step, step);

    Matrix<double> C11B(C2.data(), C2.stride(), step, step);
    Matrix<double> C21B(C2.data() + step, C2.stride(), step, step);
    Matrix<double> C31B(C2.data() + 2 * step, C2.stride(), step, step);
    Matrix<double> C12B(C2.data() + step * C2.stride(), C2.stride(), step, step);
    Matrix<double> C22B(C2.data() + step * C2.stride() + step, C2.stride(), step, step);
    Matrix<double> C32B(C2.data() + step * C2.stride() + 2 * step, C2.stride(), step, step);
    Matrix<double> C13B(C2.data() + 2 * step * C2.stride(), C2.stride(), step, step);
    Matrix<double> C23B(C2.data() + 2 * step * C2.stride() + step, C2.stride(), step, step);
    Matrix<double> C33B(C2.data() + 2 * step * C2.stride() + 2 * step, C2.stride(), step, step);

    std::cout << "(1, 1): " << FrobeniusDiff(C11A, C11B) << std::endl;
    std::cout << "(2, 1): " << FrobeniusDiff(C21A, C21B) << std::endl;
    std::cout << "(3, 1): " << FrobeniusDiff(C31A, C31B) << std::endl;

    std::cout << "(1, 2): " << FrobeniusDiff(C12A, C12B) << std::endl;
    std::cout << "(2, 2): " << FrobeniusDiff(C22A, C22B) << std::endl;
    std::cout << "(3, 2): " << FrobeniusDiff(C32A, C32B) << std::endl;

    std::cout << "(1, 3): " << FrobeniusDiff(C13A, C13B) << std::endl;
    std::cout << "(2, 3): " << FrobeniusDiff(C23A, C23B) << std::endl;
    std::cout << "(3, 3): " << FrobeniusDiff(C33A, C33B) << std::endl;
}