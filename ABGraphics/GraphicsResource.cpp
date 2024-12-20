#include "pch.h"

#include "GraphicsResource.h"

#include "Graphics.h"

ID3D11Device* GraphicsResource::GetDevice(Graphics& graphics)
{
	return graphics.mpDevice.Get();
}

ID3D11DeviceContext* GraphicsResource::GetContext(Graphics& graphics)
{
	return graphics.mpContext.Get();
}