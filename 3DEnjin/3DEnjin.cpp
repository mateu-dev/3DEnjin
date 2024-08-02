#include "framework.h"
#include "3DEnjin.h"

Window* win;

Trig::Trig(Vector3f v1, Vector3f v2, Vector3f v3, long c) {
	this->points = new Vector3f[3]{ v1,v2,v3 };
	this->color = c;
}
Trig::~Trig()
{
	delete points;
}
void Trig::Render(Scene* s, Canvas& c)
{
	Vector2f v1 = s->CalculatePointPosition(win, points[0]);
	Vector2f v2 = s->CalculatePointPosition(win, points[1]);
	Vector2f v3 = s->CalculatePointPosition(win, points[2]);

	if (v1 == Vector2f(-1, -1) || v2 == Vector2f(-1, -1) || v3 == Vector2f(-1, -1)) return;

	c.DrawShape(color, v1, v2, v3, v1);

	c.DrawLine(v1, v2);
	c.DrawLine(v2, v3);
	c.DrawLine(v3, v1);
}
float Trig::DepthTest(Scene* s)
{
	float avgZ = 0;
	for (int i = 0; i < 3; i++) {
		Vector3f point = points[i];
		point = Transform::RotateAround(point, s->cameraPosition, s->cameraRotation * -1);
		point = point - s->cameraPosition;
		avgZ += point.z;
	}

	return avgZ / 3;
}

float nearPlane = .1;
float fov = 90;

Vector2f Scene::CalculatePointPosition(Window* w, Vector3f point) {
	point = Transform::RotateAround(point, cameraPosition, cameraRotation * -1);
	point = point - cameraPosition;

	Vector2f aspectRatio = { 16,9 };
	float z = aspectRatio.x / sinf(fov / 2 * RAD);
	Vector2f nearRes = aspectRatio * nearPlane / z;
	Vector2f pointPlaneResolution = nearRes * (nearPlane + point.z) / nearPlane;

	// Don't Render Behind Camera
	if (pointPlaneResolution.x < 0 || pointPlaneResolution.y < 0) return { -1,-1 };

	Vector2f planeWorldCoordinates = pointPlaneResolution / 2;
	point = point + planeWorldCoordinates;

	Vector2f iPoint = { point.x / pointPlaneResolution.x, point.y / pointPlaneResolution.y };
	return iPoint * w->getSize();
}
void Scene::SortByDepth(std::vector<std::unique_ptr<Trig>>* v)
{
	std::sort(v->begin(), v->end(), [this](const std::unique_ptr<Trig>& a, const std::unique_ptr<Trig>& b) {
		return a->DepthTest(this) > b->DepthTest(this);
		});
}
void Scene::RenderScene(Canvas& c) {
	std::vector<std::unique_ptr<Trig>> trigs = {};
	for (Transform* t : objects) {
		t->GetTransformedGeometry(trigs);
	}

	SortByDepth(&trigs);
	for (const auto& v : trigs)
		v->Render(this, c);
}

Transform::Transform(Vector3f pos, Vector3f scale, Vector3f rot, Vector3f pivot) :pos(pos), scale(scale), rotation(rot), pivot(pivot) {}
Vector3f Transform::RotateAround(Vector3f point, Vector3f pivot, Vector3f rotation) {

	// Rotate Y
	float dist = sqrtf(powf(point.y - pivot.y, 2) + powf(point.z - pivot.z, 2));
	float defaultAngle = atan2f(point.y - pivot.y, point.z - pivot.z);
	Vector2f rotVector = { sinf(defaultAngle + rotation.y * RAD) * dist ,cosf(defaultAngle + rotation.y * RAD) * dist };
	point = { point.x,pivot.y + rotVector.y,pivot.z + rotVector.x };
	// Rotate X
	dist = sqrtf(powf(point.x - pivot.x, 2) + powf(point.z - pivot.z, 2));
	defaultAngle = atan2f(point.x - pivot.x, point.z - pivot.z);
	rotVector = { sinf(defaultAngle + rotation.x * RAD) * dist ,cosf(defaultAngle + rotation.x * RAD) * dist };
	point = { pivot.x + rotVector.x,point.y,pivot.z + rotVector.y };
	// Rotate Z
	dist = sqrtf(powf(point.x - pivot.x, 2) + powf(point.y - pivot.y, 2));
	defaultAngle = atan2f(point.x - pivot.x, point.y - pivot.y);
	rotVector = { sinf(defaultAngle + rotation.z * RAD) * dist ,cosf(defaultAngle + rotation.z * RAD) * dist };
	point = { pivot.x + rotVector.y,pivot.y + rotVector.x,point.z };

	return point;
}

Cube::Cube(Scene* scene, Vector3f pos, Vector3f scale, Vector3f rot) :Transform(pos, scale, rot, { .5,.5,.5 }) {
	scene->objects.push_back(this);
	this->parent = scene;
};
Cube::Cube(Scene* scene, Vector3f pos, Vector3f scale, Vector3f rot, long color) :Transform(pos, scale, rot, { .5,.5,.5 }), color(color) {
	scene->objects.push_back(this);
	this->parent = scene;
};
void Cube::GetTransformedGeometry(std::vector<std::unique_ptr<Trig>>& vec)
{
	for (int i = 0; i < points.size() / 3; i++) {
		Vector3f v1 = RotateAround(points[i * 3] * scale, pivot, rotation) + (pos - pivot);
		Vector3f v2 = RotateAround(points[i * 3 + 1] * scale, pivot, rotation) + (pos - pivot);
		Vector3f v3 = RotateAround(points[i * 3 + 2] * scale, pivot, rotation) + (pos - pivot);
		std::unique_ptr<Trig> ptr = std::make_unique<Trig>(v1, v2, v3, color);
		vec.push_back(std::move(ptr));
	}
}

bool lockMouse = true;

Scene mainScene;
Cube c(&mainScene, { 0,5,0 }, { 1,1,1 }, { 0,0,0 }, 0x2356FF);
Cube c2(&mainScene, { 5,50,0 }, { 100,100,1 }, { 0,0,0 }, 0x23A456);
Cube c3(&mainScene, { 0,5,5 }, { 2,3,4 }, { 45,45,0 }, 0xFFFF00);
static void Update(HDC& hdc) {
	Canvas canvas(hdc);

	c3.

	//Rotation
	if (win->inputState.keys[0x51]) lockMouse = !lockMouse;
	if (GetForegroundWindow() == win->hWnd_) {
		ShowCursor(false);
		Vector2i winCenter = win->getPos() + win->getSize() / 2;
		Vector2i mousePosition = win->getPos() + Vector2i(win->inputState.mouseX, win->inputState.mouseY);

		Vector2i mouseDelta = mousePosition - winCenter + Vector2i(8, 31);

		mainScene.cameraRotation = mainScene.cameraRotation - Vector3f(-mouseDelta.y, mouseDelta.x, 0) * 0.05;

		SetCursorPos(winCenter.x, winCenter.y);
	}

	mainScene.cameraRotation.x = clamp<float>(mainScene.cameraRotation.x, -90, 90);

	// Movement
	float speed = .7;
	Vector3f forward = { 0,cosf(-mainScene.cameraRotation.y * RAD) * speed,sinf(-mainScene.cameraRotation.y * RAD) * speed };
	Vector3f left = { 0,cosf((-mainScene.cameraRotation.y + 90) * RAD) * speed ,sinf((-mainScene.cameraRotation.y + 90) * RAD) * speed };
	if (win->inputState.keys[0x57])
		mainScene.cameraPosition = mainScene.cameraPosition + forward;
	if (win->inputState.keys[0x53])
		mainScene.cameraPosition = mainScene.cameraPosition - forward;
	if (win->inputState.keys[0x41])
		mainScene.cameraPosition = mainScene.cameraPosition - left;
	if (win->inputState.keys[0x44])
		mainScene.cameraPosition = mainScene.cameraPosition + left;
	if (win->inputState.keys[0x51])
		mainScene.cameraPosition.x++;
	if (win->inputState.keys[0x45])
		mainScene.cameraPosition.x--;

	mainScene.RenderScene(canvas);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	win = new Window(hInstance, nCmdShow);

	Window::RegisterUpdateFunction(30, Update);
	win->CreateWindowAndRun();

	return 0;
}


