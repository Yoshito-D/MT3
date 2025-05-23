#define NOMINMAX
#include <Novice.h>
#include <cmath>
#include <imgui.h>
#include <algorithm>
#include "Math/Math.h"

const char kWindowTitle[] = "LE2A_19_ヨシトダイキ_タイトル";
static const int kRowHeight = 20;
static const int kColumnWidth = 60;

static const float kWindowWidth = 1280.0f;
static const float kWindowHeight = 720.0f;

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 cameraScale = { 1.0f,1.0f,1.0f };
	Vector3 cameraRotate = { 0.26f,0.0f,0.0f };
	Vector3 cameraPosition = { 0.0f,2.0f,-6.5f };
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraScale, cameraRotate, cameraPosition);
	Matrix4x4 viewMatrix = cameraMatrix.Inverse();
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, kWindowWidth / kWindowHeight, 0.1f, 100.0f);
	Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, kWindowWidth, kWindowHeight, 0.0f, 1.0f);

	AABB aabb1 = { { -0.5f,-0.5f,-0.5f },{ 0.0f,0.0f,0.0f } };

	aabb1.min.x = std::min(aabb1.min.x, aabb1.max.x);
	aabb1.min.y = std::min(aabb1.min.y, aabb1.max.y);
	aabb1.min.z = std::min(aabb1.min.z, aabb1.max.z);
	aabb1.max.x = std::max(aabb1.min.x, aabb1.max.x);
	aabb1.max.y = std::max(aabb1.min.y, aabb1.max.y);
	aabb1.max.z = std::max(aabb1.min.z, aabb1.max.z);

	AABB aabb2 = { { 0.2f,0.2f,0.2f },{ 1.0f,1.0f,1.0f } };

	aabb2.min.x = std::min(aabb2.min.x, aabb2.max.x);
	aabb2.min.y = std::min(aabb2.min.y, aabb2.max.y);
	aabb2.min.z = std::min(aabb2.min.z, aabb2.max.z);
	aabb2.max.x = std::max(aabb2.min.x, aabb2.max.x);
	aabb2.max.y = std::max(aabb2.min.y, aabb2.max.y);
	aabb2.max.z = std::max(aabb2.min.z, aabb2.max.z);

	bool isDebugCamera = true;

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

		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
			isDebugCamera = !isDebugCamera;
		}

		if (isDebugCamera && Novice::IsPressMouse(2)) {
			ImVec2 delta = ImGui::GetIO().MouseDelta;
			if (delta.x != 0.0f || delta.y != 0.0f) {
				float moveSpeed = 0.005f;

				Vector3 forward = { cosf(cameraRotate.x) * sinf(cameraRotate.y),
					sinf(cameraRotate.x),
					cosf(cameraRotate.x) * cosf(cameraRotate.y)
				};

				Vector3 up = { 0.0f,1.0f,0.0f };
				Vector3 right = up.Cross(forward).Normalize();

				Vector3 move = -right * delta.x * moveSpeed + up * delta.y * moveSpeed;

				cameraPosition += move;
			}
		}

		if (isDebugCamera && Novice::IsPressMouse(1)) {
			ImVec2 delta = ImGui::GetIO().MouseDelta;
			if (delta.x != 0.0f || delta.y != 0.0f) {
				float rotateSpeed = 0.001f;

				cameraRotate.x += delta.y * rotateSpeed;
				cameraRotate.y += delta.x * rotateSpeed;
			}
		}

		aabb1.min.x = std::min(aabb1.min.x, aabb1.max.x);
		aabb1.max.x = std::max(aabb1.min.x, aabb1.max.x);
		aabb1.min.y = std::min(aabb1.min.y, aabb1.max.y);
		aabb1.max.y = std::max(aabb1.min.y, aabb1.max.y);
		aabb1.min.z = std::min(aabb1.min.z, aabb1.max.z);
		aabb1.max.z = std::max(aabb1.min.z, aabb1.max.z);

		aabb2.min.x = std::min(aabb2.min.x, aabb2.max.x);
		aabb2.max.x = std::max(aabb2.min.x, aabb2.max.x);
		aabb2.min.y = std::min(aabb2.min.y, aabb2.max.y);
		aabb2.max.y = std::max(aabb2.min.y, aabb2.max.y);
		aabb2.min.z = std::min(aabb2.min.z, aabb2.max.z);
		aabb2.max.z = std::max(aabb2.min.z, aabb2.max.z);

		cameraMatrix = MakeAffineMatrix(cameraScale, cameraRotate, cameraPosition);
		viewMatrix = cameraMatrix.Inverse();
		projectionMatrix = MakePerspectiveFovMatrix(0.45f, kWindowWidth / kWindowHeight, 0.1f, 100.0f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Draw::DrawGrid((viewMatrix * projectionMatrix), viewportMatrix);
		Draw::DrawAABB(aabb1, (viewMatrix * projectionMatrix), viewportMatrix, Collision::isCollision(aabb1, aabb2) ? RED : WHITE);
		Draw::DrawAABB(aabb2, (viewMatrix * projectionMatrix), viewportMatrix, WHITE);

		Novice::ScreenPrintf(0, 0, "DebugCamera %d", isDebugCamera);

#ifdef _DEBUG
		ImGui::Begin("Window");
		ImGui::DragFloat3("aabb1.min", &aabb1.min.x, 0.01f);
		ImGui::DragFloat3("aabb1.max", &aabb1.max.x, 0.01f);

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