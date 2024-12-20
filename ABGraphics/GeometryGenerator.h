#pragma once

#include <vector>
#include <directxtk/SimpleMath.h>

class GeometryGenerator
{
public:
	struct Vertex
	{
		Vertex() = default;
		Vertex(const Vertex&) = default;
		Vertex& operator=(const Vertex&) = default;
		Vertex(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 normal, DirectX::SimpleMath::Vector3 tangent, DirectX::SimpleMath::Vector2 texture)
			: Position(position)
			, Normal(normal)
			, TangentU(tangent)
			, UV(texture)
		{
		}
		Vertex(float px, float py, float pz, float nx, float ny, float nz, float tanx, float tany, float tanz, float tx, float ty)
			: Position(px, py, pz)
			, Normal(nx, ny, nz)
			, TangentU(tanx, tany, tanz)
			, UV(tx, ty)
		{
		}

		DirectX::SimpleMath::Vector3 Position;
		DirectX::SimpleMath::Vector3 Normal;
		DirectX::SimpleMath::Vector3 TangentU;
		DirectX::SimpleMath::Vector2 UV;
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};

public:
	static void CreateBox(float width, float height, float depth, MeshData* outMeshData);
	static void CreateBox(DirectX::SimpleMath::Vector3 size, MeshData* outMeshData);
	static void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData* outMeshData);
	static void CreateGeosphere(float radius, UINT numSubdivisions, MeshData* outMeshData);
	static void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData* outMeshData);
	static void CreateGrid(float width, float depth, UINT m, UINT n, MeshData* outMeshData);
	static void CreateFullscreenQuad(MeshData* outMeshData);

private:
	static void subdivide(MeshData* meshData);
	static void buildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData* outMeshData);
	static void buildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData* outMeshData);
};
