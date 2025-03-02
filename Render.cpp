#include "Render.h"
#include <d3dcompiler.h>
#include <sstream>
#include <cmath>
#include <dinput.h> // Для DirectInput

#define DX_CHECK(expr) { HRESULT hr = (expr); if (FAILED(hr)) HandleError(hr, __FILE__, __LINE__); }

Renderer::Renderer(HWND hwnd, int width, int height)
    : m_hwnd(hwnd), m_width(width), m_height(height),
      m_backgroundColor(Color::Black),
      m_useWireframe(false) {
    InitDeviceAndSwapChain();
    InitShaders();
    InitRasterizerStates();
    CreateConstantBuffer();
    InitDirectInput();
}

void Renderer::HandleError(HRESULT hr, const char* file, int line) {
    std::ostringstream ss;
    ss << "DirectX Error (0x" << std::hex << hr << ")\nFile: " << file << "\nLine: " << line;
    MessageBoxA(m_hwnd, ss.str().c_str(), "Error", MB_OK);
    exit(1);
}

void Renderer::InitDeviceAndSwapChain() {
    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = 2;
    swapDesc.BufferDesc.Width = m_width;
    swapDesc.BufferDesc.Height = m_height;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = m_hwnd;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.Windowed = TRUE;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    DX_CHECK(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_DEBUG,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapDesc,
        &m_swapChain,
        &m_device,
        nullptr,
        &m_context
    ));

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    DX_CHECK(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
    DX_CHECK(m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_rtv));
}

void Renderer::CreateConstantBuffer() {
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(DirectX::XMMATRIX);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    DX_CHECK(m_device->CreateBuffer(&cbDesc, nullptr, &m_constantBuffer));
}

void Renderer::InitShaders() {
    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, psBlob, errorBlob;

    DX_CHECK(D3DCompileFromFile(
        L"Shaders/MyVeryFirstShader.hlsl",
        nullptr,
        nullptr,
        "VSMain",
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &vsBlob,
        &errorBlob
    ));

    DX_CHECK(m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader));

    DX_CHECK(D3DCompileFromFile(
        L"Shaders/MyVeryFirstShader.hlsl",
        nullptr,
        nullptr,
        "PSMain",
        "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &psBlob,
        &errorBlob
    ));

    DX_CHECK(m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader));

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    DX_CHECK(m_device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_layout));
}

void Renderer::InitRasterizerStates() {
    D3D11_RASTERIZER_DESC desc = {};
    desc.CullMode = D3D11_CULL_NONE;
    desc.FillMode = D3D11_FILL_SOLID;
    desc.DepthClipEnable = true;
    DX_CHECK(m_device->CreateRasterizerState(&desc, &m_solidRastState));

    desc.FillMode = D3D11_FILL_WIREFRAME;
    DX_CHECK(m_device->CreateRasterizerState(&desc, &m_wireframeRastState));
}

void Renderer::InitDirectInput() {
    HRESULT hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, nullptr);
    if (FAILED(hr)) throw std::runtime_error("Failed to create DirectInput");

    hr = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, nullptr);
    if (FAILED(hr)) throw std::runtime_error("Failed to create keyboard device");

    hr = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
    if (FAILED(hr)) throw std::runtime_error("Failed to set data format");

    hr = m_keyboard->SetCooperativeLevel(m_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(hr)) throw std::runtime_error("Failed to set cooperative level");

    hr = m_keyboard->Acquire();
    if (FAILED(hr)) throw std::runtime_error("Failed to acquire keyboard");
}

void Renderer::HandleKeyboardInput() {
    BYTE keyboardState[256];
    HRESULT hr = m_keyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
    if (FAILED(hr)) return; // Если устройство недоступно, пропускаем

    float moveSpeed = 5.0f; // Скорость движения квадрата

    for (auto& square : m_squares) {
        if (square.controllable) { // Управляем только если controllable == true
            if (keyboardState[DIK_UP] & 0x80)    square.y -= moveSpeed; // Вверх
            if (keyboardState[DIK_DOWN] & 0x80)  square.y += moveSpeed; // Вниз
            if (keyboardState[DIK_LEFT] & 0x80)  square.x -= moveSpeed; // Влево
            if (keyboardState[DIK_RIGHT] & 0x80) square.x += moveSpeed; // Вправо
        }
    }

    GenerateVerticesAndIndices();
    UpdateBuffers();
}

void Renderer::Update(float deltaTime) {
    HandleKeyboardInput(); // Обработка ввода с клавиатуры
    bool needsUpdate = false;
    for (auto& square : m_squares) {
        if (square.isMoving && square.orbitSpeed != 0.0f) {
            square.currentAngle += square.orbitSpeed * deltaTime;
            square.x = square.orbitX + square.orbitRadius * cosf(square.currentAngle);
            square.y = square.orbitY + square.orbitRadius * sinf(square.currentAngle);
            needsUpdate = true;
        }
        if (square.isRotating && square.rotationSpeed != 0.0f) {
            square.currentRotationAngle += square.rotationSpeed * deltaTime;
            needsUpdate = true;
        }
    }
    if (needsUpdate) {
        GenerateVerticesAndIndices();
        UpdateBuffers();
    }
}

void Renderer::SetSquares(const std::vector<Square>& squares) {
    m_squares = squares;
    GenerateVerticesAndIndices();
    UpdateBuffers();
}

void Renderer::SetBackgroundColor(Color color) {
    m_backgroundColor = color;
}

void Renderer::SetWireframeMode(bool wireframe) {
    m_useWireframe = wireframe;
}

void Renderer::Render() {
    if (!m_context || !m_swapChain || !m_rtv) return;

    m_context->ClearState();
    SetupPipeline();
    SetupViewPort();

    DirectX::XMFLOAT4 bgColor = GetColor(m_backgroundColor);
    float clearColor[] = { bgColor.x, bgColor.y, bgColor.z, bgColor.w };
    m_context->ClearRenderTargetView(m_rtv.Get(), clearColor);

    DirectX::XMMATRIX worldViewProj = DirectX::XMMatrixIdentity();

    D3D11_MAPPED_SUBRESOURCE mapped;
    DX_CHECK(m_context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
    memcpy(mapped.pData, &worldViewProj, sizeof(DirectX::XMMATRIX));
    m_context->Unmap(m_constantBuffer.Get(), 0);

    m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    if (!m_indices.empty()) {
        m_context->DrawIndexed(static_cast<UINT>(m_indices.size()), 0, 0);
    }

    m_swapChain->Present(1, 0);
}

void Renderer::UpdateBuffers() {
    if (m_vertices.empty() || m_indices.empty()) return;

    if (m_vb) {
        D3D11_MAPPED_SUBRESOURCE mapped;
        DX_CHECK(m_context->Map(m_vb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
        memcpy(mapped.pData, m_vertices.data(), m_vertices.size() * sizeof(DirectX::XMFLOAT4));
        m_context->Unmap(m_vb.Get(), 0);
    } else {
        D3D11_BUFFER_DESC vbDesc = {};
        vbDesc.ByteWidth = static_cast<UINT>(m_vertices.size() * sizeof(DirectX::XMFLOAT4));
        vbDesc.Usage = D3D11_USAGE_DYNAMIC;
        vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA vbData = {m_vertices.data()};
        DX_CHECK(m_device->CreateBuffer(&vbDesc, &vbData, &m_vb));
    }

    if (m_ib) {
        D3D11_MAPPED_SUBRESOURCE mapped;
        DX_CHECK(m_context->Map(m_ib.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
        memcpy(mapped.pData, m_indices.data(), m_indices.size() * sizeof(UINT));
        m_context->Unmap(m_ib.Get(), 0);
    } else {
        D3D11_BUFFER_DESC ibDesc = {};
        ibDesc.ByteWidth = static_cast<UINT>(m_indices.size() * sizeof(UINT));
        ibDesc.Usage = D3D11_USAGE_DYNAMIC;
        ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA ibData = {m_indices.data()};
        DX_CHECK(m_device->CreateBuffer(&ibDesc, &ibData, &m_ib));
    }
}

void Renderer::GenerateVerticesAndIndices() {
    m_vertices.clear();
    m_indices.clear();
    const float invWidth = 2.0f / m_width;
    const float invHeight = 2.0f / m_height;

    for (const auto& square : m_squares) {
        const int tess = std::max(1, square.tessellationLevel);
        const float halfSize = square.size / 2.0f;
        const float left = (square.x - halfSize) * invWidth - 1.0f;
        const float right = (square.x + halfSize) * invWidth - 1.0f;
        const float top = 1.0f - (square.y - halfSize) * invHeight;
        const float bottom = 1.0f - (square.y + halfSize) * invHeight;
        const DirectX::XMFLOAT4 color = GetColor(square.color);

        // Вычисление центра квадрата в NDC
        const float centerX = 2.0f * (square.x / m_width) - 1.0f;
        const float centerY = 1.0f - 2.0f * (square.y / m_height);
        const float theta = square.currentRotationAngle;
        const float cosTheta = cosf(theta);
        const float sinTheta = sinf(theta);

        for (int y = 0; y <= tess; ++y) {
            const float v = static_cast<float>(y) / tess;
            for (int x = 0; x <= tess; ++x) {
                const float u = static_cast<float>(x) / tess;
                // Позиция вершины без вращения
                float vx = left + (right - left) * u;
                float vy = top + (bottom - top) * v;
                // Применение вращения вокруг центра
                float dx = vx - centerX;
                float dy = vy - centerY;
                float vx_rot = centerX + dx * cosTheta - dy * sinTheta;
                float vy_rot = centerY + dx * sinTheta + dy * cosTheta;
                m_vertices.emplace_back(vx_rot, vy_rot, 0.5f, 1.0f);
                m_vertices.push_back(color);
            }
        }

        const UINT base = static_cast<UINT>(m_vertices.size() / 2 - (tess + 1) * (tess + 1));
        for (int y = 0; y < tess; ++y) {
            for (int x = 0; x < tess; ++x) {
                const UINT i0 = base + y * (tess + 1) + x;
                m_indices.insert(m_indices.end(), {
                    i0, i0 + 1, i0 + tess + 1,
                    i0 + 1, i0 + tess + 2, i0 + tess + 1
                });
            }
        }
    }
}

void Renderer::SetupPipeline() {
    m_context->RSSetState(m_useWireframe ? m_wireframeRastState.Get() : m_solidRastState.Get());
    m_context->IASetInputLayout(m_layout.Get());
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
    m_context->OMSetRenderTargets(1, m_rtv.GetAddressOf(), nullptr);

    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, m_vb.GetAddressOf(), &VERTEX_STRIDE, &offset);
    m_context->IASetIndexBuffer(m_ib.Get(), INDEX_FORMAT, 0);
}

void Renderer::SetupViewPort() {
    D3D11_VIEWPORT vp = {0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, 1.0f};
    m_context->RSSetViewports(1, &vp);
}