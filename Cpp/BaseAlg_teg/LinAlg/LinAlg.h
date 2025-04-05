// encoding: utf-8
// author: Tegredum
// ide: Microsoft Visual Studio 2022
#pragma once

#include <cmath>
#include <tuple>
#include "../BaseFunc.h"

#define __TEG_TEMPMARK_LINALG__

#define __TEG_LINALG_ENABLE_PARAM_CHECK__	// 开启参数检查
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
#include <stdexcept>
#endif // __TEG_LINALG_ENABLE_PARAM_CHECK__

namespace Teg {
	namespace LinAlg {
		enum MatrixType {
			general,
			slice
		};
		template <int MaxRows, int MaxCols, MatrixType matType = general>
		class Matrix {
		private:
			double data[max_int<MaxRows * int(matType == general), 1>::val][max_int<MaxCols * int(matType == general), 1>::val];
			int rows;
			int cols;
			bool isValid;

			double *rowDataPtrArray[max_int<MaxRows * int(matType == slice), 1>::val];

		public:
			// 构造函数，默认为零矩阵
			Matrix(int rowsIn, int colsIn) {
				#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				if (rowsIn > MaxRows || colsIn > MaxCols) {
					throw std::out_of_range("Matrix size out of range.");
				}
				#endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				this->rows = rowsIn;
				this->cols = colsIn;
				this->isValid = true;
				if (matType == general) {
					for (int i = 0; i < rows; i++) {
						for (int j = 0; j < cols; j++) {
							this->data[i][j] = 0.0;
						}
					}
				}
				else if (matType == slice) {
					for (int i = 0; i < rows; i++) {
						this->rowDataPtrArray[i] = nullptr;
					}
				}
				#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				else {
					throw std::invalid_argument("Invalid matrix type.");
				}
				#endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
			}
			Matrix() : Matrix(MaxRows, MaxCols) {}
			// 向量构造函数
			Matrix(int size) {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				if (MaxRows != 1 && MaxCols != 1) {
					throw std::invalid_argument("Invalid matrix size.");
				}
#endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				if (MaxRows == 1) {
					*this = Matrix<1, MaxCols, matType>(1, size);
				}
				else {
					*this = Matrix<MaxRows, 1, matType>(size, 1);
				}
			}
			// 设定为无效矩阵
			void setInvalid() {
				isValid = false;
			}
			// 矩阵是否有效
			bool isValidMatrix() const { return isValid; }
			// 获取行数列数大小
			int getRows() const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				if (!isValid) {
					throw std::invalid_argument("Invalid matrix.");
				}
#endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				return rows;
			}
			int getCols() const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				if (!isValid) {
					throw std::invalid_argument("Invalid matrix.");
				}
#endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				return cols;
			}
			int getSize() const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				if (!isValid) {
					throw std::invalid_argument("Invalid matrix.");
				}
#endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				return rows * cols;
			}
			// 取元素运算符
			double& operator()(int rowIdx, int colIdx) {
				// #ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				// if (!isValid) {
				// 	throw std::invalid_argument("Invalid matrix.");
				// }
				// if (rowIdx < 0 || rowIdx >= rows || colIdx < 0 || colIdx >= cols) {
				// 	throw std::out_of_range("Matrix index out of range.");
				// }
				// #endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				// if (matType == general) {
				// 	return this->data[rowIdx][colIdx];
				// }
				// else if (matType == slice) {
				// 	return this->rowDataPtrArray[rowIdx][colIdx];
				// }
				// #ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				// else {
				// 	throw std::invalid_argument("Invalid matrix type.");
				// }
				// #endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				return *(this->getElementPtr(rowIdx, colIdx));
			}
			const double& operator()(int rowIdx, int colIdx) const {
				// #ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				// if (!isValid) {
				// 	throw std::invalid_argument("Invalid matrix.");
				// }
				// if (rowIdx < 0 || rowIdx >= rows || colIdx < 0 || colIdx >= cols) {
				// 	throw std::out_of_range("Matrix index out of range.");
				// }
				// #endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				// if (matType == general) {
				// 	return this->data[rowIdx][colIdx];
				// }
				// else if (matType == slice) {
				// 	return this->rowDataPtrArray[rowIdx][colIdx];
				// }
				// #ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				// else {
				// 	throw std::invalid_argument("Invalid matrix type.");
				// }
				// #endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				return *(this->getElementPtr(rowIdx, colIdx));
			}
			// 方括号运算符，按照优先第一行从左到右、第二行从左到右……的顺序访问元素
			double& operator[](int idx) {
				// #ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				// if (!isValid) {
				// 	throw std::invalid_argument("Invalid matrix.");
				// }
				// if (idx < 0 || idx >= rows * cols) {
				// 	throw std::out_of_range("Matrix index out of range.");
				// }
				// #endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				// if (matType == general) {
				// 	return this->data[idx / cols][idx % cols];
				// }
				// else if (matType == slice) {
				// 	return this->rowDataPtrArray[idx / cols][idx % cols];
				// }
				// #ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				// else {
				// 	throw std::invalid_argument("Invalid matrix type.");
				// }
				// #endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				return *(this->getElementPtr(idx / cols, idx % cols));
			}
			const double& operator[](int idx) const {
				// #ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				// if (!isValid) {
				// 	throw std::invalid_argument("Invalid matrix.");
				// }
				// if (idx < 0 || idx >= rows * cols) {
				// 	throw std::out_of_range("Matrix index out of range.");
				// }
				// #endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				// if (matType == general) {
				// 	return this->data[idx / cols][idx % cols];
				// }
				// else if (matType == slice) {
				// 	return this->rowDataPtrArray[idx / cols][idx % cols];
				// }
				// #ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				// else {
				// 	throw std::invalid_argument("Invalid matrix type.");
				// }
				// #endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				return (*this)(idx / cols, idx % cols);
			}
			// 取元素地址
			double* getElementPtr(int rowIdx, int colIdx) {
				#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				if (!isValid) {
					throw std::invalid_argument("Invalid matrix.");
				}
				if (rowIdx < 0 || rowIdx >= rows || colIdx < 0 || colIdx >= cols) {
					throw std::out_of_range("Matrix index out of range.");
				}
				#endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				if (matType == general) {
					return &this->data[rowIdx][colIdx];
				}
				else if (matType == slice) {
					return &this->rowDataPtrArray[rowIdx][colIdx];
				}
				#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				else {
					throw std::invalid_argument("Invalid matrix type.");
				}
				#endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
			}
			const double* getElementPtr(int rowIdx, int colIdx) const {
				#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				if (!isValid) {
					throw std::invalid_argument("Invalid matrix.");
				}
				if (rowIdx < 0 || rowIdx >= rows || colIdx < 0 || colIdx >= cols) {
					throw std::out_of_range("Matrix index out of range.");
				}
				#endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				if (matType == general) {
					return &this->data[rowIdx][colIdx];
				}
				else if (matType == slice) {
					return &this->rowDataPtrArray[rowIdx][colIdx];
				}
				#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				else {
					throw std::invalid_argument("Invalid matrix type.");
				}
				#endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
			}
			// 重载赋值运算符，进行拷贝赋值操作
			template <int OtherMaxRows, int OtherMaxCols, MatrixType OtherMatType>
			Matrix<MaxRows, MaxCols, matType>& operator=(const Matrix<OtherMaxRows, OtherMaxCols, OtherMatType>& other) {
				#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				if (other.getRows() > MaxRows || other.getCols() > MaxCols) {
					throw std::out_of_range("Matrix size out of range.");
				}
				#endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				this->isValid = other.isValidMatrix();
				if (this->isValid) {
					for (int i = 0; i < other.getRows(); i++) {
						for (int j = 0; j < other.getCols(); j++) {
							(*this)(i, j) = other(i, j);
						}
					}
					this->rows = other.getRows();
					this->cols = other.getCols();
				}
				return *this;
			}
			// 重载矩阵类型转换
			template <int OtherMaxRows, int OtherMaxCols>
			operator Matrix<OtherMaxRows, OtherMaxCols>() const {
				Matrix<OtherMaxRows, OtherMaxCols> result;
				result = *this;
				return result;
			}
			// 是否为行/列向量
			bool isVector() const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				if (!isValid) {
					throw std::invalid_argument("Invalid matrix.");
				}
#endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				return bool(rows == 1 || cols == 1);
			}
			// 是否为方阵
			bool isSquare() const {
#ifdef __TEG_LINALG_ENABLE_PARAM_CHECK__
				if (!isValid) {
					throw std::invalid_argument("Invalid matrix.");
				}
#endif // __TEG_LINALG_ENABLE_PARAM_CHECK__
				return bool(rows == cols);
			}
			// 找出最大值以及对应的行列索引
			std::tuple<double, int, int> maxElement() const {
				double maxVal = (*this)[0];
				int maxRowIdx = 0;
				int maxColIdx = 0;
				for (int i = 0; i < rows; i++) {
					for (int j = 0; j < cols; j++) {
						if ((*this)(i, j) > maxVal) {
							maxVal = (*this)(i, j);
							maxRowIdx = i;
							maxColIdx = j;
						}
					}
				}
				return std::make_tuple(maxVal, maxRowIdx, maxColIdx);
			}
			// 找到最小值以及对应的行列索引
			std::tuple<double, int, int> minElement() const {
				double minVal = (*this)[0];
				int minRowIdx = 0;
				int minColIdx = 0;
				for (int i = 0; i < rows; i++) {
					for (int j = 0; j < cols; j++) {
						if ((*this)(i, j) < minVal) {
							minVal = (*this)(i, j);
							minRowIdx = i;
							minColIdx = j;
						}
					}
				}
				return std::make_tuple(minVal, minRowIdx, minColIdx);
			}
			// 加减乘除等数学运算
			#include "CalculationOperators.teg"
			// 矩阵切片操作
			#include "SliceOperations.teg"
			// 特殊矩阵
			#include "SpecialMatrix.teg"
			// 矩阵变换操作
			#include "MatrixTransformation.teg"
		};

		template <int MaxSize>
		struct ColVector {
			typedef Matrix<MaxSize, 1> type;
		};
		template <int MaxSize>
		struct RowVector {
			typedef Matrix<1, MaxSize> type;
		};
		template <int MaxRows>
		struct SquareMatrix {
			typedef Matrix<MaxRows, MaxRows> type;
		};
	}
}

#undef __TEG_TEMPMARK_LINALG__