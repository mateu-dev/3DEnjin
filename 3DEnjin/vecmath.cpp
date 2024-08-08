#include "vecmath.h"
Vector2f::Vector2f(const Vector2i& vec) : x(static_cast<float>(vec.x)), y(static_cast<float>(vec.y)) {}
Vector2i::Vector2i(const Vector2f& vec) : x(static_cast<int>(vec.x)), y(static_cast<int>(vec.y)) {}