#include "Test.h"

bool hlab::Test::Initialize() {
    AppBase::m_backBufferFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

    if (!AppBase::Initialize()) {
        return false;
    }

    m_tex.Initialize(m_device, 1280, 1280, m_backBufferFormat);

    m_swapChain->GetBuffer(0, IID_PPV_ARGS(m_backBuffer.GetAddressOf()));

    D3D11_TEXTURE2D_DESC desc{};
    m_backBuffer->GetDesc(&desc);

    m_device->CreateTexture2D(&desc, NULL, m_texture.GetAddressOf());

    m_device->CreateUnorderedAccessView(m_texture.Get(), NULL,
                                        m_backUAV.GetAddressOf());

    D3D11Utils::CreateConstBuffer(m_device, m_constantsCPU, m_constantsGPU);

    D3D11Utils::CreateComputeShader(m_device, L"TestCS.hlsl", m_testCS);

    m_testComputePSO.m_computeShader = m_testCS;

    return true;
}

void hlab::Test::Update(float dt) {
    D3D11Utils::UpdateBuffer(m_context, m_constantsCPU, m_constantsGPU);
}

void hlab::Test::Render() {

    AppBase::SetPipelineState(m_testComputePSO);

    m_context->CSSetConstantBuffers(0, 1, m_constantsGPU.GetAddressOf());
    m_context->CSSetUnorderedAccessViews(0, 1, m_backUAV.GetAddressOf(), NULL);

    // TODO: ThreadGroupCount를 쉐이더의 numthreads에 따라 잘 바꿔주기
    // TODO: ceil() 사용하는 이유 이해하기

    float x = 32.f;
    float y = 32.f;

    m_context->Dispatch(UINT(ceil(m_screenWidth / x)), ceil(m_screenHeight / y),
                        1);

    // 컴퓨터 쉐이더가 하던 일을 끝내게 만들고 Resources 해제
    AppBase::ComputeShaderBarrier();

    m_context->CopyResource(m_backBuffer.Get(), m_texture.Get());
}

void hlab::Test::UpdateGUI() {

    ImGui::SliderFloat("Scale", &m_constantsCPU.scale, 0.0f, 1.0f);
    //ImGui::SliderFloat("Dispatch", &m_constantsCPU.dispatch, 1.0f, 32.f);
}