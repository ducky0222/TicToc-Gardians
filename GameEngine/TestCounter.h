#pragma once
#include "Component.h"

struct TestCounter : public Component
{
    GENERATE_BODY(TestCounter);
    GENERATE_PROPERTY(TestCounter, MyId, Counter)

    uint32_t MyId = 0;
    uint32_t Counter = 0;
};

