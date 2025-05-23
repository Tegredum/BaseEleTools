// encoding: utf-8
// author: Tegredum
// ide: Microsoft Visual Studio 2022

#ifndef __TEG_TEMPMARK_LINALG__
#include <tuple>
#include <cmath>
template<int a, int b>
struct min_int {
	static const int val = a < b? a : b;
};
template<int a, int b, int c>
struct min_int {
	static const int val = min_int<min_int<a, b>::val, c>::val;
};
template<typename T>
T min_int(const T& a, const T& b);
template<int a, int b>
struct max_int {
	static const int val = a > b? a : b;
};
enum MatrixType {
	general,
	slice
};
template <int MaxRows, int MaxCols, MatrixType matType = general>
class Matrix {
private:
	double data[MaxRows][MaxCols];
	int rows;
	int cols;
	bool isValid;

	double (*rowDataPtrArray[MaxRows])[MaxCols];

public:
	Matrix(int rows, int cols);
	// 向量构造函数
	Matrix(int size);
	void setInvalid();
	// 是否为行/列向量
	bool isVector();
	bool isSquare();
	int getRows();
	int getCols();
	int getSize();
	double& operator()(int rowIdx, int colIdx);
	const double& operator()(int rowIdx, int colIdx) const;
	double& operator[](int idx);
	const double& operator[](int idx) const;

	double norm() const;

	std::tuple<double, int, int> maxElement() const;

	// 取一行
	Matrix<1, MaxCols, slice> row(int i);
	// 取一列
	Matrix<MaxRows, 1, slice> col(int j);
	// 取多行
	template <int rowsSelected>
	Matrix<rowsSelected, MaxCols, slice> rowSegment(int startIdx);
	Matrix<MaxRows, MaxCols, slice> rowSegment(int startIdx, int rowsSelected);
	// 取多列
	template <int colsSelected>
	Matrix<MaxRows, colsSelected, slice> colSegment(int startIdx);
	Matrix<MaxRows, MaxCols, slice> colSegment(int startIdx, int colsSelected);
	// 取子矩阵
	template <int rowsSelected, int colsSelected>
	Matrix<rowsSelected, colsSelected, slice> submatrix(int startRowIdx, int startColIdx);
	Matrix<MaxRows, MaxCols, slice> submatrix(int startRowIdx, int rowsSelected, int startColIdx, int colsSelected);

	// 仅一个指定元素为 1，其余元素均为 0 的单位向量
	static Matrix<MaxRows, MaxCols> elementVector(int idx, int size);
	template <int size>
	static Matrix<min_int<MaxRows, size>::val, min_int<MaxCols, size>::val> elementVector(int idx);

	void setIdentity()
	static Matrix<MaxRows, MaxCols> eye(int rows);
	static Matrix<MaxRows, MaxCols> eye();

	// 左右连接矩阵
	template<int MaxRows2, int MaxCols2, MatrixType matType2>
	Matrix<min_int<MaxRows, MaxRows2>::val, MaxCols + MaxCols2> operator,(const Matrix<MaxRows2, MaxCols2, matType2>& other) const;
	// 上下连接矩阵
	template<int MaxRows2, int MaxCols2, MatrixType matType2>
	Matrix<MaxRows + MaxRows2, min_int<MaxCols, MaxCols2>::val> vertical_concat(const Matrix<MaxRows2, MaxCols2, matType2>& other) const;

	// 以下切片操作仅用于列向量
	// 取前几个元素
	template <int sizeSelected>
	Matrix<sizeSelected, 1, slice> head();
	Matrix<MaxRows, 1, slice> head(int sizeSelected);
	// 取后几个元素
	template <int sizeSelected>
	Matrix<sizeSelected, 1, slice> tail();
	Matrix<MaxRows, 1, slice> tail(int sizeSelected);
	// 取中间几个元素
	template <int sizeSelected>
	Matrix<sizeSelected, 1, slice> segment(int startIdx);
	Matrix<MaxRows, 1, slice> segment(int startIdx, int sizeSelected);
#endif

public:
// 矩阵转置
Matrix<MaxCols, MaxRows> transpose() const {
	Matrix<MaxCols, MaxRows> res(cols, rows);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			res(j, i) = (*this)(i, j);
		}
	}
	return res;
}
// 全部元素取绝对值
Matrix<MaxRows, MaxCols> abs() const {
	Matrix<MaxRows, MaxCols> res(rows, cols);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			res(i, j) = std::abs((*this)(i, j));
		}
	}
	return res;
}
// 输入矩阵，由自己左除这个矩阵
enum LeftDivideMethod {
	GaussJordan	// 高斯约当消元法
};
template <int MaxRows_other, int MaxCols_other, MatrixType matType_other>
Matrix<min_int3<MaxRows, MaxRows_other, MaxCols>::val, MaxCols_other> leftDivide(const Matrix<MaxRows_other, MaxCols_other, matType_other>& other, LeftDivideMethod method = GaussJordan) const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (cols != other.getRows()) {
		throw std::invalid_argument("Matrix size not match");
	}
	if (!this->isSquare()) {
		throw std::invalid_argument("Matrix is not square");
	}
#endif
	Matrix<min_int3<MaxRows, MaxRows_other, MaxCols>::val, MaxCols_other> res(this->rows, other.getCols());
	if (method == GaussJordan) {
		// 高斯约当消元法
		int n = this->rows;
		// 构造增广矩阵
		Matrix<min_int3<MaxRows, MaxRows_other, MaxCols>::val, min_int3<MaxRows, MaxRows_other, MaxCols>::val + MaxCols_other> augMat = (*this, other);
		for (int colIdx = 0; colIdx < n; ++colIdx) {
			// 将当前列中绝对值最大的元素作为主元
			int maxRowIdx;
			std::tie(std::ignore, maxRowIdx, std::ignore) = augMat.col(colIdx).tail(n - colIdx).abs().maxElement();
			maxRowIdx += colIdx;
			// 交换主元所在行
			if (maxRowIdx != colIdx) {
				Matrix<1, min_int3<MaxRows, MaxRows_other, MaxCols>::val + MaxCols_other> tmpRow = augMat.row(colIdx);
				augMat.row(colIdx) = augMat.row(maxRowIdx);
				augMat.row(maxRowIdx) = tmpRow;
			}
			// 使主元为 1
			double pivot = augMat(colIdx, colIdx);
			if (pivot == 0.0) {
				res.setInvalid();
				return res;
			}
			augMat.row(colIdx).colSegment(colIdx, augMat.getCols() - colIdx) /= pivot;
			// 消去当前列的其他元素
			for (int rowIdx = 0; rowIdx < n; ++rowIdx) {
				if (rowIdx != colIdx) {
					double factor = augMat(rowIdx, colIdx);
					augMat.row(rowIdx).colSegment(colIdx, augMat.getCols() - colIdx) -= augMat.row(colIdx).colSegment(colIdx, augMat.getCols() - colIdx) * factor;
				}
			}
		}
		// 取结果
		res = augMat.colSegment(n, other.getCols());
	}
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	else {
		throw std::invalid_argument("Invalid left divide method");
	}
#endif
	return res;
}
// 矩阵求逆
Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val> inverse(LeftDivideMethod method = GaussJordan) const {
	return this->leftDivide(Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val>::eye(this->rows), method);
}
// 将向量处理为列向量
Matrix<max_int<MaxRows, MaxCols>::val, 1> convertToColVector() const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (!this->isVector()) {
		throw std::invalid_argument("Matrix is not a vector");
	}
#endif
	Matrix<max_int<MaxRows, MaxCols>::val, 1> res(this->getSize());
	for (int i = 0; i < res.getSize(); i++) {
		res[i] = (*this)[i];
	}
	return res;
}
// 向量归一化（返回列向量）
Matrix<MaxRows, 1> normalize() const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (!this->isVector()) {
		throw std::invalid_argument("Matrix is not a vector");
	}
#endif
	double norm = this->norm();
	if (norm == 0.0) {
		return Matrix<MaxRows, 1>(this->getSize());	// 返回零向量
	}
	else {
		return this->convertToColVector() / norm;
	}
}
// Householder 变换，返回变换矩阵与变换后的向量（列向量）
std::tuple< Matrix<max_int<MaxRows, MaxCols>::val, max_int<MaxRows, MaxCols>::val>, Matrix<max_int<MaxRows, MaxCols>::val, 1>, Matrix<max_int<MaxRows, MaxCols>::val, 1> > householderTransform() const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (!this->isVector()) {
		throw std::invalid_argument("Matrix is not a vector");
	}
#endif
	Matrix<max_int<MaxRows, MaxCols>::val, 1> e1 = Matrix<max_int<MaxRows, MaxCols>::val, 1>::elementVector(0, this->getSize());
	Matrix<max_int<MaxRows, MaxCols>::val, 1> v = e1 * this->norm();
	Matrix<max_int<MaxRows, MaxCols>::val, 1> y = (this->convertToColVector() - v).normalize();
	Matrix<max_int<MaxRows, MaxCols>::val, max_int<MaxRows, MaxCols>::val> H = Matrix<max_int<MaxRows, MaxCols>::val, max_int<MaxRows, MaxCols>::val>::eye(this->getSize()) - y * (y.transpose() * 2.0);
	return std::make_tuple(H, v, y);
}
// QR 分解
std::tuple< Matrix<MaxRows, MaxRows>, Matrix<MaxRows, MaxCols> > qrDecomposition() const {
	// 原理详见文档
	int n = min_teg<int>(this->rows, this->cols);
	Matrix<MaxRows, MaxRows> Qk = Matrix<MaxRows, MaxRows>::eye(this->rows);
	Matrix<MaxRows, MaxCols> Ak = *this;
	for (int kk = 0; kk < n - 1; ++kk) {
		// Householder 变换
		Matrix<MaxRows, MaxRows> Hk;
		Matrix<MaxRows, 1> vk;
		std::tie(Hk, vk, std::ignore) = Ak.col(kk).tail(this->rows - kk).householderTransform();
		// 更新 Qk
		Qk.rowSegment(kk, this->rows - kk) = Hk * Qk.rowSegment(kk, this->rows - kk);
		// 更新 Ak
		Ak.submatrix(kk, this->rows - kk, kk, this->cols - kk) = (vk, Hk * Ak.submatrix(kk, this->rows - kk, kk + 1, this->cols - kk - 1));
	}
	return std::make_tuple(Qk.transpose(), Ak);
}
// 判断一个方阵是否为对角矩阵
bool isDiagonal() const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (!this->isSquare()) {
		throw std::invalid_argument("Matrix is not square");
	}
#endif
	// 这是一个数值方法。在进行判断之前，应对矩阵进行归一化处理
	const double eps = 1e-8;	// 阈值
	const double scaleConst = 1.0;	// 放缩常数
	// 进行归一化处理
	Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val> scaledMat = *this;
	for (int idx = 0; idx < this->rows; ++idx) {
		if (scaledMat(idx, idx) != 0.0) {
			double multiplier = std::sqrt(scaleConst) / std::sqrt(std::abs(scaledMat(idx, idx)));
			scaledMat.row(idx) *= multiplier;
			scaledMat.col(idx) *= multiplier;
		}
	}
	// 判断非对角线元素是否存在非零值
	for (int rowIdx = 0; rowIdx < this->rows; ++rowIdx) {
		for (int colIdx = 0; colIdx < this->cols; ++colIdx) {
			if (rowIdx != colIdx && std::abs(scaledMat(rowIdx, colIdx)) >= eps) {
				return false;
			}
		}
	}
	return true;
}
// 判断三对角矩阵是否为对角矩阵
bool isDiagonal_triangular() const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (!this->isSquare()) {
		throw std::invalid_argument("Matrix is not square");
	}
	if (this->rows < 2) {
		throw std::invalid_argument("minimum dimension of matrix is needeed to be 2");
	}
#endif
	int n = this->rows;
	Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val> A = *this;
	for (int idx = 0; idx < n - 1; ++idx) {
		if (!A.submatrix<2, 2>(idx, idx).isDiagonal()) {
			return false;
		}
	}
	return true;
}
// 提取一个方阵的对角线元素，返回一个列向量
Matrix<min_int<MaxRows, MaxCols>::val, 1> diagonalElements() const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (!this->isSquare()) {
		throw std::invalid_argument("Matrix is not square");
	}
#endif
	Matrix<min_int<MaxRows, MaxCols>::val, 1> res(this->rows);
	for (int i = 0; i < this->rows; i++) {
		res[i] = (*this)(i, i);
	}
	return res;
}
// 对实对称矩阵进行特征值分解，返回特征值向量（列向量）与特征向量正交矩阵
enum EighMethod {
	qrIteration,	// QR 迭代法
	lanczos			// Lanczos 法
};
// QR 迭代法特征值分解
std::tuple< Matrix<min_int<MaxRows, MaxCols>::val, 1>, Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val> > eigh_qrIteration() const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (!this->isSquare()) {
		throw std::invalid_argument("Matrix is not square");
	}
#endif
	int n = this->getRows();
	Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val> Vk = Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val>::eye(n);	// 特征向量矩阵
	Matrix<min_int<MaxRows, MaxCols>::val, 1> eigVals;	// 特征值向量
	// QR 迭代法，原理详见文档
	const int maxIter = 128;	// 最大迭代次数
	int iterCount = 0;			// 迭代次数
	Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val> Ak = *this;
	while (true) {
		// QR 分解
		Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val> Qk, Rk;
		std::tie(Qk, Rk) = Ak.qrDecomposition();
		// 更新特征向量矩阵
		Vk = Vk * Qk;
		// 更新 Ak
		Ak = Rk * Qk;
		// 判断是否收敛
		if (Ak.isDiagonal()) {
			// 收敛为对角矩阵，其对角线元素即为特征值
			eigVals = Ak.diagonalElements();
			return std::make_tuple(eigVals, Vk);
		}
		++iterCount;
		if (iterCount >= maxIter) {
			// 迭代次数达到最大值，仍未收敛，返回无效结果
			eigVals.setInvalid();
			Vk.setInvalid();
			return std::make_tuple(eigVals, Vk);
		}
	}
}
// 三对角矩阵专用特征值分解
std::tuple< Matrix<min_int<MaxRows, MaxCols>::val, 1>, Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val> > eigh_triangular() const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (!this->isSquare()) {
		throw std::invalid_argument("Matrix is not square");
	}
	if (this->rows < 3) {
		throw std::invalid_argument("minimum dimension of matrix is needeed to be 3");
	}
#endif
	int n = this->rows;
	Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val> A = *this;
	Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val> V;	V.setIdentity();
	Matrix<min_int<MaxRows, MaxCols>::val, 1> eigVals;	// 特征值向量
	int iterCount = 0;
	// 原理见文档
	while (true) {
		Matrix<2, 2> Qk_array[min_int<MaxRows, MaxCols>::val - 1];	// 暂存 Qk 矩阵
		// 对各指定部分进行 QR 分解
		for (int kk = 0; kk < n - 2; ++kk) {
			Matrix<2, 2> Qk;
			Matrix<2, 3> Rk;
			std::tie(Qk, Rk) = A.submatrix<2, 3>(kk, kk).qrDecomposition();
			Qk_array[kk] = Qk;
			// 更新 V 的部分操作可以在这里直接做掉
			V.colSegment<2>(kk) = V.colSegment<2>(kk) * Qk;
			// 更新 A 的部分操作也可以在这里进行
			A.submatrix<2, 3>(kk, kk) = Rk;
		}
		// 最后一部分的 QR 分解
		Matrix<2, 2> Qk;
		Matrix<2, 2> Rk;
		std::tie(Qk, Rk) = A.submatrix<2, 2>(n - 2, n - 2).qrDecomposition();
		Qk_array[n - 2] = Qk;
		// 更新 V
		V.colSegment<2>(n - 2) = V.colSegment<2>(n - 2) * Qk;
		// 更新 A
		A.submatrix<2, 2>(n - 2, n - 2) = Rk;
		// 下面对 A 的更新类似于 QR 迭代中的 R * Q，不过针对三对角矩阵，有专门的提速手段
		A.submatrix<2, 2>(0, 0) = A.submatrix<2, 2>(0, 0) * Qk_array[0];
		for (int kk = 1; kk < n - 1; ++kk) {
			A.submatrix<3, 2>(kk - 1, kk) = A.submatrix<3, 2>(kk - 1, kk) * Qk_array[kk];
		}
		// 收敛判定
		if (A.isDiagonal_triangular()) {
			eigVals = A.diagonalElements();
			return std::make_tuple(eigVals, V);
		}
		++iterCount;
		const int maxIter = 128;	// 最大迭代次数
		if (iterCount >= maxIter) {
			// 迭代次数达到最大值，仍未收敛，返回无效结果
			eigVals.setInvalid();
			V.setInvalid();
			return std::make_tuple(eigVals, V);
		}
	}
}
std::tuple< Matrix<min_int<MaxRows, MaxCols>::val, 1>, Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val> > eigh(EighMethod method = lanczos) const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (!this->isSquare()) {
		throw std::invalid_argument("Matrix is not square");
	}
#endif
	// 如果输入的矩阵是 1x1 矩阵，则直接返回自己和一个 1x1 矩阵，元素为 1.0
	int n = this->rows;
	if (n == 1) {
		return std::make_tuple(*this, Matrix<1, 1>::eye());
	}
	Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val> A = ((*this) + this->transpose()) * 0.5;	// 处理为实对称矩阵
	// 如果输入的矩阵是 2x2 矩阵，则调用 QR 迭代方法
	if (n == 2) {
		return this->eigh_qrIteration();
	}
	if (method == qrIteration) {
		return A.eigh_qrIteration();
	}
	else if (method == lanczos) {
		// Lanczos 法，原理详见文档
		auto X = A;
		Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val> M = Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val>::eye(n);
		// 执行三对角化
		for (int kk = 0; kk < n - 2; ++kk) {
			Matrix<min_int<MaxRows, MaxCols>::val - 1, min_int<MaxRows, MaxCols>::val - 1> Hk;
			Matrix<min_int<MaxRows, MaxCols>::val - 1, 1> vk;
			std::tie(Hk, vk, std::ignore) = X.col(kk).tail(n - kk - 1).householderTransform();
			// 更新 M
			M.colSegment(kk + 1, n - kk - 1) = M.colSegment(kk + 1, n - kk - 1) * Hk;
			// 更新 X
			X.col(kk).tail(n - kk - 1) = vk;
			X.row(kk).colSegment(kk + 1, n - kk - 1) = vk.transpose();
			X.submatrix(kk + 1, n - kk - 1, kk + 1, n - kk - 1) = Hk * X.submatrix(kk + 1, n - kk - 1, kk + 1, n - kk - 1) * Hk;
		}
		// 针对所得三对角矩阵进行特征值分解
		Matrix<min_int<MaxRows, MaxCols>::val, 1> eigVals;
		Matrix<min_int<MaxRows, MaxCols>::val, min_int<MaxRows, MaxCols>::val> V;
		std::tie(eigVals, V) = X.eigh_triangular();
		if (!eigVals.isValidMatrix()) {
			// 异常返回分支
			return std::make_tuple(eigVals, V);
		}
		// 计算特征向量矩阵并返回
		return std::make_tuple(eigVals, M * V);
	}
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	else {
		throw std::invalid_argument("Invalid eigh method");
	}
#endif
}

#ifndef __TEG_TEMPMARK_LINALG__
};
#endif
