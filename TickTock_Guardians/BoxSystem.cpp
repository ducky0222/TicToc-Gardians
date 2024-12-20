#include "pch.h"
#include "BoxSystem.h"
#include "Transform.h"
#include "Box.h"
#include "Tag.h"
#include "Character.h"
#include "ConveyorBelt.h"
#include <HysxEngine.h>
#include "BoxGenerator.h"
#include <RigidBody.h>
#include "Replayer.h"
#include "Replayer.h"
#include "OnOff.h"
#include <AnimationSystem.h>
#include <UIController.h>
#include "Button.h"
#include "../SoundEngine/ISoundEngine.h"
#include "NeoConveyorBelt.h"

BoxSystem::BoxSystem(GameObjectManager* goManager)
	:System(goManager)
{
	EventManager::Subscribe("OnTriggerBox", CreateListenerInfo(&BoxSystem::OnTriggerBox));
	//EventManager::Subscribe("OnExitBox", CreateListenerInfo(&BoxSystem::OnExitBox));
	//EventManager::Subscribe("OnCollisionEnter", CreateListenerInfo(&BoxSystem::OnCollisionEnter));
	EventManager::Subscribe("OnCollisionExit", CreateListenerInfo(&BoxSystem::OnCollisionExit));
	EventManager::Subscribe("OnResetScene", CreateListenerInfo(&BoxSystem::OnResetScene));
	EventManager::Subscribe("OnBoxEffectActive", CreateListenerInfo(&BoxSystem::OnBoxEffectActive));
	EventManager::Subscribe("OnCollisionStay", CreateListenerInfo(&BoxSystem::OnCollisionStay));
}

void BoxSystem::FixedUpdate(float deltaTime)
{
	//for (Character& beaver : ComponentItr<Character>())	//��� �ڽ� ������Ʈ���ؼ�.


	for (Box& box : ComponentItr<Box>())	//��� �ڽ� ������Ʈ���ؼ�.
	{
		if (!box.Sibling<OnOff>(m_GameObjectManager)->IsOn)
			continue;
		auto boxRigidbody =box.Sibling<RigidBody>(m_GameObjectManager);

		if (!box.IsMoving && /*!box.IsPushing &&*/ !box.IsGenerating)
		{
			if (box.OnGenerated)
			{

				box.SavedTarget = box.NewBoxTarget;
				auto Boxpose = box.Sibling<Transform>(m_GameObjectManager);
				auto TargetPose = m_GameObjectManager->QueryComponent<Transform>(box.SavedTarget);
				auto TargetBoxGenerator = m_GameObjectManager->QueryComponent<BoxGenerator>(box.SavedTarget);
				box.DeparturePosition = TargetBoxGenerator->GeneratePose - VSM::Vector3{ 0,2.1f,0 };
				box.ArrivalPosition = TargetBoxGenerator->GeneratePose;
				box.IsGenerating = true;
				box.OnGenerated = false;
				box.FlowingTime = 0;
				std::any newAnydata = box.GameObjectId;
				EventManager::ScheduleEvent("ChangeDynamicToStatic", newAnydata);
			}
			else if (box.OnMoveWalk)
			{
				box.SavedTarget = box.NewBoxTarget;

				auto Boxpose = box.Sibling<Transform>(m_GameObjectManager);
				auto TargetPose = m_GameObjectManager->QueryComponent<Transform>(box.SavedTarget);
				auto TargetConveyorBelt = m_GameObjectManager->QueryComponent<NeoConveyorBelt>(box.SavedTarget);
				VSM::Vector2 direction = { TargetConveyorBelt->dir.x,TargetConveyorBelt->dir.z };

				VSM::Vector3 MoveWay = { direction.x,0,direction.y };

				box.DeparturePosition = Boxpose->LocalPosition;
				box.ArrivalPosition = box.DeparturePosition + MoveWay * box.MoveLength;

				box.IsMoving = true;
				box.OnPushed = false;
				box.OnMoveWalk = false;
				box.FlowingTime = 0;
				box.OnMoveWalk = false;
				box.IsMoving = true;

			}
			
			/*else if (box.OnPushed == true)
			{
				box.SavedTarget = box.NewBoxTarget;

				auto targetRigidBody = m_GameObjectManager->QueryComponent<RigidBody>(box.SavedTarget);
				auto targetTransform = m_GameObjectManager->QueryComponent<Transform>(box.SavedTarget);
				auto targetCharacter = m_GameObjectManager->QueryComponent<Character>(box.SavedTarget);
				auto targetReplayer = m_GameObjectManager->QueryComponent<Replayer>(box.SavedTarget);
				float margin_y = targetTransform->WorldPosition.y - box.Sibling<Transform>(m_GameObjectManager)->WorldPosition.y;
				bool IsSamefloor = (margin_y < 1.f && margin_y>0.6f);
				VSM::Vector3 boxMovedirection = (box.ArrivalPosition - box.DeparturePosition);

				// if (!targetReplayer->IsFinishReplaying && targetReplayer->ReplayIndex >= targetReplayer->Tracer.size())

				if (targetReplayer->ReplayIndex < targetReplayer->TraceSize && targetReplayer->IsReplaying)	//������ ���� ĳ���Ϳ� �갡 ���.
				{
					VSM::Vector3 replayerDirection = VSM::Vector3::Zero;

					if (targetReplayer->Tracer[targetReplayer->ReplayIndex][static_cast<uint32_t>(ACTION::LEFT)])
					{
						replayerDirection += VSM::Vector3(-0.98f, 0, 0.99f);
					}
					if (targetReplayer->Tracer[targetReplayer->ReplayIndex][static_cast<uint32_t>(ACTION::RIGHT)])
					{
						replayerDirection += VSM::Vector3(0.99f, 0, -0.98f);
					}
					if (targetReplayer->Tracer[targetReplayer->ReplayIndex][static_cast<uint32_t>(ACTION::UP)])
					{
						replayerDirection += VSM::Vector3(1, 0, 1);
					}
					if (targetReplayer->Tracer[targetReplayer->ReplayIndex][static_cast<uint32_t>(ACTION::DOWN)])
					{
						replayerDirection += VSM::Vector3(-1, 0, -1);
					}
					replayerDirection.Normalize();

					VSM::Vector2 replayerDirectionXZ = { replayerDirection.x,replayerDirection.z };
					VSM::Vector2 boxMovedirectionXZ = { boxMovedirection.x,boxMovedirection.z };
					boxMovedirectionXZ.Normalize();
					float DotResult = replayerDirectionXZ.Dot(boxMovedirectionXZ);
					if (DotResult > 0.6f && replayerDirectionXZ.Length() > 0.1f && IsSamefloor)
					{
						box.FlowingTime += deltaTime;
						targetCharacter->IsPushing = true;
					}
					else
					{
						box.FlowingTime = 0;
						targetCharacter->IsPushing = false;

					}



					// �� ĳ���Ͱ� ���÷��� ���̰� ���� �����ϰ� ���� ���� ����
				}
				else if (targetCharacter->IsControllable)
				{
					VSM::Vector3 characterDirection = VSM::Vector3::Zero;
					std::bitset<8> directionBitset = targetCharacter->DirectionBitset;

					if (directionBitset[static_cast<uint32_t>(ACTION::LEFT)])
					{
						characterDirection += VSM::Vector3(-0.98f, 0, 0.99f);
					}
					if (directionBitset[static_cast<uint32_t>(ACTION::RIGHT)])
					{
						characterDirection += VSM::Vector3(0.99f, 0, -0.98f);
					}
					if (directionBitset[static_cast<uint32_t>(ACTION::UP)])
					{
						characterDirection += VSM::Vector3(1, 0, 1);
					}
					if (directionBitset[static_cast<uint32_t>(ACTION::DOWN)])
					{
						characterDirection += VSM::Vector3(-1, 0, -1);
					}
					characterDirection.Normalize();
					VSM::Vector2 characterDirectionXZ = { characterDirection.x,characterDirection.z };
					VSM::Vector2 boxMovedirectionXZ = { boxMovedirection.x,boxMovedirection.z };
					boxMovedirectionXZ.Normalize();
					float DotResult = characterDirectionXZ.Dot(boxMovedirectionXZ);


					if (DotResult > 0.6f && characterDirection.Length() > 0.1f && IsSamefloor)
					{
						box.FlowingTime += deltaTime;
						targetCharacter->IsPushing = true;
					}
					else
					{
						box.FlowingTime = 0;
						targetCharacter->IsPushing = false;

					}

					// �� ĳ���Ͱ� ���÷��� ������ �ʰ�, ���� �����ϰ� �ִ� ����
				}
				else
				{
					// �� ĳ���Ͱ� ���÷��� �ߵ� �ƴϰ�, ���۵� ���� �ʴ� ���� (���� ������ �ϴ� ���� ����)
				}


				if (box.FlowingTime > box.ReadyToPush)
				{
					
					targetCharacter->IsPushed = true;
					auto testpose = box.Sibling<RigidBody>(m_GameObjectManager);
					boxMovedirection.Normalize();
					uint32_t raycastFront = Physics::GetInstance()->RaycastFromID_Nearest(box.GameObjectId, boxMovedirection, 1.2f);
					uint32_t raycastDown = Physics::GetInstance()->RaycastFromPosition_Nearest(box.ArrivalPosition, { 0,-1,0 }, 10.f);
					if (raycastFront == 0 && raycastDown != 0)
					{
						box.IsPushing = true;
						box.OnMoveWalk = false;
						box.OnPushed = false;
						box.FlowingTime = 0;
					}
					///�ִϸ��̼� ����
					TagType UITagType = {};
					{
						if (abs(box.ArrivalPosition.x - box.DeparturePosition.x) > abs(box.ArrivalPosition.z - box.DeparturePosition.z))
							if (box.ArrivalPosition.x - box.DeparturePosition.x > 0)
								UITagType = TagType::BoxUI_RightUp;
							else
								UITagType = TagType::BoxUI_LeftDown;
						else if (abs(box.ArrivalPosition.x - box.DeparturePosition.x) < abs(box.ArrivalPosition.z - box.DeparturePosition.z))
							if (box.ArrivalPosition.z - box.DeparturePosition.z > 0)
								UITagType = TagType::BoxUI_LeftUp;
							else
								UITagType = TagType::BoxUI_RightDown;
					}		//UIBundle ���ϱ�.
					pair<uint32_t, TagType> UIdata = std::make_pair(box.GameObjectId, UITagType);
					EventManager::ScheduleEvent("OnBoxEffectActive", UIdata);


					box.OnPushed = false;
					box.FlowingTime = 0;




				}
			}*/
			else
				box.FlowingTime = 0;
				
		}
		else if (box.IsGenerating)
		{
			auto pose = box.Sibling<Transform>(m_GameObjectManager);
			box.FlowingTime += deltaTime;
			if (box.FlowingTime < box.GeneratingTime)
			{

				float percent = box.FlowingTime / box.GeneratingTime;
				VSM::Vector3 temppose = {};
				temppose = VSM::Vector3::Lerp(box.DeparturePosition, box.ArrivalPosition, (1 - (percent - 1) * (percent - 1)));
				pose->LocalPosition.x = temppose.x;
				pose->LocalPosition.y = temppose.y;

				pose->LocalPosition.z = temppose.z;
			}
			else
			{
				pose->LocalPosition.x = box.ArrivalPosition.x;
				pose->LocalPosition.y = box.ArrivalPosition.y;
				pose->LocalPosition.z = box.ArrivalPosition.z;

				std::any newdata = box.GameObjectId;
				EventManager::ScheduleEvent("ChangeStaticToDynamic", newdata);

				box.IsGenerating = false;
				box.IsMoving = false;
				box.FlowingTime = 0;
			}
		}
		//�ڽ��� push �̺�Ʈ�� �߻��ϴ°�?
		/*
		else if (box.IsPushing)
		{
			auto pose = box.Sibling<Transform>(m_GameObjectManager);
			box.FlowingTime += deltaTime;
			if (box.FlowingTime < box.Pushtime)
			{
				float percent = box.FlowingTime / box.Pushtime;
				VSM::Vector3 temppose = {};
				temppose = VSM::Vector3::Lerp(box.DeparturePosition, box.ArrivalPosition, (1 - (percent - 1) * (percent - 1)));
				pose->LocalPosition.x = temppose.x;
				pose->LocalPosition.z = temppose.z;
			}
			else
			{

				pose->LocalPosition.x = box.ArrivalPosition.x;
				pose->LocalPosition.z = box.ArrivalPosition.z;
				box.IsPushing = false;
				box.IsMoving = false;
				box.FlowingTime = 0;
				auto targetcharacter = m_GameObjectManager->QueryComponent<Character>(box.SavedTarget);
				targetcharacter->IsPushed = false;
				targetcharacter->IsPushing = false;

			}
		}
		*/
		//�ڽ��� ������ũ�� ������ �̺�Ʈ�� �߻��ϴ°�?
		else if (box.IsMoving)
		{
			auto pose = box.Sibling<Transform>(m_GameObjectManager);
			box.FlowingTime += deltaTime;
			if (box.FlowingTime < box.ConveyorBeltTime)
			{

				float percent = box.FlowingTime / box.ConveyorBeltTime;
				VSM::Vector3 temppose = {};

				temppose = VSM::Vector3::Lerp(box.DeparturePosition, box.ArrivalPosition, percent);
				pose->LocalPosition.x = temppose.x;
				pose->LocalPosition.z = temppose.z;

			}
			else
			{
				pose->LocalPosition.x = box.ArrivalPosition.x;
				pose->LocalPosition.z = box.ArrivalPosition.z;

				box.IsPushing = false;
				box.IsMoving = false;
				box.FlowingTime = 0;
				box.OnMoveWalk = false;
			}
		}

		if (box.Sibling<Children>(m_GameObjectManager))
		{
			auto UIBundles = box.Sibling<Children>(m_GameObjectManager)->ChildrenID;
			for (auto UIbundle : UIBundles)
			{
				auto UI_IDs = m_GameObjectManager->QueryComponent<Children>(UIbundle)->ChildrenID;
				for (auto UI_ID : UI_IDs)
				{
					if (m_GameObjectManager->QueryComponent<SpriteController>(UI_ID)->bIsFinishTraversal)
					{
						m_GameObjectManager->QueryComponent<OnOff>(UI_ID)->IsOn = false;
					}

				}
			}
		}




		if (boxRigidbody->Velocity.y<-1.f)
		{
			box.LandingSpriteOn = true;
		}
		else if(box.LandingSpriteOn==true&& boxRigidbody->Velocity.y<0.2f)
		{


			TagType UITagType = TagType::BoxUI_Landing;

			box.LandingSpriteOn = false;
			pair<uint32_t, TagType> UIdata = std::make_pair(box.GameObjectId, UITagType);
			EventManager::ScheduleEvent("OnBoxEffectActive", UIdata);
		}

	}
}


void BoxSystem::OnCollisionEnter(std::any data)
{
	//const auto [lhs, rhs] = any_cast<std::pair<uint32_t, uint32_t>>(data);		/// �� = ��� , �� = ��


	//auto lhsTag = m_GameObjectManager->QueryComponent<Tag>(lhs);		///���
	//auto rhstag = m_GameObjectManager->QueryComponent<Tag>(rhs);		///��
	//if (lhsTag != nullptr && rhstag != nullptr)
	//{
	//	if (rhstag->tag == TagType::Box)
	//	{
	//		switch (lhsTag->tag)
	//		{
	//		case TagType::Button:
	//		{
	//			//EventManager::ScheduleEvent("OnTriggerButton", lhs);

	//			int a = 5;
	//		}
	//		break;
	//		default:
	//			break;
	//		}
	//	}

	//}



}

void BoxSystem::OnCollisionStay(std::any data)
{
	const auto [lhs, rhs] = any_cast<std::pair<uint32_t, uint32_t>>(data);		/// �� = ��� , �� = ��

	auto lhsTag = m_GameObjectManager->QueryComponent<Tag>(lhs);		///���
	auto rhsTag = m_GameObjectManager->QueryComponent<Tag>(rhs);		///��

	if (lhsTag != nullptr && rhsTag != nullptr)
	{
		if (rhsTag->tag == TagType::Box)
		{
			switch (lhsTag->tag)
			{
			case TagType::Button:
			{
				auto lhsTransform = lhsTag->Sibling<Transform>(m_GameObjectManager);	//���
				auto rhsTransform = rhsTag->Sibling<Transform>(m_GameObjectManager);	// ��
				Vector3 margin = lhsTransform->WorldPosition - rhsTransform->WorldPosition;
				if (abs(margin.x) < 1.0f && abs(margin.z) < 1.0f&& !rhsTag->Sibling<Box>(m_GameObjectManager)->ButtonTriggerOn)
				{
					EventManager::ScheduleEvent("OnButtonClicked", lhs);
					rhsTag->Sibling<Box>(m_GameObjectManager)->ButtonTriggerOn = true;
				}
			}
			break;
			default:
				break;
			}
		}

	}


}

void BoxSystem::OnCollisionExit(std::any data)
{

	const auto [lhs, rhs] = any_cast<std::pair<uint32_t, uint32_t>>(data);		/// �� = ��� , �� = ��

	auto lhsTag = m_GameObjectManager->QueryComponent<Tag>(lhs);		///���
	auto rhsTag = m_GameObjectManager->QueryComponent<Tag>(rhs);		///��
	if (lhsTag != nullptr && rhsTag != nullptr)
	{
		if (rhsTag->tag == TagType::Box)
		{
			switch (lhsTag->tag)
			{
			case TagType::Button:
			{

				if (rhsTag->Sibling<Box>(m_GameObjectManager)->ButtonTriggerOn)
				{
				EventManager::ScheduleEvent("OnButtonUnClicked", lhs);
					rhsTag->Sibling<Box>(m_GameObjectManager)->ButtonTriggerOn = false;
				}
			}
			break;
			default:
				break;
			}
		}

	}
}

void BoxSystem::OnTriggerBox(std::any data)
{
	// data �� ���� ��?
	const auto [lhs, rhs] = any_cast<std::pair<uint32_t, uint32_t>>(data);		/// �� = ��� , �� = ��
	// �Ѵ� nullptr�� �ȵ����� Ȯ���ϰ�  �ڵ�����ϱ�.
	auto lhsTag = m_GameObjectManager->QueryComponent<Tag>(lhs);		///���
	auto rhstag = m_GameObjectManager->QueryComponent<Tag>(rhs);		///��

	if (lhsTag != nullptr && rhstag != nullptr)				//�Ѵ� �±װ� nullptr�� �ƴ���?
	{
		switch (lhsTag->tag)
		{
		/*case TagType::Beaver:
		{
			auto rhsBox = rhstag->Sibling<Box>(m_GameObjectManager);
			auto lhscharacter = lhsTag->Sibling<Character>(m_GameObjectManager);
			if (rhsBox != nullptr && lhscharacter != nullptr)					//Box Character comp�� �ִ���?
			{
				if (rhsBox->IsPushing == false && rhsBox->IsMoving == false &&rhsBox->IsGenerating==false)
				{
					rhsBox->NewBoxTarget = lhs;
					rhsBox->OnPushed = true;



					auto boxTransform = rhsBox->Sibling<Transform>(m_GameObjectManager);
					auto targetTransform = m_GameObjectManager->QueryComponent<Transform>(rhsBox->NewBoxTarget);
					auto targetRigidBody = m_GameObjectManager->QueryComponent<RigidBody>(rhsBox->NewBoxTarget);

					VSM::Vector2 direction = {
						(boxTransform->LocalPosition.x - targetTransform->LocalPosition.x)
						, (boxTransform->LocalPosition.z - targetTransform->LocalPosition.z) };

					///������ Ȱ���Ͽ� �̵��� �������ϱ�
					if (std::abs(direction.x) > std::abs(direction.y))
						direction = { direction.x ,0 };
					else
						direction = { 0 ,direction.y };
					direction.Normalize();

					VSM::Vector3 MoveWay = { direction.x,0,direction.y };
					rhsBox->DeparturePosition = boxTransform->LocalPosition;
					rhsBox->ArrivalPosition = rhsBox->DeparturePosition + MoveWay * rhsBox->MoveLength;

					// ��� �б� ����
					Sound::GetInstance()->Play(lhs, "SFX_PullingBox_2");
				}

			}
		}
		break;*/
		case TagType::ConveyorBelt:
		{
			auto rhsBox = rhstag->Sibling<Box>(m_GameObjectManager);
			auto lhsConveyorBelt = lhsTag->Sibling<NeoConveyorBelt>(m_GameObjectManager);

			if (rhsBox != nullptr && lhsConveyorBelt != nullptr)
			{
				rhsBox->NewBoxTarget = lhs;
				rhsBox->OnMoveWalk = true;
				return;
			}
			else
			{
				assert(lhsConveyorBelt != nullptr);
				assert(rhsBox != nullptr);
			}
		}
		break;
		case TagType::BoxGenerator:
		{
			auto rhsBox = rhstag->Sibling<Box>(m_GameObjectManager);
			auto lhsBoxGenerator = lhsTag->Sibling<BoxGenerator>(m_GameObjectManager);
			if (rhsBox != nullptr && lhsBoxGenerator != nullptr)
			{
				rhsBox->NewBoxTarget = lhs;
				rhsBox->OnGenerated = true;
				return;
			}
		}
		default:
			break;
		}
	}
}

void BoxSystem::OnExitBox(std::any data)
{
	const auto [lhs, rhs] = any_cast<std::pair<uint32_t, uint32_t>>(data);
	/// �Ѵ� nullptr�� �ȵ����� Ȯ���ϰ�  �ڵ�����ϱ�.
	auto lhstag = m_GameObjectManager->QueryComponent<Tag>(lhs);
	auto rhstag = m_GameObjectManager->QueryComponent<Tag>(rhs);
	if (lhstag != nullptr && rhstag != nullptr)				//�Ѵ� �±װ� nullptr�� �ƴ���?
	{
		switch (lhstag->tag)
		{
		case TagType::Beaver:
		{
			auto rhsbox = rhstag->Sibling<Box>(m_GameObjectManager);
			auto lhscharacter = lhstag->Sibling<Character>(m_GameObjectManager);
			if (rhsbox != nullptr && lhscharacter != nullptr)					//Box Character comp�� �ִ���?
			{
				if (rhsbox->OnPushed)
				{
					rhsbox->OnPushed = false;
				}
			}
		}
		break;
		case TagType::ConveyorBelt:
		{

		}
		break;

		default:
			break;
		}
	}
}

void BoxSystem::OnResetScene(std::any data)
{
	for (Box& box : ComponentItr<Box>())
	{
		if (!box.IsOrigin)
		{
			EventManager::OnDestroy(box.GameObjectId);
			continue;
		}

		auto transform = box.Sibling<Transform>(m_GameObjectManager);
		transform->LocalPosition = box.OriginPosition;
	}


}

void BoxSystem::OnBoxEffectActive(std::any data)
{
	const auto [lhs, rhs] = std::any_cast <pair< uint32_t, TagType >> (data);		//lhs = �ڽ� , rhs �� �ߵ���ų �±�

	///�����Ϳ� �ʿ��� ����, � �±׸� Ȱ��ȭ �Ұ�����, box�� ���̵�
	auto UIbundleIDs = m_GameObjectManager->QueryComponent<Children>(lhs);
	if (UIbundleIDs)
	{
		for (auto UIbundleID : UIbundleIDs->ChildrenID)
		{
			if (m_GameObjectManager->QueryComponent<Tag>(UIbundleID)->tag == rhs)
			{
				auto UI_IDs = m_GameObjectManager->QueryComponent<Children>(UIbundleID);
				for (auto UI_comp : UI_IDs->ChildrenID)
				{
					m_GameObjectManager->QueryComponent<OnOff>(UI_comp)->IsOn = true;
					m_GameObjectManager->QueryComponent<SpriteController>(UI_comp)->bIsReset = true;
				}
			}
		}
	}
}

void BoxSystem::Start(uint32_t gameObjectId)
{
	Vector3 Pos = {};
	auto temptransform = m_GameObjectManager->QueryComponent<Transform>(gameObjectId);
	if (m_GameObjectManager->QueryComponent<Transform>(gameObjectId))
	{
		Pos = m_GameObjectManager->QueryComponent<Transform>(gameObjectId)->WorldPosition;
		if (auto box = m_GameObjectManager->QueryComponent<Box>(gameObjectId))
		{
			box->OriginPosition = Pos;
		}
		//if (box->IsOrigin)
		//{
		//	temptransform->WorldPosition = Pos;
		//}


	}

}

void BoxSystem::Initialize()
{
	for (Box& box : ComponentItr<Box>())
	{
		Start(box.GameObjectId);
	}
}

void BoxSystem::Finish(uint32_t gameObjectId)
{

}

void BoxSystem::Finalize()
{

}

