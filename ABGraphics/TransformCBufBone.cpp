#include "pch.h"
#include "TransformCBufBone.h"

#include "Mesh.h"
#include "Node.h"

namespace Bind
{
	TransformCBufBone::TransformCBufBone(Graphics& graphics, UINT slot)
	{
		pVcbuf = VertexConstantBuffer<BonePallete>::Create(graphics, slot);
	}

	void TransformCBufBone::Bind(Graphics& graphics)
	{
		assert(pParent != nullptr);

		const SkinnedMesh* skinnedMesh = dynamic_cast<const SkinnedMesh*>(pParent);
		const Matrix* boneMatrix = skinnedMesh->GetMatrix();
		BonePallete bonePallete;

		// 구조체로 카피
		assert(boneMatrix != nullptr);
		memcpy(bonePallete.BoneMat, boneMatrix, sizeof(bonePallete.BoneMat));
		pVcbuf->Update(graphics, bonePallete);
		pVcbuf->Bind(graphics);
	}

	void TransformCBufBone::InitReference(const IDrawable& parent)
	{
		pParent = &parent;
	}
}