#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"

struct Sphere {
	Vector3 center; //!< 中心点
	float radius; //!< 半径
};

struct Line {
	Vector3 origin; //!< 始点
	Vector3 diff; //!< 終点への差分ベクトル
};

struct Plane {
	Vector3 normal; //!< 法線
	float distance; //!< 距離

	float SignedDistance(const Vector3& point) const {
		return normal.Dot(point) - distance;
	}
};

struct Ray {
	Vector3 origin; //!< 始点
	Vector3 diff; //!< 終点への差分ベクトル
};

struct Segment {
	Vector3 origin; //!< 始点
	Vector3 diff; //!< 終点への差分ベクトル
};

struct Triangle {
	Vector3 vertices[3]; //!< 頂点
};

struct AABB {
	Vector3 min; //!< 最小点
	Vector3 max; //!< 最大点
};

struct Spring {
	Vector3 anchor; //!< アンカー
	float naturalLength; //!< 自然長
	float stiffness; //!< 剛性
	float dampingCoefficient; //!< 減衰係数
};

struct Ball {
	Vector3 position; //!< 位置
	Vector3 velocity; //!< 速度
	Vector3 acceleration; //!< 加速度
	float mass; //!< 質量
	float radius; //!< 半径
	unsigned int color; //!< 色
};

struct Pendulum {
	Vector3 anchor; //!< アンカー
	float length; //!< 長さ	
	float angle; //!< 角度
	float angularVelocity; //!< 角速度
	float angularAcceleration; //!< 角加速度
};

struct ConicalPendulum {
	Vector3 anchor; //!< アンカー
	float length; //!< 長さ	
	float halfApexAngle; //!< 半頂角
	float angle; //!< 角度
	float angularVelocity; //!< 角速度
};

struct Capsule {
	Segment segment; //!< 線分
	float radius; //!< 半径
};

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

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

Vector3 Reflect(const Vector3& input, const Vector3& normal);

namespace Draw {
	void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
	void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
	void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
	void DrawSegment(const Segment& segment, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
	void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
	void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
	void DrawBezier(const Vector3& controlPoint0, const Vector3& contorlPoint1, const Vector3& contorlPoint2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
}

namespace Collision {
	bool isCollision(const Sphere& s1, const Sphere& s2);
	bool isCollision(const Sphere& sphere, const Plane& plane);
	bool isCollision(const Segment& segment, const Plane& plane);
	bool isCollision(const Segment& segment, const Plane& plane);
	bool isCollision(const Ray& ray, const Plane& plane);
	bool isCollision(const Line& line, const Plane& plane);
	bool isCollision(const Triangle& triangle, const Segment& segment);
	bool isCollision(const Triangle& triangle, const Ray& ray);
	bool isCollision(const Triangle& triangle, const Line& line);
	bool isCollision(const AABB& aabb1, const AABB& aabb2);
	bool isCollision(const AABB& aabb, const Sphere& sphere);
	bool isCollision(const AABB& aabb, const Segment& segment);
	bool isCollision(const AABB& aabb, const Ray& ray);
	bool isCollision(const AABB& aabb, const Line& line);
	bool IsCollision(const Capsule& capsule, const Plane& plane);
}