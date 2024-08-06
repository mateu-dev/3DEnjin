// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <iostream>
#include <algorithm>
#include <functional>
#include <mutex>
#include <atomic>
#include <unordered_set>
#include <string>
#include <sstream>
#include <fstream>

#include "vecmath.h"
#include "Window.h"
#include "Mesh.h"

#ifndef DEBUG
#define DEBUG
template<typename T>
inline std::string to_string_impl(const T& msg) {
	return std::to_string(msg);
}
inline std::string to_string_impl(const std::string& msg) {
	return msg;
}
inline std::string to_string_impl(const char* msg) {
	return std::string(msg);
}
inline std::string to_string_impl(const Vector2f& msg) {
	return "Vector2f(" + to_string_impl(msg.x) + ", " + to_string_impl(msg.y) + ")";
}
inline std::string to_string_impl(const Vector2i& msg) {
	return "Vector2i(" + to_string_impl(msg.x) + ", " + to_string_impl(msg.y) + ")";
}
inline std::string to_string_impl(const Vector3f& msg) {
	return "Vector3f(" + to_string_impl(msg.x) + ", " + to_string_impl(msg.y) + ", " + to_string_impl(msg.z) + ")";
}

template<typename T>
void Print(const T& msg) {
	std::string message = to_string_impl(msg) + "\n";
	OutputDebugStringA(message.c_str());
}
template<typename T, typename... Args>
void Print(const T& msg, const Args&... args) {
	std::ostringstream oss;
	oss << to_string_impl(msg);
	int dummy[] = { 0, ((void)(oss << " " << to_string_impl(args)), 0)... };
	(void)dummy; // Avoid unused variable warning
	oss << "\n";
	OutputDebugStringA(oss.str().c_str());
}

#endif // !DEBUG

