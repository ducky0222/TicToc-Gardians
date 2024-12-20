#include "pch.h"
#include "CharacterSelect.h"

#include "GameObjectManager.h"
#include "SerializeSystem.h"
#include "ComponentCommon.h"
#include "TickTockComponentCommon.h"
#include "UI2DRenderer.h"
#include "../SoundEngine/ISoundEngine.h"


CharacterSelect::CharacterSelect(GameObjectManager* gameObjectManager)
	: TickTockScene(eSceneState::CharacterSelect, gameObjectManager)
{
	EventManager::Subscribe("OnPlayBtnClick", CreateListenerInfo(&CharacterSelect::onFinishSelect));
	EventManager::Subscribe("OnNoBtnClick", CreateListenerInfo(&CharacterSelect::OnNoBtnClick));
	EventManager::Subscribe("OnCharacterClick", CreateListenerInfo(&CharacterSelect::OnCharacterClick));

	EventManager::Subscribe("OnBindSceneName", CreateListenerInfo(&CharacterSelect::OnBindSceneName));
}

void CharacterSelect::Enter()
{
	size_t count = 0;

	//SetEntityIdentityNameOnOff(mGameObjectManager, "PleasePressAnyKey", false);

	for (SelectCharacter& selectCharacter : System::ComponentItr<SelectCharacter>())
	{
		OnOff* onOff = selectCharacter.Sibling<OnOff>(mGameObjectManager);
		onOff->IsOn = true;
		++count;

		selectCharacter.bIsClicked = false;
		selectCharacter.bIsSelected = false;
		selectCharacter.SelectIndex = 0u;
	}

	mbIsActiveEvent = true;
}

eSceneState CharacterSelect::HandleTransition()
{
	if (mbIsTransition)
	{
		return eSceneState::FirstPlayer;
	}

	return GetGameState();
}

void CharacterSelect::Update(float deltaTime)
{

	if (selectedCharacters.size())
	{
		for (size_t i = 0; i < selectedCharacters.size(); i++)
		{
			auto currentTag = mGameObjectManager->QueryComponent<Tag>(selectedCharacters[i]);
			if (currentTag)
			{
				switch (currentTag->tag)
				{
					case TagType::SelectBeaver:
					{
						SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectBeaver", false);
						SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectedBeaver", true);
						SetEntityIdentityNameOnOff(mGameObjectManager, "UIBeaverTurn", true);

						uint32_t uiID = 0;
						GetEntityIdentityNameID(mGameObjectManager, "UIBeaverTurn", &uiID);
						auto ui2DRenderer = mGameObjectManager->QueryComponent<UI2DRenderer>(uiID);
						if (ui2DRenderer)
						{
							switch (i)
							{
								case 0:
									ui2DRenderer->FileName = "UI_CharacterSelect_1st.png";
									break;
								case 1:
									ui2DRenderer->FileName = "UI_CharacterSelect_2nd.png";
									break;
								case 2:
									ui2DRenderer->FileName = "UI_CharacterSelect_3rd.png";
									break;
							}
						}
					}
					break;
					case TagType::SelectCat:
					{
						SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectCat", false);
						SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectedCat", true);
						SetEntityIdentityNameOnOff(mGameObjectManager, "UICatTurn", true);

						uint32_t uiID = 0;
						GetEntityIdentityNameID(mGameObjectManager, "UICatTurn", &uiID);
						auto ui2DRenderer = mGameObjectManager->QueryComponent<UI2DRenderer>(uiID);
						if (ui2DRenderer)
						{
							switch (i)
							{
								case 0:
									ui2DRenderer->FileName = "UI_CharacterSelect_1st.png";
									break;
								case 1:
									ui2DRenderer->FileName = "UI_CharacterSelect_2nd.png";
									break;
								case 2:
									ui2DRenderer->FileName = "UI_CharacterSelect_3rd.png";
									break;
							}
						}
					}
					break;
					case TagType::SelectRabbit:
					{
						SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectRabbit", false);
						SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectedRabbit", true);
						SetEntityIdentityNameOnOff(mGameObjectManager, "UIRabbitTurn", true);

						uint32_t uiID = 0;
						GetEntityIdentityNameID(mGameObjectManager, "UIRabbitTurn", &uiID);
						auto ui2DRenderer = mGameObjectManager->QueryComponent<UI2DRenderer>(uiID);
						if (ui2DRenderer)
						{
							switch (i)
							{
								case 0:
									ui2DRenderer->FileName = "UI_CharacterSelect_1st.png";
									break;
								case 1:
									ui2DRenderer->FileName = "UI_CharacterSelect_2nd.png";
									break;
								case 2:
									ui2DRenderer->FileName = "UI_CharacterSelect_3rd.png";
									break;
							}
						}
					}
					break;
					default:
						break;
				}
			}
		}
	}

	if (selectedCharacters.size() == 3)
	{
		SetEntityIdentityNameOnOff(mGameObjectManager, "PickScreenText", true);
		SetEntityIdentityNameOnOff(mGameObjectManager, "YesButton", true);
		SetEntityIdentityNameOnOff(mGameObjectManager, "NoButton", true);
	}
	else
	{
		SetEntityIdentityNameOnOff(mGameObjectManager, "PickScreenText", false);
		SetEntityIdentityNameOnOff(mGameObjectManager, "YesButton", false);
		SetEntityIdentityNameOnOff(mGameObjectManager, "NoButton", false);
	}
}

void CharacterSelect::Exit()
{
	for (SelectCharacter& selectCharacter : System::ComponentItr<SelectCharacter>())
	{
		OnOff* onOff = selectCharacter.Sibling<OnOff>(mGameObjectManager);
		onOff->IsOn = false;
	}
	SetEntityIdentityNameOnOff(mGameObjectManager, "PlayBtn", false);

	mbIsTransition = false;
	mbIsActiveEvent = false;
	selectedCharacters.clear();
}

void CharacterSelect::onFinishSelect(std::any data)
{
	if (!mbIsActiveEvent)
	{
		return;
	}

	bool bIsTransition = true;

	for (SelectCharacter& selectCharacter : System::ComponentItr<SelectCharacter>())
	{
		if (!selectCharacter.bIsSelected)
		{
			bIsTransition = false;
			break;
		}
	}

	SelectTurn turn;

	TagType tag1 = mGameObjectManager->QueryComponent<Tag>(selectedCharacters[0])->tag;
	TagType tag2 = mGameObjectManager->QueryComponent<Tag>(selectedCharacters[1])->tag;
	TagType tag3 = mGameObjectManager->QueryComponent<Tag>(selectedCharacters[2])->tag;
	
	turn.first = std::make_pair(tag1, 0);
	turn.second = std::make_pair(tag2, 1);
	turn.third = std::make_pair(tag3, 2);

	EventManager::ScheduleEvent("SetCharacterIndex", turn);

	// test
	// 아마 처음부터 들어가면 mapsearchscene에서 curscenename을 저장하기 때문에 이렇게 초기화하지 않을듯

	EventManager::ScheduleEvent("OnSceneTransition", curSceneName);

	mbIsTransition = bIsTransition;

	Sound::GetInstance()->Play(0, "SFX_Char_Set_Finished");
}

void CharacterSelect::OnNoBtnClick(std::any data)
{
	/// 이걸로 괜찮을까? 문구 출력해야됨

	/// 제일 마지막 꺼 빼기
	uint32_t curID = 0;
	curID = selectedCharacters.back();
	selectedCharacters.pop_back();

	auto character = mGameObjectManager->QueryComponent<SelectCharacter>(curID);
	if (character)
	{
		character->bIsSelected = false;
		character->SelectIndex = -1;
	}

	auto currentTag = mGameObjectManager->QueryComponent<Tag>(curID);
	if (currentTag)
	{
		switch (currentTag->tag)
		{
			case TagType::SelectBeaver:
				SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectBeaver", true);
				SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectedBeaver", false);
				SetEntityIdentityNameOnOff(mGameObjectManager, "UIBeaverTurn", false);
				break;
			case TagType::SelectCat:
				SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectCat", true);
				SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectedCat", false);
				SetEntityIdentityNameOnOff(mGameObjectManager, "UICatTurn", false);
				break;
			case TagType::SelectRabbit:
				SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectRabbit", true);
				SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectedRabbit", false);
				SetEntityIdentityNameOnOff(mGameObjectManager, "UIRabbitTurn", false);
				break;

			default:
				break;
		}
	}
}

void CharacterSelect::OnCharacterClick(std::any data)
{
	uint32_t tagNum = stoi(std::any_cast<std::string>(data));

	TagType curTag = static_cast<TagType>(tagNum);

	int lastIndex = -1;
	for (SelectCharacter& character : System::ComponentItr<SelectCharacter>())
	{
		lastIndex = max(lastIndex, character.SelectIndex);
	}

	for (SelectCharacter& character : System::ComponentItr<SelectCharacter>())
	{
		auto charTag = character.Sibling<Tag>(mGameObjectManager);

		if (charTag == nullptr)
		{
			continue;
		}

		if (charTag->tag != curTag)
		{
			continue;
		}

		// 해제 조건
		if (character.bIsSelected)
		{
			if (character.SelectIndex == lastIndex)
			{
				character.bIsSelected = false;
				character.SelectIndex = -1;

				uint32_t id = selectedCharacters.back();
				selectedCharacters.pop_back();

				auto currentTag = mGameObjectManager->QueryComponent<Tag>(id);
				if (currentTag)
				{
					switch (currentTag->tag)
					{
						case TagType::SelectBeaver:
							SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectBeaver", true);
							SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectedBeaver", false);
							SetEntityIdentityNameOnOff(mGameObjectManager, "UIBeaverTurn", false);
							break;
						case TagType::SelectCat:
							SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectCat", true);
							SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectedCat", false);
							SetEntityIdentityNameOnOff(mGameObjectManager, "UICatTurn", false);
							break;
						case TagType::SelectRabbit:
							SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectRabbit", true);
							SetEntityIdentityNameOnOff(mGameObjectManager, "CharacterSelectedRabbit", false);
							SetEntityIdentityNameOnOff(mGameObjectManager, "UIRabbitTurn", false);
							break;

						default:
							break;
					}
				}
			}
		}
		else
		{
			character.bIsSelected = true;
			character.SelectIndex = lastIndex + 1;
			selectedCharacters.push_back(character.GameObjectId);
			Sound::GetInstance()->Play(0, "SFX_PickNumber");
		}
	}
}

void CharacterSelect::OnBindSceneName(std::any data)
{
	curSceneName = std::any_cast<std::string>(data);
}

