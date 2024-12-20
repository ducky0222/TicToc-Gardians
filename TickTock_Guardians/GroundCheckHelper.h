#pragma once
#include <Component.h>
struct GroundCheckHelper :
    public Component
{
public:
    GENERATE_BODY(GroundCheckHelper)
    GENERATE_PROPERTY(GroundCheckHelper, Offset, GroundCheckDistance)

    float Offset = 0.5f;
    float GroundCheckDistance = 1.6f;

};

