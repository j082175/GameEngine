#include "Party.h"

#include <DirectXCollision.h> // 구와 광선 충돌 계산에 사용
#include <directxtk/DDSTextureLoader.h>
#include <directxtk/SimpleMath.h>
#include <tuple>
#include <vector>

#include "GeometryGenerator.h"
#include "GraphicsCommon.h"
#include "SkinnedMeshModel.h"

namespace hlab {

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Party::Party() : AppBase() {}

bool Party::InitScene() {


    AppBase::m_globalConstsCPU.strengthIBL = 1.0f;

    AppBase::m_camera.Reset(Vector3(3.74966f, 5.03645f, -2.54918f), -0.819048f,
                            0.741502f);

    AppBase::InitCubemaps(
        L"../Assets/Textures/Cubemaps/HDRI/", L"clear_pureskyEnvHDR.dds",
        L"clear_pureskySpecularHDR.dds", L"clear_pureskyDiffuseHDR.dds",
        L"clear_pureskyBrdf.dds", 0);

    AppBase::InitScene();

    // 조명 설정
    {
        // 조명 0은 고정
        m_globalConstsCPU.lights[0].radiance = Vector3(5.0f);
        m_globalConstsCPU.lights[0].position = Vector3(0.0f, 2.0f, 4.0f);

        m_globalConstsCPU.lights[0].direction = Vector3(0.2f, -1.0f, 0.0f);
        m_globalConstsCPU.lights[0].spotPower = 5.0f;
        m_globalConstsCPU.lights[0].radius = 0.1f;
        m_globalConstsCPU.lights[0].haloRadius = 2.f;
        m_globalConstsCPU.lights[0].haloStrength = 10.f;
        m_globalConstsCPU.lights[0].lightColor = Vector3(1.f, 0.f, 0.f);

        m_globalConstsCPU.lights[0].type = LIGHT_SPOT | LIGHT_SHADOW;
        // m_globalConstsCPU.lights[0].type = LIGHT_OFF;

        m_globalConstsCPU.lights[1].position = Vector3(0.0f, 2.0f, 4.0f);
        m_globalConstsCPU.lights[1].spotPower = 5.f;
        m_globalConstsCPU.lights[1].lightColor = Vector3(0.f, 1.f, 0.f);
        m_globalConstsCPU.lights[1].type = LIGHT_SPOT | LIGHT_SHADOW;

        m_globalConstsCPU.lights[2].position = Vector3(0.0f, 2.0f, 4.0f);
        m_globalConstsCPU.lights[2].spotPower = 5.f;
        m_globalConstsCPU.lights[2].lightColor = Vector3(0.f, 0.f, 1.f);
        m_globalConstsCPU.lights[2].type = LIGHT_SPOT | LIGHT_SHADOW;
        //m_globalConstsCPU.lights[2].type = LIGHT_OFF;
    }

    // 바닥(거울)
    {
        // https://freepbr.com/materials/stringy-marble-pbr/
        auto mesh = GeometryGenerator::MakeSquare(5.0);
        string path = "../Assets/Textures/PBR/stringy-marble-ue/";
        mesh.albedoTextureFilename = path + "stringy_marble_albedo.png";
        mesh.emissiveTextureFilename = "";
        mesh.aoTextureFilename = path + "stringy_marble_ao.png";
        mesh.metallicTextureFilename = path + "stringy_marble_Metallic.png";
        mesh.normalTextureFilename = path + "stringy_marble_Normal-dx.png";
        mesh.roughnessTextureFilename = path + "stringy_marble_Roughness.png";

        m_ground = make_shared<Model>(m_device, m_context, vector{mesh});
        m_ground->m_materialConsts.GetCpu().albedoFactor = Vector3(0.7f);
        m_ground->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
        m_ground->m_materialConsts.GetCpu().metallicFactor = 0.5f;
        m_ground->m_materialConsts.GetCpu().roughnessFactor = 0.3f;

        Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
        m_ground->UpdateWorldRow(Matrix::CreateRotationX(3.141592f * 0.5f) *
                                 Matrix::CreateTranslation(position));
        m_ground->m_castShadow = false; // 바닥은 그림자 만들기 생략

        // m_mirrorPlane = SimpleMath::Plane(position, Vector3(0.0f, 1.0f,
        // 0.0f)); m_mirror = m_ground; // 바닥에 거울처럼 반사 구현

        std::shared_ptr<Plane> mirrorPlane = std::make_shared<Plane>(
            SimpleMath::Plane(position, Vector3(0.f, 1.f, 0.f)));

        m_mirrorList.insert(std::make_pair(m_ground, mirrorPlane));

        m_mirror = m_ground;

        // m_basicList.push_back(m_ground); // 거울은 리스트에 등록 X
    }

    // Main Object
    //{
    //    string path = "../Assets/Characters/Mixamo/";
    //    vector<string> clipNames = {"CatwalkIdle.fbx",
    //                                "CatwalkIdleToWalkForward.fbx",
    //                                "CatwalkWalkForward.fbx",
    //                                "CatwalkWalkStop.fbx",
    //                                "Hip Hop Dancing.fbx",
    //                                "Dying.fbx",
    //                                ""};

    //    AnimationData aniData;

    //    auto [meshes, _] =
    //        GeometryGenerator::ReadAnimationFromFile(path, "character.fbx");

    //    std::cout << meshes.size() << '\n';

    //    for (auto &name : clipNames) {
    //        auto [_, ani] =
    //            GeometryGenerator::ReadAnimationFromFile(path, name);

    //        if (aniData.clips.empty()) {
    //            aniData = ani;
    //        } else {
    //            aniData.clips.push_back(ani.clips.front());
    //        }
    //    }

    //    {
    //        Vector3 center(0.0f, 0.5f, 2.0f);
    //        // m_characterArr[0] =
    //        //     make_shared<SkinnedMeshModel>(m_device, m_context, meshes,
    //        //     aniData);
    //        m_characterArr.push_back(make_shared<SkinnedMeshModel>(
    //            m_device, m_context, meshes, aniData));

    //        m_characterArr.back()->m_materialConsts.GetCpu().albedoFactor =
    //            Vector3(1.0f);
    //        m_characterArr.back()->m_materialConsts.GetCpu().roughnessFactor =
    //            0.8f;
    //        m_characterArr.back()->m_materialConsts.GetCpu().metallicFactor =
    //            0.0f;
    //        m_characterArr.back()->UpdateWorldRow(
    //            Matrix::CreateScale(1.0f) *
    //            Matrix::CreateTranslation(center));

    //        m_basicList.push_back(m_characterArr.back()); // 리스트에 등록
    //        m_basicListMap.insert(
    //            std::make_pair("Mixamo", m_characterArr.back()));
    //        m_pickedModel = m_characterArr.back();
    //    }
    //}

    // Mixamo Dance
    {
        //string path = "../Assets/Characters/Mixamo/";
        string path = basePath + "Characters/Mixamo/";

        vector<string> clipNames = {"Hip Hop Dancing.fbx"};

        AnimationData aniData;

        auto [meshes, _] =
            GeometryGenerator::ReadAnimationFromFile(path, "character.fbx");

        std::cout << meshes.size() << '\n';

        for (auto &name : clipNames) {
            auto [_, ani] =
                GeometryGenerator::ReadAnimationFromFile(path, name);

            if (aniData.clips.empty()) {
                aniData = ani;
            } else {
                aniData.clips.push_back(ani.clips.front());
            }
        }

        {
            Vector3 center(0.0f, 0.5f, 2.0f);
            // m_characterArr[0] =
            //     make_shared<SkinnedMeshModel>(m_device, m_context, meshes,
            //     aniData);
            m_characterArr.push_back(make_shared<SkinnedMeshModel>(
                m_device, m_context, meshes, aniData));

            m_characterArr.back()->m_materialConsts.GetCpu().albedoFactor =
                Vector3(1.0f);
            m_characterArr.back()->m_materialConsts.GetCpu().roughnessFactor =
                0.8f;
            m_characterArr.back()->m_materialConsts.GetCpu().metallicFactor =
                0.0f;
            m_characterArr.back()->UpdateWorldRow(
                Matrix::CreateScale(1.0f) * Matrix::CreateTranslation(center));

            m_basicList.push_back(m_characterArr.back()); // 리스트에 등록
            m_basicListMap.insert(
                std::make_pair("Mixamo", m_characterArr.back()));
            m_pickedModel = m_characterArr.back();
        }
    }

    // Amy
    {
        //string path = "../Assets/Characters/Amy/";
        string path = basePath + "Characters/Amy/";

        vector<string> clipNames = {"Silly Dancing.fbx"};

        AnimationData aniData;

        auto [meshes, _] =
            GeometryGenerator::ReadAnimationFromFile(path, "character.fbx");

        std::cout << meshes.size() << '\n';

        for (auto &name : clipNames) {
            auto [_, ani] =
                GeometryGenerator::ReadAnimationFromFile(path, name);

            if (aniData.clips.empty()) {
                aniData = ani;
            } else {
                aniData.clips.push_back(ani.clips.front());
            }
        }

        {
            Vector3 center(2.0f, 0.5f, 2.0f);
            // m_characterArr[0] =
            //     make_shared<SkinnedMeshModel>(m_device, m_context, meshes,
            //     aniData);
            m_characterArr.push_back(make_shared<SkinnedMeshModel>(
                m_device, m_context, meshes, aniData));

            m_characterArr.back()->m_materialConsts.GetCpu().albedoFactor =
                Vector3(1.0f);
            m_characterArr.back()->m_materialConsts.GetCpu().roughnessFactor =
                0.8f;
            m_characterArr.back()->m_materialConsts.GetCpu().metallicFactor =
                0.0f;
            m_characterArr.back()->UpdateWorldRow(
                Matrix::CreateScale(1.0f) * Matrix::CreateTranslation(center));

            m_basicList.push_back(m_characterArr.back()); // 리스트에 등록
            m_basicListMap.insert(
                std::make_pair("Mixamo", m_characterArr.back()));
            m_pickedModel = m_characterArr.back();
        }
    }

    // Arissa
    {
        //string path = "../Assets/Characters/Arissa/";
        string path = basePath + "Characters/Arissa/";

        vector<string> clipNames = {"Bboy Hip Hop Move.fbx"};

        AnimationData aniData;

        auto [meshes, _] =
            GeometryGenerator::ReadAnimationFromFile(path, "character.fbx");

        std::cout << meshes.size() << '\n';

        for (auto &name : clipNames) {
            auto [_, ani] =
                GeometryGenerator::ReadAnimationFromFile(path, name);

            if (aniData.clips.empty()) {
                aniData = ani;
            } else {
                aniData.clips.push_back(ani.clips.front());
            }
        }

        {
            Vector3 center(1.0f, 0.5f, 2.0f);
            // m_characterArr[0] =
            //     make_shared<SkinnedMeshModel>(m_device, m_context, meshes,
            //     aniData);
            m_characterArr.push_back(make_shared<SkinnedMeshModel>(
                m_device, m_context, meshes, aniData));

            m_characterArr.back()->m_materialConsts.GetCpu().albedoFactor =
                Vector3(1.0f);
            m_characterArr.back()->m_materialConsts.GetCpu().roughnessFactor =
                0.8f;
            m_characterArr.back()->m_materialConsts.GetCpu().metallicFactor =
                0.0f;
            m_characterArr.back()->UpdateWorldRow(
                Matrix::CreateScale(1.0f) * Matrix::CreateTranslation(center));

            m_basicList.push_back(m_characterArr.back()); // 리스트에 등록
            m_basicListMap.insert(
                std::make_pair("Mixamo", m_characterArr.back()));
            m_pickedModel = m_characterArr.back();
        }
    }

    // Kachujin
    {
        //string path = "../Assets/Characters/Kachujin/";
        string path = basePath + "Characters/kachujin/";

        vector<string> clipNames = {"Rumba Dancing.fbx"};

        AnimationData aniData;

        auto [meshes, _] =
            GeometryGenerator::ReadAnimationFromFile(path, "character.fbx");

        std::cout << meshes.size() << '\n';

        for (auto &name : clipNames) {
            auto [_, ani] =
                GeometryGenerator::ReadAnimationFromFile(path, name);

            if (aniData.clips.empty()) {
                aniData = ani;
            } else {
                aniData.clips.push_back(ani.clips.front());
            }
        }

        {
            Vector3 center(-1.0f, 0.5f, 2.0f);
            // m_characterArr[0] =
            //     make_shared<SkinnedMeshModel>(m_device, m_context, meshes,
            //     aniData);
            m_characterArr.push_back(make_shared<SkinnedMeshModel>(
                m_device, m_context, meshes, aniData));

            m_characterArr.back()->m_materialConsts.GetCpu().albedoFactor =
                Vector3(1.0f);
            m_characterArr.back()->m_materialConsts.GetCpu().roughnessFactor =
                0.8f;
            m_characterArr.back()->m_materialConsts.GetCpu().metallicFactor =
                0.0f;
            m_characterArr.back()->UpdateWorldRow(
                Matrix::CreateScale(1.0f) * Matrix::CreateTranslation(center));

            m_basicList.push_back(m_characterArr.back()); // 리스트에 등록
            m_basicListMap.insert(
                std::make_pair("Mixamo", m_characterArr.back()));
            m_pickedModel = m_characterArr.back();
        }
    }

    // Boss
    {
        //string path = "../Assets/Characters/Boss/";
        string path = basePath + "Characters/Boss/";

        vector<string> clipNames = {"Swing Dancing.fbx"};

        AnimationData aniData;

        auto [meshes, _] =
            GeometryGenerator::ReadAnimationFromFile(path, "character.fbx");

        std::cout << meshes.size() << '\n';

        for (auto &name : clipNames) {
            auto [_, ani] =
                GeometryGenerator::ReadAnimationFromFile(path, name);

            if (aniData.clips.empty()) {
                aniData = ani;
            } else {
                aniData.clips.push_back(ani.clips.front());
            }
        }

        {
            Vector3 center(-1.0f, 0.5f, 3.0f);
            // m_characterArr[0] =
            //     make_shared<SkinnedMeshModel>(m_device, m_context, meshes,
            //     aniData);
            m_characterArr.push_back(make_shared<SkinnedMeshModel>(
                m_device, m_context, meshes, aniData));

            m_characterArr.back()->m_materialConsts.GetCpu().albedoFactor =
                Vector3(1.0f);
            m_characterArr.back()->m_materialConsts.GetCpu().roughnessFactor =
                0.8f;
            m_characterArr.back()->m_materialConsts.GetCpu().metallicFactor =
                0.0f;
            m_characterArr.back()->UpdateWorldRow(
                Matrix::CreateScale(1.0f) * Matrix::CreateTranslation(center));

            m_basicList.push_back(m_characterArr.back()); // 리스트에 등록
            m_basicListMap.insert(
                std::make_pair("Mixamo", m_characterArr.back()));
            m_pickedModel = m_characterArr.back();
        }
    }

    // Maria
    {
        //string path = "../Assets/Characters/Maria/";
        string path = basePath + "Characters/Maria/";

        vector<string> clipNames = {"Hip Hop Dancing.fbx"};

        AnimationData aniData;

        auto [meshes, _] =
            GeometryGenerator::ReadAnimationFromFile(path, "character.fbx");

        std::cout << meshes.size() << '\n';

        for (auto &name : clipNames) {
            auto [_, ani] =
                GeometryGenerator::ReadAnimationFromFile(path, name);

            if (aniData.clips.empty()) {
                aniData = ani;
            } else {
                aniData.clips.push_back(ani.clips.front());
            }
        }

        {
            Vector3 center(0.0f, 0.5f, 3.0f);
            // m_characterArr[0] =
            //     make_shared<SkinnedMeshModel>(m_device, m_context, meshes,
            //     aniData);
            m_characterArr.push_back(make_shared<SkinnedMeshModel>(
                m_device, m_context, meshes, aniData));

            m_characterArr.back()->m_materialConsts.GetCpu().albedoFactor =
                Vector3(1.0f);
            m_characterArr.back()->m_materialConsts.GetCpu().roughnessFactor =
                0.8f;
            m_characterArr.back()->m_materialConsts.GetCpu().metallicFactor =
                0.0f;
            m_characterArr.back()->UpdateWorldRow(
                Matrix::CreateScale(1.0f) * Matrix::CreateTranslation(center));

            m_basicList.push_back(m_characterArr.back()); // 리스트에 등록
            m_basicListMap.insert(
                std::make_pair("Mixamo", m_characterArr.back()));
            m_pickedModel = m_characterArr.back();
        }
    }

    // Erika
    {
        //string path = "../Assets/Characters/Erika/";
        string path = basePath + "Characters/Erika/";

        vector<string> clipNames = {"Samba Dancing.fbx"};

        AnimationData aniData;

        auto [meshes, _] =
            GeometryGenerator::ReadAnimationFromFile(path, "character.fbx");

        std::cout << meshes.size() << '\n';

        for (auto &name : clipNames) {
            auto [_, ani] =
                GeometryGenerator::ReadAnimationFromFile(path, name);

            if (aniData.clips.empty()) {
                aniData = ani;
            } else {
                aniData.clips.push_back(ani.clips.front());
            }
        }

        {
            Vector3 center(1.0f, 0.5f, 3.0f);
            // m_characterArr[0] =
            //     make_shared<SkinnedMeshModel>(m_device, m_context, meshes,
            //     aniData);
            m_characterArr.push_back(make_shared<SkinnedMeshModel>(
                m_device, m_context, meshes, aniData));

            m_characterArr.back()->m_materialConsts.GetCpu().albedoFactor =
                Vector3(1.0f);
            m_characterArr.back()->m_materialConsts.GetCpu().roughnessFactor =
                0.8f;
            m_characterArr.back()->m_materialConsts.GetCpu().metallicFactor =
                0.0f;
            m_characterArr.back()->UpdateWorldRow(
                Matrix::CreateScale(1.0f) * Matrix::CreateTranslation(center));

            m_basicList.push_back(m_characterArr.back()); // 리스트에 등록
            m_basicListMap.insert(
                std::make_pair("Mixamo", m_characterArr.back()));
            m_pickedModel = m_characterArr.back();
        }
    }

    // Eve
    //{
    //    string path = basePath + "Characters/Eve/";
    //    vector<string> clipNames = {"Dancing Maraschino Step.fbx"};

    //    AnimationData aniData;

    //    auto [meshes, _] =
    //        GeometryGenerator::ReadAnimationFromFile(path, "character.fbx");

    //    std::cout << meshes.size() << '\n';

    //    for (auto &name : clipNames) {
    //        auto [_, ani] =
    //            GeometryGenerator::ReadAnimationFromFile(path, name);

    //        if (aniData.clips.empty()) {
    //            aniData = ani;
    //        } else {
    //            aniData.clips.push_back(ani.clips.front());
    //        }
    //    }

    //    {
    //        Vector3 center(2.0f, 0.5f, 3.0f);
    //        // m_characterArr[0] =
    //        //     make_shared<SkinnedMeshModel>(m_device, m_context, meshes,
    //        //     aniData);
    //        m_characterArr.push_back(make_shared<SkinnedMeshModel>(
    //            m_device, m_context, meshes, aniData));

    //        m_characterArr.back()->m_materialConsts.GetCpu().albedoFactor =
    //            Vector3(1.0f);
    //        m_characterArr.back()->m_materialConsts.GetCpu().roughnessFactor =
    //            0.8f;
    //        m_characterArr.back()->m_materialConsts.GetCpu().metallicFactor =
    //            0.0f;
    //        m_characterArr.back()->UpdateWorldRow(
    //            Matrix::CreateScale(1.0f) * Matrix::CreateTranslation(center));

    //        m_basicList.push_back(m_characterArr.back()); // 리스트에 등록
    //        m_basicListMap.insert(
    //            std::make_pair("Mixamo", m_characterArr.back()));
    //        m_pickedModel = m_characterArr.back();
    //    }
    //}

    // Club Atomic: Where the holograms go.
    {
        // https://freepbr.com/materials/stringy-marble-pbr/
        //string basePath =
        //    "../Assets/3DModel/club_atomic_where_the_holograms_go/";
        string path = basePath + "3DModel/club_atomic_where_the_holograms_go/";

        auto mesh = GeometryGenerator::ReadFromFile(path, "scene.gltf");
        // mesh.albedoTextureFilename = path + "stringy_marble_albedo.png";
        // mesh.emissiveTextureFilename = "";
        // mesh.aoTextureFilename = path + "stringy_marble_ao.png";
        // mesh.metallicTextureFilename = path + "stringy_marble_Metallic.png";
        // mesh.normalTextureFilename = path + "stringy_marble_Normal-dx.png";
        // mesh.roughnessTextureFilename = path +
        // "stringy_marble_Roughness.png";

        m_ground = make_shared<Model>(m_device, m_context, vector{mesh});
        m_ground->m_materialConsts.GetCpu().albedoFactor = Vector3(0.7f);
        m_ground->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
        m_ground->m_materialConsts.GetCpu().metallicFactor = 0.5f;
        m_ground->m_materialConsts.GetCpu().roughnessFactor = 0.3f;

        Vector3 position = Vector3(-1.0f, 1.1f, 4.0f);
        m_ground->UpdateWorldRow(
            Matrix::CreateScale(
                10.f) * /*Matrix::CreateRotationX(3.141592f * 0.5f) **/
            Matrix::CreateTranslation(position));
        m_ground->m_castShadow = false; // 바닥은 그림자 만들기 생략
        m_ground->m_isPickable = true;

        std::shared_ptr<Plane> mirrorPlane = std::make_shared<Plane>(
            SimpleMath::Plane(position, Vector3(0.f, 1.f, 0.f)));

        m_basicList.push_back(m_ground); // 거울은 리스트에 등록 X
    }

    // beautiful-sphere
    {
        // https://freepbr.com/materials/stringy-marble-pbr/
        //string basePath = "../Assets/Models/beautiful-sphere/";
        string path = basePath + "Models/beautiful-sphere/";

        auto mesh = GeometryGenerator::ReadFromFile(path, "sphere.fbx");
        // mesh.albedoTextureFilename = path + "stringy_marble_albedo.png";
        // mesh.emissiveTextureFilename = "";
        // mesh.aoTextureFilename = path + "stringy_marble_ao.png";
        // mesh.metallicTextureFilename = path + "stringy_marble_Metallic.png";
        // mesh.normalTextureFilename = path + "stringy_marble_Normal-dx.png";
        // mesh.roughnessTextureFilename = path +
        // "stringy_marble_Roughness.png";

        m_ground = make_shared<Model>(m_device, m_context, vector{mesh});
        m_ground->m_materialConsts.GetCpu().albedoFactor = Vector3(0.7f);
        m_ground->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
        m_ground->m_materialConsts.GetCpu().metallicFactor = 0.5f;
        m_ground->m_materialConsts.GetCpu().roughnessFactor = 0.3f;

        Vector3 position = Vector3(0.0f, 2.0f, 0.0f);
        m_ground->UpdateWorldRow(
            Matrix::CreateScale(
                1.f) * /*Matrix::CreateRotationX(3.141592f * 0.5f) **/
            Matrix::CreateTranslation(position));
        m_ground->m_castShadow = true; // 바닥은 그림자 만들기 생략
        m_ground->m_isPickable = true;

        m_basicList.push_back(m_ground); // 거울은 리스트에 등록 X
        m_basicListMap.insert(std::pair("BeautifulSphere", m_ground));
    }

    //// Night Club
    //{
    //    // https://freepbr.com/materials/stringy-marble-pbr/
    //    string basePath = "../Assets/3DModel/night-club/";
    //    auto mesh = GeometryGenerator::ReadFromFile(basePath,
    //    "Sketchfab_4.fbx");
    //    // mesh.albedoTextureFilename = path + "stringy_marble_albedo.png";
    //    // mesh.emissiveTextureFilename = "";
    //    // mesh.aoTextureFilename = path + "stringy_marble_ao.png";
    //    // mesh.metallicTextureFilename = path +
    //    "stringy_marble_Metallic.png";
    //    // mesh.normalTextureFilename = path + "stringy_marble_Normal-dx.png";
    //    // mesh.roughnessTextureFilename = path +
    //    // "stringy_marble_Roughness.png";

    //    m_ground = make_shared<Model>(m_device, m_context, vector{mesh});
    //    m_ground->m_materialConsts.GetCpu().albedoFactor = Vector3(0.7f);
    //    m_ground->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
    //    m_ground->m_materialConsts.GetCpu().metallicFactor = 0.5f;
    //    m_ground->m_materialConsts.GetCpu().roughnessFactor = 0.3f;

    //    Vector3 position = Vector3(1.0f, 1.1f, 4.0f);
    //    m_ground->UpdateWorldRow(
    //        Matrix::CreateScale(
    //            3.f) * /*Matrix::CreateRotationX(3.141592f * 0.5f) **/
    //        Matrix::CreateTranslation(position));
    //    m_ground->m_castShadow = false; // 바닥은 그림자 만들기 생략

    //    m_basicList.push_back(m_ground); // 거울은 리스트에 등록 X
    //}

    // 80s-warehouse
    //{
    //    // https://freepbr.com/materials/stringy-marble-pbr/
    //    string basePath =
    //        "../Assets/3DModel/warehouse/";
    //    auto mesh = GeometryGenerator::ReadFromFile(basePath, "80sHub.fbx");
    //    // mesh.albedoTextureFilename = path + "stringy_marble_albedo.png";
    //    // mesh.emissiveTextureFilename = "";
    //    // mesh.aoTextureFilename = path + "stringy_marble_ao.png";
    //    // mesh.metallicTextureFilename = path +
    //    "stringy_marble_Metallic.png";
    //    // mesh.normalTextureFilename = path + "stringy_marble_Normal-dx.png";
    //    // mesh.roughnessTextureFilename = path +
    //    // "stringy_marble_Roughness.png";

    //    m_ground = make_shared<Model>(m_device, m_context, vector{mesh});
    //    m_ground->m_materialConsts.GetCpu().albedoFactor = Vector3(0.7f);
    //    m_ground->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
    //    m_ground->m_materialConsts.GetCpu().metallicFactor = 0.5f;
    //    m_ground->m_materialConsts.GetCpu().roughnessFactor = 0.3f;

    //    Vector3 position = Vector3(-1.0f, 1.1f, 4.0f);
    //    m_ground->UpdateWorldRow(
    //        Matrix::CreateScale(
    //            10.f) * /*Matrix::CreateRotationX(3.141592f * 0.5f) **/
    //        Matrix::CreateTranslation(position));
    //    m_ground->m_castShadow = false; // 바닥은 그림자 만들기 생략
    //    m_ground->m_isPickable = true;

    //    m_basicList.push_back(m_ground); // 거울은 리스트에 등록 X
    //}


    return true;
}

void Party::UpdateLights(float dt) {
    // AppBase::UpdateLights(dt);

    // light 0
    m_globalConstsCPU.lights[1].position = Vector3(0.f, 2.f, 0.f);

    static Vector3 focusPosition = Vector3(-1.f, -1.f, 1.f);
    focusPosition = Vector3::Transform(
        focusPosition, Matrix::CreateRotationY(dt * 3.14 * 0.5f));

    m_globalConstsCPU.lights[1].direction =
        focusPosition - m_globalConstsCPU.lights[1].position;

    m_globalConstsCPU.lights[1].direction.Normalize();

    // light 1
    m_globalConstsCPU.lights[0].position = Vector3(0.f, 2.f, 0.f);

    static Vector3 focusPosition2 = Vector3(1.f, -1.f, 1.f);
    focusPosition2 = Vector3::Transform(
        focusPosition2, Matrix::CreateRotationY(dt * 3.14 * 0.5f));

    m_globalConstsCPU.lights[0].direction =
        focusPosition2 - m_globalConstsCPU.lights[1].position;

    m_globalConstsCPU.lights[0].direction.Normalize();

    // light 2
    m_globalConstsCPU.lights[2].position = Vector3(0.f, 2.f, 0.f);

    static Vector3 focusPosition3 = Vector3(1.f, -1.f, -1.f);
    focusPosition3 = Vector3::Transform(
        focusPosition3, Matrix::CreateRotationY(dt * 3.14 * 0.5f));

    m_globalConstsCPU.lights[2].direction =
        focusPosition3 - m_globalConstsCPU.lights[2].position;

    m_globalConstsCPU.lights[2].direction.Normalize();

    // 그림자맵을 만들기 위한 시점
    for (int i = 0; i < MAX_LIGHTS; i++) {
        auto &light = m_globalConstsCPU.lights[i];
        if (light.type & LIGHT_SHADOW) {

            Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
            if (abs(up.Dot(light.direction) + 1.0f) < 1e-5)
                up = Vector3(1.0f, 0.0f, 0.0f);

            // 그림자맵을 만들 때 필요
            // Vector3 focusPos = m_basicListMap.find("MainSphere")
            //                       ->second->m_worldRow.Translation();
            // light.direction = focusPos;
            // light.direction.Normalize();
            Matrix lightViewRow = XMMatrixLookAtLH(
                light.position, light.position + light.direction, up);

            Matrix lightProjRow = XMMatrixPerspectiveFovLH(
                XMConvertToRadians(120.0f), 1.0f, 0.1f, 20.0f);

            m_shadowGlobalConstsCPU[i].eyeWorld = light.position;
            m_shadowGlobalConstsCPU[i].view = lightViewRow.Transpose();
            m_shadowGlobalConstsCPU[i].proj = lightProjRow.Transpose();
            m_shadowGlobalConstsCPU[i].invProj =
                lightProjRow.Invert().Transpose();
            m_shadowGlobalConstsCPU[i].viewProj =
                (lightViewRow * lightProjRow).Transpose();

            // LIGHT_FRUSTUM_WIDTH 확인
            // Vector4 eye(0.0f, 0.0f, 0.0f, 1.0f);
            // Vector4 xLeft(-1.0f, -1.0f, 0.0f, 1.0f);
            // Vector4 xRight(1.0f, 1.0f, 0.0f, 1.0f);
            // eye = Vector4::Transform(eye, lightProjRow);
            // xLeft = Vector4::Transform(xLeft, lightProjRow.Invert());
            // xRight = Vector4::Transform(xRight, lightProjRow.Invert());
            // xLeft /= xLeft.w;
            // xRight /= xRight.w;
            // cout << "LIGHT_FRUSTUM_WIDTH = " << xRight.x - xLeft.x <<
            // endl;

            D3D11Utils::UpdateBuffer(m_context, m_shadowGlobalConstsCPU[i],
                                     m_shadowGlobalConstsGPU[i]);

            // 그림자를 실제로 렌더링할 때 필요
            m_globalConstsCPU.lights[i].viewProj =
                m_shadowGlobalConstsCPU[i].viewProj;
            m_globalConstsCPU.lights[i].invProj =
                m_shadowGlobalConstsCPU[i].invProj;

            // 반사된 장면에서도 그림자를 그리고 싶다면 조명도 반사시켜서
            // 넣어주면 됩니다.
        }
    }
}

void Party::Update(float dt) {
    AppBase::Update(dt);

    assert(m_characterArr.size() < 10);

    static float frameCount[10] = {0.f};
    static int state[10] = {0};
    static bool isRun[10] = {false};

    for (size_t i = 0; i < m_characterArr.size(); i++) {
        // UpdateAnimationMove(m_characterArr[i], dt, frameCount[i], state[i],
        //                     isRun[i]);

        UpdateAnimationDance(m_characterArr[i], dt, frameCount[i], state[i],
                             isRun[i]);
    }

	auto m = m_basicListMap.find("BeautifulSphere");

	if (m != m_basicListMap.end()) {
        m->second->UpdateWorldRow(Matrix::CreateRotationY(dt) *
                                  m->second->m_worldRow);
	}
}

void Party::Render() {
    AppBase::Render();
    AppBase::PostRender();
}

void Party::UpdateAnimationMove(
    const std::shared_ptr<SkinnedMeshModel> &character, float dt,
    float &frameCount, int &state, bool &isRun) {
    // static float frameCount = 0;

    // States
    // 0: idle
    // 1: idle to walk
    // 2: walk forward
    // 3: walk to stop
    // 4: dance

    // static int state = 0;
    // static bool isRun = false;

    // TODO:
    // 간단한 모션 그래프 구현
    // "Motion Graphs" by Kovar et al. ACM SIGGRAPH 2002

    // 힌트:
    // if (AppBase::m_keyPressed[VK_UP]) , VK_RIGHT, VK_LEFT, VK_UP
    // if (frameCount == m_character->m_aniData.clips[state].keys[0].size())
    // m_character->m_aniData.accumulatedRootTransform =
    //     Matrix::CreateRotationY(3.141592f * 60.0f / 180.0f * dt) *
    //     m_character->m_aniData.accumulatedRootTransform

    // 주의: frameCount = 0;

    std::cout << state << '\n';
    std::cout << frameCount << '\n';

    if (state == 0) { // 정지 상태
        // TODO:

        if (AppBase::m_keyPressed[VK_UP]) {
            state = 1;
            frameCount = 0;
        } else if (frameCount >=
                       character->m_aniData.clips[state].keys[0].size() ||
                   AppBase::m_keyPressed[VK_UP]) {
            frameCount = 0; // 상태 변화 없이 반복
        }
    } else if (state == 1) { // 걷기 시작

        if (frameCount >= character->m_aniData.clips[state].keys[0].size()) {
            state = 2;
            frameCount = 0;
        }
    } else if (state == 2) { // 걸어가기

        if (AppBase::m_keyPressed[VK_SHIFT]) {
            isRun = true;
        } else {
            isRun = false;
        }

        if (AppBase::m_keyPressed[VK_RIGHT]) { // 걸으면서 회전
            character->m_aniData.accumulatedRootTransform =
                Matrix::CreateRotationY(3.141592f * 160.f / 180.f * dt) *
                character->m_aniData.accumulatedRootTransform;
        }

        if (AppBase::m_keyPressed[VK_LEFT]) {
            character->m_aniData.accumulatedRootTransform =
                Matrix::CreateRotationY(-3.141592f * 160.f / 180.f * dt) *
                character->m_aniData.accumulatedRootTransform;
        }

        if (frameCount >= character->m_aniData.clips[state].keys[0].size()) {
            // 방향키를 누르고 있지 않으면 정지 (누르고 있으면 계속 걷기)
            if (!AppBase::m_keyPressed[VK_UP]) {
                state = 3;
            }
            frameCount = 0;
        }
    } else if (state == 3) {
        if (frameCount >= character->m_aniData.clips[state].keys[0].size()) {
            // state = 4;
            state = 0;
            frameCount = 0;
        }
    }

    if (state == 0 && AppBase::m_keyPressed[VK_SPACE]) {
        state = 4;
        frameCount = 0;
    } else if (frameCount >= character->m_aniData.clips[state].keys[0].size()) {
        state = 0;
        frameCount = 0;
    }

    if (state == 0 && AppBase::m_keyPressed['R']) {
        state = 5;
        frameCount = 0;
    } else if (frameCount >= character->m_aniData.clips[state].keys[0].size()) {
        state = 0;
        frameCount = 0;
    }

    character->UpdateAnimation(m_context, state, frameCount);

    if ((!isRun && state == 2) || state == 4 || state == 5) {
        frameCount += 0.5f;
    } else {
        frameCount += 1;
    }
}

void Party::UpdateAnimationDance(
    const std::shared_ptr<SkinnedMeshModel> &character, float dt,
    float &frameCount, int &state, bool &isRun) {

     //std::cout << state << '\n';
     //std::cout << frameCount << '\n';
     //std::cout << dt << '\n';

    if (state == 0) { // 정지 상태
        // TODO:

        if (frameCount >= character->m_aniData.clips[state].keys[0].size()) {
            frameCount = 0; // 상태 변화 없이 반복
        }
    }

    // if (state == 0) {
    //     state = 1;
    //     frameCount = 0;
    // } else if (frameCount >=
    // character->m_aniData.clips[state].keys[0].size()) {
    //     state = 0;
    //     frameCount = 0;
    // }

    // if (state == 0 && AppBase::m_keyPressed['R']) {
    //     state = 5;
    //     frameCount = 0;
    // } else if (frameCount >=
    // character->m_aniData.clips[state].keys[0].size()) {
    //     state = 0;
    //     frameCount = 0;
    // }

    character->UpdateAnimation(m_context, state, frameCount);

    frameCount += 60.f * dt;
}

void Party::UpdateGUI() {

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("General")) {
        ImGui::Checkbox("Use FPV", &m_camera.m_useFirstPersonView);
        ImGui::Checkbox("Wireframe", &m_drawAsWire);
        ImGui::Checkbox("DrawOBB", &m_drawOBB);
        ImGui::Checkbox("DrawBSphere", &m_drawBS);
        if (ImGui::Checkbox("MSAA ON", &m_useMSAA)) {
            CreateBuffers();
        }
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Skybox")) {
        ImGui::SliderFloat("Strength", &m_globalConstsCPU.strengthIBL, 0.0f,
                           0.5f);
        ImGui::RadioButton("Env", &m_globalConstsCPU.textureToDraw, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Specular", &m_globalConstsCPU.textureToDraw, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Irradiance", &m_globalConstsCPU.textureToDraw, 2);
        ImGui::SliderFloat("EnvLodBias", &m_globalConstsCPU.envLodBias, 0.0f,
                           10.0f);
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Post Effects")) {
        int flag = 0;
        flag += ImGui::RadioButton("Render", &m_postEffectsConstsCPU.mode, 1);
        ImGui::SameLine();
        flag += ImGui::RadioButton("Depth", &m_postEffectsConstsCPU.mode, 2);
        flag += ImGui::SliderFloat(
            "DepthScale", &m_postEffectsConstsCPU.depthScale, 0.0, 1.0);
        flag += ImGui::SliderFloat("Fog", &m_postEffectsConstsCPU.fogStrength,
                                   0.0, 10.0);

        if (flag)
            D3D11Utils::UpdateBuffer(m_context, m_postEffectsConstsCPU,
                                     m_postEffectsConstsGPU);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Post Processing")) {
        int flag = 0;
        flag += ImGui::SliderFloat(
            "Bloom Strength",
            &m_postProcess.m_combineFilter.m_constData.strength, 0.0f, 1.0f);
        flag += ImGui::SliderFloat(
            "Exposure", &m_postProcess.m_combineFilter.m_constData.option1,
            0.0f, 10.0f);
        flag += ImGui::SliderFloat(
            "Gamma", &m_postProcess.m_combineFilter.m_constData.option2, 0.1f,
            5.0f);
        // 편의상 사용자 입력이 인식되면 바로 GPU 버퍼를 업데이트
        if (flag) {
            m_postProcess.m_combineFilter.UpdateConstantBuffers(m_context);
        }
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Mirror")) {

        ImGui::SliderFloat("Alpha", &m_mirrorAlpha, 0.0f, 1.0f);
        const float blendColor[4] = {m_mirrorAlpha, m_mirrorAlpha,
                                     m_mirrorAlpha, 1.0f};
        if (m_drawAsWire)
            Graphics::mirrorBlendWirePSO.SetBlendFactor(blendColor);
        else
            Graphics::mirrorBlendSolidPSO.SetBlendFactor(blendColor);

        ImGui::SliderFloat("Metallic",
                           &m_mirror->m_materialConsts.GetCpu().metallicFactor,
                           0.0f, 1.0f);
        ImGui::SliderFloat("Roughness",
                           &m_mirror->m_materialConsts.GetCpu().roughnessFactor,
                           0.0f, 1.0f);

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Light")) {
        // ImGui::SliderFloat3("Position",
        // &m_globalConstsCPU.lights[0].position.x,
        //                     -5.0f, 5.0f);
        ImGui::SliderFloat("Halo Radius",
                           &m_globalConstsCPU.lights[1].haloRadius, 0.0f, 2.0f);
        ImGui::SliderFloat("Halo Strength",
                           &m_globalConstsCPU.lights[1].haloStrength, 0.0f,
                           1.0f);
        ImGui::SliderFloat("Radius", &m_globalConstsCPU.lights[1].radius, 0.0f,
                           0.5f);
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Material")) {
        ImGui::SliderFloat("LodBias", &m_globalConstsCPU.lodBias, 0.0f, 10.0f);

        int flag = 0;

        if (m_pickedModel) {
            flag += ImGui::SliderFloat(
                "Metallic",
                &m_pickedModel->m_materialConsts.GetCpu().metallicFactor, 0.0f,
                1.0f);
            flag += ImGui::SliderFloat(
                "Roughness",
                &m_pickedModel->m_materialConsts.GetCpu().roughnessFactor, 0.0f,
                1.0f);
            flag += ImGui::CheckboxFlags(
                "AlbedoTexture",
                &m_pickedModel->m_materialConsts.GetCpu().useAlbedoMap, 1);
            flag += ImGui::CheckboxFlags(
                "EmissiveTexture",
                &m_pickedModel->m_materialConsts.GetCpu().useEmissiveMap, 1);
            flag += ImGui::CheckboxFlags(
                "Use NormalMapping",
                &m_pickedModel->m_materialConsts.GetCpu().useNormalMap, 1);
            flag += ImGui::CheckboxFlags(
                "Use AO", &m_pickedModel->m_materialConsts.GetCpu().useAOMap,
                1);
            flag += ImGui::CheckboxFlags(
                "Use HeightMapping",
                &m_pickedModel->m_meshConsts.GetCpu().useHeightMap, 1);
            flag += ImGui::SliderFloat(
                "HeightScale",
                &m_pickedModel->m_meshConsts.GetCpu().heightScale, 0.0f, 0.1f);
            flag += ImGui::CheckboxFlags(
                "Use MetallicMap",
                &m_pickedModel->m_materialConsts.GetCpu().useMetallicMap, 1);
            flag += ImGui::CheckboxFlags(
                "Use RoughnessMap",
                &m_pickedModel->m_materialConsts.GetCpu().useRoughnessMap, 1);
            if (flag) {
                m_pickedModel->UpdateConstantBuffers(m_device, m_context);
            }
            ImGui::Checkbox("Draw Normals", &m_pickedModel->m_drawNormals);
        }

        ImGui::TreePop();
    }
}

} // namespace hlab
