#pragma once

//#ifndef PCH_H
//#define PCH_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

// STL
#include <memory>
#include <iostream>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <string_view>
#include <algorithm>
#include <type_traits>

// WIN
#include <windows.h>
#include <assert.h>

// DX
#include "VerySimpleMath.h"
namespace VSM = Very::SimpleMath;

#include "../ABGraphics/pch.h"

// USER
#define EDITOR_MODE
//#define LOG_CONSOLE

#include <glassify/glassify.h>
#include <glassify/serialization/glas_serialization_config_json.h>
#include <nlohmann/json.hpp>

#include "../Reflection/TypeInfoMacro.h"
#include "../Reflection/PropertyMacro.h"


//#endif //PCH_H
