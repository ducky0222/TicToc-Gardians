#pragma once
#include "BindingPass.h"
#include "Job.h"
#include <vector>

namespace Rgph
{
	class RenderQueuePass : public BindingPass
	{
	public:
		RenderQueuePass(std::string name, std::vector<std::shared_ptr<IBindable>> binds = {}, bool bUseAlpha = false)
			: BindingPass(std::move(name), std::move(binds))
			, mbAlphaUsed(bUseAlpha)
		{
		}
		virtual ~RenderQueuePass() { mJobs.clear(); }

		void Accept(Job job);
		void Execute(Graphics& graphics) const override;
		void Reset()  override;

	protected:
		std::vector<Job> mJobs;
		bool mbAlphaUsed;
	};
}