#include "pch.h"

#include "RenderGraph.h"
#include "Pass.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "BindableCommon.h"
#include "RenderQueuePass.h"
#include "Sink.h"
#include "Source.h"
#include "Util.h"

namespace Rgph
{
	RenderGraph::RenderGraph(Graphics& graphics)
		: mRenderTarget(graphics.GetRenderTarget())
		, mBackBufferRenderTarget(graphics.GetBackBufferRenderTarget())
		, mPostEffectRenderTarget(graphics.GetPostEffectRenderTarget())
		, mMasterDepth(graphics.GetDepthStencil())
	{
		addGlobalSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", mRenderTarget));
		addGlobalSource(DirectBufferSource<Bind::RenderTarget>::Make("postEffectRenderTarget", mPostEffectRenderTarget));
		addGlobalSource(DirectBufferSource<Bind::RenderTarget>::Make("backbufferRenderTarget", mBackBufferRenderTarget));
		addGlobalSource(DirectBufferSource<Bind::DepthStencil>::Make("masterDepth", mMasterDepth));

		addGlobalSink(DirectBufferSink<Bind::RenderTarget>::Make("backbufferRenderTarget", mBackBufferRenderTarget));
	}

	void RenderGraph::Execute(Graphics& graphics)
	{
		assert(mbIsFinalized);

		for (auto& p : mPasses)
		{
			p->Execute(graphics);
		}
	}

	void RenderGraph::Reset()
	{
		assert(mbIsFinalized);

		for (auto& p : mPasses)
		{
			p->Reset();
		}
	}

	RenderQueuePass& RenderGraph::GetRenderQueue(const std::string& passName)
	{
		// todo : ����ó�� �������� ����
		try
		{
			for (const auto& p : mPasses)
			{
				if (p->GetName() == passName)
				{
					return dynamic_cast<RenderQueuePass&>(*p);
				}
			}
		}
		catch (std::bad_cast&)
		{
			assert(false);
		}
		assert(false);
	}

	void RenderGraph::finalize()
	{
		assert(!mbIsFinalized);

		// �����׷����� ����� ��� �н��� ���� ������ Ȯ����
		for (const auto& p : mPasses)
		{
			p->Finalize();
		}

		linkGlobalSinks();
		mbIsFinalized = true;
	}


	void RenderGraph::addGlobalSource(std::unique_ptr<Source> out)
	{
		mGlobalSources.push_back(std::move(out));
	}

	void RenderGraph::addGlobalSink(std::unique_ptr<Sink> in)
	{
		mGlobalSinks.push_back(std::move(in));
	}

	void RenderGraph::appendPass(std::unique_ptr<Pass> pass)
	{
		assert(!mbIsFinalized);

		// ���� �׷������� ������ �н��� ������ �� ����.
		for (const auto& p : mPasses)
		{
			if (pass->GetName() == p->GetName())
			{
				assert(false);
			}
		}

		// �н��� �߰��� ������ �ҽ�/��ũ ������ ������
		linkSinks(*pass);

		mPasses.push_back(std::move(pass));
	}

	void RenderGraph::setSinkTarget(const std::string& sinkName, const std::string& target)
	{
		const auto finder = [&sinkName](const std::unique_ptr<Sink>& p) -> bool
			{
				return p->GetRegisteredName() == sinkName;
			};

		const auto finded = std::find_if(mGlobalSinks.begin(), mGlobalSinks.end(), finder);

		if (finded == mGlobalSinks.end())
		{
			assert(false);
		}

		auto targetSplit = Util::SplitString(target, ".");

		if (targetSplit.size() != 2u)
		{
			assert(false);
		}

		(*finded)->SetTarget(targetSplit[0], targetSplit[1]);
	}

	Pass* RenderGraph::findPassByNameOrNull(const std::string& name)
	{
		for (unique_ptr<Pass>& pass : mPasses)
		{
			if (pass->GetName() == name)
			{
				return pass.get();
			}
		}

		return nullptr;
	}

	void RenderGraph::linkSinks(Pass& pass)
	{
		for (auto& sink : pass.GetSinks())
		{
			const auto& inputSourcePassName = sink->GetPassName();

			if (inputSourcePassName == "$")
			{
				bool bound = false;

				for (auto& source : mGlobalSources)
				{
					if (source->GetName() == sink->GetOutputName())
					{
						sink->BindSink(*source);
						bound = true;

						break;
					}
				}

				// ���� �׷����� ��ũ ������ �ݵ�� �����ؾ� �Ѵ�.
				if (!bound)
				{
					assert(false);
				}
			}
			else
			{
				bool bound = false;

				for (auto& pass : mPasses)
				{
					if (pass->GetName() == inputSourcePassName)
					{
						auto& source = pass->GetSource(sink->GetOutputName());
						sink->BindSink(source);
						bound = true;

						break;
					}
				}

				// ���� �׷����� ��ũ ������ �ݵ�� �����ؾ� �Ѵ�.
				if (!bound)
				{
					assert(false);
				}
			}
		}
	}

	void RenderGraph::linkGlobalSinks()
	{
		for (auto& globalSink : mGlobalSinks)
		{
			const auto& inputSourcePassName = globalSink->GetPassName();

			for (auto& existingPass : mPasses)
			{
				if (existingPass->GetName() == inputSourcePassName)
				{
					auto& source = existingPass->GetSource(globalSink->GetOutputName());
					globalSink->BindSink(source);

					break;
				}
			}
		}
	}


}