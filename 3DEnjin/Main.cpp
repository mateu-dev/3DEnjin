#include "framework.h"

Window* win;

Scene mainScene;
Cube c(&mainScene, { 0,0,5 }, { 1,1,1 }, { 0,0,0 }, 0x2356FF);

Sun sun(&mainScene, 0);

static void Update(HDC& hdc) {
	Canvas canvas(hdc, win->getSize());
	static auto previousTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> d = currentTime - previousTime;
	float deltaTime = d.count();
	Print("Fps: ", 1.0f / deltaTime);

	// Update Code Here


	mainScene.Freecam(win->inputState, 10, deltaTime);
	mainScene.DrawSkyBox(canvas, RGB(201, 242, 255), RGB(105, 210, 245));
	mainScene.RenderScene(canvas);

	previousTime = currentTime;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	win = new Window(hInstance, nCmdShow);

	Window::RegisterUpdateFunction(-1, Update);
	win->CreateWindowAndRun();


	return 0;
}


