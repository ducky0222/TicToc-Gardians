#include "pch.h"

#include "Sample.h"

#include "../ABGraphics/RenderManager.h"
#include "../ABGraphics/MathUtil.h"
#include "../ABGraphics/GeometryGenerator.h"
#include "../ABGraphics/DebugRenderManager.h"

namespace entryPoint
{
	Sample::Sample(HINSTANCE hInstance, UINT width, UINT height, std::wstring name)
		: Processor(hInstance, width, height, name)
	{
	}

	Sample::~Sample()
	{
		RenderManager::GetInstance().Destroy();
	}
	bool Sample::Init()
	{
		if (!Processor::Init())
		{
			return false;
		}

		RenderManager::GetInstance().SetShadowRadius(2048);
		RenderManager::GetInstance().Init(mhWnd, mWidth, mHeight, true);
		DebugRenderManager::Get().Init(
			RenderManager::GetInstance().GetDevice(),
			RenderManager::GetInstance().GetContext(),
			RenderManager::GetInstance().GetCamera());

		Camera* cam = RenderManager::GetInstance().GetCamera();
		cam->SetLens(0.25f * PI, GetAspectRatio(), 1.0f, 5000.f);
		cam->SetPosition(0, 500, -1000.f);

		RenderManager::GetInstance().SetSkymap("../Resources/TestTexture/BakerSampleEnvHDR.dds");
		RenderManager::GetInstance().SetIBL(
			"../Resources/TestTexture/BakerSampleDiffuseHDR.dds",
			"../Resources/TestTexture/BakerSampleSpecularHDR.dds",
			"../Resources/TestTexture/BakerSampleBrdf.dds");

		RenderManager::GetInstance().CreateModel("boxA", "../Resources/Model/TileA.fbx");
		RenderManager::GetInstance().CreateModel("boxB", "../Resources/Model/TileB.fbx");
		RenderManager::GetInstance().CreateModel("boxC", "../Resources/Model/TileC.fbx");
		RenderManager::GetInstance().CreateModel("boxD", "../Resources/Model/TileD.fbx");
		RenderManager::GetInstance().CreateModel("boxE", "../Resources/Model/test_TileE.fbx");
		RenderManager::GetInstance().CreateModel("boxEPlane", "../Resources/Model/test_TileEPlane.fbx");
		RenderManager::GetInstance().CreateModel("Gun", "../Resources/TestFBX/gun.fbx");

		std::vector<std::string> cutScnenTextures;
		cutScnenTextures.reserve(10);
		cutScnenTextures.push_back("../Resources/Texture/beaver_BaseColor.png");
		cutScnenTextures.push_back("../Resources/Texture/big_dog.png");
		cutScnenTextures.push_back("../Resources/Texture/cat_head_BaseColor.png");
		cutScnenTextures.push_back("../Resources/Texture/TileA_BaseColor.png");
		RenderManager::GetInstance().CreateCutSceneModel("myFirstCutScene", cutScnenTextures);

		RenderManager::GetInstance().CreateBackgroundModel("back1", "../Resources/Background/background_01.png");

		for (size_t i = 0; i < TILE_DEPTH; ++i)
		{
			for (size_t j = 0; j < TILE_WIDTH; ++j)
			{
				ModelInstance instance;
				instance.Key = "boxA";
				instance.Key[3] = 'A' + rand() % 5;

				if (instance.Key[3] == 'E')
				{
					ModelInstance head;
					head.ModelType = eModelType::Basic;
					head.Key = "boxEPlane";
					head.Transform = Matrix::CreateTranslation({ j * 200.f, i * 200.f, i * 200.f });
					mModelInstance.push_back(head);
				}

				instance.ModelType = eModelType::Basic;
				instance.Transform = Matrix::CreateTranslation({ j * 200.f, i * 200.f, i * 200.f });
				mModelInstance.push_back(instance);
			}
		}


		return true;
	}

	void Sample::Update(float deltaTime)
	{
		if (GetAsyncKeyState('5') & 0x8000)
		{
			RenderManager::GetInstance().OnResize(mWidth, mHeight);
		}
		static size_t i = 0;
		if (GetAsyncKeyState('1') & 0x8000)
		{
			Sleep(100);
			i++;
			RenderManager::GetInstance().SetCutSceneIndex("myFirstCutScene", i, true);
		}

		camMove(deltaTime);
		setShadowTransform();
	}

	void Sample::Render()
	{
		RenderManager::GetInstance().BeginRender();

		// //RenderManager::GetInstance().Submit(eModelType::CutScene, "myFirstCutScene");
		// RenderManager::GetInstance().Submit(eModelType::Background, "back1");
		// 
		for (int i = 0; i < mModelInstance.size(); ++i)
		{
			assert(RenderManager::GetInstance().ApplyAlpha(mModelInstance[i].ModelType, mModelInstance[i].Key, false, mModelInstance[i].Alpha));
			assert(RenderManager::GetInstance().ApplyTransform(mModelInstance[i].ModelType, mModelInstance[i].Key, mModelInstance[i].Transform));
			assert(RenderManager::GetInstance().Submit(mModelInstance[i].ModelType, mModelInstance[i].Key));
		}

		RenderManager::GetInstance().ApplyTransform(eModelType::Basic, "Gun",  Matrix::CreateScale(3) * Matrix::CreateTranslation(300, 500, 100));
		RenderManager::GetInstance().Submit(eModelType::Basic, "Gun");

		RenderManager::GetInstance().Excute();

		RenderManager::GetInstance().EndRender();
	}

	void Sample::camMove(float deltaTime)
	{
		Camera* cam = RenderManager::GetInstance().GetCamera();

		float moveSpeed = deltaTime * 1000;

		if (GetAsyncKeyState('W') & 0x8000)
		{
			cam->TranslateLook(moveSpeed);
		}
		if (GetAsyncKeyState('S') & 0x8000)
		{
			cam->TranslateLook(-moveSpeed);
		}
		if (GetAsyncKeyState('A') & 0x8000)
		{
			cam->TranslateRight(-moveSpeed);
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			cam->TranslateRight(moveSpeed);
		}
		if (GetAsyncKeyState('Q') & 0x8000)
		{
			cam->RotateYaw(deltaTime);
		}
		if (GetAsyncKeyState('E') & 0x8000)
		{
			cam->RotateYaw(-deltaTime);
		}
		if (GetAsyncKeyState('Z') & 0x8000)
		{
			cam->RotatePitch(deltaTime);
		}
		if (GetAsyncKeyState('X') & 0x8000)
		{
			cam->RotatePitch(-deltaTime);
		}
	}

	void Sample::setShadowTransform()
	{

	}

	void Sample::initGeometry()
	{
		GeometryGenerator::MeshData box;
		GeometryGenerator::MeshData grid;
		GeometryGenerator::MeshData sphere;
		GeometryGenerator::MeshData cylinder;

		GeometryGenerator geoGen;
		GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, &box);
		GeometryGenerator::CreateGrid(20.0f, 30.0f, 50, 40, &grid);
		GeometryGenerator::CreateSphere(0.5f, 20, 20, &sphere);
		GeometryGenerator::CreateCylinder(0.5f, 0.5f, 3.0f, 15, 15, &cylinder);

		RenderManager::GetInstance().CreateGeometryModel("box", box);
		RenderManager::GetInstance().CreateGeometryModel("grid", grid);
		RenderManager::GetInstance().CreateGeometryModel("sphere", sphere);
		RenderManager::GetInstance().CreateGeometryModel("cylinder", cylinder);

		ModelInstance instance;
		instance.ModelType = eModelType::Geometry;
		instance.Key = "box";
		instance.Transform = Matrix::CreateScale(3.f, 1.f, 3.f) * Matrix::CreateTranslation(0.f, 0.5f, 0.f);
		mModelInstance.push_back(instance);

		instance.Key = "grid";
		instance.Transform = Matrix::Identity;
		mModelInstance.push_back(instance);

		for (int i = 0; i < 5; ++i)
		{
			instance.Key = "cylinder";
			instance.Transform = Matrix::CreateTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
			mModelInstance.push_back(instance);

			instance.Transform = Matrix::CreateTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);
			mModelInstance.push_back(instance);

			instance.Key = "sphere";
			instance.Transform = Matrix::CreateTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
			mModelInstance.push_back(instance);

			instance.Transform = Matrix::CreateTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);
			mModelInstance.push_back(instance);
		}
	}
}