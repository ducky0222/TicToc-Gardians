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
		// todo : 예외처리 로직으로 수정
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

		// 랜더그래프에 연결된 모든 패스에 대해 연결을 확인함
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

		// 랜더 그래프에는 동일한 패스가 존재할 수 없다.
		for (const auto& p : mPasses)
		{
			if (pass->GetName() == p->GetName())
			{
				assert(false);
			}
		}

		// 패스가 추가될 때마다 소스/싱크 매핑을 시행함
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

				// 랜더 그래프의 싱크 연결은 반드시 성공해야 한다.
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

				// 랜더 그래프의 싱크 연결은 반드시 성공해야 한다.
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