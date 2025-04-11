#include <Novice.h>
#include <cmath>
#include <vector>
#include <iostream>

const char kWindowTitle[] = "LE2A_19_ヨシトダイキ_タイトル";

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
		if (det == 0) {
			std::cerr << "エラー: 行列は特異であり、逆行列は存在しません。" << std::endl;
			std::exit(EXIT_FAILURE); // プログラムを終了
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

static const int kRowHeight = 20;
static const int kColumnWidth = 60;
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label);

Matrix4x4 MakeIdentity();

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Matrix4x4 m1 = {
		3.2f,0.7f,9.6f,4.4f,
		5.5f,1.3f,7.8f,2.1f,
		6.9f,8.0f,2.6f,1.0f,
		0.5f,7.2f,5.1f,3.3f
	};

	Matrix4x4 m2 = {
		4.1f,6.5f,3.3f,2.2f,
		8.8f,0.6f,9.9f,7.7f,
		1.1f,5.5f,6.6f,0.0f,
		3.3f,9.9f,8.8f,2.2f
	};

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

		MatrixScreenPrintf(0, 0, m1 + m2, "Add");
		MatrixScreenPrintf(0, kRowHeight * 5, m1 - m2, "Subtract");
		MatrixScreenPrintf(0, kRowHeight * 5 * 2, m1 * m2, "Multiply");
		MatrixScreenPrintf(0, kRowHeight * 5 * 3, m1.Inverse(), "InverseM1");
		MatrixScreenPrintf(0, kRowHeight * 5 * 4, m2.Inverse(), "InverseM2");
		MatrixScreenPrintf(kColumnWidth * 5, 0, m1.Transpose(), "TransposeM1");
		MatrixScreenPrintf(kColumnWidth * 5, kRowHeight * 5, m2.Transpose(), "TransposeM2");
		MatrixScreenPrintf(kColumnWidth * 5, kRowHeight * 5 * 2, MakeIdentity(), "Identity");

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

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + row * kRowHeight + kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}

Matrix4x4 MakeIdentity() {
	Matrix4x4 matrix = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return matrix;
}