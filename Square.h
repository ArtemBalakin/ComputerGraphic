#ifndef SQUARE_H
#define SQUARE_H

#include "Color.h"

struct Square {
    float x, y;                // Позиция центра квадрата в пикселях
    float size;                // Размер квадрата в пикселях
    Color color;               // Цвет
    int tessellationLevel;     // Уровень тесселяции
    bool isMoving;             // Флаг движения по орбите
    float orbitSpeed;          // Скорость орбитального движения (рад/с)
    float orbitRadius;         // Радиус орбиты
    float orbitX, orbitY;      // Центр орбиты
    float currentAngle;        // Текущий угол орбиты
    bool isRotating;           // Флаг вращения вокруг своей оси
    float rotationSpeed;       // Скорость вращения (рад/с)
    float currentRotationAngle;// Текущий угол вращения
    bool controllable;         // Флаг управления с клавиатуры
};

#endif