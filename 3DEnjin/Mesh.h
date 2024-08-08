#pragma once
#include "framework.h"
#define RAD 0.0174533 

struct Trig;
struct InputState;
class Transform;
class Cube;
class Canvas;
class Light;

class Scene {
	void SortByDepth(std::vector<std::unique_ptr<Trig>>&);
public:
	std::vector<Transform*> objects;
	std::vector<Light*> lights;


	Vector3f cameraPosition = { 0,0,0 };
	Vector3f cameraRotation = { 0,0,0 };

	void RenderScene(Canvas&);
	Vector2f CalculatePointPosition(Vector2i, Vector3f);
	void Freecam(InputState&, float, float);
	void DrawSkyBox(Canvas&, long, long) const;
};

struct Trig {
private:
	std::vector<Vector3f> cameraSpaceCache;
public:
	Vector3f* points;
	long color;

	Trig(Vector3f, Vector3f, Vector3f, long);
	~Trig();

	std::vector<Vector3f> ToCameraSpace(Scene*);
	void Render(Scene*, Canvas&);
	float DepthTest(Scene*);
	Vector3f CalculateNormal(Scene* s, bool inCameraSpace = true);
	bool IsBackFace(Scene*);
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

class Cube : public Transform {
	static const std::vector<Vector3f> points;
public:
	long color = 0x0;
	Cube(Scene*, Vector3f, Vector3f, Vector3f);
	Cube(Scene*, Vector3f, Vector3f, Vector3f, long);

	void GetTransformedGeometry(std::vector<std::unique_ptr<Trig>>& vec);
};

class Mesh : public Transform
{
private:
	std::vector<Vector3f> vertices;
	std::vector<std::vector<int>> faces;

	void FileReader(std::string);
	bool startsWith(std::string string, std::string start) {
		for (int i = 0; i < start.size(); i++)
			if (string[i] != start[i]) return false;
		return true;
	}
	std::vector<std::string> splitString(const std::string& str, char delimiter = ' ') {
		std::vector<std::string> result;
		std::istringstream iss(str);
		std::string token;
		while (std::getline(iss, token, delimiter)) {
			result.push_back(token);
		}
		return result;
	}
public:
	Mesh(std::string, Scene*, Vector3f, Vector3f, Vector3f);
	void GetTransformedGeometry(std::vector<std::unique_ptr<Trig>>& vec);
};

class Light : public Transform {
public:
	long color = RGB(255, 252, 232);
	float intensity = .1;
	Light(Scene*, Vector3f, Vector3f, Vector3f);
	virtual void Render(Canvas&);
};

class Sun :public Light {
	Vector2f lastPos = { -1,-1 };
	Vector3f initialPosition = { 0,0,0 };
public:
	float time;

	Sun(Scene* scene, float time);
	Sun(Scene* scene, float time, long color, float intensity);
	Vector3f CalculatePositionAtTime(float time);
	void Render(Canvas& c);
};

