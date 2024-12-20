#pragma once
#include "EventListener.h"
#include "Replayer.h"
enum class TagType;

enum class eSceneState
{
	Title,

	LevelSelect,

	InGameCutScene, // 랜덤하게 씬이 여러개 나오므로 별도의 씬으로 뺀다.

	MapSearchScene,
	CharacterSelect,
	FirstPlayer,		// 랜딩 오더 1
	SecondPlayer,		// 랜딩 오더 2
	ThirdPlayer,		// 랜딩 오더 3
	CheckMissionClear,
	StageOver,

	TutorialPlayer,
	CheckTutorialClear,


};

struct SelectTurn
{
	std::pair<TagType, uint32_t> first;
	std::pair<TagType, uint32_t> second;
	std::pair<TagType, uint32_t> third;
};

class TickTockScene : public EventListener
{
public:
	TickTockScene(eSceneState gameState, GameObjectManager* gameObjectManager);
	virtual ~TickTockScene() = default;

	static bool GetEntityIdentityNameID(GameObjectManager* gameObjectManager, const std::string& uuid, uint32_t* outID);
	static void SetEntityIdentityNameOnOff(GameObjectManager* gameObjectManager, const std::string& uuid, bool bIsOn);
	static void SetEntityIdentityNameOnOffs(GameObjectManager* gameObjectManager, const std::set<std::string>& uuids, bool bIsOn);
	static TagType GetSelectTagMappingPlayerTag(TagType selectTag);
	static void SetTagTypeOnOff(GameObjectManager* gameObjectManager, TagType tagType, bool bIsOn);

	void FindTimerID(uint32_t* timer1000, uint32_t* timer100, uint32_t* timer10, uint32_t* timer1, uint32_t* colon);
	void ShowTimer(Replayer* replayer, uint32_t timer1000, uint32_t timer100, uint32_t timer10, uint32_t timer1, uint32_t colon);
	void ActiveTimer(uint32_t timer1000, uint32_t timer100, uint32_t timer10, uint32_t timer1);
	void InActiveTimer(uint32_t timer1000, uint32_t timer100, uint32_t timer10, uint32_t timer1, uint32_t colon);

	static std::string MatchPortraitPlayer(TagType tag);
	static std::string MatchPortraitDim(TagType tag);

	virtual void Enter() = 0;
	virtual eSceneState HandleTransition() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Exit() = 0;

	eSceneState GetGameState() const { return mGameState; }

protected:
	GameObjectManager* mGameObjectManager;
	TagType firstType;
	TagType secondType;
	TagType thirdType;
	std::string curSceneName;

	bool isLandingThisScene;
	uint32_t currentLandingPointId;

	static uint32_t landingList[4];

private:
	eSceneState mGameState;
};