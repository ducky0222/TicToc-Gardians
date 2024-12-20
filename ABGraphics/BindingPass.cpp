#include "pch.h"

#include "BindingPass.h"
#include "IBindable.h"
#include "RenderTarget.h"
#include "DepthStencil.h"

namespace Rgph
{
	BindingPass::BindingPass(std::string name, std::vector<std::shared_ptr<IBindable>> binds)
		: Pass(std::move(name))
		, binds(std::move(binds))
	{
	}

	void BindingPass::AddBind(std::shared_ptr<IBindable> bind)
	{
		binds.push_back(std::move(bind));
	}

	void BindingPass::BindAll(Graphics& graphics) const
	{
		bindBufferResources(graphics);

		for (auto& bind : binds)
		{
			bind->Bind(graphics);
		}
	}

	void BindingPass::Finalize()
	{
		Pass::Finalize();
		assert(mRenderTarget || mDepthStencil);
	}

	void BindingPass::bindBufferResources(Graphics& graphics) const
	{
		if (mRenderTarget)
		{
			mRenderTarget->BindBuffer(graphics, mDepthStencil.get());
		}
		else
		{
			mDepthStencil->BindBuffer(graphics);
		}
	}
}