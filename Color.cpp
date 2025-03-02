#include "Color.h"
#include <DirectXMath.h>

DirectX::XMFLOAT4 GetColor(Color color) {
    using namespace DirectX;

    switch(color) {
        case Color::Red:    return XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
        case Color::Green: return XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
        case Color::Blue:  return XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
        case Color::White: return XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        case Color::Black: return XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        default:           return XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}