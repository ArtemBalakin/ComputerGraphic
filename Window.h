#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>
#include <string>

class Window {
public:
    Window(HINSTANCE hInstance, const std::wstring& title, int width, int height);
    ~Window();

    HWND GetHandle() const;
    void Show();
    bool ProcessMessages();

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
    static Window* s_instance;

    HWND m_hwnd;
    HINSTANCE m_hInstance;
    std::wstring m_title;
    int m_width;
    int m_height;
};

#endif // WINDOW_H