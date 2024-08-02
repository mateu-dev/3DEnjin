#pragma once

#include "resource.h"

template<typename T>
std::string to_string_impl(const T& msg) {
	return std::to_string(msg);
}

std::string to_string_impl(const std::string& msg) {
	return msg;
}

std::string to_string_impl(const char* msg) {
	return std::string(msg);
}

std::string to_string_impl(const Vector2f& msg) {
	return "Vector2f(" + to_string_impl(msg.x) + ", " + to_string_impl(msg.y) + ")";
}
std::string to_string_impl(const Vector2i& msg) {
	return "Vector2i(" + to_string_impl(msg.x) + ", " + to_string_impl(msg.y) + ")";
}
std::string to_string_impl(const Vector3f& msg) {
	return "Vector3f(" + to_string_impl(msg.x) + ", " + to_string_impl(msg.y) + ", " + to_string_impl(msg.z) + ")";
}

template<typename T>
void Print(const T& msg) {
	std::string message = to_string_impl(msg) + "\n";
	OutputDebugStringA(message.c_str());
}

template<typename T, typename... Args>
void Print(const T& msg, const Args&... args) {
	std::ostringstream oss;
	oss << to_string_impl(msg);
	int dummy[] = { 0, ((void)(oss << " " << to_string_impl(args)), 0)... };
	(void)dummy; // Avoid unused variable warning
	oss << "\n";
	OutputDebugStringA(oss.str().c_str());
}

// ---------
#define RAD 0.0174533 
class Scene;
struct Trig;
class Transform;
class Cube;

struct Trig {
public:
	Vector3f* points;
	long color;

	Trig(Vector3f, Vector3f, Vector3f, long);
	~Trig();
	void Render(Scene*, Canvas&);
	float DepthTest(Scene*);
};


class Transform {
public:
	Vector3f pos, scale, pivot, rotation;
	static Vector3f RotateAround(Vector3f, Vector3f, Vector3f);
	virtual void GetTransformedGeometry(std::vector<std::unique_ptr<Trig>>& vec) {};
protected:
	Scene* parent = nullptr;
	Transform(Vector3f, Vector3f, Vector3f, Vector3f);
};

class Scene {
	void SortByDepth(std::vector<std::unique_ptr<Trig>>*);
public:
	std::vector<Transform*> objects;

	Vector3f cameraPosition = { 0,0,0 };
	Vector3f cameraRotation = { 0,0,0 };

	void RenderScene(Canvas&);
	Vector2f CalculatePointPosition(Window*, Vector3f);
};

class Cube :Transform {
	std::vector<Vector3f> points = {
		//Front
		{0,0,0},{1,0,0},{1,1,0},
		{0,0,0},{0,1,0},{1,1,0},
		//Back
		{0,0,1},{1,0,1},{1,1,1},
		{0,0,1},{0,1,1},{1,1,1},
		//Top
		{0,0,0},{1,0,0},{1,0,1},
		{0,0,0},{0,0,1},{1,0,1},
		//Bottom
		{0,1,0},{1,1,0},{1,1,1},
		{0,1,0},{0,1,1},{1,1,1},
		//Left
		{0,0,0},{0,0,1},{0,1,1},
		{0,0,0},{0,1,0},{0,1,1},
		//Right
		{1,0,0},{1,0,1},{1,1,1},
		{1,0,0},{1,1,0},{1,1,1},
	};
public:
	long color = 0x0;
	Cube(Scene*, Vector3f, Vector3f, Vector3f);
	Cube(Scene*, Vector3f, Vector3f, Vector3f, long);

	void GetTransformedGeometry(std::vector<std::unique_ptr<Trig>>& vec);
};

//class Camera {
//
//};

