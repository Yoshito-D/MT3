#pragma once
#include <cmath>

struct Matrix4x4 {
	float m[4][4];

	Matrix4x4 operator+(const Matrix4x4& a) const {
		Matrix4x4 result;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				result.m[i][j] = m[i][j] + a.m[i][j];
			}
		}
		return result;
	}

	Matrix4x4 operator-(const Matrix4x4& a) const {
		Matrix4x4 result;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				result.m[i][j] = m[i][j] - a.m[i][j];
			}
		}
		return result;
	}

	Matrix4x4 operator*(const Matrix4x4& a) const {
		Matrix4x4 result;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				result.m[i][j] = 0;
				for (int k = 0; k < 4; k++) {
					result.m[i][j] += m[i][k] * a.m[k][j];
				}
			}
		}
		return result;
	}

	Matrix4x4 Inverse() const {
		Matrix4x4 result;
		Matrix4x4 mat = *this; // オリジナルの行列を保持するためにコピーを作成

		float det = 0;

		// 行列式を計算
		for (int i = 0; i < 4; ++i) {
			// 部分行列を作成
			float subMat[3][3];
			for (int sub_i = 1; sub_i < 4; ++sub_i) {
				int sub_j = 0;
				for (int sub_k = 0; sub_k < 4; ++sub_k) {
					if (sub_k == i) continue;
					subMat[sub_i - 1][sub_j] = mat.m[sub_i][sub_k];
					++sub_j;
				}
			}

			// 行列式を計算
			float subDet =
				subMat[0][0] * (subMat[1][1] * subMat[2][2] - subMat[1][2] * subMat[2][1])
				- subMat[0][1] * (subMat[1][0] * subMat[2][2] - subMat[1][2] * subMat[2][0])
				+ subMat[0][2] * (subMat[1][0] * subMat[2][1] - subMat[1][1] * subMat[2][0]);
			det += (i % 2 == 0 ? 1 : -1) * mat.m[0][i] * subDet;
		}

		// 各要素の余因子を計算して逆行列を求める
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				// 部分行列を作成
				float subMat[3][3];
				for (int sub_i = 0, row = 0; sub_i < 4; ++sub_i) {
					if (sub_i == i) continue;
					for (int sub_j = 0, col = 0; sub_j < 4; ++sub_j) {
						if (sub_j == j) continue;
						subMat[row][col] = mat.m[sub_i][sub_j];
						++col;
					}
					++row;
				}

				// 余因子行列の計算
				float subDet =
					subMat[0][0] * (subMat[1][1] * subMat[2][2] - subMat[1][2] * subMat[2][1])
					- subMat[0][1] * (subMat[1][0] * subMat[2][2] - subMat[1][2] * subMat[2][0])
					+ subMat[0][2] * (subMat[1][0] * subMat[2][1] - subMat[1][1] * subMat[2][0]);

				result.m[j][i] = ((i + j) % 2 == 0 ? 1 : -1) * subDet / det;
			}
		}

		return result;
	}

	Matrix4x4 Transpose() const {
		Matrix4x4 result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = m[j][i];
			}
		}
		return result;
	}
};