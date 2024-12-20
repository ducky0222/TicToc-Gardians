#pragma once
#include <Component.h>
struct CharacterBarrier :
    public Component
{
public:
    GENERATE_BODY(CharacterBarrier)
    GENERATE_PROPERTY(CharacterBarrier, BarrierOffsetSize)


    VSM::Vector3 BarrierOffsetSize = VSM::Vector3(0.5f, 0, 0.6f);
    float CharacterSpeed = 0.0f;

};

