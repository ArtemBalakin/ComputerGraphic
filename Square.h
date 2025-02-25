#ifndef SQUARE_H
#define SQUARE_H

#include <DirectXMath.h>
#include "Color.h"

struct Square {
    DirectX::XMFLOAT2 center;
    float size;
    Color color;
    int tessellationLevel;

    bool operator==(const Square& other) const {
        return
            center.x == other.center.x &&
            center.y == other.center.y &&
            size == other.size &&
            color == other.color &&
            tessellationLevel == other.tessellationLevel;
    }
};

#endif // SQUARE_H