#include "pch.h"

#include "Pass.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "Sink.h"
#include "Source.h"
#include "Util.h"

namespace Rgph
{
	Pass::Pass(std::string name)
		: mName(std::move(name))
	{
	}

	void Pass::Reset()
	{
	}
	void Pass::Finalize()
	{
		// 싱크와 소스가 연결되었는지 확인
		for (auto& in : mSinks)
		{
			in->PostLinkValidate();
		}
		for (auto& out : mSources)
		{
			out->PostLinkValidate();
		}
	}

	const std::string& Pass::GetName() const
	{
		return mName;
	}
	const std::vector<std::unique_ptr<Sink>>& Pass::GetSinks() const
	{
		return mSinks;
	}

	Source& Pass::GetSource(const std::string& name) const
	{
		for (auto& src : mSources)
		{
			if (src->GetName() == name)
			{
				return *src;
			}
		}

		assert(false);
	}

	Sink& Pass::GetSink(const std::string& registeredName) const
	{
		for (auto& sink : mSinks)
		{
			if (sink->GetRegisteredName() == registeredName)
			{
				return *sink;
			}
		}

		assert(false);
	}

	void Pass::registerSink(std::unique_ptr<Sink> sink)
	{
		// 하나의 랜더 그래프에 중복된 이름이 있으면 안됨
		for (auto& si : mSinks)
		{
			if (si->GetRegisteredName() == sink->GetRegisteredName())
			{
				assert(false);
			}
		}

		mSinks.push_back(std::move(sink));
	}

	void Pass::registerSource(std::unique_ptr<Source> source)
	{
		// 하나의 랜더 그래프에 중복된 이름이 있으면 안됨
		for (auto& src : mSources)
		{
			if (src->GetName() == source->GetName())
			{
				assert(false);
			}
		}

		mSources.push_back(std::move(source));
	}

	void Pass::SetSinkLinkage(const std::string& registeredName, const std::string& target)
	{
		auto& sink = GetSink(registeredName); // 패스 생성 시 등록해둔 이름으로 싱크를 찾음

		auto targetSplit = Util::SplitString(target, "."); 
		if (targetSplit.size() != 2u) // 포맷 체크, 항상 "<이전버퍼 혹은 전역>.<target Naem>"
		{
			assert(false);
		}

		// 이름의 유효성을 체크한 후 설정해준다.
		sink.SetTarget(targetSplit[0], std::move(targetSplit[1]));
	}
}
