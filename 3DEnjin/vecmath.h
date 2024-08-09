#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
struct Vector2i;

struct Vector2f {
	float x, y;

	Vector2f() :x(0), y(0) {}
	Vector2f(float x, float y) : x(x), y(y) {}
	Vector2f(const Vector2i& vec);
};

struct Vector2i {
	int x, y;

	Vector2i() :x(0), y(0) {}
	Vector2i(int x, int y) : x(x), y(y) {}
	Vector2i(const Vector2f& vec);
};

struct Vector3f {
	float x, y, z;
	Vector3f() :x(0), y(0), z(0) {}
	Vector3f(float x, float y, float z) : x(x), y(y), z(z) {}

	Vector3f Cross(const Vector3f& other) const {
		return Vector3f(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	float Dot(const Vector3f& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	Vector3f Normalize() const {
		float length = std::sqrt(x * x + y * y + z * z);
		return Vector3f(x / length, y / length, z / length);
	}
};

struct Quaternion {
	float w, x, y, z;

	Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}
	Quaternion(Vector3f v) : w(0), x(v.x), y(v.y), z(v.z) {}

	Quaternion operator*(const Quaternion& q) const {
		return Quaternion(
			w * q.w - x * q.x - y * q.y - z * q.z,
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y - x * q.z + y * q.w + z * q.x,
			w * q.z + x * q.y - y * q.x + z * q.w
		);
	}

	Quaternion conjugate() const {
		return Quaternion(w, -x, -y, -z);
	}

	Quaternion rotateVector(const Quaternion& v) const {
		Quaternion q_conjugate = this->conjugate();
		Quaternion rotated_v = (*this) * v * q_conjugate;
		return rotated_v;
	}
};

inline Quaternion createRotationQuaternion(float angle, float ux, float uy, float uz) {
	float halfAngle = angle / 2;
	float sinHalfAngle = sin(halfAngle);

	return Quaternion(
		cos(halfAngle),
		ux * sinHalfAngle,
		uy * sinHalfAngle,
		uz * sinHalfAngle
	);
}

// Addition Operators
inline static Vector2f operator+(const Vector2f& a, const Vector2f& b) {
	return { a.x + b.x, a.y + b.y };
}

inline static Vector2f operator+(const Vector2i& a, const Vector2f& b) {
	return { a.x + b.x, a.y + b.y };
}

inline static Vector2f operator+(const Vector2f& a, const Vector2i& b) {
	return { a.x + b.x, a.y + b.y };
}

inline static Vector2i operator+(const Vector2i& a, const Vector2i& b) {
	return { a.x + b.x, a.y + b.y };
}

inline static Vector3f operator+(const Vector3f& a, const Vector2f& b) {
	return { a.x + b.x, a.y + b.y, a.z };
}

inline static Vector3f operator+(const Vector3f& a, const Vector2i& b) {
	return { a.x + b.x, a.y + b.y, a.z };
}

inline static Vector3f operator+(const Vector3f& a, const Vector3f& b) {
	return { a.x + b.x, a.y + b.y, a.z + b.z };
}

// Subtraction Operators
inline static Vector2f operator-(const Vector2f& a, const Vector2f& b) {
	return { a.x - b.x, a.y - b.y };
}

inline static Vector2f operator-(const Vector2i& a, const Vector2f& b) {
	return { a.x - b.x, a.y - b.y };
}

inline static Vector2f operator-(const Vector2f& a, const Vector2i& b) {
	return { a.x - b.x, a.y - b.y };
}

inline static Vector2i operator-(const Vector2i& a, const Vector2i& b) {
	return { a.x - b.x, a.y - b.y };
}

inline static Vector3f operator-(const Vector3f& a, const Vector3f& b) {
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}

// Multiplication Operators
inline static Vector2f operator*(const Vector2f& a, const Vector2f& b) {
	return { a.x * b.x, a.y * b.y };
}

inline static Vector2f operator*(const Vector2i& a, const Vector2f& b) {
	return { a.x * b.x, a.y * b.y };
}

inline static Vector2f operator*(const Vector2f& a, const Vector2i& b) {
	return { a.x * b.x, a.y * b.y };
}

inline static Vector2i operator*(const Vector2i& a, const Vector2i& b) {
	return { a.x * b.x, a.y * b.y };
}

inline static Vector2i operator*(const int& a, const Vector2i b) {
	return { a * b.x, a * b.y };
}

inline static Vector2i operator*(const Vector2i b, const int& a) {
	return { a * b.x, a * b.y };
}

inline static Vector2f operator*(const float& a, const Vector2f b) {
	return { a * b.x, a * b.y };
}

inline static Vector2f operator*(const Vector2f b, const float& a) {
	return { a * b.x, a * b.y };
}

inline static Vector3f operator*(const Vector3f& a, const Vector2f& b) {
	return { a.x * b.x, a.y * b.y, a.z };
}

inline static Vector3f operator*(const Vector3f& a, const Vector2i& b) {
	return { a.x * b.x, a.y * b.y, a.z };
}

inline static Vector3f operator*(const Vector3f& a, const Vector3f& b) {
	return { a.x * b.x, a.y * b.y, a.z * b.z };
}

inline static Vector3f operator*(const Vector3f b, const float& a) {
	return { a * b.x, a * b.y, a * b.z };
}

// Division Operators
inline static Vector2i operator/(const Vector2i b, const int& a) {
	return { b.x / a, b.y / a };
}

inline static Vector2f operator/(const Vector2f b, const float& a) {
	return { b.x / a, b.y / a };
}

inline static Vector2f operator/(const Vector2f b, const Vector2f a) {
	return { b.x / a.x, b.y / a.y };
}

inline static Vector3f operator/(const Vector3f b, const float& a) {
	return { b.x / a, b.y / a, b.z / a };
}

// Compound Assignment Operators
inline static Vector2f& operator+=(Vector2f& a, const Vector2f& b) {
	a.x += b.x;
	a.y += b.y;
	return a;
}

inline static Vector2i& operator+=(Vector2i& a, const Vector2i& b) {
	a.x += b.x;
	a.y += b.y;
	return a;
}

inline static Vector2f& operator-=(Vector2f& a, const Vector2f& b) {
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

inline static Vector2i& operator-=(Vector2i& a, const Vector2i& b) {
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

inline static Vector2f& operator*=(Vector2f& a, const Vector2f& b) {
	a.x *= b.x;
	a.y *= b.y;
	return a;
}

inline static Vector2i& operator*=(Vector2i& a, const Vector2i& b) {
	a.x *= b.x;
	a.y *= b.y;
	return a;
}

inline static Vector2f& operator/=(Vector2f& a, const Vector2f& b) {
	a.x /= b.x;
	a.y /= b.y;
	return a;
}

inline static Vector2i& operator/=(Vector2i& a, const int& b) {
	a.x /= b;
	a.y /= b;
	return a;
}

inline static Vector2f& operator/=(Vector2f& a, const float& b) {
	a.x /= b;
	a.y /= b;
	return a;
}

// Equality Operators
inline static bool operator==(const Vector2f& a, const Vector2f& b) {
	return a.x == b.x && a.y == b.y;
}

inline static bool operator==(const Vector2i& a, const Vector2i& b) {
	return a.x == b.x && a.y == b.y;
}

inline static bool operator==(const Vector3f& a, const Vector3f& b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

#endif // VECTOR_H