#include "Window.h"

Window::Window(HINSTANCE hInstance, int nCmdShow) : hInstance_(hInstance), nCmdShow_(nCmdShow) {}
Canvas::Canvas(HDC hdc, Vector2i size) : hdc_(hdc), size(size) {}
UpdateCallback Window::updateCallback_;
InputState Window::inputState;
int Window::fps_ = 60;

bool Window::CreateWindowAndRun() {
	inputState.parent = this;
	if (!RegisterWindowClass()) {
		return false;
	}

	if (!CreateMainWindow()) {
		return false;
	}

	ShowWindow(hWnd_, nCmdShow_);
	UpdateWindow(hWnd_);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

void Window::RegisterUpdateFunction(int fps, UpdateCallback update) {
	fps_ = fps;
	updateCallback_ = update;
}

Vector2i Window::getSize()
{
	RECT windowRect;
	GetWindowRect(hWnd_, &windowRect);
	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;
	return { width, height };
}
Vector2i Window::getPos()
{
	RECT windowRect;
	GetWindowRect(hWnd_, &windowRect);
	return { windowRect.left, windowRect.top };
}

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOUSEWHEEL: {
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		inputState.scrollDirection = clamp(zDelta / 120, -1, 1);
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		int clientWidth = clientRect.right - clientRect.left;
		int clientHeight = clientRect.bottom - clientRect.top;

		// Create a compatible device context for the back buffer
		HDC backBufferDC = CreateCompatibleDC(hdc);

		// Create a compatible bitmap for the back buffer
		HBITMAP backBufferBitmap = CreateCompatibleBitmap(hdc, clientWidth, clientHeight);

		// Select the back buffer bitmap into the back buffer device context
		HBITMAP oldBackBufferBitmap = (HBITMAP)SelectObject(backBufferDC, backBufferBitmap);

		HBRUSH hBackgroundBrush = CreateSolidBrush(RGB(255, 255, 255)); // Example: White background
		RECT backBufferRect = { 0, 0, clientWidth, clientHeight };
		FillRect(backBufferDC, &backBufferRect, hBackgroundBrush);
		DeleteObject(hBackgroundBrush);
		// Perform drawing operations on the back buffer
		if (updateCallback_)
			updateCallback_(backBufferDC);
		inputState.scrollDirection = 0;
		// Copy the content of the back buffer to the window's device context
		BitBlt(hdc, 0, 0, clientWidth, clientHeight, backBufferDC, 0, 0, SRCCOPY);

		// Clean up
		SelectObject(backBufferDC, oldBackBufferBitmap);
		DeleteObject(backBufferBitmap);
		DeleteDC(backBufferDC);

		EndPaint(hWnd, &ps);

		if (fps_ != -1)
			Sleep(1000 / fps_);
		InvalidateRect(hWnd, nullptr, false);
	}break;
	case WM_KEYDOWN:
		inputState.keys[wParam] = true;
		break;
	case WM_KEYUP:
		inputState.keys[wParam] = false;
		break;
	case WM_MOUSEMOVE:
		inputState.mouseX = LOWORD(lParam);
		inputState.mouseY = HIWORD(lParam);
		break;
	case WM_LBUTTONDOWN:
		inputState.leftButtonDown = true;
		break;
	case WM_LBUTTONUP:
		inputState.leftButtonDown = false;
		break;
	case WM_RBUTTONDOWN:
		inputState.rightButtonDown = true;
		break;
	case WM_RBUTTONUP:
		inputState.rightButtonDown = false;
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

bool Window::RegisterWindowClass() {
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance_;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"Win32WindowClass";

	if (!RegisterClass(&wc)) {
		MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONERROR | MB_OK);
		return false;
	}

	return true;
}

bool Window::CreateMainWindow() {
	hWnd_ = CreateWindowEx(
		0,
		L"Win32WindowClass",
		L"Win32 Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance_,
		NULL);

	if (!hWnd_) {
		MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONERROR | MB_OK);
		return false;
	}

	return true;
}

void Canvas::DrawLine(const Vector2f& pos1, const Vector2f& pos2) {
	MoveToEx(hdc_, pos1.x, pos1.y, nullptr);
	LineTo(hdc_, pos2.x, pos2.y);
}

void Canvas::DrawShape(const long& color, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4) {
	Vector2f vertices[] = { v1,v2,v3,v4 };

	POINT points[4];
	for (int i = 0; i < 4; ++i) {
		points[i] = { static_cast<LONG>(vertices[i].x), static_cast<LONG>(vertices[i].y) };
	}

	HPEN hPen = CreatePen(PS_SOLID, 1, color);
	HPEN hOldPen = (HPEN)SelectObject(hdc_, hPen);

	HBRUSH hBrush = (HBRUSH)CreateSolidBrush(color);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc_, hBrush);

	Polygon(hdc_, points, 4);

	SelectObject(hdc_, hOldPen);
	SelectObject(hdc_, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}
void Canvas::DrawShape(const long& topColor, const long& bottomColor, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4) {
	TRIVERTEX vertex[4];
	vertex[0].x = static_cast<LONG>(v1.x);
	vertex[0].y = static_cast<LONG>(v1.y);
	vertex[0].Red = GetRValue(topColor) << 8;
	vertex[0].Green = GetGValue(topColor) << 8;
	vertex[0].Blue = GetBValue(topColor) << 8;
	vertex[0].Alpha = 0x0000;

	vertex[1].x = static_cast<LONG>(v2.x);
	vertex[1].y = static_cast<LONG>(v2.y);
	vertex[1].Red = GetRValue(topColor) << 8;
	vertex[1].Green = GetGValue(topColor) << 8;
	vertex[1].Blue = GetBValue(topColor) << 8;
	vertex[1].Alpha = 0x0000;

	vertex[2].x = static_cast<LONG>(v3.x);
	vertex[2].y = static_cast<LONG>(v3.y);
	vertex[2].Red = GetRValue(bottomColor) << 8;
	vertex[2].Green = GetGValue(bottomColor) << 8;
	vertex[2].Blue = GetBValue(bottomColor) << 8;
	vertex[2].Alpha = 0x0000;

	vertex[3].x = static_cast<LONG>(v4.x);
	vertex[3].y = static_cast<LONG>(v4.y);
	vertex[3].Red = GetRValue(bottomColor) << 8;
	vertex[3].Green = GetGValue(bottomColor) << 8;
	vertex[3].Blue = GetBValue(bottomColor) << 8;
	vertex[3].Alpha = 0x0000;

	GRADIENT_TRIANGLE gTriangle[2];
	gTriangle[0].Vertex1 = 0;
	gTriangle[0].Vertex2 = 1;
	gTriangle[0].Vertex3 = 2;

	gTriangle[1].Vertex1 = 0;
	gTriangle[1].Vertex2 = 2;
	gTriangle[1].Vertex3 = 3;

	GradientFill(hdc_, vertex, 4, gTriangle, 2, GRADIENT_FILL_TRIANGLE);
}

void Canvas::DrawCircle(const long& color, const Vector2f& pos, const float radius) {
	// Convert color from long to COLORREF
	COLORREF colorRef = static_cast<COLORREF>(color);

	// Save the current brush
	HBRUSH hBrushOld;
	HBRUSH hBrush = CreateSolidBrush(colorRef);
	hBrushOld = (HBRUSH)SelectObject(hdc_, hBrush);

	// Save the current pen
	HPEN hPenOld;
	HPEN hPen = CreatePen(PS_SOLID, 1, colorRef);
	hPenOld = (HPEN)SelectObject(hdc_, hPen);

	// Draw the circle
	Ellipse(hdc_, static_cast<int>(pos.x - radius), static_cast<int>(pos.y - radius), static_cast<int>(pos.x + radius), static_cast<int>(pos.y + radius));

	// Restore the original brush and pen
	SelectObject(hdc_, hBrushOld);
	SelectObject(hdc_, hPenOld);

	// Clean up
	DeleteObject(hBrush);
	DeleteObject(hPen);
}

void Canvas::DrawRadialGradient(Vector2f center, float radius, const long& innerColor, const long& outerColor) {
	int steps = 30;
	for (int i = steps; i > 0; --i) {
		float t = static_cast<float>(i) / steps;
		float currentRadius = t * radius;

		// Interpolate colors
		BYTE r = static_cast<BYTE>(GetRValue(innerColor) * (1 - t) + GetRValue(outerColor) * t);
		BYTE g = static_cast<BYTE>(GetGValue(innerColor) * (1 - t) + GetGValue(outerColor) * t);
		BYTE b = static_cast<BYTE>(GetBValue(innerColor) * (1 - t) + GetBValue(outerColor) * t);
		long currentColor = RGB(r, g, b);
		HBRUSH hBrush = CreateSolidBrush(currentColor);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc_, hBrush);

		HPEN hOldPen = (HPEN)SelectObject(hdc_, GetStockObject(NULL_PEN));
		// Draw the circle
		Ellipse(hdc_,
			static_cast<int>(center.x - currentRadius),
			static_cast<int>(center.y - currentRadius),
			static_cast<int>(center.x + currentRadius),
			static_cast<int>(center.y + currentRadius));

		SelectObject(hdc_, hOldPen);
		SelectObject(hdc_, hOldBrush);
		DeleteObject(hBrush);
	}
}
void Canvas::DrawRadialGradient(Vector2f center, float radius, const long& innerColor) {
	int steps = 30;
	for (int i = steps; i > 0; --i) {
		float t = static_cast<float>(i) / steps;
		float currentRadius = t * radius;

		// Keep the RGB values of the inner color constant
		BYTE r = GetRValue(innerColor);
		BYTE g = GetGValue(innerColor);
		BYTE b = GetBValue(innerColor);

		// Interpolate the alpha value from 255 (opaque) to 0 (transparent)
		BYTE a = static_cast<BYTE>(255 * (1 - t)); // Alpha transitions to 0

		// Create an ARGB color
		long currentColor = (a << 24) | RGB(r, g, b); // Assuming ARGB format

		// Create a brush with the current color
		HBRUSH hBrush = CreateSolidBrush(currentColor);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc_, hBrush);

		HPEN hOldPen = (HPEN)SelectObject(hdc_, GetStockObject(NULL_PEN));
		// Draw the circle
		Ellipse(hdc_,
			static_cast<int>(center.x - currentRadius),
			static_cast<int>(center.y - currentRadius),
			static_cast<int>(center.x + currentRadius),
			static_cast<int>(center.y + currentRadius));

		SelectObject(hdc_, hOldPen);
		SelectObject(hdc_, hOldBrush);
		DeleteObject(hBrush);
	}
}
COLORREF Canvas::MixColors(COLORREF color1, COLORREF color2, double percentage) {
	percentage = clamp<double>(percentage, 0, 1);

	BYTE r = static_cast<BYTE>(GetRValue(color1) * percentage + GetRValue(color2) * (1 - percentage));
	BYTE g = static_cast<BYTE>(GetGValue(color1) * percentage + GetGValue(color2) * (1 - percentage));
	BYTE b = static_cast<BYTE>(GetBValue(color1) * percentage + GetBValue(color2) * (1 - percentage));
	return RGB(r, g, b);
}

COLORREF Canvas::MixColors(COLORREF color1, COLORREF color2) {
	BYTE r = (GetRValue(color1) + GetRValue(color2)) / 2;
	BYTE g = (GetGValue(color1) + GetGValue(color2)) / 2;
	BYTE b = (GetBValue(color1) + GetBValue(color2)) / 2;
	return RGB(r, g, b);
}

COLORREF Canvas::DarkenColor(COLORREF color, double percentage) {
	BYTE r = static_cast<BYTE>(GetRValue(color) * (1 - percentage));
	BYTE g = static_cast<BYTE>(GetGValue(color) * (1 - percentage));
	BYTE b = static_cast<BYTE>(GetBValue(color) * (1 - percentage));
	return RGB(r, g, b);
}

COLORREF Canvas::LightenColor(COLORREF color, double percentage) {
	BYTE r = static_cast<BYTE>(GetRValue(color) + (255 - GetRValue(color)) * percentage);
	BYTE g = static_cast<BYTE>(GetGValue(color) + (255 - GetGValue(color)) * percentage);
	BYTE b = static_cast<BYTE>(GetBValue(color) + (255 - GetBValue(color)) * percentage);
	return RGB(r, g, b);
}