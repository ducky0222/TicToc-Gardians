#pragma once

#include "Graphics.h"
#include "Technique.h"
#include "LightHelper.h"

class TechniqueProbe;
class Material;
struct aiMesh;

namespace Rgph
{
	class RenderGraph;
}

namespace Bind
{
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
	class InputLayout;
}

enum class RenderType
{
	Forward, Deferred
};

enum class eDrawableType
{
	Mesh,
	SkinnedMesh,
	Geometry,
	UI,
	UI3D,
	Debug,
	None,
};

class IDrawable
{
public:
	enum { UI_MAX_LAYER = 5 };

	struct AlphaData
	{
		float Alpha;
		bool bUsed;
	};

	struct InstanceData
	{
		Matrix Transform;
		Matrix TexTransform;
		AlphaData AlphaData;
	};

public:
	IDrawable(eDrawableType drawableType = eDrawableType::None);
	virtual ~IDrawable() = default;
	IDrawable(const IDrawable&) = delete;

	virtual void Submit();
	void Bind(Graphics& graphics) const;

	void LinkTechniques(Rgph::RenderGraph& renderGraph);
	void AddTechnique(Technique technique);

	void SetTransform(Matrix transform) {
		mTransform = transform;
	}
	void SetTexTransform(Matrix transform) { mTexTransform = transform; }
	void SetAlphaData(AlphaData alphaData) { mAlphaData = alphaData; }

	eDrawableType GetDrawableType() const { return mDrawableType; }
	const Matrix& GetTransform() const { return mTransform; }
	const Matrix& GetTexTransform() const { 
		return mTexTransform; 
	}
	const AlphaData& GetAlphaData() const { return mAlphaData; }
	UINT GetIndexCount() const;

protected:

	mutable Matrix mTransform;
	mutable Matrix mTexTransform;
	mutable AlphaData mAlphaData = { 1.f, false };

	const eDrawableType mDrawableType;
	std::shared_ptr<Bind::IndexBuffer> pIndices;
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::map<std::string, Technique> mTechniques;
};