// encoding: utf-8
// author: Tegredum
// ide: Microsoft Visual Studio 2022

#ifndef __TEG_TEMPMARK_LINALG__
#include <cmath>
template<int a, int b>
struct min_int {
	static const int val = a < b? a : b;
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

public:
	bool isVector();
	int getRows();
	int getCols();
	int getSize();
	double& operator[](int idx);
#endif

public:
// 矩阵相加
template <int MaxRows_other, int MaxCols_other, MatrixType Type_other>
Matrix<min_int<MaxRows, MaxRows_other>::val, min_int<MaxCols, MaxCols_other>::val> operator+(const Matrix<MaxRows_other, MaxCols_other, Type_other>& other) const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (rows != other.getRows() || cols != other.getCols()) {
		throw std::invalid_argument("matrix size not match, cannot add");
	}
#endif
	Matrix<min_int<MaxRows, MaxRows_other>::val, min_int<MaxCols, MaxCols_other>::val> result(rows, cols);
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			result(i, j) = (*this)(i, j) + other(i, j);
		}
	}
	return result;
}
template <int MaxRows_other, int MaxCols_other, MatrixType Type_other>
Matrix<MaxRows, MaxCols_other, matType>& operator+=(const Matrix<MaxRows_other, MaxCols_other, Type_other>& other) {
	*this = *this + other;
	return *this;
}
// 矩阵相减
template <int MaxRows_other, int MaxCols_other, MatrixType Type_other>
Matrix<min_int<MaxRows, MaxRows_other>::val, min_int<MaxCols, MaxCols_other>::val> operator-(const Matrix<MaxRows_other, MaxCols_other, Type_other>& other) const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (rows != other.getRows() || cols != other.getCols()) {
		throw std::invalid_argument("matrix size not match, cannot subtract");
	}
#endif
	Matrix<min_int<MaxRows, MaxRows_other>::val, min_int<MaxCols, MaxCols_other>::val> result(rows, cols);
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			result(i, j) = (*this)(i, j) - other(i, j);
		}
	}
	return result;
}
template <int MaxRows_other, int MaxCols_other, MatrixType Type_other>
Matrix<MaxRows, MaxCols_other, matType>& operator-=(const Matrix<MaxRows_other, MaxCols_other, Type_other>& other) {
	*this = *this - other;
	return *this;
}
// 矩阵乘法
template <int MaxRows_other, int MaxCols_other, MatrixType Type_other>
Matrix<MaxRows, MaxCols_other> operator*(const Matrix<MaxRows_other, MaxCols_other, Type_other>& other) const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (cols != other.getRows()) {
		throw std::invalid_argument("matrix size not match, cannot multiply");
	}
#endif
Matrix<MaxRows, MaxCols_other> result(rows, other.getCols());
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < other.getCols(); ++j) {
			result(i, j) = 0;
			for (int k = 0; k < cols; ++k) {
				result(i, j) += (*this)(i, k) * other(k, j);
			}
		}
	}
	return result;
}
// 数乘
Matrix<MaxRows, MaxCols> operator*(double scalar) const {
	Matrix<MaxRows, MaxCols> result(rows, cols);
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			result(i, j) = (*this)(i, j) * scalar;
		}
	}
	return result;
}
Matrix<MaxRows, MaxCols, matType>& operator*=(double scalar) {
	*this = *this * scalar;
	return *this;
}
// 数除
Matrix<MaxRows, MaxCols> operator/(double scalar) const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (scalar == 0) {
		throw std::invalid_argument("division by zero");
	}
#endif
	Matrix<MaxRows, MaxCols> result(rows, cols);
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			result(i, j) = (*this)(i, j) / scalar;
		}
	}
	return result;
}
Matrix<MaxRows, MaxCols, matType>& operator/=(double scalar) {
	*this = *this / scalar;
	return *this;
}
// 向量点积
template <int MaxRows_other, int MaxCols_other, MatrixType Type_other>
double dot(const Matrix<MaxRows_other, MaxCols_other, Type_other>& other) const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (!this->isVector() || !other.isVector() || this->getSize() != other.getSize()) {
		throw std::invalid_argument("vector size not match, cannot dot product");
	}
#endif
	double result = 0.0;
	for (int i = 0; i < this->getSize(); ++i) {
		result += (*this)[i] * other[i];
	}
	return result;
}
// 向量叉乘（返回 3 维列向量）
template <int MaxRows_other, int MaxCols_other, MatrixType Type_other>
Matrix<3, 1> cross(const Matrix<MaxRows_other, MaxCols_other, Type_other>& other) const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (!this->isVector() || !other.isVector() || this->getSize() != 3 || other.getSize() != 3) {
		throw std::invalid_argument("vector size not match, cannot cross product");
	}
#endif
	Matrix<3, 1> result;
	result[0] = (*this)[1] * other[2] - (*this)[2] * other[1];
	result[1] = (*this)[2] * other[0] - (*this)[0] * other[2];
	result[2] = (*this)[0] * other[1] - (*this)[1] * other[0];
	return result;
}
// 范数计算（2-范数）
double norm() const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (!this->isVector()) {
		throw std::invalid_argument("matrix is not a vector, norm calculation has not been implemented");
	}
#endif
	double result = 0.0;
	for (int i = 0; i < this->getSize(); ++i) {
		result += (*this)[i] * (*this)[i];
	}
	return std::sqrt(result);
}

#ifndef __TEG_TEMPMARK_LINALG__
};
#endif
