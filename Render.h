#ifndef RENDERER_H
#define RENDERER_H

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <string>
#include "Square.h"
#include "Color.h"

class Renderer {
public:
    Renderer(HWND hwnd, int width, int height);
    ~Renderer() = default;

    void SetBackgroundColor(Color color);
    void SetWireframeMode(bool wireframe);
    void SetSquares(std::vector<Square> squares);
    void Render();

private:
    void HandleError(HRESULT hr, const char* file, int line); // Объявление метода
    void InitDeviceAndSwapChain();
    void InitShaders();
    void InitRasterizerStates();
    void UpdateBuffers();
    void GenerateVerticesAndIndices();
    void SetupPipeline();
    void SetupViewport();


    HWND m_hwnd;
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vb;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_ib;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_solidRastState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireframeRastState;

    int m_width;
    int m_height;
    Color m_backgroundColor;
    bool m_useWireframe;
    std::vector<Square> m_squares;
    std::vector<DirectX::XMFLOAT4> m_vertices;
    std::vector<UINT> m_indices;

    static constexpr UINT VERTEX_STRIDE = sizeof(DirectX::XMFLOAT4) * 2;
    static constexpr DXGI_FORMAT INDEX_FORMAT = DXGI_FORMAT_R32_UINT;
};

#endif // RENDERER_H