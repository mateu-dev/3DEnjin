#include "Mesh.h"

float nearPlane = .01;
float fov = 180;

Vector2f Scene::CalculatePointPosition(Vector2i canvasSize, Vector3f point) {
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
	return iPoint * canvasSize;
}
void Scene::SortByDepth(std::vector<std::unique_ptr<Trig>>& v)
{
	std::vector<std::pair<float, size_t>> depthPairs;
	depthPairs.reserve(v.size());

	for (size_t i = 0; i < v.size(); ++i)
		depthPairs.emplace_back(v[i]->DepthTest(this), i);

	std::sort(depthPairs.begin(), depthPairs.end(), [](const auto& a, const auto& b) {
		return a.first > b.first;
		});

	std::vector<std::unique_ptr<Trig>> sortedV;
	sortedV.reserve(v.size());

	for (const auto& pair : depthPairs) {
		sortedV.push_back(std::move(v[pair.second]));
	}

	v = std::move(sortedV);
}
void Scene::RenderScene(Canvas& c) {
	std::vector<std::unique_ptr<Trig>> trigs = {};
	for (Transform* t : objects) {
		t->GetTransformedGeometry(trigs);
	}

	//OutputDebugStringA((std::to_string(trigs.size()) + "\n").c_str());
	SortByDepth(trigs);
	for (const auto& v : trigs)
		v->Render(this, c);
}

Trig::Trig(Vector3f v1, Vector3f v2, Vector3f v3, long c) {
	this->points = new Vector3f[3]{ v1,v2,v3 };
	this->color = c;
}
Trig::~Trig()
{
	delete points;
}
std::vector<Vector3f> Trig::ToCameraSpace(Scene* s) {
	if (!cameraSpaceCache.empty()) return cameraSpaceCache;

	cameraSpaceCache.reserve(3);
	Vector3f inverseRotation = s->cameraRotation * -1;
	for (int i = 0; i < 3; i++)
		cameraSpaceCache.emplace_back(Transform::RotateAround(points[i], s->cameraPosition, inverseRotation) - s->cameraPosition);

	return cameraSpaceCache;
}
void Trig::Render(Scene* s, Canvas& c)
{
	Vector2f v1 = s->CalculatePointPosition(c.size, points[0]);
	Vector2f v2 = s->CalculatePointPosition(c.size, points[1]);
	Vector2f v3 = s->CalculatePointPosition(c.size, points[2]);

	if (v1 == Vector2f(-1, -1) || v2 == Vector2f(-1, -1) || v3 == Vector2f(-1, -1)) return;

	c.DrawShape(color, v1, v2, v3, v1);
	//c.DrawShapeWithTexture("C:\\Users\\mateu\\Desktop\\skybox.bmp", v1, v2, v3, v1);

	c.DrawLine(v1, v2);
	c.DrawLine(v2, v3);
	c.DrawLine(v3, v1);
}
float Trig::DepthTest(Scene* s)
{
	Vector3f centroid(0, 0, 0);
	for (Vector3f point : ToCameraSpace(s)) {
		centroid = centroid + point;
	}

	centroid = centroid / 3;

	return centroid.z;
}
Vector3f Trig::CalculateNormal(Scene* s)
{
	std::vector<Vector3f> points = ToCameraSpace(s);
	Vector3f edge1 = points[1] - points[0];
	Vector3f edge2 = points[2] - points[0];
	Vector3f normal = edge1.Cross(edge2);
	return normal.Normalize();
}
bool Trig::IsBackFace(Scene* s)
{
	Vector3f normal = CalculateNormal(s);
	Vector3f viewDirection = ToCameraSpace(s)[0].Normalize();
	return normal.Dot(viewDirection) > 0;
}


Transform::Transform(Vector3f pos, Vector3f scale, Vector3f rot, Vector3f pivot) :pos(pos), scale(scale), rotation(rot), pivot(pivot) {}
Vector3f Transform::RotateAround(Vector3f point, Vector3f pivot, Vector3f rotation) {

	rotation = rotation * RAD;
	// Rotate Y
	float dist = sqrtf(powf(point.y - pivot.y, 2) + powf(point.z - pivot.z, 2));
	float defaultAngle = atan2f(point.y - pivot.y, point.z - pivot.z);
	Vector2f rotVector = { sinf(defaultAngle + rotation.y) * dist ,cosf(defaultAngle + rotation.y) * dist };
	point = { point.x,pivot.y + rotVector.y,pivot.z + rotVector.x };
	// Rotate X
	dist = sqrtf(powf(point.x - pivot.x, 2) + powf(point.z - pivot.z, 2));
	defaultAngle = atan2f(point.x - pivot.x, point.z - pivot.z);
	rotVector = { sinf(defaultAngle + rotation.x) * dist ,cosf(defaultAngle + rotation.x) * dist };
	point = { pivot.x + rotVector.x,point.y,pivot.z + rotVector.y };
	// Rotate Z
	dist = sqrtf(powf(point.x - pivot.x, 2) + powf(point.y - pivot.y, 2));
	defaultAngle = atan2f(point.x - pivot.x, point.y - pivot.y);
	rotVector = { sinf(defaultAngle + rotation.z) * dist ,cosf(defaultAngle + rotation.z) * dist };
	point = { pivot.x + rotVector.y,pivot.y + rotVector.x,point.z };

	return point;
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
void Cube::GetTransformedGeometry(std::vector<std::unique_ptr<Trig>>& vec)
{
	for (int i = 0; i < points.size() / 3; i++) {
		Vector3f v1 = RotateAround(points[i * 3] * scale, pivot, rotation) + (pos - pivot);
		Vector3f v2 = RotateAround(points[i * 3 + 1] * scale, pivot, rotation) + (pos - pivot);
		Vector3f v3 = RotateAround(points[i * 3 + 2] * scale, pivot, rotation) + (pos - pivot);
		std::unique_ptr<Trig> ptr = std::make_unique<Trig>(v1, v2, v3, color);
		if (!ptr.get()->IsBackFace(parent))
			vec.push_back(std::move(ptr));
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
void Mesh::GetTransformedGeometry(std::vector<std::unique_ptr<Trig>>& vec)
{
	for (const auto& face : faces) {
		if (face.size() < 3) return;
		Vector3f v1 = RotateAround(vertices[face[0]] * scale, pivot, rotation) + (pos - pivot);
		for (int i = 1; i < face.size() - 1; i++) {
			Vector3f v2 = RotateAround(vertices[face[i]] * scale, pivot, rotation) + (pos - pivot);
			Vector3f v3 = RotateAround(vertices[face[i + 1]] * scale, pivot, rotation) + (pos - pivot);
			std::unique_ptr<Trig> ptr = std::make_unique<Trig>(v1, v2, v3, 0x515151);
			if (!ptr.get()->IsBackFace(parent))
				vec.push_back(std::move(ptr));
		}
	}
}

