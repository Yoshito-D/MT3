#include <Novice.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <assert.h>

const char kWindowTitle[] = "LE2A_19_ヨシトダイキ_タイトル";

struct Vector3 {
	float x, y, z;

	Vector3 operator+(const Vector3& vector) const {
		return { x + vector.x, y + vector.y, z + vector.z };
	}

	Vector3 operator-(const Vector3& vector) const {
		return { x - vector.x, y - vector.y, z - vector.z };
	}

	Vector3 operator*(float scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	Vector3 operator/(float scalar) const {
		return { x / scalar, y / scalar, z / scalar };
	}

	float operator*(const Vector3& vector) const {
		return x * vector.x + y * vector.y + z * vector.z;
	}

	float Dot(const Vector3& vector) const {
		return x * vector.x + y * vector.y + z * vector.z;
	}

	float Length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	Vector3 Normalize() const {
		float length = Length();
		return { x / length, y / length, z / length };
	}
};

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

		// 行列式が0の場合、エラーを表示してプログラムを終了
		assert(det == 0);

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

static const int kRowHeight = 20;
static const int kColumnWidth = 60;
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label);

Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
Matrix4x4 MakeRotateXMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float radian);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Matrix4x4 orthographicMatrix = MakeOrthographicMatrix(-160.0f, 160.0f, 200.0f, 300.0f, 0.0f, 1000.0f);
	Matrix4x4 perspectiveFovMatrix = MakePerspectiveFovMatrix(0.63f, 1.33f, 0.1f, 1000.0f);
	Matrix4x4 viewportMatrix = MakeViewportMatrix(100.0f, 200.0f, 600.0f, 300.0f, 0.0f, 1.0f);

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		MatrixScreenPrintf(0, 0, orthographicMatrix, "orthographicMatrix");
		MatrixScreenPrintf(0, kRowHeight * 5, perspectiveFovMatrix, "perspectiveFovMatrix");
		MatrixScreenPrintf(0, kRowHeight * 5 * 2, viewportMatrix, "viewportMatrix");

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + row * kRowHeight + kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		translate.x,translate.y,translate.z,1.0f
	};

	return result;
}

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result = {
			scale.x,0.0f,0.0f,0.0f,
			0.0f,scale.y,0.0f,0.0f,
			0.0f,0.0f,scale.z,0.0f,
			0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result = {
			1.0f,0.0f,0.0f,0.0f,
			0.0f,std::cos(radian),std::sin(radian),0.0f,
			0.0f,-std::sin(radian),std::cos(radian),0.0f,
			0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result = {
				std::cos(radian),0.0f,-std::sin(radian),0.0f,
				0.0f,1.0f,0.0f,0.0f,
				std::sin(radian),0.0f,std::cos(radian),0.0f,
				0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result = {
				std::cos(radian),std::sin(radian),0.0f,0.0f,
				-std::sin(radian),std::cos(radian),0.0f,0.0f,
				0.0f,0.0f,1.0f,0.0f,
				0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 rotateXYZMatrix = MakeRotateXMatrix(rotate.x) * MakeRotateYMatrix(rotate.y) * MakeRotateZMatrix(rotate.z);
	Matrix4x4 result = {
				scale.x * rotateXYZMatrix.m[0][0],scale.x * rotateXYZMatrix.m[0][1],scale.x * rotateXYZMatrix.m[0][2],0.0f,
				scale.y * rotateXYZMatrix.m[1][0],scale.y * rotateXYZMatrix.m[1][1],scale.y * rotateXYZMatrix.m[1][2],0.0f,
				scale.z * rotateXYZMatrix.m[2][0],scale.z * rotateXYZMatrix.m[2][1],scale.z * rotateXYZMatrix.m[2][2],0.0f,
				translate.x,translate.y,translate.z,1.0f
	};

	return result;
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];
	assert(w != 0.0f);
	result = result / w;

	return result;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result = {
		1.0f / aspectRatio * (std::cos(fovY * 0.5f) / std::sin(fovY * 0.5f)),0.0f,0.0f,0.0f,
		0.0f,std::cos(fovY * 0.5f) / std::sin(fovY * 0.5f),0.0f,0.0f,
		0.0f,0.0f,farClip / (farClip - nearClip),1.0f,
		0.0f,0.0f,(-nearClip * farClip) / (farClip - nearClip),0.0f
	};

	return result;
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result = {
		2.0f / (right - left),0.0f,0.0f,0.0f,
		0.0f,2.0f / (top - bottom),0.0f,0.0f,
		0.0f,0.0f,1.0f / (farClip - nearClip),0.0f,
		(left + right) / (left - right),(top + bottom) / (bottom - top),nearClip / (nearClip - farClip),1.0f
	};

	return result;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result = {
		width * 0.5f,0.0f,0.0f,0.0f,
		0.0f,-height * 0.5f,0.0f,0.0f,
		0.0f,0.0f,maxDepth - minDepth,0.0f,
		left + width * 0.5f,top + height * 0.5f,minDepth,1.0f
	};

	return result;
}