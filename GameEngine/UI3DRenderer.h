#pragma once
#include "Component.h"

class UI3DRenderer : public Component
{
	GENERATE_BODY(UI3DRenderer)
	GENERATE_PROPERTY(UI3DRenderer, FileName, Alpha, bUseAlpha, Layer, LookVector, Position, Size, Rotation, TexSize, TexTranslate)

public:
	// common
	std::string FileName{ ".png" };
	float Alpha{ 1.f };
	bool bUseAlpha;

	// ui
	unsigned int Layer; 
	VSM::Vector3 LookVector{ 0, 0, -1 };
	VSM::Vector3 Position;
	VSM::Vector2 Size{ 1,1 };
	float Rotation;

	// texture
	VSM::Vector2 TexSize{ 1, 1 };
	VSM::Vector2 TexTranslate;
};

