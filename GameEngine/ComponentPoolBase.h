#pragma once

#include <bitset>

class ComponentPoolBase
{
public:

	enum BaseType
	{
		Updatable = 0x01,
		Renderable = 0x02,
		FixedUpdatable = 0x04,

		Size = 3
	};

	virtual void Initialize(size_t arraySize) = 0;
	virtual ~ComponentPoolBase() = default;

public:
	std::bitset<BaseType::Size> m_baseType = 0;
};

