#ifndef VECTOR_H
#define VECTOR_H

struct Vector2f {
	float x, y;

	Vector2f(float x, float y);
};

struct Vector2i {
	int x, y;
	Vector2i(int x, int y);
};

struct Vector3f {
	float x, y, z;
	Vector3f(float x, float y, float z);
};

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
	return { a.x * b.x, a.y + b.y };
}

inline static Vector2i operator*(const int& a, const Vector2i b) {
	return { a * b.x,a * b.y };
}
inline static Vector2i operator*(const Vector2i b, const int& a) {
	return { a * b.x,a * b.y };
}

inline static Vector2f operator*(const float& a, const Vector2f b) {
	return { a * b.x,a * b.y };
}
inline static Vector2f operator*(const Vector2f b, const float& a) {
	return { a * b.x,a * b.y };
}

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

inline static bool operator==(const Vector2f& a, const Vector2f& b) {
	return a.x == b.x && a.y == b.y;
}inline static bool operator==(const Vector2i& a, const Vector2i& b) {
	return a.x == b.x && a.y == b.y;
}

inline static Vector2i operator/(const Vector2i b, const int& a) {
	return { b.x / a,b.y / a };
}
inline static Vector2f operator/(const Vector2f b, const float& a) {
	return { b.x / a,b.y / a };
}

inline static Vector2f operator/(const Vector2f b, const Vector2f a) {
	return { b.x / a.x,b.y / a.y };
}

inline static Vector3f operator+(const Vector3f& a, const Vector2f& b) {
	return { a.x + b.x, a.y + b.y,a.z };
}
inline static Vector3f operator+(const Vector3f& a, const Vector2i& b) {
	return { a.x + b.x, a.y + b.y,a.z };
}

inline static Vector3f operator*(const Vector3f& a, const Vector2f& b) {
	return { a.x * b.x, a.y * b.y,a.z };
}
inline static Vector3f operator*(const Vector3f& a, const Vector2i& b) {
	return { a.x * b.x, a.y * b.y,a.z };
}

inline static Vector3f operator+(const Vector3f& a, const Vector3f& b) {
	return { a.x + b.x, a.y + b.y,a.z + b.z };
}
inline static Vector3f operator-(const Vector3f& a, const Vector3f& b) {
	return { a.x - b.x, a.y - b.y,a.z - b.z };
}

inline static Vector3f operator*(const Vector3f& a, const Vector3f& b) {
	return { a.x * b.x, a.y * b.y,a.z * b.z };
}
inline static Vector3f operator*(const Vector3f b, const float& a) {
	return { a * b.x,a * b.y,a * b.z };
}
#endif // VECTOR_H