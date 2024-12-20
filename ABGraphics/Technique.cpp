#include "pch.h"

#include "Technique.h"
#include "IDrawable.h"

Technique::Technique(std::string name, bool startmbIsActive)
	: mbIsActive(startmbIsActive)
	, mName(name)
{
}

void Technique::Submit(IDrawable& drawable) const
{
	if (mbIsActive)
	{
		for (const auto& step : mSteps)
		{
			step.Submit(drawable);
		}
	}
}

void Technique::Link(Rgph::RenderGraph& rg)
{
	for (auto& step : mSteps)
	{
		step.Link(rg);
	}
}

void Technique::InitReferences(const IDrawable& parent)
{
	for (auto& step : mSteps)
	{
		step.InitReferences(parent);
	}
}


void Technique::AddStep(Step step)
{
	mSteps.push_back(std::move(step));
}

void Technique::SetActiveState(bool bIsActive_in)
{
	mbIsActive = bIsActive_in;
}

const std::string& Technique::GetName() const
{
	return mName;
}

bool Technique::IsActive() const
{
	return mbIsActive;
}
