#pragma once

#include <algorithm>
#include <directxtk/SimpleMath.h>
#include <iostream>
#include <memory>

#include "AppBase.h"
#include "GeometryGenerator.h"
#include "ImageFilter.h"
#include "Model.h"
#include "Timer.h"
#include <map>

namespace hlab {

using DirectX::BoundingSphere;
using DirectX::SimpleMath::Vector3;

class World : public AppBase {

	struct Tree
	{
        std::shared_ptr<Model> m_leaves;
        std::shared_ptr<Model> m_trunks;
	};

  public:
    World();

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

    Timer m_timer;

	Tree m_tree;
};

} // namespace hlab
