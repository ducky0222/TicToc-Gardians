#pragma once

#include "IBindable.h"

namespace Bind
{
	class Topology : public IBindable
	{
	public:
		Topology(Graphics& graphics, D3D11_PRIMITIVE_TOPOLOGY type);
		virtual ~Topology() = default;

		static std::shared_ptr<Topology> Create(Graphics& graphics, D3D11_PRIMITIVE_TOPOLOGY type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		static std::string GenerateUID(D3D11_PRIMITIVE_TOPOLOGY type);

		void Bind(Graphics& graphics) override;

		std::string GetUID() const  override;

	protected:
		D3D11_PRIMITIVE_TOPOLOGY mType;
	};
}