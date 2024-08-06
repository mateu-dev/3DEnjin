#include "framework.h"

Window* win;

bool lockMouse = true;

Scene mainScene;
Cube c(&mainScene, { 0,5,0 }, { 1,1,1 }, { 0,0,0 }, 0x2356FF);
Cube c2(&mainScene, { 5,50,0 }, { 100,100,1 }, { 0,0,0 }, 0x23A456);
Cube c3(&mainScene, { 0,5,5 }, { 2,3,4 }, { 45,45,0 }, 0xFFFF00);
Mesh m("D:\\Media\\GAME ASSETS\\INDUSTRIAL\\industrial_wooden_crate01.obj", &mainScene, { 2,-10,0 }, { 1,1,1 }, { 0,90,0 });
static void Update(HDC& hdc) {
	static auto previousTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> d = currentTime - previousTime;
	float deltaTime = d.count();

	Print("Fps: ",1.0f / deltaTime);

	Canvas canvas(hdc, win->getSize());

	c3.rotation = c3.rotation + Vector3f(1, 1, 1) * deltaTime;

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
	float speed = 10;
	Vector3f forward = Vector3f(0, cosf(-mainScene.cameraRotation.y * RAD), sinf(-mainScene.cameraRotation.y * RAD)) * speed * deltaTime;
	Vector3f left = Vector3f(0, cosf((-mainScene.cameraRotation.y + 90) * RAD), sinf((-mainScene.cameraRotation.y + 90) * RAD)) * speed * deltaTime;
	if (win->inputState.keys[0x57])
		mainScene.cameraPosition = mainScene.cameraPosition + forward;
	if (win->inputState.keys[0x53])
		mainScene.cameraPosition = mainScene.cameraPosition - forward;
	if (win->inputState.keys[0x41])
		mainScene.cameraPosition = mainScene.cameraPosition - left;
	if (win->inputState.keys[0x44])
		mainScene.cameraPosition = mainScene.cameraPosition + left;
	if (win->inputState.keys[0x51])
		mainScene.cameraPosition.x += speed * deltaTime;
	if (win->inputState.keys[0x45])
		mainScene.cameraPosition.x -= speed * deltaTime;

	mainScene.RenderScene(canvas);

	previousTime = currentTime;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	win = new Window(hInstance, nCmdShow);

	Window::RegisterUpdateFunction(-1, Update);
	win->CreateWindowAndRun();

	return 0;
}


