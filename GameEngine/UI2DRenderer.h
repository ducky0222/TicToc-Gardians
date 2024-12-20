#pragma once
#include "Component.h"

class UI2DRenderer : public Component
{
	GENERATE_BODY(UI2DRenderer)
	GENERATE_PROPERTY(UI2DRenderer, FileName, Alpha, bUseAlpha, Layer, Position, Size, Rotation, TexSize, TexTranslate)

public:
	// common
	std::string FileName{ ".png" };
	float Alpha{ 1.f };
	bool bUseAlpha;

	// ui
	unsigned int Layer;
	VSM::Vector2 Position;
	VSM::Vector2 Size{ 100, 100 };
	float Rotation;

	// texture
	VSM::Vector2 TexSize{ 1, 1 };
	VSM::Vector2 TexTranslate;
};


class NeoUI2DRenderer : public Component
{
	GENERATE_BODY(NeoUI2DRenderer)
	GENERATE_PROPERTY(NeoUI2DRenderer, FileName, Alpha, bUseAlpha, Layer, TexSize, TexTranslate)

public:
	// common
	std::string FileName{ ".png" };
	float Alpha{ 1.f };
	bool bUseAlpha;

	// ui
	unsigned int Layer;

	// texture
	VSM::Vector2 TexSize{ 1, 1 };
	VSM::Vector2 TexTranslate;
};

