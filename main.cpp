#include <Novice.h>
#include <cmath>

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

	float operator*(const Vector3& vector) const {
		return x * vector.x + y * vector.y + z * vector.z;
	}

	float Dot(const Vector3& vector) const {
		return x * vector.x + y * vector.y + z * vector.z;
	}

	float Length() const {
		return sqrtf(x * x + y * y + z * z);
	}

	Vector3 Normalize() const {
		float length = Length();
		return { x / length, y / length, z / length };
	}
};

static const int kColumnWidth = 60;
static const int kRowHeight = 20;

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 v1{ 1.0f,3.0f,-5.0f };
	Vector3 v2{ 4.0f,-1.0f,2.0f };
	float k = 4.0f;

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

		VectorScreenPrintf(0, 0, v1 + v2, " : Add");
		VectorScreenPrintf(0, kRowHeight, v1 - v2, " : Subtract");
		VectorScreenPrintf(0, kRowHeight * 2, v1 * k, " : Multiply");
		Novice::ScreenPrintf(0, kRowHeight * 3, "%.02f : Dot", v1.Dot(v2));
		Novice::ScreenPrintf(0, kRowHeight * 4, "%.02f : Length", v1.Length());
		VectorScreenPrintf(0, kRowHeight * 5, v2.Normalize(), " : Normalize");

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
