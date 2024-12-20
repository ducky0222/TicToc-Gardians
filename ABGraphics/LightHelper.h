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
		Vector3 AttenuationParam; // ���� �Ű����� a0, a1, a2
		float pad;
	};

	struct SpotLight
	{
		Vector4 Ambient;
		Vector4 Diffuse;
		Vector4 Specular;
		Vector3 Direction;
		float Spot; // ���Կ� ���� ����
		Vector3 Position;
		float Range;
		Vector3 AttenuationParam; // ���� �Ű����� a0, a1, a2
		float pad;
	};

	struct Material
	{
		Vector4 Ambient = { 0.1f, 0.1f, 0.1f, 0.1f };
		Vector4 Diffuse = { 0.5f, 0.5f, 0.5f, 0.5f };
		Vector4 Specular = { 0.5f, 0.5f, 0.5f, 8.f }; // specular�� ������ ������ ������ �����
		bool bUseAlpha = false;
		float Alpha = 1.f;
		float unused[2];
	};
}