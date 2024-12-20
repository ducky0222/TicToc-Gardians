#pragma once
#include "Component.h"

struct SkinnedMeshRenderer : public Component
{
public:
	GENERATE_BODY(SkinnedMeshRenderer)
	GENERATE_PROPERTY(SkinnedMeshRenderer, FileName, Alpha, bUseAlpha, TexSize, TexTranslate)

public:
	// render common
	std::string FileName{ ".fbx" };
	float Alpha{ 1.f };
	bool bUseAlpha;

	// texture common
	VSM::Vector2 TexSize{ 1.f, 1.f };
	VSM::Vector2 TexTranslate;
};
