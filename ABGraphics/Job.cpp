#include "pch.h"
#include "Job.h"
#include "Graphics.h"
#include "Step.h"
#include "IDrawable.h"
#include "Mesh.h"
#include "Model.h"

namespace Rgph
{
	Job::Job(const Step* pStep, IDrawable* pDrawable)
		: mpDrawable{ pDrawable }
		, mpStep{ pStep }
	{
	}

	void Job::Execute(Graphics& graphics, bool hasAlpha, size_t layer) const
	{
		mpDrawable->Bind(graphics);

		switch (mpDrawable->GetDrawableType())
		{
		case eDrawableType::Mesh:
		{
			if (layer != 0) { return; }

			Mesh* model = static_cast<Mesh*>(mpDrawable);
			auto& instanceArr = !hasAlpha ? model->mInstanceDataArr : model->mHasAlphaInstanceDataArr;

			for (const auto& instanceData : instanceArr)
			{
				model->SetTransform(instanceData.Transform);
				model->SetAlphaData(instanceData.AlphaData);
				model->SetTexTransform(instanceData.TexTransform);

				mpStep->Bind(graphics);
				graphics.DrawIndexed(mpDrawable->GetIndexCount());
			}

			break;
		}
		case eDrawableType::SkinnedMesh:
		{
			if (layer != 0) { return; }

			SkinnedMesh* model = static_cast<SkinnedMesh*>(mpDrawable);
			auto& instanceArr = !hasAlpha ? model->mInstanceDataArr : model->mHasAlphaInstanceDataArr;

			for (const auto& instanceData : instanceArr)
			{
				model->SetTransform(instanceData.DrawableInstanceData.Transform);
				model->SetAlphaData(instanceData.DrawableInstanceData.AlphaData);
				model->SetTexTransform(instanceData.DrawableInstanceData.TexTransform);
				
				// �𵨿� ī��
				memcpy(model->mBonePallet, instanceData.BonePallet, sizeof(instanceData.BonePallet));

				mpStep->Bind(graphics);
				graphics.DrawIndexed(mpDrawable->GetIndexCount());
			}

			break;
		}
		case eDrawableType::Geometry:
		{
			if (layer != 0) { return; }

			GeometryModel* model = static_cast<GeometryModel*>(mpDrawable);
			auto& instanceArr = !hasAlpha ? model->mInstanceDataArr : model->mHasAlphaInstanceDataArr;

			for (const auto& instanceData : instanceArr)
			{
				model->SetTransform(instanceData.Transform);
				model->SetAlphaData(instanceData.AlphaData);
				model->SetTexTransform(instanceData.TexTransform);

				mpStep->Bind(graphics);
				graphics.DrawIndexed(mpDrawable->GetIndexCount());
			}

			break;
		}
		case eDrawableType::UI:
		{
			UIModel* model = static_cast<UIModel*>(mpDrawable);
			auto& instance2DArr = !hasAlpha ? model->mInstanceDataArr : model->mHasAlphaInstanceDataArr;
			auto& instanceArr = instance2DArr[layer];

			// �׸��� ������ ����ȭ �ѰŶ� Layer�� ���ε� �� �ʿ䰡 ����.
			for (const auto& instanceData : instanceArr)
			{
				model->SetTransform(instanceData.Transform);
				model->SetTexTransform(instanceData.TexTransform);
				model->SetAlphaData(instanceData.AlphaData);
				// model->SetLayer(instanceData.Layer);

				mpStep->Bind(graphics);
				graphics.DrawIndexed(mpDrawable->GetIndexCount());
			}

			break;
		}
		case eDrawableType::UI3D:
		{
			UI3DModel* model = static_cast<UI3DModel*>(mpDrawable);
			auto& instance2DArr = !hasAlpha ? model->mInstanceDataArr : model->mHasAlphaInstanceDataArr;
			auto& instanceArr = instance2DArr[layer];

			// �׸��� ������ ����ȭ �ѰŶ� Layer�� ���ε� �� �ʿ䰡 ����.
			// �׸��� ������ ��ü������ ����ȭ �Ǿ�� �Ѵ�.
			for (const auto& instanceData : instanceArr)
			{
				model->SetTexTransform(instanceData.TexTransform);
				model->SetAlphaData(instanceData.AlphaData);
				model->SetLookVector(instanceData.LookVector);
				model->SetPosition(instanceData.Position);
				model->SetSize(instanceData.Size);
				model->SetRotation(instanceData.Rotation);
				// model->SetLayer(instanceData.Layer);

				mpStep->Bind(graphics);
				graphics.DrawIndexed(mpDrawable->GetIndexCount());
			}

			break;
		}
		default:
		{
			mpStep->Bind(graphics);
			graphics.DrawIndexed(mpDrawable->GetIndexCount());

			break;
		}
		}
	}
}