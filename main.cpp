#define _USE_MATH_DEFINES
#include <Novice.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <assert.h>
#include <imgui.h>
#include <algorithm>

const char kWindowTitle[] = "LE2A_19_ヨシトダイキ_タイトル";
static const int kRowHeight = 20;
static const int kColumnWidth = 60;

static const float kWindowWidth = 1280.0f;
static const float kWindowHeight = 720.0f;

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

	Vector3 Cross(const Vector3& vector) const {
		return { y * vector.z - z * vector.y, z * vector.x - x * vector.z, x * vector.y - y * vector.x };
	}

	float Dot(const Vector3& vector) const {
		return x * vector.x + y * vector.y + z * vector.z;
	}

	float Length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	float LengthSquared() const {
		return x * x + y * y + z * z;
	}

	Vector3 Normalize() const {
		float length = Length();
		return { x / length, y / length, z / length };
	}

	Vector3 Project(const Vector3& vector) const {
		Vector3 normalized = vector.Normalize();
		return normalized * Dot(normalized);
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

struct Sphere {
	Vector3 center; //!< 中心点
	float radius; //!< 半径
};

struct Line {
	Vector3 origin; //!< 始点
	Vector3 diff; //!< 終点への差分ベクトル
};

struct Ray {
	Vector3 origin; //!< 始点
	Vector3 diff; //!< 終点への差分ベクトル
};

struct Segment {
	Vector3 origin; //!< 始点
	Vector3 diff; //!< 終点への差分ベクトル
};

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label);

Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
Matrix4x4 MakeRotateXMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float radian);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

bool IsCollision(const Sphere& s1, const Sphere& s2);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 cameraPosition = { 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate = { 0.26f,0.0f,0.0f };
	Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraPosition);
	Matrix4x4 viewMatrix = cameraMatrix.Inverse();
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, kWindowWidth / kWindowHeight, 0.1f, 100.0f);
	Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, kWindowWidth, kWindowHeight, 0.0f, 1.0f);

	Sphere sphere[2];
	sphere[0] = {
		{0.0f,0.0f,0.0f},
		0.5f
	};

	sphere[1] = {
		{1.0f,1.0f,1.0f},
		0.25f
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

		cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraPosition);
		viewMatrix = cameraMatrix.Inverse();
		projectionMatrix = MakePerspectiveFovMatrix(0.45f, kWindowWidth / kWindowHeight, 0.1f, 100.0f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid((viewMatrix * projectionMatrix), viewportMatrix);
		DrawSphere(sphere[0], (viewMatrix * projectionMatrix), viewportMatrix, IsCollision(sphere[0], sphere[1]) ? RED : WHITE);
		DrawSphere(sphere[1], (viewMatrix * projectionMatrix), viewportMatrix, WHITE);

#ifdef _DEBUG
		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraPosition.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("Sphere center", &sphere[0].center.x, 0.01f);
		ImGui::DragFloat("Sphere radius", &sphere[0].radius, 0.005f);

		ImGui::End();
#endif // _DEBUG


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
	//assert(w != 0.0f);
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

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / static_cast<float>(kSubdivision);
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		Vector3 start = { kGridEvery * static_cast<float>(xIndex) - kGridHalfWidth,0.0f,kGridEvery * 0.5f * static_cast<int>(kSubdivision) };
		Vector3 end = { kGridEvery * static_cast<float>(xIndex) - kGridHalfWidth,0.0f,-kGridEvery * 0.5f * static_cast<int>(kSubdivision) };
		Vector3 startScreenPos = Transform(start, viewProjectionMatrix);
		Vector3 endScreenPos = Transform(end, viewProjectionMatrix);
		startScreenPos = Transform(startScreenPos, viewportMatrix);
		endScreenPos = Transform(endScreenPos, viewportMatrix);
		if (xIndex == kSubdivision / 2) {
			Novice::DrawLine(
				static_cast<int>(startScreenPos.x),
				static_cast<int>(startScreenPos.y),
				static_cast<int>(endScreenPos.x),
				static_cast<int>(endScreenPos.y),
				0x222222ff
			);
		} else {
			Novice::DrawLine(
				static_cast<int>(startScreenPos.x),
				static_cast<int>(startScreenPos.y),
				static_cast<int>(endScreenPos.x),
				static_cast<int>(endScreenPos.y),
				0xaaaaaaff
			);
		}
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		Vector3 start = { kGridEvery * 0.5f * static_cast<int>(kSubdivision),0.0f,kGridEvery * static_cast<float>(zIndex) - kGridHalfWidth };
		Vector3 end = { -kGridEvery * 0.5f * static_cast<int>(kSubdivision),0.0f,kGridEvery * static_cast<float>(zIndex) - kGridHalfWidth };
		Vector3 startScreenPos = Transform(start, viewProjectionMatrix);
		Vector3 endScreenPos = Transform(end, viewProjectionMatrix);
		startScreenPos = Transform(startScreenPos, viewportMatrix);
		endScreenPos = Transform(endScreenPos, viewportMatrix);
		if (zIndex == kSubdivision / 2) {
			Novice::DrawLine(
				static_cast<int>(startScreenPos.x),
				static_cast<int>(startScreenPos.y),
				static_cast<int>(endScreenPos.x),
				static_cast<int>(endScreenPos.y),
				0x222222ff
			);
		} else {
			Novice::DrawLine(
				static_cast<int>(startScreenPos.x),
				static_cast<int>(startScreenPos.y),
				static_cast<int>(endScreenPos.x),
				static_cast<int>(endScreenPos.y),
				0xaaaaaaff
			);
		}
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;
	const float kLonEvery = 2.0f * static_cast<float>(M_PI) / static_cast<float>(kSubdivision);
	const float kLatEvery = static_cast<float>(M_PI) / static_cast<float>(kSubdivision);

	for (uint32_t latIndex = 0; latIndex <= kSubdivision; ++latIndex) {
		float lat = -static_cast<float>(M_PI) * 0.5f + kLatEvery * static_cast<float>(latIndex);
		for (uint32_t lonIndex = 0; lonIndex <= kSubdivision; ++lonIndex) {
			float lon = static_cast<float>(lonIndex) * kLonEvery;

			Vector3 a, b, c;
			// a, b, cをscreen座標系まで変換
			a.x = std::cos(lat) * std::cos(lon) * sphere.radius + sphere.center.x;
			a.y = std::sin(lat) * sphere.radius + sphere.center.y;
			a.z = std::cos(lat) * std::sin(lon) * sphere.radius + sphere.center.z;

			b.x = std::cos(lat + kLatEvery) * std::cos(lon) * sphere.radius + sphere.center.x;
			b.y = std::sin(lat + kLatEvery) * sphere.radius + sphere.center.y;
			b.z = std::cos(lat + kLatEvery) * std::sin(lon) * sphere.radius + sphere.center.z;

			c.x = std::cos(lat) * std::cos(lon + kLonEvery) * sphere.radius + sphere.center.x;
			c.y = std::sin(lat) * sphere.radius + sphere.center.y;
			c.z = std::cos(lat) * std::sin(lon + kLonEvery) * sphere.radius + sphere.center.z;

			a = Transform(a, viewProjectionMatrix);
			b = Transform(b, viewProjectionMatrix);
			c = Transform(c, viewProjectionMatrix);

			a = Transform(a, viewportMatrix);
			b = Transform(b, viewportMatrix);
			c = Transform(c, viewportMatrix);

			// abの線分を描画
			Novice::DrawLine(
				static_cast<int>(a.x),
				static_cast<int>(a.y),
				static_cast<int>(b.x),
				static_cast<int>(b.y),
				color
			);

			// bcの線分を描画
			Novice::DrawLine(
				static_cast<int>(a.x),
				static_cast<int>(a.y),
				static_cast<int>(c.x),
				static_cast<int>(c.y),
				color
			);
		}
	}
}

Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 result = segment.origin + (point - segment.origin).Project(segment.diff);
	return result;
}

bool IsCollision(const Sphere& s1, const Sphere& s2) {
	float distance = (s1.center - s2.center).Length();

	if (distance <= s1.radius + s2.radius) {
		return true;
	}

	return false;
}