#pragma once
#include <Component.h>
struct DimensionCrack :
    public Component
{
public:
    GENERATE_BODY(DimensionCrack)
    GENERATE_PROPERTY(DimensionCrack, CharactersNearCrack)

    uint32_t CharactersNearCrack = 0;
};


