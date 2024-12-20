// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#pragma once

//#ifndef PCH_H
//#define PCH_H

#define _HAS_STD_BYTE 0

// STL
#include <array>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cctype>
#include <iostream>
#include <list>
#include <memory>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <optional>
#include <typeinfo>
#include <type_traits>
#include <random>

using namespace std;

// WIN
#include <windows.h>
#include <optional>

// DirectX 
#include <DirectXMath.h>
#include <directxtk/SimpleMath.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <wrl.h>
#include <dxgi.h>
#include <directxtk/DDSTextureLoader.h>
#include <directxtk/WICTextureLoader.h>
#include "DirectXTex.h"
#include "DirectXTex.inl"

using namespace DirectX;
using namespace Microsoft::WRL;
using Vector2 = DirectX::SimpleMath::Vector2;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Quaternion = DirectX::SimpleMath::Quaternion;

// Assimp
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

// ImGUI
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "../lib/vld.lib")

#ifdef _DEBUG
#pragma comment(lib, "../lib/DirectXTex.lib")
#pragma comment(lib, "assimp-vc143-mtd.lib")
#endif

//#endif //PCH_H
