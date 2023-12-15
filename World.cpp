#include "World.h"

#include <DirectXCollision.h> // 구와 광선 충돌 계산에 사용
#include <directxtk/DDSTextureLoader.h>
#include <directxtk/SimpleMath.h>
#include <tuple>
#include <vector>

#include "GeometryGenerator.h"
#include "GraphicsCommon.h"

namespace hlab {

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

World::World() : AppBase() {}

bool World::InitScene() {

    string basePath =
        "C:\\Users\\aa\\Documents\\Google Drive\\Graphics Assets\\Assets\\";

    AppBase::m_camera.Reset(Vector3(-0.112852f, 0.307729f, -0.542159f),
                            0.0589047f, 0.14399f);
    AppBase::m_globalConstsCPU.strengthIBL = 0.1f;

    AppBase::InitCubemaps(L"../Assets/Textures/Cubemaps/HDRI/",
                          L"SampleEnvHDR.dds", L"SampleSpecularHDR.dds",
                          L"SampleDiffuseHDR.dds", L"SampleBrdf.dds", 0);

    AppBase::InitCubemaps(
        L"../Assets/Textures/Cubemaps/HDRI/", L"clear_pureskyEnvHDR.dds",
        L"clear_pureskySpecularHDR.dds", L"clear_pureskyDiffuseHDR.dds",
        L"clear_pureskyBrdf.dds", 1);

    AppBase::InitScene();

    // AppBase::m_backBufferFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

    // if (!AppBase::Initialize()) {
    //     return false;
    // }

    // 바닥(거울)
    {
        // https://freepbr.com/materials/stringy-marble-pbr/
        auto mesh = GeometryGenerator::MakeSquare(10.0, {10.0f, 10.0f});
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
        m_ground->m_name = "Ground";

        Vector3 position = Vector3(0.0f, -0.5f, 2.0f);
        m_ground->UpdateWorldRow(Matrix::CreateRotationX(3.141592f * 0.5f) *
                                 Matrix::CreateTranslation(position));

        // m_mirrorPlane = SimpleMath::Plane(position, Vector3(0.0f, 1.0f,
        // 0.0f)); m_mirror = m_ground; // 바닥에 거울처럼 반사 구현

        std::shared_ptr<Plane> mirrorPlane =
            std::make_shared<Plane>(position, Vector3(0.0f, 1.0f, 0.0f));
        std::shared_ptr<Model> mirror = m_ground; // 바닥에 거울처럼 반사 구현

        m_mirrorList.insert(std::make_pair(mirror, mirrorPlane));

        // m_basicList.push_back(m_ground); // 거울은 리스트에 등록 X
    }

    // 전방벽(거울)
    {
        // https://freepbr.com/materials/stringy-marble-pbr/
        auto mesh = GeometryGenerator::MakeSquare(10.0, {10.0f, 10.0f});
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
        m_ground->m_name = "Wall";

        Vector3 position = Vector3(0.0f, -0.5f, 12.0f);
        m_ground->UpdateWorldRow(Matrix::CreateRotationX(0.1f) *
                                 Matrix::CreateTranslation(position));

        std::shared_ptr<Plane> mirrorPlane =
            std::make_shared<Plane>(position, Vector3(0.0f, 0.0f, -1.0f));
        std::shared_ptr<Model> mirror = m_ground; // 바닥에 거울처럼 반사 구현

        // m_mirrorList.insert(std::make_pair(mirror, mirrorPlane));

        // m_basicList.push_back(m_ground); // 거울은 리스트에 등록 X
    }

    // Main Object
    {
        auto meshes = GeometryGenerator::ReadFromFile(
            "../Assets/Models/DamagedHelmet/", "DamagedHelmet.gltf");

        // auto meshes = GeometryGenerator::ReadFromFile(
        //     "../Assets/Models/medieval_vagrant_knights/", "scene.gltf",
        //     true);

        // 컴퓨터가 느릴 때는 간단한 물체로 테스트 하세요.
        // vector<MeshData> meshes = {GeometryGenerator::MakeSphere(0.4f, 50,
        // 50)};

        // string path = "../Assets/Characters/armored-female-future-soldier/";
        // auto meshes = GeometryGenerator::ReadFromFile(path,
        // "angel_armor.fbx"); meshes[0].albedoTextureFilename = path +
        // "/angel_armor_albedo.jpg"; meshes[0].emissiveTextureFilename = path +
        // "/angel_armor_e.jpg"; meshes[0].metallicTextureFilename = path +
        // "/angel_armor_metalness.jpg"; meshes[0].normalTextureFilename = path
        // + "/angel_armor_normal.jpg"; meshes[0].roughnessTextureFilename =
        //     path + "/angel_armor_roughness.jpg";

        string basePath = "..\\Assets\\Models\\DamagedHelmet\\";
        meshes[0].albedoTextureFilename = basePath + "Default_albedo.jpg";
        meshes[0].aoTextureFilename = basePath + "Default_AO.jpg";
        meshes[0].emissiveTextureFilename = basePath + "Default_emissive.jpg";
        meshes[0].metallicTextureFilename =
            basePath + "Default_metalRoughness.jpg";
        meshes[0].normalTextureFilename = basePath + "Default_normal.jpg";
        meshes[0].roughnessTextureFilename =
            basePath + "Default_metalRoughness.jpg";

        Vector3 center(0.0f, -0.05f, 2.0f);
        auto newModel = make_shared<Model>(m_device, m_context, meshes);
        newModel->m_materialConsts.GetCpu().invertNormalMapY =
            true; // GLTF는 true로
        newModel->m_materialConsts.GetCpu().albedoFactor = Vector3(1.0f);
        newModel->m_materialConsts.GetCpu().roughnessFactor = 0.3f;
        newModel->m_materialConsts.GetCpu().metallicFactor = 0.8f;
        newModel->UpdateWorldRow(Matrix::CreateTranslation(center));
        newModel->m_isPickable = true; // 마우스로 선택/이동 가능
        newModel->m_name = "MainSphere";

        m_basicList.push_back(newModel); // 리스트에 등록
        m_basicListMap.insert(std::make_pair("MainSphere", newModel));
    }

    // 추가 물체1
    //{
    //    MeshData mesh = GeometryGenerator::MakeSphere(0.2f, 200, 200);
    //    Vector3 center(0.5f, 0.5f, 2.0f);
    //    auto newModel = make_shared<Model>(m_device, m_context, vector{mesh});

    //    newModel->UpdateWorldRow(Matrix::CreateTranslation(center));
    //    newModel->m_materialConsts.GetCpu().albedoFactor =
    //        Vector3(0.1f, 0.1f, 1.0f);
    //    newModel->m_materialConsts.GetCpu().roughnessFactor = 0.2f;
    //    newModel->m_materialConsts.GetCpu().metallicFactor = 0.6f;
    //    newModel->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
    //    newModel->UpdateConstantBuffers(m_device, m_context);
    //    newModel->m_isPickable = true; // 마우스로 선택/이동 가능
    //    newModel->m_name = "SecondSphere";
    //    m_basicList.push_back(newModel);
    //}

    // 추가 물체2
    {
        MeshData mesh = GeometryGenerator::MakeSphere(0.2f, 200, 200);
        // MeshData mesh =
        //     GeometryGenerator::MakeSphere(0.4f, 100, 100, {2.0f, 2.0f});
        Vector3 center(0.5f, 0.5f, 2.0f);

        string basePath = "..\\Assets\\Textures\\PBR\\worn-painted-metal-ue\\";
        mesh.albedoTextureFilename = basePath + "worn-painted-metal_albedo.png";
        mesh.aoTextureFilename = basePath + "worn-painted-metal_ao.png";
        mesh.metallicTextureFilename =
            basePath + "worn-painted-metal_metallic.png";
        mesh.normalTextureFilename =
            basePath + "worn-painted-metal_normal-dx.png";
        mesh.roughnessTextureFilename =
            basePath + "worn-painted-metal_roughness.png";
        mesh.heightTextureFilename = basePath + "worn-painted-metal_height.png";

        auto newModel = make_shared<Model>(m_device, m_context, vector{mesh});

        newModel->UpdateWorldRow(Matrix::CreateTranslation(center));
        // newModel->m_materialConsts.GetCpu().albedoFactor =
        //     Vector3(0.1f, 0.1f, 1.0f);
        newModel->m_materialConsts.GetCpu().roughnessFactor = 0.2f;
        newModel->m_materialConsts.GetCpu().metallicFactor = 0.6f;
        newModel->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
        newModel->m_meshConsts.GetCpu().heightScale = 0.03f;
        newModel->UpdateConstantBuffers(m_device, m_context);
        newModel->m_isPickable = true; // 마우스로 선택/이동 가능
        newModel->m_name = "SecondSphere";

        m_basicList.push_back(newModel);
        m_basicListMap.insert(std::make_pair("SecondSphere", newModel));
    }

    // 추가 물체3
    //{
    //    string path =
    //    "..\\Assets\\Characters\\armored-female-future-soldier\\";
    //    vector<MeshData> meshes =
    //        GeometryGenerator::ReadFromFile(path, "angel_armor.fbx");
    //    meshes[0].albedoTextureFilename = path + "angel_armor_albedo.jpg";
    //    meshes[0].emissiveTextureFilename = path + "angel_armor_e.jpg";
    //    meshes[0].metallicTextureFilename = path +
    //    "angel_armor_metalness.jpg"; meshes[0].normalTextureFilename = path +
    //    "angel_armor_normal.jpg"; meshes[0].roughnessTextureFilename = path +
    //    "angel_armor_roughness.jpg";

    //    Vector3 center(2.0f, 0.5f, 2.5f);
    //    auto newModel = make_shared<Model>(m_device, m_context, meshes);

    //    newModel->UpdateWorldRow(Matrix::CreateTranslation(center));
    //    // newModel->m_materialConsts.GetCpu().albedoFactor = Vector3(0.f);
    //    newModel->m_materialConsts.GetCpu().roughnessFactor = 0.5f;
    //    newModel->m_materialConsts.GetCpu().metallicFactor = 0.9f;
    //    newModel->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
    //    newModel->UpdateConstantBuffers(m_device, m_context);
    //    newModel->m_isPickable = true; // 마우스로 선택/이동 가능
    //    newModel->m_name = "Soldier";
    //    m_basicList.push_back(newModel);
    //}

    //// 추가 물체4
    {
        MeshData mesh = GeometryGenerator::MakeBox(0.3f);
        string basePath = "..\\Assets\\Textures\\";
        mesh.albedoTextureFilename = basePath + "blender_uv_grid_2k.png";
        Vector3 center(0.0f, 0.5f, 2.5f);
        auto newModel = make_shared<Model>(m_device, m_context, vector{mesh});
        newModel->UpdateWorldRow(Matrix::CreateTranslation(center));
        newModel->m_materialConsts.GetCpu().albedoFactor =
            Vector3(1.0f, 1.f, 1.f);
        newModel->m_materialConsts.GetCpu().roughnessFactor = 0.f;
        newModel->m_materialConsts.GetCpu().metallicFactor = 0.f;
        newModel->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
        newModel->UpdateConstantBuffers(m_device, m_context);
        newModel->m_isPickable = true; // 마우스로 선택/이동 가능
        newModel->m_name = "Box";
        // m_basicList.push_back(newModel);
        // m_basicListMap.insert(std::make_pair("firstBox", newModel));
    }

    // House
    //{
    //    // https://freepbr.com/materials/stringy-marble-pbr/
    //    // string basePath =
    //    //    "../Assets/3DModel/club_atomic_where_the_holograms_go/";

    //    string path = "..\\Assets\\Terrain\\Modern House\\maps\\";

    //    auto mesh = GeometryGenerator::ReadFromFile(
    //        path, "uploads_files_3088696_villa+fbx.fbx");
    //    // mesh.albedoTextureFilename = path + "stringy_marble_albedo.png";
    //    // mesh.emissiveTextureFilename = "";
    //    // mesh.aoTextureFilename = path + "stringy_marble_ao.png";
    //    // mesh.metallicTextureFilename = path +
    //    "stringy_marble_Metallic.png";
    //    // mesh.normalTextureFilename = path + "stringy_marble_Normal-dx.png";
    //    // mesh.roughnessTextureFilename = path +
    //    // "stringy_marble_Roughness.png";

    //    m_ground = make_shared<Model>(m_device, m_context, vector{mesh});
    //    // m_ground->m_materialConsts.GetCpu().albedoFactor = Vector3(0.7f);
    //    // m_ground->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
    //    // m_ground->m_materialConsts.GetCpu().metallicFactor = 0.5f;
    //    // m_ground->m_materialConsts.GetCpu().roughnessFactor = 0.3f;

    //    Vector3 position = Vector3(-1.0f, 1.1f, 4.0f);
    //    m_ground->UpdateWorldRow(
    //        Matrix::CreateScale(
    //            10.f) * /*Matrix::CreateRotationX(3.141592f * 0.5f) **/
    //        Matrix::CreateTranslation(position));
    //    m_ground->m_castShadow = false; // 바닥은 그림자 만들기 생략
    //    m_ground->m_isPickable = true;

    //    m_basicList.push_back(m_ground); // 거울은 리스트에 등록 X
    //}

    // Terrain0
    //{
    //    // https://freepbr.com/materials/stringy-marble-pbr/
    //    // string basePath =
    //    //    "../Assets/3DModel/club_atomic_where_the_holograms_go/";

    //    string path = "..\\Assets\\Terrain\\Grass field\\";

    //    auto mesh = GeometryGenerator::ReadFromFile(
    //        path, "Grass field.fbx");
    //    // mesh.albedoTextureFilename = path + "stringy_marble_albedo.png";
    //    // mesh.emissiveTextureFilename = "";
    //    // mesh.aoTextureFilename = path + "stringy_marble_ao.png";
    //    // mesh.metallicTextureFilename = path +
    //    "stringy_marble_Metallic.png";
    //    // mesh.normalTextureFilename = path + "stringy_marble_Normal-dx.png";
    //    // mesh.roughnessTextureFilename = path +
    //    // "stringy_marble_Roughness.png";

    //    m_ground = make_shared<Model>(m_device, m_context, vector{mesh});
    //    // m_ground->m_materialConsts.GetCpu().albedoFactor = Vector3(0.7f);
    //    // m_ground->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
    //    // m_ground->m_materialConsts.GetCpu().metallicFactor = 0.5f;
    //    // m_ground->m_materialConsts.GetCpu().roughnessFactor = 0.3f;

    //    Vector3 position = Vector3(-1.0f, 1.1f, 4.0f);
    //    m_ground->UpdateWorldRow(
    //        Matrix::CreateScale(
    //            10.f) * /*Matrix::CreateRotationX(3.141592f * 0.5f) **/
    //        Matrix::CreateTranslation(position));
    //    m_ground->m_castShadow = false; // 바닥은 그림자 만들기 생략
    //    m_ground->m_isPickable = true;

    //    m_basicList.push_back(m_ground); // 거울은 리스트에 등록 X
    //}

    // Terrain_Dessert
    //{

    //    string path = basePath +
    //    "\\Terrain\\Mountain\\3dexport_terraindesert_"
    //                  "texture_1592143852\\";

    //    auto mesh = GeometryGenerator::ReadFromFile(path,
    //    "Terraindesert.fbx");

    //    mesh[0].albedoTextureFilename = path + "Default OBJ_Base_Color.png";
    //    // mesh.emissiveTextureFilename = "";
    //    mesh[0].aoTextureFilename = path + "Default OBJ_Mixed_AO.png";
    //    mesh[0].metallicTextureFilename = path + "Default OBJ_Metallic.png";
    //    mesh[0].normalTextureFilename = path + "Default OBJ_Normal.png";
    //    mesh[0].roughnessTextureFilename = path + "Default OBJ_Roughness.png";
    //    mesh[0].heightTextureFilename = path + "Default OBJ_Height.png";

    //    m_ground = make_shared<Model>(m_device, m_context, vector{mesh});
    //    // m_ground->m_materialConsts.GetCpu().albedoFactor = Vector3(0.7f);
    //    // m_ground->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
    //    // m_ground->m_materialConsts.GetCpu().metallicFactor = 0.5f;
    //    // m_ground->m_materialConsts.GetCpu().roughnessFactor = 0.3f;

    //    Vector3 position = Vector3(0.0f, 0.f, 0.0f);
    //    m_ground->UpdateWorldRow(
    //        Matrix::CreateScale(
    //            200.f) * /*Matrix::CreateRotationX(3.141592f * 0.5f) **/
    //        Matrix::CreateTranslation(position));
    //    m_ground->m_castShadow = false; // 바닥은 그림자 만들기 생략
    //    m_ground->m_isPickable = true;

    //    m_basicList.push_back(m_ground); // 거울은 리스트에 등록 X
    //}

    // Terrain Forest
    {
        string path =
            basePath +
            "\\Terrain\\Mountain\\3dexport_terrainforest_texture_1592143852\\";

        auto terrainMeshes =
            GeometryGenerator::ReadFromFile(path, "Terrainforest.fbx");

        terrainMeshes[0].albedoTextureFilename =
            path + "Default OBJ_Base_Color.png";
        // mesh.emissiveTextureFilename = "";
        terrainMeshes[0].aoTextureFilename = path + "Default OBJ_Mixed_AO.png";
        terrainMeshes[0].metallicTextureFilename =
            path + "Default OBJ_Metallic.png";
        terrainMeshes[0].normalTextureFilename =
            path + "Default OBJ_Normal.png";
        terrainMeshes[0].roughnessTextureFilename =
            path + "Default OBJ_Roughness.png";
        terrainMeshes[0].heightTextureFilename =
            path + "Default OBJ_Height.png";

        m_ground =
            make_shared<Model>(m_device, m_context, vector{terrainMeshes});
        // m_ground->m_materialConsts.GetCpu().albedoFactor = Vector3(0.7f);
        // m_ground->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
        // m_ground->m_materialConsts.GetCpu().metallicFactor = 0.5f;
        // m_ground->m_materialConsts.GetCpu().roughnessFactor = 0.3f;

        Vector3 position = Vector3(0.0f, 0.f, 0.0f);
        m_ground->UpdateWorldRow(
            Matrix::CreateScale(
                10.f) * /*Matrix::CreateRotationX(3.141592f * 0.5f) **/
            Matrix::CreateTranslation(position));
        m_ground->m_castShadow = false; // 바닥은 그림자 만들기 생략
        m_ground->m_isPickable = false;

        m_basicList.push_back(m_ground); // 거울은 리스트에 등록 X

        // Tree0

        path = basePath + "Foliage/Gledista_Triacanthos_FBX/";
        auto meshes = GeometryGenerator::ReadFromFile(
            path, "Gledista_Triacanthos_3.fbx", false);

        Vector3 center(0.f);

        // m_tree.m_leaves = make_shared<Model>(m_device, m_context,
        //                                      vector{meshes[2], meshes[3]});
        m_tree.m_leaves = make_shared<Model>();

        m_tree.m_leaves->m_meshConsts.GetCpu().windTrunk = 0.1f;
        m_tree.m_leaves->m_meshConsts.GetCpu().windLeaves = 0.01f;
        m_tree.m_leaves->m_materialConsts.GetCpu().albedoFactor = Vector3(0.3f);
        m_tree.m_leaves->m_materialConsts.GetCpu().roughnessFactor = 0.8f;
        m_tree.m_leaves->m_materialConsts.GetCpu().metallicFactor = 0.2f;
        m_tree.m_leaves->UpdateWorldRow(Matrix::CreateScale(1.0f) *
                                        Matrix::CreateTranslation(center));
        //m_tree.m_leaves->m_isPickable = true;

        m_basicList.push_back(m_tree.m_leaves); // 리스트에 등록

        // m_tree.m_trunks = make_shared<Model>(
        //     m_device, m_context,
        //     vector{meshes[0], meshes[1],
        //            meshes[4]}); // Trunk and branches (4 is trunk)
        m_tree.m_trunks = make_shared<Model>();

        m_tree.m_trunks->m_meshConsts.GetCpu().windTrunk = 0.1f;
        m_tree.m_trunks->m_meshConsts.GetCpu().windLeaves = 0.0f;
        m_tree.m_trunks->m_materialConsts.GetCpu().albedoFactor = Vector3(1.0f);
        m_tree.m_trunks->m_materialConsts.GetCpu().roughnessFactor = 0.8f;
        m_tree.m_trunks->m_materialConsts.GetCpu().metallicFactor = 0.0f;
        m_tree.m_trunks->UpdateWorldRow(Matrix::CreateScale(1.0f) *
                                        Matrix::CreateTranslation(center));
        //m_tree.m_trunks->m_isPickable = true;

        m_basicList.push_back(m_tree.m_trunks); // 리스트에 등록

        std::mt19937 gen(1);
        std::uniform_real_distribution<float> dist(
            0, terrainMeshes[0].vertices.size());

        for (size_t i = 0; i < 10; i++) {
            Vector3 center = Vector3(
                terrainMeshes[0].vertices[(int)round(dist(gen))].position);
            // Vector3 center = Vector3(terrainMeshes[0].vertices[0].position);

            center = Vector3::Transform(center, m_ground->m_worldRow);
            center += 1.f * Vector3(0.f, 0.5f, 0.f);

            ModelInstance mi;
            mi.instanceWorld =
                Matrix::CreateScale(1.f) *
                Matrix::CreateTranslation(center).Transpose();

            m_tree.m_leaves->m_instancesCpu.push_back(mi);
            m_tree.m_trunks->m_instancesCpu.push_back(mi);
        }

        m_tree.m_leaves->Initialize(m_device, m_context,
                                    vector{meshes[2], meshes[3]});
        m_tree.m_trunks->Initialize(m_device, m_context,
                                    vector{meshes[0], meshes[1], meshes[4]});

        m_tree.m_leaves->m_isPickable = false;
        m_tree.m_trunks->m_isPickable = false;

        if (m_tree.m_leaves->m_instancesCpu.size() > 0) {
            D3D11Utils::UpdateBuffer(m_context, m_tree.m_leaves->m_instancesCpu,
                                     m_tree.m_leaves->m_instancesGpu);
            D3D11Utils::UpdateBuffer(m_context, m_tree.m_trunks->m_instancesCpu,
                                     m_tree.m_trunks->m_instancesGpu);
        }
    }

    // Billboard
    {
        vector<Vector4> points;
        Vector4 p = {-4.0f, 1.0f, 10.0f, 1.0f};
        for (int i = 0; i < 5; i++) {
            points.push_back(p);
            p.x += 1.8f;
        }
        float width = 2.4f;

        m_billboard = std::make_shared<BillboardModel>();
        m_billboard->Initialize(m_device, m_context, points, width, NULL);
    }

    ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));

    D3D11_TEXTURE2D_DESC desc{};
    backBuffer->GetDesc(&desc);

    m_device->CreateTexture2D(&desc, NULL, m_texture.GetAddressOf());

    m_device->CreateUnorderedAccessView(m_texture.Get(), NULL,
                                        m_uav.GetAddressOf());

    D3D11Utils::CreateComputeShader(m_device, L"TestCS.hlsl", m_cs);

    m_pso.m_computeShader = m_cs;

    return true;
}

void World::UpdateLights(float dt) { AppBase::UpdateLights(dt); }

void World::Update(float dt) {
    AppBase::Update(dt);

    auto iter = m_basicListMap.find("SecondSphere");

    if (iter != m_basicListMap.end()) {
        auto model = iter->second;

        Matrix a = model->m_worldRow;
        Matrix b = model->m_meshConsts.GetCpu().world;

        model->UpdateWorldRow(Matrix::CreateRotationY(dt) * model->m_worldRow);

        // iter->second->UpdateConstantBuffers(m_device, m_context);
    }

    iter = m_basicListMap.find("firstBox");
    if (iter != m_basicListMap.end()) {
        auto model = iter->second;

        Vector3 n(0.f, 1.f, 0.f);
        Quaternion q = Quaternion::CreateFromAxisAngle(n, dt);

        // model->UpdateWorldRow(Matrix::CreateFromQuaternion(q) *
        //                       model->m_worldRow);
    }

    //if (m_tree.m_leaves->m_instancesCpu.size() > 0) {
    //    D3D11Utils::UpdateBuffer(m_context, m_tree.m_leaves->m_instancesCpu,
    //                             m_tree.m_leaves->m_instancesGpu);
    //    D3D11Utils::UpdateBuffer(m_context, m_tree.m_trunks->m_instancesCpu,
    //                             m_tree.m_trunks->m_instancesGpu);
    //}
}

void World::Render() {
    AppBase::Render();
    // AppBase::PostRender();

    AppBase::SetPipelineState(m_pso);
    m_context->CSSetUnorderedAccessViews(0, 1, m_uav.GetAddressOf(), NULL);
    m_context->Dispatch(ceil(m_screenWidth / 32.f), ceil(m_screenHeight / 32.f),
                        1);

    // ComPtr<ID3D11Texture2D> backBuffer;
    // m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    // m_context->CopyResource(backBuffer.Get(), m_texture.Get());

    AppBase::ComputeShaderBarrier();
}

void World::UpdateGUI() {
    ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Appearing);
    if (ImGui::TreeNode("SelectSkybox")) {
        int flag = 0;
        flag += ImGui::RadioButton("skybox0", &m_skyboxCheck, 0);
        ImGui::SameLine();
        flag += ImGui::RadioButton("skybox1", &m_skyboxCheck, 1);

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("General")) {
        ImGui::Checkbox("Use FPV", &m_camera.m_useFirstPersonView);
        ImGui::Checkbox("Wireframe", &m_drawAsWire);
        ImGui::Checkbox("DrawOBB", &m_drawOBB);
        ImGui::Checkbox("DrawBSphere", &m_drawBS);
        if (ImGui::Checkbox("MSAA ON", &m_useMSAA)) {
            CreateBuffers();
        }
        ImGui::Checkbox("Coordinate System", &m_coordinateCheck);
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

        // ImGui::SliderFloat("Metallic",
        //                    &m_mirror->m_materialConsts.GetCpu().metallicFactor,
        //                    0.0f, 1.0f);
        // ImGui::SliderFloat("Roughness",
        //                    &m_mirror->m_materialConsts.GetCpu().roughnessFactor,
        //                    0.0f, 1.0f);

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
