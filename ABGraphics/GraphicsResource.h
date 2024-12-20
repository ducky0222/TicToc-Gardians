#pragma once

class Graphics;

class GraphicsResource
{
protected:
	static ID3D11Device* GetDevice(Graphics& graphics);
	static ID3D11DeviceContext* GetContext(Graphics& graphics);
};

