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
	
	// ===== 矩阵类用法演示 =====
	std::cout << "\n===== Matrix Class Demonstration =====" << std::endl;
	
	// 1. 矩阵构造和初始化
	Matrix<5, 5> matA(3, 4);  // 3行4列矩阵
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 4; ++j) {
			matA(i, j) = i * 4 + j + 1;
		}
	}
	std::cout << "\n1. Manually specified 3x4 matrix A:\n" << matA << std::endl;
	
	// 2. 零矩阵和单位矩阵
	Matrix<3, 3> matZero;  // 默认构造零矩阵
	Matrix<3, 3> matI = Matrix<3, 3>::eye(2);  // 2x2单位矩阵
	std::cout << "\n2. Zero matrix and identity matrix:\nZero matrix:\n" << matZero
			  << "\n2x2 identity matrix:\n" << matI << std::endl;
	
	// 3. 矩阵运算
	Matrix<2, 2> matB;
	matB(0, 0) = 1; matB(0, 1) = 2;
	matB(1, 0) = 3; matB(1, 1) = 4;
	
	Matrix<2, 2> matC;
	matC(0, 0) = 5; matC(0, 1) = 6;
	matC(1, 0) = 7; matC(1, 1) = 8;
	
	Matrix<2, 2> matAdd = matB + matC;
	Matrix<2, 2> matSub = matB - matC;
	Matrix<2, 2> matMul = matB * matC;
	Matrix<2, 2> matScalarMul = matB * 2.5;  // 矩阵数乘
	std::cout << "\n3. Matrix operations:\nAddition:\n" << matAdd
			  << "\nSubtraction:\n" << matSub
			  << "\nMultiplication:\n" << matMul
			  << "\nScalar multiplication (2.5):\n" << matScalarMul << std::endl;
	
	// 4. 矩阵最大/最小元素搜索
	Matrix<3, 3> matD;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			matD(i, j) = i * 3 + j + 1;
		}
	}
	double maxVal, minVal;
	int maxRow, maxCol, minRow, minCol;
	std::tie(maxVal, maxRow, maxCol) = matD.maxElement();
	std::tie(minVal, minRow, minCol) = matD.minElement();
	std::cout << "\n4. Matrix Max/Min Element Search:\nMax Element: " << maxVal << " (row index: " << maxRow << ", col index: " << maxCol << ')'
			  << "\nMin Element: " << minVal << " (row index: " << minRow << ", col index: " << minCol << ')' << std::endl;
	
	// 5. 矩阵求逆和左除
	Matrix<2, 2> matE;
	matE(0, 0) = 4; matE(0, 1) = 7;
	matE(1, 0) = 2; matE(1, 1) = 6;
	
	Matrix<2, 2> matE_inv = matE.inverse();
	Matrix<2, 1> matF;
	matF(0, 0) = 1; matF(1, 0) = 2;
	Matrix<2, 1> matG = matE.leftDivide(matF);
	std::cout << "\n5. Matrix inversion and left division:\nInverse matrix:\n" << matE_inv
			  << "\nLeft division result:\n" << matG << std::endl;
	
	// 6. 实对称矩阵特征值分解
	Matrix<3, 3> matSym;
	matSym(0, 0) = 2; matSym(0, 1) = -1; matSym(0, 2) = 0;
	matSym(1, 0) = -1; matSym(1, 1) = 2; matSym(1, 2) = -1;
	matSym(2, 0) = 0; matSym(2, 1) = -1; matSym(2, 2) = 2;
	
	ColVector<3>::type eigVals;
	SquareMatrix<3>::type eigVecs;
	std::tie(eigVals, eigVecs) = matSym.eigh();
	std::cout << "\n6. Eigen decomposition of real symmetric matrix:\nEigenvalues:\n" << eigVals
			  << "\nEigenvector matrix:\n" << eigVecs << std::endl;
	
	// 7. 子矩阵操作
	Matrix<4, 4> matH = Matrix<4, 4>::eye();
	Matrix<2, 2> matI2;
	matI2(0, 0) = 1; matI2(0, 1) = 1;
	matI2(1, 0) = 1; matI2(1, 1) = 1;
	
	// submatrix参数说明:
	// 参数1: 起始行索引
	// 参数2: 选择的行数
	// 参数3: 起始列索引
	// 参数4: 选择的列数
	matH.submatrix(1, 2, 1, 2) = matI2;
	
	std::cout << "\n7. Submatrix operations:\n" << matH << std::endl;
	
	// 8. 行操作和列操作
	Matrix<4, 1> matJ(3);
	matJ(0, 0) = 2; matJ(1, 0) = 2; matJ(2, 0) = 2;
	matH.col(0) = matJ;
	Matrix<3, 3> matK;
	Matrix<1, 3> matL;
	matL(0, 0) = 1; matL(0, 1) = 2; matL(0, 2) = 3;
	matK.row(1) = matL;
	std::cout << "\n8. Row operations:\n" << matK << std::endl;
	matK.col(0) = matJ;
	std::cout << "\nCol operations:\n" << matK << '\n';
	
	// 9. 向量操作
	ColVector<5>::type vecA;
	for (int i = 0; i < 5; ++i) vecA[i] = i + 1;
	ColVector<3>::type vecHead = vecA.head(3);
	ColVector<2>::type vecTail = vecA.tail(2);
	
	// segment参数说明:
	// 参数1: 起始索引
	// 参数2: 选择的元素个数
	ColVector<3>::type vecSeg = vecA.segment(1, 3);
	std::cout << "\n9. Vector operations:\nFirst 3 elements:\n" << vecHead
			  << "\nLast 2 elements:\n" << vecTail
			  << "\nElements 1-3 (segment(1,3)):\n" << vecSeg << std::endl;
	
	// 10. 矩阵连接
	Matrix<2, 2> matM = Matrix<2, 2>::eye();
	Matrix<2, 2> matN;
	matN(0, 0) = 1; matN(0, 1) = 2;
	matN(1, 0) = 3; matN(1, 1) = 4;
	Matrix<2, 4> matO = (matM, matN);  // 左右连接
	Matrix<4, 2> matP = matM.vertical_concat(matN);  // 上下连接
	std::cout << "\n10. Matrix concatenation:\nHorizontal concatenation:\n" << matO
			  << "\nVertical concatenation:\n" << matP << std::endl;
	
	// 11. 绝对值
	Matrix<2, 2> matQ;
	matQ(0, 0) = -1; matQ(0, 1) = -2;
	matQ(1, 0) = -3; matQ(1, 1) = -4;
	Matrix<2, 2> matAbs = matQ.abs();
	std::cout << "\n11. Element access and absolute value:\nOriginal matrix:\n" << matQ
			  << "\nAbsolute value matrix:\n" << matAbs << std::endl;
	
	return passed == total ? 0 : 1;
}
