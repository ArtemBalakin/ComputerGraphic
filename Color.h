#ifndef COLOR_H
#define COLOR_H

#include <DirectXMath.h>

enum class Color {
    Red,
    Green,
    Blue,
    White,
    Black
};

DirectX::XMFLOAT4 GetColor(Color color);

#endif // COLOR_H