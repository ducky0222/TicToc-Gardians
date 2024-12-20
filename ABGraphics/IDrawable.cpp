#include "pch.h"

#include <assimp/scene.h>

#include "IDrawable.h"
#include "BindableCommon.h"
#include "BindableResourceManager.h"
#include "Material.h"

using namespace Bind;

IDrawable::IDrawable(eDrawableType drawableType)
	: mDrawableType(drawableType)
{
}

void IDrawable::Submit()
{
	for (const auto& technique : mTechniques)
	{
		technique.second.Submit(*this);
	}
}

void IDrawable::Bind(Graphics& graphics) const
{
	pTopology->Bind(graphics);
	pIndices->Bind(graphics);
	pVertices->Bind(graphics);
}

void IDrawable::LinkTechniques(Rgph::RenderGraph& renderGraph)
{
	for (auto& tech : mTechniques)
	{
		tech.second.Link(renderGraph);
	}
}

void IDrawable::AddTechnique(Technique technique)
{
	technique.InitReferences(*this);
	mTechniques.insert({ technique.GetName(), technique });
}

UINT IDrawable::GetIndexCount() const
{
	return pIndices->GetCount();
}
