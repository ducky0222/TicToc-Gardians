#pragma once
#include "Component.h"

class SkyBoxRenderer : public Component
{
	GENERATE_BODY(SkyBoxRenderer)
		GENERATE_PROPERTY(SkyBoxRenderer, bUseSkyBox, bUseIBL, SkyBoxName, DiffuseName, SpecularName, BRDF_LUTName)

public:
	bool bUseSkyBox{ false };
	bool bUseIBL{ false };
	std::string SkyBoxName;
	std::string DiffuseName;
	std::string SpecularName;
	std::string BRDF_LUTName;
};

