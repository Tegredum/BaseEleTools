// encoding: utf-8
// author: Tegredum
// ide: Microsoft Visual Studio 2022

#ifndef __TEG_TEMPMARK_LINALG__
template<int a, int b>
struct min_int {
	static const int val = a < b? a : b;
};
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
	// 是否为行/列向量
	bool isVector();
	int getRows() const;
	int getCols() const;
	int getSize();
	double& operator()(int rowIdx, int colIdx);
	double& operator[](int idx);

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
#endif

public:
// 单位阵
void setIdentity() {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (rows!= cols) {
		throw std::runtime_error("Matrix must be square");
	}
#endif
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (i == j) {
				(*this)(i, j) = 1.0;
			}
			else {
				(*this)(i, j) = 0.0;
			}
		}
	}
}
static Matrix<MaxRows, MaxCols> eye(int rows) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (MaxRows != MaxCols) {
		throw std::runtime_error("Matrix must be square");
	}
	if (rows > MaxRows) {
		throw std::runtime_error("Matrix size out of range");
	}
#endif
	Matrix<MaxRows, MaxCols> result(rows, rows);
	result.setIdentity();
	return result;
}
static Matrix<MaxRows, MaxCols> eye() {
	return Matrix<MaxRows, MaxCols>::eye(MaxRows);
}
// 零阵
void setZero() {
	*this = Matrix<MaxRows, MaxCols>(rows, cols);
}
// 对角矩阵
static Matrix<max_int<MaxRows, MaxCols>::val, max_int<MaxRows, MaxCols>::val> diag() {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (!this->isVector()) {
		throw std::runtime_error("Matrix must be a vector");
	}
#endif
	int size = this->getSize();
	Matrix<max_int<MaxRows, MaxCols>::val, max_int<MaxRows, MaxCols>::val> result(size, size);
	for (int i = 0; i < size; i++) {
		result(i, i) = (*this)[i];
	}
	return result;
}
// 仅一个指定元素为 1，其余元素均为 0 的单位向量
static Matrix<MaxRows, MaxCols> elementVector(int idx, int size) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (idx < 0 || idx >= size) {
		throw std::runtime_error("Index out of range");
	}
	if (MaxRows != 1 && MaxCols != 1) {
		throw std::runtime_error("Matrix must be a vector");
	}
#endif
	Matrix<MaxRows, MaxCols> result(size);
	result[idx] = 1.0;
	return result;
}
template<int size>
static Matrix<min_int<MaxRows, size>::val, min_int<MaxCols, size>::val> elementVector(int idx) {
	return Matrix<min_int<MaxRows, size>::val, min_int<MaxCols, size>::val>::elementVector(idx, max_int<MaxRows, MaxCols>::val);
}
// 左右连接矩阵
template<int MaxRows2, int MaxCols2, MatrixType matType2>
Matrix<min_int<MaxRows, MaxRows2>::val, MaxCols + MaxCols2> operator,(const Matrix<MaxRows2, MaxCols2, matType2>& other) const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (rows != other.getRows()) {
		throw std::runtime_error("Matrix size mismatch");
	}
#endif
	Matrix<min_int<MaxRows, MaxRows2>::val, MaxCols + MaxCols2> result(rows, cols + other.getCols());
	result.colSegment(0, cols) = *this;
	result.colSegment(cols, other.getCols()) = other;
	return result;
}
// 上下连接矩阵
template<int MaxRows2, int MaxCols2, MatrixType matType2>
Matrix<MaxRows + MaxRows2, min_int<MaxCols, MaxCols2>::val> vertical_concat(const Matrix<MaxRows2, MaxCols2, matType2>& other) const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (this->cols != other.getCols()) {
		throw std::runtime_error("Matrix size mismatch");
	}
#endif
	Matrix<MaxRows + MaxRows2, min_int<MaxCols, MaxCols2>::val> result(rows + other.getRows(), cols);
	result.rowSegment(0, rows) = *this;
	result.rowSegment(rows, other.getRows()) = other;
	return result;
}

#ifndef __TEG_TEMPMARK_LINALG__
};
#endif
