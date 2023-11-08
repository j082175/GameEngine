#pragma once

#include "Texture2D.h"
#include "AppBase.h"
#include "Model.h"

namespace hlab {
class Test : public AppBase {

  public:
    __declspec(align(256)) struct Constants {
        float scale = 1.0f;
    };

    virtual bool Initialize() override;
    virtual void UpdateGUI() override;
    virtual void Update(float dt) override;
    virtual void Render() override;

  protected:
    // 예시용이라서 GraphicsCommon.h/cpp에 구현하지 않음
    ComPtr<ID3D11ComputeShader> m_testCS;
    ComputePSO m_testComputePSO;

    ComPtr<ID3D11UnorderedAccessView> m_backUAV;
    ComPtr<ID3D11ShaderResourceView> m_srv;

	ComPtr<ID3D11Texture2D> m_backBuffer;
	ComPtr<ID3D11Texture2D> m_texture;

	Constants m_constantsCPU;
    ComPtr<ID3D11Buffer> m_constantsGPU;

	Texture2D m_tex;

}; //

} // namespace hlab
