#pragma once
#include "Component.h"

struct MeshRenderer : public Component
{
	GENERATE_BODY(MeshRenderer)
	GENERATE_PROPERTY(MeshRenderer, FileName, Alpha, bUseAlpha, AnimName, ClipName, TimePos, bIsRecursive, TexSize, TexTranslate)

public:
	// render common
	std::string FileName{ ".fbx" };
	float Alpha{ 1.f };
	bool bUseAlpha;

	// anim common
	std::string AnimName{ ".fbx" };
	std::string ClipName;
	float TimePos;
	bool bIsRecursive{ true };

	// texture common
	VSM::Vector2 TexSize{ 1.f };
	VSM::Vector2 TexTranslate;
};

