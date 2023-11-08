#pragma once

#include <algorithm>
#include <directxtk/SimpleMath.h>
#include <iostream>
#include <memory>

#include "AppBase.h"
#include "GeometryGenerator.h"
#include "ImageFilter.h"
#include "Model.h"
#include <map>
#include "Timer.h"

namespace hlab {

using DirectX::BoundingSphere;
using DirectX::SimpleMath::Vector3;

class ExampleApp : public AppBase {
  public:
    ExampleApp();
    
    virtual bool InitScene() override;

    void UpdateLights(float dt);
    void UpdateGUI() override;
    void Update(float dt) override;
    void Render() override;

  protected:
    shared_ptr<Model> m_ground;

	ComPtr<ID3D11Texture2D> m_texture;

	ComPtr<ID3D11RenderTargetView> m_rtv;
    ComPtr<ID3D11ShaderResourceView> m_srv;
	ComPtr<ID3D11UnorderedAccessView> m_uav;
	ComPtr<ID3D11ComputeShader> m_cs;
    ComputePSO m_pso;

	std::map<std::string, std::shared_ptr<Model>> m_basicListMap;

	Timer m_timer;
};

} // namespace hlab
