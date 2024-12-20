#pragma once

#include "Graphics.h"
#include "Technique.h"
#include "Vertex.h"

struct aiMaterial;
struct aiMesh;

namespace Bind
{
	class VertexBuffer;
	class IndexBuffer;
	class Texture;
}

// Material�� aiMaterial�� �����͸� ������� �������� bindable �����͸� �����.

class Material
{
public:
	Material(Graphics& graphics, const aiMaterial& material, const std::filesystem::path& path);
	~Material() = default;

	std::vector<Technique>& GetTechniques() { return mTechniques; }

	const std::vector<Technique>& GetTechniques()const { return mTechniques; }
	std::string GetName() const { return mName; }

private:
	std::shared_ptr<Bind::Texture> loadTexture(Graphics& graphics, const std::filesystem::path& path, const aiMaterial& material, aiTextureType aitype, unsigned int bindIndex);

private:
	std::vector<Technique> mTechniques;
	std::string mName;
};