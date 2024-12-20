#include "pch.h"
#include "TickTockAnimationSystem.h"
#include "TickTockComponentCommon.h"
#include "ComponentCommon.h"

TickTockAnimationSystem::TickTockAnimationSystem(GameObjectManager* gameObjectManager)
	: AnimationSystem(gameObjectManager)
{
	// beaver_Idle 예시
	{
		// RegisterTransitionData registerData;
		// 
		// registerData.AnimName = "beaver_idle.fbx";
		// registerData.Transitions.reserve(transitionCount);
		// 
		// auto run = [](std::any data) { 	
		// 		BeaverTransitionData data = any_cast();
		// 		return data.Speed > 0.2f;
		//  };
		// registerData.Transitions.push_back({ "beaver_run.fbx", run });
		// 
		// EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

#pragma region Beaver Animation
	// Beaver idle
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "beaver_idle.fbx";
		registerData.Transitions.reserve(3);

		auto move = [](std::any data)
			{
				BeaverData tempData = std::any_cast<BeaverData>(data);
				return tempData.isMoving;
			};

		registerData.Transitions.push_back({ "beaver_run.fbx", move });

		auto jump = [](std::any data)
			{
				BeaverData tempData = std::any_cast<BeaverData>(data);
				return tempData.isJumping;
			};
		registerData.Transitions.push_back({ "beaver_jump1.fbx", jump });

		auto create = [](std::any data)
			{
				BeaverData tempData = std::any_cast<BeaverData>(data);
				return tempData.isSpawning;
			};
		registerData.Transitions.push_back({ "beaver_create.fbx", create });

		auto wait = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "beaver_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "beaver_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Beaver move
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "beaver_run.fbx";
		registerData.Transitions.reserve(3);

		auto idle = [](std::any data)
			{
				BeaverData tempData = std::any_cast<BeaverData>(data);
				return !tempData.isMoving;
			};

		registerData.Transitions.push_back({ "beaver_idle.fbx", idle });

		auto jump = [](std::any data)
			{
				BeaverData tempData = std::any_cast<BeaverData>(data);
				return tempData.isJumping;
			};
		registerData.Transitions.push_back({ "beaver_jump1.fbx", jump });

		auto ready = [](std::any data)
			{
				BeaverData tempData = std::any_cast<BeaverData>(data);
				return tempData.isPushing;
			};
		registerData.Transitions.push_back({ "beaver_push_waiting.fbx", ready });

		auto create = [](std::any data)
			{
				BeaverData tempData = std::any_cast<BeaverData>(data);
				return tempData.isSpawning;
			};
		registerData.Transitions.push_back({ "beaver_create.fbx", create });

		auto wait = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "beaver_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "beaver_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Beaver jump
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "beaver_jump1.fbx";
		registerData.Transitions.reserve(3);

		auto jumpDown = [](std::any data)
			{
				BeaverData tempData = std::any_cast<BeaverData>(data);
				return !tempData.isJumping;
			};
		registerData.Transitions.push_back({ "beaver_jump2.fbx", jumpDown });

		auto wait = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "beaver_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "beaver_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Beaver land
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "beaver_jump2.fbx";
		registerData.Transitions.reserve(3);

		auto move = [](std::any data)
			{
				BeaverData tempData = std::any_cast<BeaverData>(data);
				return tempData.isMoving;
			};

		registerData.Transitions.push_back({ "beaver_run.fbx", move });

		auto wait = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "beaver_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "beaver_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Beaver create
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "beaver_create.fbx";
		registerData.Transitions.reserve(3);

		auto move = [](std::any data)
			{
				BeaverData tempData = std::any_cast<BeaverData>(data);
				return tempData.isMoving;
			};

		registerData.Transitions.push_back({ "beaver_run.fbx", move });

		auto wait = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "beaver_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "beaver_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Beaver Ready
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "beaver_push_waiting.fbx";
		registerData.Transitions.reserve(3);

		auto move = [](std::any data)
			{
				BeaverData tempData = std::any_cast<BeaverData>(data);
				return (!tempData.isPushing) && (tempData.isMoving);
			};

		registerData.Transitions.push_back({ "beaver_run.fbx", move });

		auto push = [](std::any data)
			{
				BeaverData tempData = std::any_cast<BeaverData>(data);
				return tempData.isPushed;
			};

		registerData.Transitions.push_back({ "beaver_push_B.fbx", move });

		auto wait = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "beaver_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "beaver_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);

	}

	// Beaver push
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "beaver_push_B.fbx";
		registerData.Transitions.reserve(3);

		auto move = [](std::any data)
			{
				BeaverData tempData = std::any_cast<BeaverData>(data);
				return (!tempData.isPushing) && (!tempData.isPushed) && (tempData.isMoving);
			};

		registerData.Transitions.push_back({ "beaver_run.fbx", move });

		auto wait = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "beaver_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "beaver_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Beaver wait
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "beaver_waiting.fbx";
		registerData.Transitions.reserve(3);

		//auto idle = [](std::any data)
		//	{
		//		BeaverData tempData = std::any_cast<BeaverData>(data);
		//		return !tempData.isMoving;
		//	};

		//registerData.Transitions.push_back({ "beaver_idle.fbx", idle });

		auto move = [](std::any data)
			{
				BeaverData tempData = std::any_cast<BeaverData>(data);
				return tempData.isMoving;
			};

		registerData.Transitions.push_back({ "beaver_run.fbx", move });
		
		auto repair = [](std::any data)
			{
				BeaverData tempdata = std::any_cast<BeaverData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "beaver_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}



#pragma endregion

#pragma region Cat Animation
	// Cat idle
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "cat_idle.fbx";
		registerData.Transitions.reserve(3);

		auto move = [](std::any data)
			{
				CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
				return tempData.isMoving;
			};

		registerData.Transitions.push_back({ "cat_run.fbx", move });

		auto jump = [](std::any data)
			{
				CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
				return tempData.isJumping;
			};
		registerData.Transitions.push_back({ "cat_jump_1.fbx", jump });

		auto wait = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "cat_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "cat_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Cat move
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "cat_run.fbx";
		registerData.Transitions.reserve(3);

		auto idle = [](std::any data)
			{
				CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
				return !tempData.isMoving;
			};

		registerData.Transitions.push_back({ "cat_idle.fbx", idle });

		auto jump = [](std::any data)
			{
				CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
				return tempData.isJumping;
			};
		registerData.Transitions.push_back({ "cat_jump_1.fbx", jump });

		auto wait = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "cat_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "cat_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Cat jump
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "cat_jump_1.fbx";
		registerData.Transitions.reserve(3);

		auto land = [](std::any data)
			{
				CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
				return !tempData.isJumping;
			};
		registerData.Transitions.push_back({ "cat_jump_2.fbx", land });

		auto wait = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "cat_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "cat_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Cat land
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "cat_jump_2.fbx";
		registerData.Transitions.reserve(3);

		auto move = [](std::any data)
			{
				CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
				return tempData.isMoving;
			};

		registerData.Transitions.push_back({ "cat_run.fbx", move });

		auto wait = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "cat_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "cat_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Cat wait
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "cat_waiting.fbx";
		registerData.Transitions.reserve(3);

		//auto idle = [](std::any data)
		//	{
		//		CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
		//		return !tempData.isMoving;
		//	};

		//registerData.Transitions.push_back({ "cat_idle.fbx", idle });

		auto move = [](std::any data)
			{
				CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
				return tempData.isMoving;
			};

		registerData.Transitions.push_back({ "cat_run.fbx", move });

		auto repair = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "cat_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}


#pragma endregion

#pragma region Rabbit Animation
	// Rabbit idle
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "Rabbit_idle.fbx";
		registerData.Transitions.reserve(3);

		auto move = [](std::any data)
			{
				CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
				return tempData.isMoving;
			};

		registerData.Transitions.push_back({ "Rabbit_run.fbx", move });

		auto jump = [](std::any data)
			{
				CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
				return tempData.isJumping;
			};
		registerData.Transitions.push_back({ "Rabbit_jump1.fbx", jump });

		auto wait = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "Rabbit_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "Rabbit_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Rabbit move
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "Rabbit_run.fbx";
		registerData.Transitions.reserve(3);

		auto idle = [](std::any data)
			{
				CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
				return !tempData.isMoving;
			};

		registerData.Transitions.push_back({ "Rabbit_idle.fbx", idle });

		auto jump = [](std::any data)
			{
				CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
				return tempData.isJumping;
			};
		registerData.Transitions.push_back({ "Rabbit_jump1.fbx", jump });

		auto wait = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "Rabbit_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "Rabbit_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Rabbit jump
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "Rabbit_jump1.fbx";
		registerData.Transitions.reserve(3);

		auto land = [](std::any data)
			{
				CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
				return !tempData.isJumping;
			};
		registerData.Transitions.push_back({ "Rabbit_jump2.fbx", land });

		auto wait = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "Rabbit_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "Rabbit_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Rabbit land
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "Rabbit_jump2.fbx";
		registerData.Transitions.reserve(3);

		auto move = [](std::any data)
			{
				CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
				return tempData.isMoving;
			};

		registerData.Transitions.push_back({ "Rabbit_run.fbx", move });

		auto wait = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isWaiting;
			};
		registerData.Transitions.push_back({ "Rabbit_waiting.fbx", wait });

		auto repair = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "Rabbit_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Rabbit wait
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "Rabbit_waiting.fbx";
		registerData.Transitions.reserve(3);

		//auto idle = [](std::any data)
		//	{
		//		CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
		//		return !tempData.isMoving;
		//	};

		//registerData.Transitions.push_back({ "Rabbit_idle.fbx", idle });

		auto move = [](std::any data)
			{
				CommonCharacterData tempData = std::any_cast<CommonCharacterData>(data);
				return tempData.isMoving;
			};

		registerData.Transitions.push_back({ "Rabbit_run.fbx", move });

		auto repair = [](std::any data)
			{
				CommonCharacterData tempdata = std::any_cast<CommonCharacterData>(data);
				return tempdata.isFixable;
			};
		registerData.Transitions.push_back({ "Rabbit_repair.fbx", repair });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}


#pragma endregion

#pragma region Character Select
	// Beaver pick idle
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "beaver_pick_idle.fbx";
		registerData.Transitions.reserve(3);

		auto pick = [](std::any data)
			{
				SelectCharacterData tempData = std::any_cast<SelectCharacterData>(data);
				return tempData.bIsCheckSelect;
			};
		registerData.Transitions.push_back({ "beaver_pick.fbx", pick });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}
	
	// Beaver pick
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "beaver_pick.fbx";
		registerData.Transitions.reserve(3);

		auto pick = [](std::any data)
			{
				SelectCharacterData tempData = std::any_cast<SelectCharacterData>(data);
				return !tempData.bIsCheckSelect;
			};
		registerData.Transitions.push_back({ "beaver_pick_idle.fbx", pick });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Cat pick idle
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "cat_pick_idle.fbx";
		registerData.Transitions.reserve(3);

		auto pick = [](std::any data)
			{
				SelectCharacterData tempData = std::any_cast<SelectCharacterData>(data);
				return tempData.bIsCheckSelect;
			};
		registerData.Transitions.push_back({ "cat_pick.fbx", pick });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}
	
	// Cat pick
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "cat_pick.fbx";
		registerData.Transitions.reserve(3);

		auto pick = [](std::any data)
			{
				SelectCharacterData tempData = std::any_cast<SelectCharacterData>(data);
				return !tempData.bIsCheckSelect;
			};
		registerData.Transitions.push_back({ "cat_pick_idle.fbx", pick });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}

	// Rabbit pick idle
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "Rabbit_pick_idle.fbx";
		registerData.Transitions.reserve(3);

		auto pick = [](std::any data)
			{
				SelectCharacterData tempData = std::any_cast<SelectCharacterData>(data);
				return tempData.bIsCheckSelect;
			};
		registerData.Transitions.push_back({ "Rabbit_pick.fbx", pick });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}
	
	// Rabbit pick
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "Rabbit_pick.fbx";
		registerData.Transitions.reserve(3);

		auto pick = [](std::any data)
			{
				SelectCharacterData tempData = std::any_cast<SelectCharacterData>(data);
				return !tempData.bIsCheckSelect;
			};
		registerData.Transitions.push_back({ "Rabbit_pick_idle.fbx", pick });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}
#pragma endregion
	//orangeButton
	{
	//isactivate  true
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "Obj_Button_Orange_Nonclick.fbx";
		registerData.Transitions.reserve(3);

		auto clicked = [](std::any data)
			{
				ButtonData tempData = std::any_cast<ButtonData>(data);
				return tempData.IsActivating;
			};
		registerData.Transitions.push_back({ "Obj_Button_Orange_Click.fbx", clicked });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}
	//isactivate Orange false
	{
		RegisterTransitionData registerData;
		registerData.AnimName = "Obj_Button_Orange_Click.fbx";
		registerData.Transitions.reserve(3);

		auto unclicked = [](std::any data)
			{
				ButtonData tempData = std::any_cast<ButtonData>(data);
				return !tempData.IsActivating;
			};
		registerData.Transitions.push_back({ "Obj_Button_Orange_Nonclick.fbx", unclicked });

		EventManager::PublishImmediate("OnRegisterTransition", registerData);
	}
	}
	//BlueButton
	{
		//isactivate  true
		{
			RegisterTransitionData registerData;
			registerData.AnimName = "Obj_Button_Blue_Nonclick.fbx";
			registerData.Transitions.reserve(3);

			auto clicked = [](std::any data)
				{
					ButtonData tempData = std::any_cast<ButtonData>(data);
					return tempData.IsActivating;
				};
			registerData.Transitions.push_back({ "Obj_Button_Blue_Click.fbx", clicked });

			EventManager::PublishImmediate("OnRegisterTransition", registerData);
		}
		//isactivate  false
		{
			RegisterTransitionData registerData;
			registerData.AnimName = "Obj_Button_Blue_Click.fbx";
			registerData.Transitions.reserve(3);

			auto unclicked = [](std::any data)
				{
					ButtonData tempData = std::any_cast<ButtonData>(data);
					return !tempData.IsActivating;
				};
			registerData.Transitions.push_back({ "Obj_Button_Blue_Nonclick.fbx", unclicked });

			EventManager::PublishImmediate("OnRegisterTransition", registerData);
		}
	}	
	//Obj_Button_Green
	{
		//isactivate  true
		{
			RegisterTransitionData registerData;
			registerData.AnimName = "Obj_Button_Green_NonClick.fbx";
			registerData.Transitions.reserve(3);

			auto clicked = [](std::any data)
				{
					ButtonData tempData = std::any_cast<ButtonData>(data);
					return tempData.IsActivating;
				};
			registerData.Transitions.push_back({ "Obj_Button_Green_Click.fbx", clicked });

			EventManager::PublishImmediate("OnRegisterTransition", registerData);
		}
		//isactivate Orange false
		{
			RegisterTransitionData registerData;
			registerData.AnimName = "Obj_Button_Green_Click.fbx";
			registerData.Transitions.reserve(3);

			auto unclicked = [](std::any data)
				{
					ButtonData tempData = std::any_cast<ButtonData>(data);
					return !tempData.IsActivating;
				};
			registerData.Transitions.push_back({ "Obj_Button_Green_NonClick.fbx", unclicked });

			EventManager::PublishImmediate("OnRegisterTransition", registerData);
		}
	}	
	//RedButton
	{
		//isactivate  true
		{
			RegisterTransitionData registerData;
			registerData.AnimName = "Obj_Button_Red_Nonclick.fbx";
			registerData.Transitions.reserve(3);

			auto clicked = [](std::any data)
				{
					ButtonData tempData = std::any_cast<ButtonData>(data);
					return tempData.IsActivating;
				};
			registerData.Transitions.push_back({ "Obj_Button_Red_Click.fbx", clicked });

			EventManager::PublishImmediate("OnRegisterTransition", registerData);
		}
		//isactivate Orange false
		{
			RegisterTransitionData registerData;
			registerData.AnimName = "Obj_Button_Red_Click.fbx";
			registerData.Transitions.reserve(3);

			auto unclicked = [](std::any data)
				{
					ButtonData tempData = std::any_cast<ButtonData>(data);
					return !tempData.IsActivating;
				};
			registerData.Transitions.push_back({ "Obj_Button_Red_Nonclick.fbx", unclicked });

			EventManager::PublishImmediate("OnRegisterTransition", registerData);
		}
	}	
	//whiteButton
	{
		//isactivate  true
		{
			RegisterTransitionData registerData;
			registerData.AnimName = "Obj_Button_White_Nonclick.fbx";
			registerData.Transitions.reserve(3);

			auto clicked = [](std::any data)
				{
					ButtonData tempData = std::any_cast<ButtonData>(data);
					return tempData.IsActivating;
				};
			registerData.Transitions.push_back({ "Obj_Button_White_Click.fbx", clicked });

			EventManager::PublishImmediate("OnRegisterTransition", registerData);
		}
		//isactivate Orange false
		{
			RegisterTransitionData registerData;
			registerData.AnimName = "Obj_Button_White_Click.fbx";
			registerData.Transitions.reserve(3);

			auto unclicked = [](std::any data)
				{
					ButtonData tempData = std::any_cast<ButtonData>(data);
					return !tempData.IsActivating;
				};
			registerData.Transitions.push_back({ "Obj_Button_White_Nonclick.fbx", unclicked });

			EventManager::PublishImmediate("OnRegisterTransition", registerData);
		}
	}	//orangeButton
	//YellowButton
	{
		//isactivate  true
		{
			RegisterTransitionData registerData;
			registerData.AnimName = "Obj_Button_Yellow_Nonclick.fbx";
			registerData.Transitions.reserve(3);

			auto clicked = [](std::any data)
				{
					ButtonData tempData = std::any_cast<ButtonData>(data);
					return tempData.IsActivating;
				};
			registerData.Transitions.push_back({ "Obj_Button_Yellow_Click.fbx", clicked });

			EventManager::PublishImmediate("OnRegisterTransition", registerData);
		}
		//isactivate Orange false
		{
			RegisterTransitionData registerData;
			registerData.AnimName = "Obj_Button_Yellow_Click.fbx";
			registerData.Transitions.reserve(3);

			auto unclicked = [](std::any data)
				{
					ButtonData tempData = std::any_cast<ButtonData>(data);
					return !tempData.IsActivating;
				};
			registerData.Transitions.push_back({ "Obj_Button_Yellow_Nonclick.fbx", unclicked });

			EventManager::PublishImmediate("OnRegisterTransition", registerData);
		}
	}	//orangeButton

}

bool TickTockAnimationSystem::handleTransition(AnimationController& controller)
{
	std::string outNextAnimName = "";
	bool outbIsTransition = false;

	TransitionData transitionData;
	transitionData.AnimName = controller.CurrentFileName;
	transitionData.OutNextAnimName = &outNextAnimName;
	transitionData.OutbIsTransition = &outbIsTransition;

	const auto objectTag = controller.Sibling<Tag>(m_GameObjectManager);

	if (objectTag)
	{
		switch (objectTag->tag)
		{
			case TagType::Beaver:
			{
				const auto rigidBody = objectTag->Sibling<RigidBody>(m_GameObjectManager);
				const auto character = objectTag->Sibling<Character>(m_GameObjectManager);
				const auto replayer = objectTag->Sibling<Replayer>(m_GameObjectManager);
				if (rigidBody != nullptr && character != nullptr && replayer != nullptr)
				{
					BeaverData tempData = {};
					tempData.speed = character->Speed;
					tempData.isJumping = character->IsJumping;
					tempData.isFixable = character->IsFixable;
					tempData.isPushed = character->IsPushed;
					tempData.isPushing = character->IsPushing;
					tempData.isMoving = character->IsArrowDown;
					tempData.isSpawning = character->IsSpawning;
					tempData.isWaiting = replayer->IsWaiting;
					transitionData.Data = tempData;
					EventManager::PublishImmediate("OnTransition", transitionData);
				}
			}
			break;
			case TagType::Cat:
			case TagType::Rabbit:
			{
				const auto rigidBody = objectTag->Sibling<RigidBody>(m_GameObjectManager);
				const auto character = objectTag->Sibling<Character>(m_GameObjectManager);
				const auto replayer = objectTag->Sibling<Replayer>(m_GameObjectManager);
				if (rigidBody != nullptr && character != nullptr && replayer != nullptr)
				{
					CommonCharacterData tempData = {};
					tempData.speed = character->Speed;
					tempData.isJumping = character->IsJumping;
					tempData.isFixable = character->IsFixable;
					tempData.isMoving = character->IsArrowDown;
					tempData.isWaiting = replayer->IsWaiting;
					transitionData.Data = tempData;
					EventManager::PublishImmediate("OnTransition", transitionData);
				}
			}

			break;

			case TagType::SelectBeaver:
			case TagType::SelectCat:
			case TagType::SelectRabbit:
			{
				const auto select = objectTag->Sibling<SelectCharacter>(m_GameObjectManager);
				if (select)
				{
					SelectCharacterData tempData = {};
					tempData.bIsCheckSelect = select->bIsSelected;
					transitionData.Data = tempData;
					EventManager::PublishImmediate("OnTransition", transitionData);
				}

			}
			break;

			case TagType::Button:
			{
				const auto button = objectTag->Sibling<Button>(m_GameObjectManager);
				if (button)
				{
					ButtonData tempData = {};
					tempData.IsActivating = button->IsActivated;
					transitionData.Data = tempData;
					EventManager::PublishImmediate("OnTransition", transitionData);
				}
			}
			break;
			default:
				break;
		}
	}


	/*
	switch (animType)
	{
		case CommonChracter:
			transitionData.Data = CommonTransitionData;
		case Beaver:
			transitionData.Data = BeaverTransitionData;
		case BoxGenerator:
			transitionData.Data = BoxGenTransitionData;
	}
	*/
	//EventManager::PublishImmediate("OnTransition", transitionData);

	// 트랜지션 enter
	if (outbIsTransition)
	{
		controller.bIsTransitionState = true;
		controller.BeforeFineName = controller.CurrentFileName;
		controller.CurrentFileName = outNextAnimName;
		return true;
	}

	return false;
}
