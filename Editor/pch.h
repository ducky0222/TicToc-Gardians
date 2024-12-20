#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN

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
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// WIN
#include <windows.h>
#include <assert.h>

#include "VerySimpleMath.h"
namespace VSM = Very::SimpleMath;

#include "../ABGraphics/pch.h"


// ImGuizmo
#include "imgui.h"
#include "ImGuizmo.h"

#endif //PCH_H
