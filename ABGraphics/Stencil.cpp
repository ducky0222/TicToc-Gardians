#include "pch.h"

#include "Stencil.h"

#include "BindableResourceManager.h"

namespace Bind
{
	Stencil::Stencil(Graphics& graphics, Mode mode)
		: mMode(mode)
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };

		switch (mode)
		{
		case Bind::Stencil::Mode::Off:
			break;
		case Bind::Stencil::Mode::Write:
			dsDesc.DepthEnable = FALSE;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			dsDesc.StencilEnable = TRUE;
			dsDesc.StencilWriteMask = 0xFF;
			dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			break;
		case Bind::Stencil::Mode::Mask:
			dsDesc.DepthEnable = FALSE;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			dsDesc.StencilEnable = TRUE;
			dsDesc.StencilReadMask = 0xFF;
			dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			break;
		case Bind::Stencil::Mode::DepthOff:
			dsDesc.DepthEnable = FALSE;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			break;
		case Bind::Stencil::Mode::DepthReversed:
			dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
			break;
		case Bind::Stencil::Mode::DepthFirst:
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			break;
		default:
			assert(false);
			break;
		}

		GetDevice(graphics)->CreateDepthStencilState(&dsDesc, &mpStencil);
	}

	std::shared_ptr<Stencil> Stencil::Create(Graphics& graphics, Mode mode)
	{
		return ResourceManager::Create<Stencil>(graphics, mode);
	}
	std::string Stencil::GenerateUID(Mode mode)
	{
		using namespace std::string_literals;
		const auto modeName = [mode]() {
			switch (mode) {
			case Mode::Off:
				return "off"s;
			case Mode::Write:
				return "write"s;
			case Mode::Mask:
				return "mask"s;
			case Mode::DepthOff:
				return "depth-off"s;
			case Mode::DepthReversed:
				return "depth-reversed"s;
			case Mode::DepthFirst:
				return "depth-first"s;
			}
			return "ERROR"s;
			};
		return typeid(Stencil).name() + "#"s + modeName();
	}

	void Stencil::Bind(Graphics& graphics)
	{
		GetContext(graphics)->OMSetDepthStencilState(mpStencil.Get(), 0xFF);
	}

	std::string Stencil::GetUID() const
	{
		return GenerateUID(mMode);
	}
}