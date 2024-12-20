#pragma once

#include "Graphics.h"
#include "ConstantBuffer.h"
#include "LightHelper.h"
#include "Camera.h"

namespace Rgph
{
	class RenderGraph;
}

class FrameLight
{
public:
	FrameLight(Graphics& graphics)
		: mCBuffer(graphics)
	{
		Reset();
	}
	~FrameLight() = default;

	void SpawnControlWindow()
	{
		if (ImGui::Begin("Light"))
		{
			ImGui::Text("Light0");
			ImGui::SliderFloat3("Ambient0", &mCBData.DirectionLights[0].Ambient.x, 0.f, 1.f, "%.1f");
			ImGui::SliderFloat3("Diffuse0", &mCBData.DirectionLights[0].Diffuse.x, 0.f, 1.f, "%.1f");
			ImGui::SliderFloat3("Specular0", &mCBData.DirectionLights[0].Specular.x, 0.f, 1.f, "%.1f");
			static Vector3 unnormalizeDir0 = mCBData.DirectionLights[0].Direction;
			ImGui::SliderFloat3("Direction0", &unnormalizeDir0.x, -1.f, 1.f, "%.1f");
			ImGui::SliderFloat("SpecularExponent0", &mCBData.DirectionLights[0].Specular.w, 1.f, 32.f);
			mCBData.DirectionLights[0].Direction = unnormalizeDir0;
			mCBData.DirectionLights[0].Direction.Normalize();

			ImGui::Text("Light1");
			ImGui::SliderFloat3("Ambient1", &mCBData.DirectionLights[1].Ambient.x, 0.f, 1.f, "%.1f");
			ImGui::SliderFloat3("Diffuse1", &mCBData.DirectionLights[1].Diffuse.x, 0.f, 1.f, "%.1f");
			ImGui::SliderFloat3("Specular1", &mCBData.DirectionLights[1].Specular.x, 0.f, 1.f, "%.1f");
			static Vector3 unnormalizeDir1 = mCBData.DirectionLights[1].Direction;
			ImGui::SliderFloat3("Direction1", &unnormalizeDir1.x, -1.f, 1.f, "%.1f");
			ImGui::SliderFloat("SpecularExponent1", &mCBData.DirectionLights[1].Specular.w, 1.f, 32.f);
			mCBData.DirectionLights[1].Direction = unnormalizeDir1;
			mCBData.DirectionLights[1].Direction.Normalize();

			ImGui::Text("Light2");
			ImGui::SliderFloat3("Ambient2", &(mCBData.DirectionLights[2].Ambient.x), 0.f, 1.f, "%.1f");
			ImGui::SliderFloat3("Diffuse2", &(mCBData.DirectionLights[2].Diffuse.x), 0.f, 1.f, "%.1f");
			ImGui::SliderFloat3("Specular2", &(mCBData.DirectionLights[2].Specular.x), 0.f, 1.f, "%.1f");
			static Vector3 unnormalizeDir2 = mCBData.DirectionLights[2].Direction;
			ImGui::SliderFloat3("Direction2", &(unnormalizeDir2.x), -1.f, 1.f, "%.1f");
			ImGui::SliderFloat("SpecularExponent2", &mCBData.DirectionLights[2].Specular.w, 1.f, 32.f);
			mCBData.DirectionLights[2].Direction = unnormalizeDir2;
			mCBData.DirectionLights[2].Direction.Normalize();

			ImGui::Text("Fog");
			static bool bUseFog = false;
			ImGui::Checkbox("UseFog", &bUseFog);
			mCBData.bUseFog = bUseFog;
			ImGui::SliderFloat3("Color", &mCBData.FogColor.x, 0.f, 1.f, "%.1f");
			ImGui::SliderFloat("Start", &mCBData.FogStart, 10.f, 1000.f, "%.1f");
			ImGui::SliderFloat("Range", &mCBData.FogRange, 10.f, 1000.f, "%.1f");

			ImGui::Text("IBL");
			static bool bUseIBL = true;
			ImGui::Checkbox("UseIBL", &bUseIBL);
			mCBData.bUseIBL = bUseIBL;

			ImGui::Text("Ssao");
			static bool bUseSsao = false;
			ImGui::Checkbox("UseSsao", &bUseSsao);
			mCBData.bUseSsao = bUseSsao;

			ImGui::Text("Tonemapping");
			static bool bUseTonemapping = true;
			ImGui::Checkbox("UseTonemapping", &bUseTonemapping);
			mCBData.bUseTonemapping = bUseTonemapping;

			if (ImGui::Button("Reset"))
			{
				Reset();
			}
		}
		ImGui::End();
	}

	void Reset()
	{
		// ÈÄ±¤
		mCBData.DirectionLights[0].Ambient = { 1.f, 1.f, 1.f, 1.f };
		mCBData.DirectionLights[0].Diffuse = { 1.f, 1.f, 1.f, 1.f };
		mCBData.DirectionLights[0].Specular = { 1.f, 1.f, 1.f, 8.f };
		mCBData.DirectionLights[0].Direction = { 0.f, 0.f, 1.f };
		mCBData.DirectionLights[0].Direction.Normalize();

		// ¾ÕÁÂÃø
		mCBData.DirectionLights[1].Ambient = { 1.f, 1.f, 1.f, 1.f };
		mCBData.DirectionLights[1].Diffuse = { 1.f, 1.f, 1.f, 1.f };
		mCBData.DirectionLights[1].Specular = { 1.f, 1.f, 1.f, 8.f };
		mCBData.DirectionLights[1].Direction = { 1.f, 0.f, 0.f };
		mCBData.DirectionLights[1].Direction.Normalize();

		// ¾Õ¿ìÃø
		mCBData.DirectionLights[2].Ambient = { 1.f, 1.f, 1.f, 1.f };
		mCBData.DirectionLights[2].Diffuse = { 1.f, 1.f, 1.f, 1.f };
		mCBData.DirectionLights[2].Specular = { 1.f, 1.f, 1.f, 8.f };
		mCBData.DirectionLights[2].Direction = { 0.f , -1.f, 0.f };
		mCBData.DirectionLights[2].Direction.Normalize();

		mCBData.LightCount = 3;

		mCBData.EyePosW = { 0, };

		mCBData.FogColor = { 0,0,0,0 };

		mCBData.FogStart = 500.f;

		mCBData.FogRange = 1000.f;

		mCBData.bUseFog = false;

		mCBData.bUseIBL = true;

		mCBData.bUseSsao = false;

		mCBData.bUseTonemapping = true;
	}

	void Submit() const
	{
		// render data submit
	}

	void Bind(Graphics& graphics)
	{
		const Matrix& view = graphics.GetView();

		mCBData.EyePosW = Camera::GetCamPos(view);
		mCBData.bUseSsao = false;
		mCBuffer.Update(graphics, mCBData);
		mCBuffer.Bind(graphics);
	}

	const auto& GetCBData() const { return mCBData; }

private:
	struct FrameLightCBuf
	{
		light::DirectionLight DirectionLights[3];
		int LightCount;
		Vector3 EyePosW;
		Vector4 FogColor;
		float FogStart;
		float FogRange;
		int bUseFog;
		int bUseIBL;
		int bUseSsao;
		int bUseTonemapping;
		int unused[2];
	};

private:
	FrameLightCBuf mCBData;
	mutable Bind::PixelConstantBuffer<FrameLightCBuf> mCBuffer;
};

