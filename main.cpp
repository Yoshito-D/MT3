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

	bool isDebugCamera = true;

	float deltaTime = 1.0f / 60.0f;

	Plane plane{};
	plane.normal = Vector3(-0.2f, 0.9f, -0.3f).Normalize();
	plane.distance = 0.0f;

	Ball ball{};
	ball.position = { 0.8f, 1.2f, 0.3f };
	ball.radius = 0.05f;
	ball.mass = 2.0f;
	ball.color = WHITE;
	ball.acceleration = { 0.0f, -9.8f, 0.0f };

	Sphere sphere{};
	sphere.center = ball.position;
	sphere.radius = ball.radius;

	bool isStrated = false;

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

		cameraMatrix = MakeAffineMatrix(cameraScale, cameraRotate, cameraPosition);
		viewMatrix = cameraMatrix.Inverse();
		projectionMatrix = MakePerspectiveFovMatrix(0.45f, kWindowWidth / kWindowHeight, 0.1f, 100.0f);

		if (isStrated) {
			Vector3 prevPosition = ball.position;

			ball.velocity += ball.acceleration * deltaTime;
			ball.position += ball.velocity * deltaTime;

			Capsule capsule;
			capsule.segment.origin = prevPosition;
			capsule.segment.diff = ball.position - prevPosition;
			capsule.radius = ball.radius;

			if (Collision::IsCollision(capsule, plane)) {
				float e = 0.8f;
				Vector3 reflected = Reflect(ball.velocity, plane.normal);
				Vector3 projectToNormal = reflected.Project(plane.normal);
				Vector3 movingDirection = reflected - projectToNormal;
				ball.velocity = projectToNormal * e + movingDirection;

				float distToPlane = plane.SignedDistance(ball.position);
				float penetration = ball.radius - distToPlane;
				if (penetration > 0.0f) {
					ball.position += plane.normal * penetration;
				}
			}

			sphere.center = ball.position;

			if (ball.position.y <= -2.0f) {
				ball.position = { 0.8f, 3.0f, 0.9f };
				ball.velocity = { 0.0f, 0.0f, 0.0f };
			}
		}

		///
		/// ↑更新処理ここまで
		///

		Draw::DrawGrid((viewMatrix * projectionMatrix), viewportMatrix);
		Draw::DrawPlane(plane, (viewMatrix * projectionMatrix), viewportMatrix, WHITE);
		Draw::DrawSphere(sphere, (viewMatrix * projectionMatrix), viewportMatrix, WHITE);

		///
		/// ↓描画処理ここから
		///

#ifdef _DEBUG
		ImGui::Begin("Window");
		if (ImGui::Button("Start")) {
			isStrated = true;
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