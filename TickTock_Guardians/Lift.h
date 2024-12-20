#pragma once
#include <Component.h>
struct Lift :
    public Component
{
public:
    GENERATE_BODY(Lift)
    GENERATE_PROPERTY(Lift, ChainedButtonId, LiftSpeed, MaxLiftDistance, LiftDirectionUp)

    uint32_t ChainedButtonId = 0;
    float LiftSpeed = 0.0f;
    float MaxLiftDistance = 0.0f;
    bool LiftDirectionUp = false;
    bool IsLiftActive = false;

    float LiftDistance = 0.0f;

    Vector3 StartPosition;
};

