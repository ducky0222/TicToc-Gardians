#pragma once

namespace light
{
	struct DirectionLight
	{
		Vector4 Ambient;
		Vector4 Diffuse;
		Vector4 Specular;
		Vector3 Direction;
		float pad;
	};

	struct PointLight
	{
		Vector4 Ambient;
		Vector4 Diffuse;
		Vector4 Specular;
		Vector3 Position;
		float Range;
		Vector3 AttenuationParam; // 감쇠 매개변수 a0, a1, a2
		float pad;
	};

	struct SpotLight
	{
		Vector4 Ambient;
		Vector4 Diffuse;
		Vector4 Specular;
		Vector3 Direction;
		float Spot; // 원뿔에 사용될 지수
		Vector3 Position;
		float Range;
		Vector3 AttenuationParam; // 감쇠 매개변수 a0, a1, a2
		float pad;
	};

	struct Material
	{
		Vector4 Ambient = { 0.1f, 0.1f, 0.1f, 0.1f };
		Vector4 Diffuse = { 0.5f, 0.5f, 0.5f, 0.5f };
		Vector4 Specular = { 0.5f, 0.5f, 0.5f, 8.f }; // specular의 마지막 성분을 지수로 사용함
		bool bUseAlpha = false;
		float Alpha = 1.f;
		float unused[2];
	};
}