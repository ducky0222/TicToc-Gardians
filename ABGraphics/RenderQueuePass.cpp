#include "pch.h"

#include "RenderQueuePass.h"
#include "Graphics.h"
#include "IDrawable.h"

namespace Rgph
{
	void RenderQueuePass::Accept(Job job)
	{
		mJobs.push_back(job);
	}

	void RenderQueuePass::Execute(Graphics& graphics) const
	{
		ID3D11ShaderResourceView* SRVs[4] = { nullptr, };
		graphics.GetContext()->PSSetShaderResources(0, 4, SRVs);

		BindAll(graphics);

		for (size_t i = 0; i < IDrawable::UI_MAX_LAYER; ++i)
		{
			for (const auto& job : mJobs)
			{
				job.Execute(graphics, mbAlphaUsed, i);
			}
		}
	}

	void RenderQueuePass::Reset()
	{
		mJobs.clear();
	}
}
