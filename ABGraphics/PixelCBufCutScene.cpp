#include "pch.h"
#include "PixelCBufCutScene.h"
#include "Model.h"

namespace Bind
{
	PixelCBufCutScene::PixelCBufCutScene(Graphics& graphics, UINT slot)
	{
		mPixelsCB = PixelConstantBuffer<Data>::Create(graphics, slot);
	}

	void PixelCBufCutScene::Bind(Graphics& graphics)
	{
		const CutSceneModel* model = dynamic_cast<const CutSceneModel*>(mParent);
		Data data;
		data.TextureIndex = model->GetIndex();

		mPixelsCB->Update(graphics, data);
		mPixelsCB->Bind(graphics);
	}

	void PixelCBufCutScene::InitReference(const IDrawable& parent)
	{
		mParent = &parent;
	}
}