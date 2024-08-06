#ifndef WIN32_WINDOW_H
#define WIN32_WINDOW_H

#include "framework.h"

typedef std::function<void(HDC&)> UpdateCallback;

struct InputState {
	bool keys[256];
	int mouseX;
	int mouseY;
	int scrollDirection;
	bool leftButtonDown;
	bool rightButtonDown;
	bool isScrolling;
};

class Window {
public:
	HWND hWnd_;
	Window(HINSTANCE, int);
	bool CreateWindowAndRun();
	static void RegisterUpdateFunction(int, UpdateCallback);
	Vector2i getSize();
	Vector2i getPos();
	static InputState inputState;
private:
	static UpdateCallback updateCallback_;
	static int fps_;
	static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
	bool RegisterWindowClass();
	bool CreateMainWindow();

	HINSTANCE hInstance_;
	int nCmdShow_;
};



class Canvas {
public:
	Canvas(HDC hdc, Vector2i size);
	void DrawLine(const Vector2f& pos1, const Vector2f& pos2);
	void DrawShape(const long& color, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4);
	void DrawShapeWithTexture(const std::string& textureFilePath, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4);
	HDC hdc_;
	Vector2i size;
private:
};

template <typename T>
T clamp(const T& n, const T& lower, const T& upper) {
	return max(lower, min(n, upper));
}

#endif // WIN32_WINDOW_H