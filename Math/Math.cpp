#define NOMINMAX
#include "Math.h"
#include <cmath>
#include <Novice.h>
#include <numbers>
#include <algorithm>

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

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 result;
	result.x = v1.x + (v2.x - v1.x) * t;
	result.y = v1.y + (v2.y - v1.y) * t;
	result.z = v1.z + (v2.z - v1.z) * t;
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

void Draw::DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 a = Transform(Transform(triangle.vertices[0], viewProjectionMatrix), viewportMatrix);
	Vector3 b = Transform(Transform(triangle.vertices[1], viewProjectionMatrix), viewportMatrix);
	Vector3 c = Transform(Transform(triangle.vertices[2], viewProjectionMatrix), viewportMatrix);

	Novice::DrawLine(
		static_cast<int>(a.x),
		static_cast<int>(a.y),
		static_cast<int>(b.x),
		static_cast<int>(b.y),
		color
	);
	Novice::DrawLine(
		static_cast<int>(b.x),
		static_cast<int>(b.y),
		static_cast<int>(c.x),
		static_cast<int>(c.y),
		color
	);
	Novice::DrawLine(
		static_cast<int>(c.x),
		static_cast<int>(c.y),
		static_cast<int>(a.x),
		static_cast<int>(a.y),
		color
	);
}

void Draw::DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 vertices[8] = {
		{ aabb.min.x, aabb.min.y, aabb.min.z },
		{ aabb.min.x, aabb.min.y, aabb.max.z },
		{ aabb.max.x, aabb.min.y, aabb.max.z },
		{ aabb.max.x, aabb.min.y, aabb.min.z },
		{ aabb.min.x, aabb.max.y, aabb.min.z },
		{ aabb.min.x, aabb.max.y, aabb.max.z },
		{ aabb.max.x, aabb.max.y, aabb.max.z },
		{ aabb.max.x, aabb.max.y, aabb.min.z }
	};

	for (int32_t i = 0; i < 8; ++i) {
		vertices[i] = Transform(Transform(vertices[i], viewProjectionMatrix), viewportMatrix);
	}

	Novice::DrawLine(
		static_cast<int>(vertices[0].x),
		static_cast<int>(vertices[0].y),
		static_cast<int>(vertices[1].x),
		static_cast<int>(vertices[1].y),
		color
	);

	Novice::DrawLine(
		static_cast<int>(vertices[1].x),
		static_cast<int>(vertices[1].y),
		static_cast<int>(vertices[2].x),
		static_cast<int>(vertices[2].y),
		color
	);

	Novice::DrawLine(
		static_cast<int>(vertices[2].x),
		static_cast<int>(vertices[2].y),
		static_cast<int>(vertices[3].x),
		static_cast<int>(vertices[3].y),
		color
	);

	Novice::DrawLine(
		static_cast<int>(vertices[3].x),
		static_cast<int>(vertices[3].y),
		static_cast<int>(vertices[0].x),
		static_cast<int>(vertices[0].y),
		color
	);

	Novice::DrawLine(
		static_cast<int>(vertices[4].x),
		static_cast<int>(vertices[4].y),
		static_cast<int>(vertices[5].x),
		static_cast<int>(vertices[5].y),
		color
	);

	Novice::DrawLine(
		static_cast<int>(vertices[5].x),
		static_cast<int>(vertices[5].y),
		static_cast<int>(vertices[6].x),
		static_cast<int>(vertices[6].y),
		color
	);

	Novice::DrawLine(
		static_cast<int>(vertices[6].x),
		static_cast<int>(vertices[6].y),
		static_cast<int>(vertices[7].x),
		static_cast<int>(vertices[7].y),
		color
	);

	Novice::DrawLine(
		static_cast<int>(vertices[7].x),
		static_cast<int>(vertices[7].y),
		static_cast<int>(vertices[4].x),
		static_cast<int>(vertices[4].y),
		color
	);

	Novice::DrawLine(
		static_cast<int>(vertices[0].x),
		static_cast<int>(vertices[0].y),
		static_cast<int>(vertices[4].x),
		static_cast<int>(vertices[4].y),
		color
	);

	Novice::DrawLine(
		static_cast<int>(vertices[1].x),
		static_cast<int>(vertices[1].y),
		static_cast<int>(vertices[5].x),
		static_cast<int>(vertices[5].y),
		color
	);

	Novice::DrawLine(
		static_cast<int>(vertices[2].x),
		static_cast<int>(vertices[2].y),
		static_cast<int>(vertices[6].x),
		static_cast<int>(vertices[6].y),
		color
	);

	Novice::DrawLine(
		static_cast<int>(vertices[3].x),
		static_cast<int>(vertices[3].y),
		static_cast<int>(vertices[7].x),
		static_cast<int>(vertices[7].y),
		color
	);
}

void Draw::DrawBezier(const Vector3& controlPoint0, const Vector3& contorlPoint1, const Vector3& contorlPoint2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const int32_t kSubdivision = 100;
	for (int32_t i = 0; i < kSubdivision; ++i) {
		float t0 = static_cast<float>(i) / static_cast<float>(kSubdivision);
		float t1 = static_cast<float>(i + 1) / static_cast<float>(kSubdivision);
		Vector3 p0 = controlPoint0 * (1.0f - t0) * (1.0f - t0) + contorlPoint1 * 2.0f * (1.0f - t0) * t0 + contorlPoint2 * t0 * t0;
		Vector3 p1 = controlPoint0 * (1.0f - t1) * (1.0f - t1) + contorlPoint1 * 2.0f * (1.0f - t1) * t1 + contorlPoint2 * t1 * t1;
		p0 = Transform(Transform(p0, viewProjectionMatrix), viewportMatrix);
		p1 = Transform(Transform(p1, viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine(
			static_cast<int>(p0.x),
			static_cast<int>(p0.y),
			static_cast<int>(p1.x),
			static_cast<int>(p1.y),
			color
		);
	}
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

bool Collision::isCollision(const Triangle& triangle, const Segment& segment) {
	Vector3 normal = (triangle.vertices[1] - triangle.vertices[0]).Cross(triangle.vertices[2] - triangle.vertices[0]);
	normal.Normalize();

	float dot = normal.Dot(segment.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (triangle.vertices[0] - segment.origin).Dot(normal) / segment.diff.Dot(normal);
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	Vector3 p = segment.origin + segment.diff * t;

	Vector3 cross01 = (triangle.vertices[0] - triangle.vertices[1]).Cross(triangle.vertices[1] - p);
	Vector3 cross12 = (triangle.vertices[1] - triangle.vertices[2]).Cross(triangle.vertices[2] - p);
	Vector3 cross20 = (triangle.vertices[2] - triangle.vertices[0]).Cross(triangle.vertices[0] - p);

	if (cross01.Dot(normal) >= 0.0f && cross12.Dot(normal) >= 0.0f && cross20.Dot(normal) >= 0.0f) {
		return true;
	}

	return false;
}

bool Collision::isCollision(const Triangle& triangle, const Ray& ray) {
	Vector3 normal = (triangle.vertices[1] - triangle.vertices[0]).Cross(triangle.vertices[2] - triangle.vertices[0]);
	normal.Normalize();

	float dot = normal.Dot(ray.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (triangle.vertices[0] - ray.origin).Dot(normal) / ray.diff.Dot(normal);
	if (t < 0.0f) {
		return false;
	}

	Vector3 p = ray.origin + ray.diff * t;
	Vector3 cross01 = (triangle.vertices[0] - triangle.vertices[1]).Cross(triangle.vertices[1] - p);
	Vector3 cross12 = (triangle.vertices[1] - triangle.vertices[2]).Cross(triangle.vertices[2] - p);
	Vector3 cross20 = (triangle.vertices[2] - triangle.vertices[0]).Cross(triangle.vertices[0] - p);

	if (cross01.Dot(normal) >= 0.0f && cross12.Dot(normal) >= 0.0f && cross20.Dot(normal) >= 0.0f) {
		return true;
	}

	return false;
}

bool Collision::isCollision(const Triangle& triangle, const Line& line) {
	Vector3 normal = (triangle.vertices[1] - triangle.vertices[0]).Cross(triangle.vertices[2] - triangle.vertices[0]);
	normal.Normalize();

	float dot = normal.Dot(line.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (triangle.vertices[0] - line.origin).Dot(normal) / line.diff.Dot(normal);
	Vector3 p = line.origin + line.diff * t;
	Vector3 cross01 = (triangle.vertices[0] - triangle.vertices[1]).Cross(triangle.vertices[1] - p);
	Vector3 cross12 = (triangle.vertices[1] - triangle.vertices[2]).Cross(triangle.vertices[2] - p);
	Vector3 cross20 = (triangle.vertices[2] - triangle.vertices[0]).Cross(triangle.vertices[0] - p);

	if (cross01.Dot(normal) >= 0.0f && cross12.Dot(normal) >= 0.0f && cross20.Dot(normal) >= 0.0f) {
		return true;
	}
	return false;
}

bool Collision::isCollision(const AABB& aabb1, const AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
		(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
		return true;
	}

	return false;
}

bool Collision::isCollision(const AABB& aabb, const Sphere& sphere) {
	Vector3 closestPoint{
		std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),
		std::clamp(sphere.center.y,aabb.min.y,aabb.max.y),
		std::clamp(sphere.center.z,aabb.min.z,aabb.max.z)
	};

	float distance = (closestPoint - sphere.center).Length();

	if (distance <= sphere.radius) {
		return true;
	}

	return false;
}

bool Collision::isCollision(const AABB& aabb, const Segment& segment) {
	float tMin = -INFINITY;
	float tMax = INFINITY;

	// X軸
	if (segment.diff.x != 0.0f) {
		float t1 = (aabb.min.x - segment.origin.x) / segment.diff.x;
		float t2 = (aabb.max.x - segment.origin.x) / segment.diff.x;
		tMin = std::max(tMin, std::min(t1, t2));
		tMax = std::min(tMax, std::max(t1, t2));
	} else if (segment.origin.x < aabb.min.x || segment.origin.x > aabb.max.x) {
		return false;
	}

	// Y軸
	if (segment.diff.y != 0.0f) {
		float t1 = (aabb.min.y - segment.origin.y) / segment.diff.y;
		float t2 = (aabb.max.y - segment.origin.y) / segment.diff.y;
		tMin = std::max(tMin, std::min(t1, t2));
		tMax = std::min(tMax, std::max(t1, t2));
	} else if (segment.origin.y < aabb.min.y || segment.origin.y > aabb.max.y) {
		return false;
	}

	// Z軸
	if (segment.diff.z != 0.0f) {
		float t1 = (aabb.min.z - segment.origin.z) / segment.diff.z;
		float t2 = (aabb.max.z - segment.origin.z) / segment.diff.z;
		tMin = std::max(tMin, std::min(t1, t2));
		tMax = std::min(tMax, std::max(t1, t2));
	} else if (segment.origin.z < aabb.min.z || segment.origin.z > aabb.max.z) {
		return false;
	}

	if (tMin > tMax) {
		return false;
	}

	return tMax >= 0.0f && tMin <= 1.0f;
}

bool Collision::isCollision(const AABB& aabb, const Ray& ray) {
	float tMin = -INFINITY;
	float tMax = INFINITY;
	// X軸

	if (ray.diff.x != 0.0f) {
		float t1 = (aabb.min.x - ray.origin.x) / ray.diff.x;
		float t2 = (aabb.max.x - ray.origin.x) / ray.diff.x;
		tMin = std::max(tMin, std::min(t1, t2));
		tMax = std::min(tMax, std::max(t1, t2));
	} else if (ray.origin.x < aabb.min.x || ray.origin.x > aabb.max.x) {
		return false;
	}

	// Y軸
	if (ray.diff.y != 0.0f) {
		float t1 = (aabb.min.y - ray.origin.y) / ray.diff.y;
		float t2 = (aabb.max.y - ray.origin.y) / ray.diff.y;
		tMin = std::max(tMin, std::min(t1, t2));
		tMax = std::min(tMax, std::max(t1, t2));
	} else if (ray.origin.y < aabb.min.y || ray.origin.y > aabb.max.y) {
		return false;
	}

	// Z軸
	if (ray.diff.z != 0.0f) {
		float t1 = (aabb.min.z - ray.origin.z) / ray.diff.z;
		float t2 = (aabb.max.z - ray.origin.z) / ray.diff.z;
		tMin = std::max(tMin, std::min(t1, t2));
		tMax = std::min(tMax, std::max(t1, t2));
	} else if (ray.origin.z < aabb.min.z || ray.origin.z > aabb.max.z) {
		return false;
	}

	if (tMin > tMax) {
		return false;
	}

	return tMax >= 0.0f;
}

bool Collision::isCollision(const AABB& aabb, const Line& line) {
	float tMin = -INFINITY;
	float tMax = INFINITY;

	// X軸
	if (line.diff.x != 0.0f) {
		float t1 = (aabb.min.x - line.origin.x) / line.diff.x;
		float t2 = (aabb.max.x - line.origin.x) / line.diff.x;
		tMin = std::max(tMin, std::min(t1, t2));
		tMax = std::min(tMax, std::max(t1, t2));
	} else if (line.origin.x < aabb.min.x || line.origin.x > aabb.max.x) {
		return false;
	}

	// Y軸
	if (line.diff.y != 0.0f) {
		float t1 = (aabb.min.y - line.origin.y) / line.diff.y;
		float t2 = (aabb.max.y - line.origin.y) / line.diff.y;
		tMin = std::max(tMin, std::min(t1, t2));
		tMax = std::min(tMax, std::max(t1, t2));
	} else if (line.origin.y < aabb.min.y || line.origin.y > aabb.max.y) {
		return false;
	}

	// Z軸
	if (line.diff.z != 0.0f) {
		float t1 = (aabb.min.z - line.origin.z) / line.diff.z;
		float t2 = (aabb.max.z - line.origin.z) / line.diff.z;
		tMin = std::max(tMin, std::min(t1, t2));
		tMax = std::min(tMax, std::max(t1, t2));
	} else if (line.origin.z < aabb.min.z || line.origin.z > aabb.max.z) {
		return false;
	}

	if (tMin > tMax) {
		return false;
	}

	return true;
}

