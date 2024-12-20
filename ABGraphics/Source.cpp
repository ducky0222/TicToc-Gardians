#include "pch.h"

#include "Source.h"

namespace Rgph
{
	Source::Source(std::string nameIn)
		: mName(std::move(nameIn))
	{
		assert(!mName.empty());

		const bool nameCharsValid = std::all_of(mName.begin(), mName.end(), [](char c)
			{
				return std::isalnum(c) || c == '_';
			});

		if (!nameCharsValid || std::isdigit(mName.front()))
		{
			assert(false);
		}
	}

	// 두 함수는 재정의해서 사용해야 하며 기본 호출 시 assert(false)에 걸린다.
	std::shared_ptr<IBindable> Source::YieldBindable()
	{
		assert(false);
		throw;
	}

	std::shared_ptr<Bind::BufferResource> Source::YieldBuffer()
	{
		assert(false);
		throw;
	}

	const std::string& Source::GetName() const
	{
		return mName;
	}
}