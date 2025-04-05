#include "../../BaseAlg_teg/LinAlg/LinAlg.h"
#include <iostream>
#include <cmath>

using namespace Teg::LinAlg;

#define TEST_CASE(name) std::cout << "\n=== TEST CASE: " << #name << " ===" << std::endl
#define TEST_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "Assertion failed: " << #expr << std::endl; \
            return false; \
        } else { \
            std::cout << "Passed: " << #expr << std::endl; \
        } \
    } while (0)

bool testConstruction() {
    TEST_CASE(Construction);
    
    Matrix<3, 3> mat1;
    TEST_ASSERT(mat1.getRows() == 3);
    TEST_ASSERT(mat1.getCols() == 3);
    
    Matrix<5, 5> mat2(2, 3);
    TEST_ASSERT(mat2.getRows() == 2);
    TEST_ASSERT(mat2.getCols() == 3);
    
    return true;
}

bool testElementAccess() {
    TEST_CASE(ElementAccess);
    
    Matrix<3, 3> mat;
    mat(0, 0) = 1.0;
    mat(1, 1) = 2.0;
    TEST_ASSERT(mat(0, 0) == 1.0);
    TEST_ASSERT(mat(1, 1) == 2.0);
    
    return true;
}

bool testMatrixInverse() {
    TEST_CASE(MatrixInverse);
    
    // 测试2x2矩阵求逆
    Matrix<2, 2> matA;
    matA(0, 0) = 4; matA(0, 1) = 7;
    matA(1, 0) = 2; matA(1, 1) = 6;
    
    auto invA = matA.inverse();
    
    // 验证A*A⁻¹ ≈ I
    auto product = matA * invA;
    TEST_ASSERT(fabs(product(0, 0) - 1.0) < 1e-6);
    TEST_ASSERT(fabs(product(1, 1) - 1.0) < 1e-6);
    TEST_ASSERT(fabs(product(0, 1)) < 1e-6);
    TEST_ASSERT(fabs(product(1, 0)) < 1e-6);
    
    // 测试单位矩阵求逆
    Matrix<3, 3> identity = Matrix<3, 3>::eye();
    auto invIdentity = identity.inverse();
    TEST_ASSERT(fabs(invIdentity(0, 0) - 1.0) < 1e-6);
    
    return true;
}

bool testEigenDecomposition() {
    TEST_CASE(EigenDecomposition);
    
    // 测试对称矩阵特征值分解
    Matrix<3, 3> symMat;
    symMat(0, 0) = 2; symMat(0, 1) = -1; symMat(0, 2) = 0;
    symMat(1, 0) = -1; symMat(1, 1) = 2; symMat(1, 2) = -1;
    symMat(2, 0) = 0; symMat(2, 1) = -1; symMat(2, 2) = 2;
    
    ColVector<3>::type eigVals;
    SquareMatrix<3>::type eigVecs;
    std::tie(eigVals, eigVecs) = symMat.eigh();
    
    // 验证特征值和特征向量性质
    for (int i = 0; i < 3; ++i) {
        auto vec = eigVecs.col(i);
        auto transformed = symMat * vec;
        auto scaled = vec * eigVals[i];
        
        // 验证 A*v = λ*v
        for (int j = 0; j < 3; ++j) {
            TEST_ASSERT(fabs(transformed[j] - scaled[j]) < 1e-6);
        }
    }
    
    // 验证特征向量正交性
    for (int i = 0; i < 3; ++i) {
        for (int j = i+1; j < 3; ++j) {
            double dotProduct = (eigVecs.col(i).transpose() * eigVecs.col(j))[0];
            TEST_ASSERT(fabs(dotProduct) < 1e-6);
        }
    }
    
    return true;
}

int main() {
    int passed = 0;
    int total = 0;
    
    std::cout << "===== Starting LinAlg Test Suite =====" << std::endl;
    
    if (testConstruction()) passed++; total++;
    if (testElementAccess()) passed++; total++;
    if (testMatrixInverse()) passed++; total++;
    if (testEigenDecomposition()) passed++; total++;
    
    std::cout << "\n===== Test Summary =====" << std::endl;
    std::cout << "Passed: " << passed << "/" << total << std::endl;
    
    return passed == total ? 0 : 1;
}
