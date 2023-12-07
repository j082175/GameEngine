#pragma once

#include <algorithm>
#include <directxtk/SimpleMath.h>
#include <iostream>
#include <memory>

#include "AnimationClip.h"
#include "AppBase.h"
#include "GeometryGenerator.h"
#include "ImageFilter.h"
#include "Model.h"
#include "SkinnedMeshModel.h"

namespace hlab {

using DirectX::BoundingSphere;
using DirectX::SimpleMath::Vector3;

class Party : public AppBase {
  public:
    Party();

    virtual bool InitScene() override;

    void UpdateLights(float dt) override;
    void UpdateGUI() override;
    void Update(float dt) override;
    void Render() override;

  protected:
    shared_ptr<Model> m_ground;

    vector<shared_ptr<SkinnedMeshModel>> m_characterArr;

  private:
    void UpdateAnimationMove(const std::shared_ptr<SkinnedMeshModel> &character,
                             float dt, float &frameCount, int &state,
                             bool &isRun);

    void
    UpdateAnimationDance(const std::shared_ptr<SkinnedMeshModel> &character,
                         float dt, float &frameCount, int &state, bool &isRun);
};

} // namespace hlab
