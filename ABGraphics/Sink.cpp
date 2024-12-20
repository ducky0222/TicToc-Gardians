#include "pch.h"

#include "Sink.h"
#include "Source.h"
#include "IBindable.h"
#include "BufferResource.h"

namespace Rgph
{
	Sink::Sink(std::string registeredNameIn)
		: mRegisteredName(std::move(registeredNameIn))
	{
		if (mRegisteredName.empty())
		{
			assert(false);
		}

		// 이름의 모든 요소가 문자(로케일 반영)이거나 언더바이면 통과
		const bool nameCharsValid = std::all_of(mRegisteredName.begin(), mRegisteredName.end(), [](char c)
			{
				return std::isalnum(c) || c == '_';
			});

		if (!nameCharsValid || std::isdigit(mRegisteredName.front()))
		{
			assert(false);
		}
	}

	const std::string& Sink::GetRegisteredName() const
	{
		return mRegisteredName;
	}

	const std::string& Sink::GetPassName() const
	{
		return mPassName;
	}

	const std::string& Sink::GetOutputName() const
	{
		return mOutputName;
	}

	void Sink::SetTarget(const std::string& passName, const std::string& outputName)
	{
		assert(!passName.empty());
		assert(!outputName.empty());

		{
			// 패스 네임 유효성 체크
			const bool nameCharsValid = std::all_of(passName.begin(), passName.end(), [](char c)
				{
					return std::isalnum(c) || c == '_';
				});

			if (passName != "$" && (!nameCharsValid || std::isdigit(passName.front())))
			{
				assert(false);
			}

			mPassName = passName;
		}
		{
			// 출력 네임 유효성 체크
			const bool nameCharsValid = std::all_of(outputName.begin(), outputName.end(), [](char c)
				{
					return std::isalnum(c) || c == '_';
				});

			if (!nameCharsValid || std::isdigit(outputName.front()))
			{
				assert(false);
			}

			mOutputName = outputName;
		}
	}
}
