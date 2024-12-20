#pragma once
#include "Component.h"

struct Player : public Component
{
    GENERATE_BODY(Player)
    GENERATE_PROPERTY(Player, IsMove, IsJump)

	bool IsMove = false;
    bool IsJump = false;
};

