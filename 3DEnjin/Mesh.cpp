#include "Mesh.h"

float nearPlane = .1;
float fov = 180;

Vector2f Scene::CalculatePointPosition(Vector2i canvasSize, Vector3f point) {
	point = Transform::RotateAround(point, cameraPosition, cameraRotation * -1);
	point = point - cameraPosition;

	Vector2f aspectRatio = { 16,9 };
	float z = aspectRatio.x / sinf(fov / 2 * RAD);
	Vector2f nearRes = aspectRatio * nearPlane / z;
	float pointZ = nearPlane + point.z;
	if (pointZ < 0) return { -1,-1 };
	Vector2f pointPlaneResolution = nearRes * pointZ / nearPlane;


	Vector2f planeWorldCoordinates = pointPlaneResolution / 2.0f;
	point = point + planeWorldCoordinates;

	Vector2f iPoint = { point.x / pointPlaneResolution.x, point.y / pointPlaneResolution.y };
	return iPoint * canvasSize;
}
void Scene::Freecam(InputState& input, float speed, float deltaTime = 1)
{
	//Rotation
	if (GetForegroundWindow() == input.parent->hWnd_) {
		ShowCursor(false);
		Vector2i winCenter = input.parent->getPos() + input.parent->getSize() / 2;
		Vector2i mousePosition = input.parent->getPos() + Vector2i(input.mouseX, input.mouseY);

		Vector2i mouseDelta = mousePosition - winCenter + Vector2i(8, 31);

		cameraRotation = cameraRotation - Vector3f(mouseDelta.y, -mouseDelta.x, 0) * 0.05;

		SetCursorPos(winCenter.x, winCenter.y);
	}

	cameraRotation.x = clamp<float>(cameraRotation.x, -90, 90);

	// Movement
	Vector3f forward = Vector3f(sinf(cameraRotation.y * RAD), 0, cosf(cameraRotation.y * RAD)) * speed * deltaTime;
	Vector3f left = Vector3f(sinf((cameraRotation.y + 90) * RAD), 0, cosf((cameraRotation.y + 90) * RAD)) * speed * deltaTime;
	if (input.keys[0x57])
		cameraPosition = cameraPosition + forward;
	if (input.keys[0x53])
		cameraPosition = cameraPosition - forward;
	if (input.keys[0x41])
		cameraPosition = cameraPosition - left;
	if (input.keys[0x44])
		cameraPosition = cameraPosition + left;
	if (input.keys[0x51])
		cameraPosition.y += speed * deltaTime;
	if (input.keys[0x45])
		cameraPosition.y -= speed * deltaTime;
}
void Scene::DrawSkyBox(Canvas& c, long topColor, long bottomColor) const
{
	Vector2f res = c.size;

	Vector2f aspectRatio = { 16,9 };
	float z = aspectRatio.x / sinf(fov / 2 * RAD);

	float yAngle = atan2f(aspectRatio.y / 2, z) / RAD;
	float topCameraY = cameraRotation.x + yAngle;
	float bottomCameraY = cameraRotation.x - yAngle;

	float h = -topCameraY / (bottomCameraY - topCameraY);
	float horizonY = h * res.y;

	// Top part of the sky
	c.DrawShape(topColor,
		{ 0, 0 },
		{ res.x, 0 },
		{ res.x, horizonY },
		{ 0, horizonY });

	// Bottom part of the sky
	c.DrawShape(bottomColor,
		{ 0, horizonY },
		{ res.x, horizonY },
		{ res },
		{ 0, res.y });

	float gradientSize = res.y;
	c.DrawShape(topColor, bottomColor, { 0, horizonY - gradientSize }, { res.x, horizonY - gradientSize }, { res.x, horizonY + gradientSize }, { 0, horizonY + gradientSize });
}
void Scene::SortByDepth(std::vector<Trig>& v)
{
	std::vector<std::pair<float, size_t>> depthPairs;
	depthPairs.reserve(v.size());

	for (size_t i = 0; i < v.size(); ++i)
		depthPairs.emplace_back(v[i].DepthTest(this), i);

	std::sort(depthPairs.begin(), depthPairs.end(), [](const auto& a, const auto& b) {
		return a.first > b.first;
		});

	std::vector<Trig> sortedV;
	sortedV.reserve(v.size());

	for (const auto& pair : depthPairs) {
		sortedV.push_back(std::move(v[pair.second]));
	}

	v = std::move(sortedV);
}
void Scene::RenderScene(Canvas& c) {
	for (const auto& light : lights) {
		light->Render(c);
	}

	std::vector<Trig> trigs = {};
	for (Transform* t : objects) {
		t->GetTransformedGeometry(trigs);
	}

	SortByDepth(trigs);
	for (Trig v : trigs)
		v.Render(this, c);
}

Trig::Trig(Scene* s, Vector3f v1, Vector3f v2, Vector3f v3, long c) {
	this->points[0] = v1;
	this->points[1] = v2;
	this->points[2] = v3;

	this->color = c;

	Vector3f inverseRotation = s->cameraRotation * -1;
	for (int i = 0; i < 3; i++)
		cameraSpacePoints[i] = Transform::RotateAround(points[i], s->cameraPosition, inverseRotation) - s->cameraPosition;
}
void Trig::Render(Scene* s, Canvas& c)
{
	Vector2f v1 = s->CalculatePointPosition(c.size, points[0]);
	Vector2f v2 = s->CalculatePointPosition(c.size, points[1]);
	Vector2f v3 = s->CalculatePointPosition(c.size, points[2]);

	if (v1 == Vector2f(-1, -1) || v2 == Vector2f(-1, -1) || v3 == Vector2f(-1, -1)) return;


	Vector3f normal = CalculateNormal(s, false) * -1;

	for (Light* l : s->lights) {
		float darkenVal = (normal.Dot(l->pos.Normalize()) + 1) / 2;
		color = Canvas::MixColors(color, l->color, 1 - l->intensity);

		color = Canvas::DarkenColor(color, darkenVal);
	}

	c.DrawShape(color, v1, v2, v3, v1);

	// Draw Outlines
	/*c.DrawLine(v1, v2);
	c.DrawLine(v2, v3);
	c.DrawLine(v3, v1);*/
}
float Trig::DepthTest(Scene* s) const
{
	Vector3f centroid(0, 0, 0);
	for (int i = 0; i < 3; i++) {
		centroid = centroid + cameraSpacePoints[i];
	}

	centroid = centroid / 3;

	return centroid.z;
}
Vector3f Trig::CalculateNormal(Scene* s, bool inCameraSpace)
{
	Vector3f* pointSpace = inCameraSpace ? cameraSpacePoints : points;

	Vector3f edge1 = pointSpace[1] - pointSpace[0];
	Vector3f edge2 = pointSpace[2] - pointSpace[0];
	Vector3f normal = edge1.Cross(edge2);
	return normal.Normalize();
}
bool Trig::IsBackFace(Scene* s)
{
	Vector3f normal = CalculateNormal(s);
	Vector3f viewDirection = cameraSpacePoints[0].Normalize();
	return normal.Dot(viewDirection) > 0;
}


Transform::Transform(Vector3f pos, Vector3f scale, Vector3f rot, Vector3f pivot) :pos(pos), scale(scale), rotation(rot), pivot(pivot) {}
Vector3f Transform::RotateAround(Vector3f point, Vector3f pivot, Vector3f rotation) {
	rotation = rotation * RAD;

	point = point - pivot;

	Quaternion base(point);

	Quaternion x = createRotationQuaternion(rotation.x, 1, 0, 0);
	Quaternion y = createRotationQuaternion(rotation.y, 0, 1, 0);
	Quaternion z = createRotationQuaternion(rotation.z, 0, 0, 1);

	Quaternion rot = x * y * z;

	Quaternion res = rot.rotateVector(base);

	return Vector3f(res.x, res.y, res.z) + pivot;
}

const std::vector<Vector3f> Cube::points = {
	//Top
	{1,1,0},{1,0,0},{0,0,0},
	{0,0,0},{0,1,0},{1,1,0},
	//Bottom
	{0,0,1},{1,0,1},{1,1,1},
	{1,1,1},{0,1,1},{0,0,1},
	//Top
	{0,0,0},{1,0,0},{1,0,1},
	{1,0,1},{0,0,1},{0,0,0},
	//Bottom
	{1,1,1},{1,1,0},{0,1,0},
	{0,1,0},{0,1,1},{1,1,1},
	//Left
	{0,0,0},{0,0,1},{0,1,1},
	{0,1,1},{0,1,0},{0,0,0},
	//Right
	{1,1,1},{1,0,1},{1,0,0},
	{1,0,0},{1,1,0},{1,1,1},
};
Cube::Cube(Scene* scene, Vector3f pos, Vector3f scale, Vector3f rot) :Transform(pos, scale, rot, { .5,.5,.5 }) {
	scene->objects.push_back(this);
	this->parent = scene;
};
Cube::Cube(Scene* scene, Vector3f pos, Vector3f scale, Vector3f rot, long color) :Transform(pos, scale, rot, { .5,.5,.5 }), color(color) {
	scene->objects.push_back(this);
	this->parent = scene;
};
void Cube::GetTransformedGeometry(std::vector<Trig>& vec)
{
	for (int i = 0; i < points.size() / 3; i++) {
		Vector3f v1 = RotateAround(points[i * 3] * scale, pivot, rotation) + (pos - pivot);
		Vector3f v2 = RotateAround(points[i * 3 + 1] * scale, pivot, rotation) + (pos - pivot);
		Vector3f v3 = RotateAround(points[i * 3 + 2] * scale, pivot, rotation) + (pos - pivot);
		Trig trig(parent, v1, v2, v3, color);
		if (!trig.IsBackFace(parent))
			vec.push_back(trig);
	}
}

void Mesh::FileReader(std::string filename)
{
	std::ifstream file(filename);
	std::string line;
	while (std::getline(file, line))
	{
		std::vector<std::string> data = splitString(line);
		// Vertices
		if (startsWith(line, "v "))
			vertices.push_back(Vector3f(std::stof(data[1]), std::stof(data[2]), std::stof(data[3])));

		// Faces
		if (startsWith(line, "f ")) {
			std::vector<int> face;
			for (int i = 1; i < data.size(); i++)
				face.push_back(std::stoi(splitString(data[i], '/')[0]) - 1);
			faces.push_back(face);
		}
	}
}
Mesh::Mesh(std::string filename, Scene* scene, Vector3f pos, Vector3f scale, Vector3f rot) :Transform(pos, scale, rot, { 0,0,0 })
{
	scene->objects.push_back(this);
	this->parent = scene;
	FileReader(filename);
}
void Mesh::GetTransformedGeometry(std::vector<Trig>& vec)
{
	for (const auto& face : faces) {
		if (face.size() < 3) return;
		Vector3f v1 = RotateAround(vertices[face[0]] * scale, pivot, rotation) + (pos - pivot);
		for (int i = 1; i < face.size() - 1; i++) {
			Vector3f v2 = RotateAround(vertices[face[i]] * scale, pivot, rotation) + (pos - pivot);
			Vector3f v3 = RotateAround(vertices[face[i + 1]] * scale, pivot, rotation) + (pos - pivot);
			Trig trig(parent, v1, v2, v3, 0x515151);
			if (!trig.IsBackFace(parent))
				vec.push_back(trig);
		}
	}
}

Light::Light(Scene* scene, Vector3f pos, Vector3f scale, Vector3f rot) :Transform(pos, scale, rot, { 0,0,0 })
{
	scene->lights.push_back(this);
	this->parent = scene;
}
void Light::Render(Canvas& c)
{
	Vector2f position = parent->CalculatePointPosition(c.size, pos);
	if (position == Vector2f(-1, -1)) return;
	c.DrawCircle(color, position, 10);
}

Sun::Sun(Scene* scene, float time) :Light(scene, { 0,50000,0 }, { 1,1,1 }, { 0,0,0 }), time(time), initialPosition({ 50000,0,0 }) {}
Sun::Sun(Scene* scene, float time, long color, float intensity) :Light(scene, { -50000,0,0 }, { 1,1,1 }, { 0,0,0 }) {
	this->time = time;
	this->initialPosition = { -50000,0,0 };
	this->color = color;
	this->intensity = intensity;
}
Vector3f Sun::CalculatePositionAtTime(float time) {
	float defaultAngle = atan2f(initialPosition.y, initialPosition.z);
	Vector2f rotVector = { sinf(defaultAngle + time) * -initialPosition.x ,cosf(defaultAngle + time) * -initialPosition.x };
	pos = { initialPosition.x,rotVector.y,rotVector.x };
	return pos;
}
void Sun::Render(Canvas& c)
{
	Vector2f position = parent->CalculatePointPosition(c.size, CalculatePositionAtTime(time));

	if (position == Vector2f(-1, -1)) position = lastPos;

	if (position == Vector2f(-1, -1)) return;
	c.DrawRadialGradient(position, 250, color, RGB(201, 242, 255));
	c.DrawCircle(color, position, 50);

	if (!(position == Vector2f(-1, -1))) lastPos = position;
}