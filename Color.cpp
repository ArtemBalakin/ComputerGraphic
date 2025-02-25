#include "Color.h"

DirectX::XMFLOAT4 GetColor(Color color) {
    switch (color) {
        case Color::Red: return DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
        case Color::Green: return DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
        case Color::Blue: return DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
        case Color::White: return DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        case Color::Black: return DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        default: return DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}