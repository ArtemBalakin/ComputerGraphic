#include "Window.h"
#include "Render.h"
#include "Square.h"
#include <vector>
#include <chrono>

int main() {
    Window window(GetModuleHandle(nullptr), L"My3DApp", 800, 800);
    window.Show();

    Renderer renderer(window.GetHandle(), 800, 800);

    std::vector<Square> squares = {
        { {400, 400}, 200, Color::Red, 1 },
        { {200, 200}, 100, Color::Green, 00 },
    };
    renderer.SetSquares(squares);
    renderer.SetBackgroundColor(Color::Black);
    renderer.SetWireframeMode(true);

    std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
    float totalTime = 0;
    unsigned int frameCount = 0;

    while (window.ProcessMessages()) {
        auto curTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
        PrevTime = curTime;

        totalTime += deltaTime;
        frameCount++;

        if (totalTime > 1.0f) {
            float fps = frameCount / totalTime;
            totalTime -= 1.0f;
            WCHAR text[256];
            swprintf_s(text, TEXT("FPS: %f"), fps);
            SetWindowText(window.GetHandle(), text);
            frameCount = 0;
        }

        renderer.Render();
    }

    return 0;
}