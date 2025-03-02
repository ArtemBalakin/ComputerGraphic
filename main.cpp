#include "Window.h"
#include "Render.h"
#include "Square.h"
#include <vector>
#include <chrono>

int main() {
    Window window(GetModuleHandle(nullptr), L"My3DApp", 800, 800);
    window.Show();
    Renderer renderer(window.GetHandle(), 800, 600);

    // Создаем два квадрата
    std::vector<Square> squares = {
        {400.0f, 400.0f, 200.0f, Color::Red, 1, false, 1.0f, 150.0f, 400.0f, 400.0f, 0.0f, false, 0.0f, 0.0f, true},  // Орбитальное движение, управляемый
        {200.0f, 200.0f, 100.0f, Color::Green, 0, false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, true, 2.0f, 0.0f, false}        // Вращение на месте, не управляемый
    };

    renderer.SetSquares(squares);
    renderer.SetBackgroundColor(Color::Black);

    MSG msg = {};
    while (true) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            renderer.Update(0.016f); // Примерный deltaTime = 1/60 сек
            renderer.Render();
        }
    }
    return 0;
}