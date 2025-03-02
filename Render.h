#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <vector>
#include "Color.h"
#include "Square.h"
#include <dinput.h> // Для DirectInput

class Renderer {
public:
    Renderer(HWND hwnd, int width, int height);
    void Render();
    void Update(float deltaTime);
    void SetSquares(const std::vector<Square>& squares);
    void SetBackgroundColor(Color color);
    void SetWireframeMode(bool wireframe);

private:
    void HandleError(HRESULT hr, const char* file, int line);
    void InitDeviceAndSwapChain();
    void InitShaders();
    void InitRasterizerStates();
    void CreateConstantBuffer();
    void UpdateBuffers();
    void GenerateVerticesAndIndices();
    void SetupPipeline();
    void SetupViewPort();
    void InitDirectInput();      // Инициализация DirectInput
    void HandleKeyboardInput();  // Обработка ввода с клавиатуры

    HWND m_hwnd;
    int m_width, m_height;
    Color m_backgroundColor;
    bool m_useWireframe;

    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_solidRastState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireframeRastState;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vb, m_ib, m_constantBuffer;

    // DirectInput
    Microsoft::WRL::ComPtr<IDirectInput8> m_directInput;
    Microsoft::WRL::ComPtr<IDirectInputDevice8> m_keyboard;

    std::vector<Square> m_squares;
    std::vector<DirectX::XMFLOAT4> m_vertices;
    std::vector<UINT> m_indices;

    static constexpr UINT VERTEX_STRIDE = sizeof(DirectX::XMFLOAT4) * 2;
    static constexpr DXGI_FORMAT INDEX_FORMAT = DXGI_FORMAT_R32_UINT;
};