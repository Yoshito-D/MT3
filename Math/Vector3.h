#pragma once
#include <cmath>

struct Vector3 {
	float x, y, z;

	Vector3 operator-() const {
		return { -x, -y, -z };
	}

	Vector3 operator+(const Vector3& vector) const {
		return { x + vector.x, y + vector.y, z + vector.z };
	}

	Vector3 operator-(const Vector3& vector) const {
		return { x - vector.x, y - vector.y, z - vector.z };
	}

	Vector3 operator*(const Vector3& vector) const {
		return { x * vector.x, y * vector.y, z * vector.z };
	}

	Vector3 operator*(const float& scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	Vector3 operator/(const float& scalar) const {
		return { x / scalar, y / scalar, z / scalar };
	}

	Vector3& operator+=(const Vector3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vector3& operator-=(const Vector3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
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

	Vector3 Perpendicular() const {
		if (x != 0.0f || y != 0.0f) {
			return { -y,x,0.0f };
		}
		return { 0.0f,-z,y };
	}
};