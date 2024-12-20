#pragma once
#include "IBindable.h"

namespace Bind
{
	class NullPixelShader : public IBindable
	{
	public:
		NullPixelShader(Graphics& graphics);
		virtual ~NullPixelShader() = default;

		static std::shared_ptr<NullPixelShader> Create(Graphics& graphics);
		static std::string GenerateUID();
		
		void Bind(Graphics& graphics) override;
		
		std::string GetUID() const override;
	};
}