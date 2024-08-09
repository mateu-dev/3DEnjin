#ifndef WIN32_WINDOW_H
#define WIN32_WINDOW_H

#include "framework.h"
#pragma comment(lib, "Msimg32.lib")

typedef std::function<void(HDC&)> UpdateCallback;
class Window;

struct InputState {
	bool keys[256];
	int mouseX;
	int mouseY;
	int scrollDirection;
	bool leftButtonDown;
	bool rightButtonDown;
	bool isScrolling;

	Window* parent;
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
	void DrawCircle(const long& color, const Vector2f& pos, const float radius);
	void DrawShape(const long& color, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4);
	void DrawShape(const long& topColor, const long& bottomColor, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4);
	void DrawRadialGradient(Vector2f center, float radius, const long& innerColor);
	void DrawRadialGradient(Vector2f center, float radius, const long& innerColor, const long& outerColor);
	HDC hdc_;
	Vector2i size;

	static COLORREF MixColors(COLORREF color1, COLORREF color2, double percentage);
	static COLORREF MixColors(COLORREF color1, COLORREF color2);
	static COLORREF DarkenColor(COLORREF color, double percentage);
	static COLORREF LightenColor(COLORREF color, double percentage);
private:
};

template <typename T>
T clamp(const T& n, const T& lower, const T& upper) {
	return max(lower, min(n, upper));
}

#endif // WIN32_WINDOW_H