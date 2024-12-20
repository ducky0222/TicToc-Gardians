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
		// ��ũ�� �ҽ��� ����Ǿ����� Ȯ��
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
		// �ϳ��� ���� �׷����� �ߺ��� �̸��� ������ �ȵ�
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
		// �ϳ��� ���� �׷����� �ߺ��� �̸��� ������ �ȵ�
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
		auto& sink = GetSink(registeredName); // �н� ���� �� ����ص� �̸����� ��ũ�� ã��

		auto targetSplit = Util::SplitString(target, "."); 
		if (targetSplit.size() != 2u) // ���� üũ, �׻� "<�������� Ȥ�� ����>.<target Naem>"
		{
			assert(false);
		}

		// �̸��� ��ȿ���� üũ�� �� �������ش�.
		sink.SetTarget(targetSplit[0], std::move(targetSplit[1]));
	}
}
