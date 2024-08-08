#include "framework.h"

Window* win;

bool lockMouse = true;

Scene mainScene;
Cube c(&mainScene, { 0,5,0 }, { 1,1,1 }, { 0,0,0 }, 0x2356FF);
Cube c2(&mainScene, { 5,50,0 }, { 100,100,1 }, { 0,0,0 }, 0x23A456);
Cube c3(&mainScene, { 0,5,5 }, { 2,3,4 }, { 45,45,0 }, 0xFFFF00);
Mesh m("D:\\Media\\GAME ASSETS\\INDUSTRIAL\\industrial_wooden_crate01.obj", &mainScene, { 2,-10,0 }, { 1,1,1 }, { 0,90,0 });

Sun sun(&mainScene, 0);

static void Update(HDC& hdc) {
	Canvas canvas(hdc, win->getSize());
	static auto previousTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> d = currentTime - previousTime;
	float deltaTime = d.count();
	Print("Fps: ", 1.0f / deltaTime);

	sun.time += .1f * deltaTime;

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


