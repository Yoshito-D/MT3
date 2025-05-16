#include "Math.h"
#include <cmath>
#include <Novice.h>
#include <numbers>

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

void Draw::DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
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

void Draw::DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const float kPi = std::numbers::pi_v<float>;
	const uint32_t kSubdivision = 16;
	const float kLonEvery = 2.0f * kPi / static_cast<float>(kSubdivision);
	const float kLatEvery = kPi / static_cast<float>(kSubdivision);

	for (uint32_t latIndex = 0; latIndex <= kSubdivision; ++latIndex) {
		float lat = -kPi * 0.5f + kLatEvery * static_cast<float>(latIndex);
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

void Draw::DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 center = plane.normal * plane.distance;
	Vector3 perpendiculars[4];
	perpendiculars[0] = plane.normal.Perpendicular().Normalize();
	perpendiculars[1] = { -perpendiculars[0].x,-perpendiculars[0].y,-perpendiculars[0].z };
	perpendiculars[2] = plane.normal.Cross(perpendiculars[0]);
	perpendiculars[3] = { -perpendiculars[2].x,-perpendiculars[2].y,-perpendiculars[2].z };

	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = perpendiculars[index] * 2.0f;
		Vector3 point = center + extend;
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}

	Novice::DrawLine(
		static_cast<int>(points[0].x),
		static_cast<int>(points[0].y),
		static_cast<int>(points[2].x),
		static_cast<int>(points[2].y),
		color
	);

	Novice::DrawLine(
		static_cast<int>(points[1].x),
		static_cast<int>(points[1].y),
		static_cast<int>(points[3].x),
		static_cast<int>(points[3].y),
		color
	);

	Novice::DrawLine(
		static_cast<int>(points[2].x),
		static_cast<int>(points[2].y),
		static_cast<int>(points[1].x),
		static_cast<int>(points[1].y),
		color
	);

	Novice::DrawLine(
		static_cast<int>(points[3].x),
		static_cast<int>(points[3].y),
		static_cast<int>(points[0].x),
		static_cast<int>(points[0].y),
		color
	);
}

void Draw::DrawSegment(const Segment& segment, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 start = Transform(Transform(segment.origin, viewProjectionMatrix), viewportMatrix);
	Vector3 end = Transform(Transform(segment.origin + segment.diff, viewProjectionMatrix), viewportMatrix);
	Novice::DrawLine(
		static_cast<int>(start.x),
		static_cast<int>(start.y),
		static_cast<int>(end.x),
		static_cast<int>(end.y),
		color
	);
}

Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 result = segment.origin + (point - segment.origin).Project(segment.diff);
	return result;
}

bool Collision::isCollision(const Sphere& s1, const Sphere& s2) {
	float distance = (s1.center - s2.center).Length();

	if (distance <= s1.radius + s2.radius) {
		return true;
	}

	return false;
}

bool Collision::isCollision(const Sphere& sphere, const Plane& plane) {
	float distance = std::abs(plane.normal.Dot(sphere.center) - plane.distance);

	if (distance <= sphere.radius) {
		return true;
	}

	return false;
}

bool Collision::isCollision(const Segment& segment, const Plane& plane) {
	float dot = plane.normal.Dot(segment.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (plane.distance - segment.origin.Dot(plane.normal)) / dot;

	if (t >= 0.0f && t <= 1.0f) {
		return true;
	}

	return false;
}

bool Collision::isCollision(const Ray& ray, const Plane& plane) {
	float dot = plane.normal.Dot(ray.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (plane.distance - ray.origin.Dot(plane.normal)) / dot;

	if (t >= 0.0f) {
		return true;
	}

	return false;
}

bool Collision::isCollision(const Line& line, const Plane& plane) {
	float dot = plane.normal.Dot(line.diff);

	if (dot == 0.0f) {
		return false;
	}

	return true;
}
