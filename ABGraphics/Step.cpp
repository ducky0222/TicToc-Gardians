#include "pch.h"

#include "Step.h"
#include "IDrawable.h"
#include "RenderGraph.h"
#include "RenderQueuePass.h"
#include "Job.h"

Step::Step(std::string targetPassName)
	: mTargetPassName{ std::move(targetPassName) }
{
}

void Step::Submit(IDrawable& drawable) const
{
	assert(mpTargetPass != nullptr);
	mpTargetPass->Accept(Rgph::Job{ this,&drawable });
}

void Step::InitReferences(const IDrawable& parent)
{
	for (auto& bindable : mBindables)
	{
		bindable->InitReference(parent);
	}
}

Step::Step(const Step& src)
	: mTargetPassName(src.mTargetPassName)
{
	mBindables.reserve(src.mBindables.size());

	for (auto& bindable : src.mBindables)
	{
		// ���� Ŭ�� �ؾ� �ϴ°�? �ϴ� ���� ������ �� ������Ʈ���� transform Cbuf�� �����ȴ�.
		// if (auto* pCloning = dynamic_cast<const ICloningBindable*>(bindable.get()))
		// {
		// 	mBindables.push_back(pCloning->Clone());
		// }
		mBindables.push_back(bindable);
	}
}

void Step::AddBindable(std::shared_ptr<IBindable> bind_in)
{
	mBindables.push_back(std::move(bind_in));
}

void Step::Bind(Graphics& graphics) const
{
	for (const auto& bindable : mBindables)
	{
		bindable->Bind(graphics);
	}
}

void Step::Link(Rgph::RenderGraph& renderGraph)
{
	assert(mpTargetPass == nullptr);
	mpTargetPass = &renderGraph.GetRenderQueue(mTargetPassName);
}
