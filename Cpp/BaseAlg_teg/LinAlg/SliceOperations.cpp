// encoding: utf-8
// author: Tegredum
// ide: Microsoft Visual Studio 2022

#ifndef __TEG_TEMPMARK_LINALG__
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
	// 取元素地址
	double* getElementPtr(int rowIdx, int colIdx);
#endif

public:
void setRowdataPtr_forSelectedRow(double ptr[MaxCols], int rowIdx) {
	this->rowDataPtrArray[rowIdx] = &ptr[0];
}
// 取一行
Matrix<1, MaxCols, slice> row(int i) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (i >= rows) {
		throw std::out_of_range("row index out of range");
	}
#endif
	Matrix<1, MaxCols, slice> res(1, cols);
	res.setRowdataPtr_forSelectedRow(this->getElementPtr(i, 0), 0);
	return res;
}
// 取一列
Matrix<MaxRows, 1, slice> col(int j) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (j >= cols) {
		throw std::out_of_range("column index out of range");
	}
#endif
	Matrix<MaxRows, 1, slice> res(rows, 1);
	for (int i = 0; i < rows; i++) {
		res.setRowdataPtr_forSelectedRow(this->getElementPtr(i, j), i);
	}
	return res;
}
// 取多行
template <int rowsSelected>
Matrix<rowsSelected, MaxCols, slice> rowSegment(int startIdx) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (startIdx + rowsSelected > rows) {
		throw std::out_of_range("row index out of range");
	}
#endif
	Matrix<rowsSelected, MaxCols, slice> res(rowsSelected, cols);
	for (int i = 0; i < rowsSelected; i++) {
		res.setRowdataPtr_forSelectedRow(this->getElementPtr(startIdx + i, 0), i);
	}
	return res;
}
Matrix<MaxRows, MaxCols, slice> rowSegment(int startIdx, int rowsSelected) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (startIdx + rowsSelected > rows) {
		throw std::out_of_range("row index out of range");
	}
#endif
	Matrix<MaxRows, MaxCols, slice> res(rowsSelected, cols);
	for (int i = 0; i < rowsSelected; i++) {
		res.setRowdataPtr_forSelectedRow(this->getElementPtr(startIdx + i, 0), i);
	}
	return res;
}
// 取多列
template <int colsSelected>
Matrix<MaxRows, colsSelected, slice> colSegment(int startIdx) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (startIdx + colsSelected > cols) {
		throw std::out_of_range("column index out of range");
	}
#endif
	Matrix<MaxRows, colsSelected, slice> res(rows, colsSelected);
	for (int i = 0; i < rows; i++) {
		res.setRowdataPtr_forSelectedRow(this->getElementPtr(i, startIdx), i);
	}
	return res;
}
Matrix<MaxRows, MaxCols, slice> colSegment(int startIdx, int colsSelected) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (startIdx + colsSelected > cols) {
		throw std::out_of_range("column index out of range");
	}
#endif
	Matrix<MaxRows, MaxCols, slice> res(rows, colsSelected);
	for (int i = 0; i < rows; i++) {
		res.setRowdataPtr_forSelectedRow(this->getElementPtr(i, startIdx), i);
	}
	return res;
}
// 取子矩阵
template <int rowsSelected, int colsSelected>
Matrix<rowsSelected, colsSelected, slice> submatrix(int startRowIdx, int startColIdx) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (startRowIdx + rowsSelected > rows || startColIdx + colsSelected > cols) {
		throw std::out_of_range("submatrix index out of range");
	}
#endif
	Matrix<rowsSelected, colsSelected, slice> res(rowsSelected, colsSelected);
	for (int i = 0; i < rowsSelected; i++) {
		res.setRowdataPtr_forSelectedRow(this->getElementPtr(startRowIdx + i, startColIdx), i);
	}
	return res;
}
Matrix<MaxRows, MaxCols, slice> submatrix(int startRowIdx, int rowsSelected, int startColIdx, int colsSelected) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (startRowIdx + rowsSelected > rows || startColIdx + colsSelected > cols) {
		throw std::out_of_range("submatrix index out of range");
	}
#endif
	Matrix<MaxRows, MaxCols, slice> res(rowsSelected, colsSelected);
	for (int i = 0; i < rowsSelected; i++) {
		res.setRowdataPtr_forSelectedRow(this->getElementPtr(startRowIdx + i, startColIdx), i);
	}
	return res;
}
// 以下切片操作仅用于列向量
// 取前几个元素
template <int sizeSelected>
Matrix<sizeSelected, 1, slice> head() {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (this->cols != 1) {
		throw std::invalid_argument("head() only works for column vectors");
	}
	if (sizeSelected > this->rows) {
		throw std::out_of_range("head index out of range");
	}
#endif
	Matrix<sizeSelected, 1, slice> res;
	for (int i = 0; i < sizeSelected; i++) {
		res.setRowdataPtr_forSelectedRow(this->getElementPtr(i, 0), i);
	}
	return res;
}
Matrix<MaxRows, 1, slice> head(int sizeSelected) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (this->cols != 1) {
		throw std::invalid_argument("head() only works for column vectors");
	}
	if (sizeSelected > this->rows) {
		throw std::out_of_range("head index out of range");
	}
#endif
	Matrix<MaxRows, 1, slice> res(sizeSelected);
	for (int i = 0; i < sizeSelected; i++) {
		res.setRowdataPtr_forSelectedRow(this->getElementPtr(i, 0), i);
	}
	return res;
}
// 取后几个元素
template <int sizeSelected>
Matrix<sizeSelected, 1, slice> tail() {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (this->cols != 1) {
		throw std::invalid_argument("tail() only works for column vectors");
	}
	if (sizeSelected > this->rows) {
		throw std::out_of_range("tail index out of range");
	}
#endif
	Matrix<sizeSelected, 1, slice> res;
	for (int i = 0; i < sizeSelected; i++) {
		res.setRowdataPtr_forSelectedRow(this->getElementPtr(this->rows - sizeSelected + i, 0), i);
	}
	return res;
}
Matrix<MaxRows, 1, slice> tail(int sizeSelected) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (this->cols != 1) {
		throw std::invalid_argument("tail() only works for column vectors");
	}
	if (sizeSelected > this->rows) {
		throw std::out_of_range("tail index out of range");
	}
#endif
	Matrix<MaxRows, 1, slice> res(sizeSelected);
	for (int i = 0; i < sizeSelected; i++) {
		res.setRowdataPtr_forSelectedRow(this->getElementPtr(this->rows - sizeSelected + i, 0), i);
	}
	return res;
}
// 取中间几个元素
template <int sizeSelected>
Matrix<sizeSelected, 1, slice> segment(int startIdx) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (this->cols != 1) {
		throw std::invalid_argument("segment() only works for column vectors");
	}
	if (startIdx + sizeSelected > this->rows) {
		throw std::out_of_range("segment index out of range");
	}
#endif
	Matrix<sizeSelected, 1, slice> res;
	for (int i = 0; i < sizeSelected; i++) {
		res.setRowdataPtr_forSelectedRow(this->getElementPtr(startIdx + i, 0), i);
	}
	return res;
}
Matrix<MaxRows, 1, slice> segment(int startIdx, int sizeSelected) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
	if (this->cols != 1) {
		throw std::invalid_argument("segment() only works for column vectors");
	}
	if (startIdx + sizeSelected > this->rows) {
		throw std::out_of_range("segment index out of range");
	}
#endif
	Matrix<MaxRows, 1, slice> res(sizeSelected);
	for (int i = 0; i < sizeSelected; i++) {
		res.setRowdataPtr_forSelectedRow(this->getElementPtr(startIdx + i, 0), i);
	}
	return res;
}

#ifndef __TEG_TEMPMARK_LINALG__
};
#endif
