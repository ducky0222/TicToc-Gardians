#pragma once
#include <Component.h>
struct InGameManager :
    public Component
{
    GENERATE_BODY(InGameManager)
    GENERATE_PROPERTY(InGameManager, LevelName, PlayableCharacterCount, GlobalGravity)

    // Next scene name
    std::string LevelName;

    // unused
    uint32_t PlayableCharacterCount;


    VSM::Vector3 GlobalGravity = { 0, -9.8f, 0 };
    
    // innerValue
    uint32_t PausePopupId;
    bool IsMissionClear = false;
    bool IsResetAble = true;

};

