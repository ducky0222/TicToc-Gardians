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

	// �� �Լ��� �������ؼ� ����ؾ� �ϸ� �⺻ ȣ�� �� assert(false)�� �ɸ���.
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