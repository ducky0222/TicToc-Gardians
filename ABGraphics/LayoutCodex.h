#pragma once

#include "DynamicConstant.h"

namespace Dcb
{
	class LayoutCodex
	{
	public:
		static Dcb::CookedLayout Create(Dcb::RawLayout&& layout);

	private:
		static LayoutCodex& Get_();

	private:
		std::unordered_map<std::string, std::shared_ptr<Dcb::LayoutElement>> map;
	};
}