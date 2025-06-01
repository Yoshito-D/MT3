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

	Vector3 translates[3] = {
		{0.2f,1.0f,0.0f},
		{0.4f,0.0f,0.0f},
		{0.3f,0.0f,0.0f}
	};

	Vector3 rotates[3] = {
		{0.0f,0.0f,-6.8f},
		{0.0f,0.0f,-1.4f},
		{0.0f,0.0f,0.0f}
	};

	Vector3 scales[3] = {
		{1.0f,1.0f,1.0f},
		{1.0f,1.0f,1.0f},
		{1.0f,1.0f,1.0f}
	};

	Matrix4x4 localMatrices[3] = {
		MakeAffineMatrix(scales[0], rotates[0], translates[0]),
		MakeAffineMatrix(scales[1], rotates[1], translates[1]),
		MakeAffineMatrix(scales[2], rotates[2], translates[2])
	};

	Matrix4x4 worldMatrices[3] = {
		localMatrices[0],
		localMatrices[1] * localMatrices[0],
		localMatrices[2] * localMatrices[1] * localMatrices[0]
	};

	Sphere spheres[3] = {};

	for (int32_t i = 0; i < 3; ++i) {
		spheres[i].radius = 0.05f;
		spheres[i].center = Transform(Vector3(0.0f, 0.0f, 0.0f), worldMatrices[i]);
	}

	Segment segments[2] = {
		{spheres[0].center, spheres[1].center - spheres[0].center},
		{spheres[1].center, spheres[2].center - spheres[1].center}
	};

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

		if (isDebugCamera) {
			if (Novice::IsPressMouse(2)) {
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

			if (Novice::IsPressMouse(1)) {
				ImVec2 delta = ImGui::GetIO().MouseDelta;
				if (delta.x != 0.0f || delta.y != 0.0f) {
					float rotateSpeed = 0.001f;

					cameraRotate.x += delta.y * rotateSpeed;
					cameraRotate.y += delta.x * rotateSpeed;
				}
			}
		}

		for (int32_t i = 0; i < 3; ++i) {
			localMatrices[i] = MakeAffineMatrix(scales[i], rotates[i], translates[i]);
		}

		worldMatrices[0] = localMatrices[0];
		worldMatrices[1] = localMatrices[1] * worldMatrices[0];
		worldMatrices[2] = localMatrices[2] * worldMatrices[1];

		for (int32_t i = 0; i < 3; ++i) {
			spheres[i].center = Transform(Vector3(0.0f, 0.0f, 0.0f), worldMatrices[i]);
		}

		segments[0].origin = spheres[0].center;
		segments[0].diff = spheres[1].center - spheres[0].center;
		segments[1].origin = spheres[1].center;
		segments[1].diff = spheres[2].center - spheres[1].center;

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
		Draw::DrawSphere(spheres[0], (viewMatrix * projectionMatrix), viewportMatrix, RED);
		Draw::DrawSphere(spheres[1], (viewMatrix * projectionMatrix), viewportMatrix, GREEN);
		Draw::DrawSphere(spheres[2], (viewMatrix * projectionMatrix), viewportMatrix, BLUE);
		Draw::DrawSegment(segments[0], (viewMatrix * projectionMatrix), viewportMatrix, WHITE);
		Draw::DrawSegment(segments[1], (viewMatrix * projectionMatrix), viewportMatrix, WHITE);

		Novice::ScreenPrintf(0, 0, "DebugCamera %d", isDebugCamera);

#ifdef _DEBUG
		ImGui::Begin("Window");
		const char* items[] = { "Shoulder", "Elbow", "Hand" };
		static int current_item = 0;

		if (ImGui::BeginCombo("Node", items[current_item])) {
			for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
				bool is_selected = (current_item == i);
				if (ImGui::Selectable(items[i], is_selected))
					current_item = i;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		// 選択に応じてスライダーを表示
		if (current_item == 0) {
			ImGui::DragFloat3("Scale", &scales[0].x, 0.01f);
			ImGui::DragFloat3("Rotate", &rotates[0].x, 0.01f);
			ImGui::DragFloat3("Translate", &translates[0].x, 0.01f);
		} else if (current_item == 1) {
			ImGui::DragFloat3("Scale", &scales[1].x, 0.01f);
			ImGui::DragFloat3("Rotate", &rotates[1].x, 0.01f);
			ImGui::DragFloat3("Translate", &translates[1].x, 0.01f);
		} else if (current_item == 2) {
			ImGui::DragFloat3("Scale", &scales[2].x, 0.01f);
			ImGui::DragFloat3("Rotate", &rotates[2].x, 0.01f);
			ImGui::DragFloat3("Translate", &translates[2].x, 0.01f);
		}
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