#pragma once

#include "IBindable.h"

namespace Bind
{
	class Viewport : public IBindable
	{
	public:
		Viewport(Graphics& graphics);
		Viewport(Graphics& graphics, float width, float height);

		void Bind(Graphics& graphics) override;

	private:
		D3D11_VIEWPORT mViewport;
	};
}