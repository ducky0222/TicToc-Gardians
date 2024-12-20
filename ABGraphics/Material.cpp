#include "pch.h"

#include "Material.h"
#include "BindableCommon.h"
#include "DynamicConstant.h"
#include "ConstantBufferEx.h"
#include "Stencil.h"
#include "Texture.h"
#include "CubeTexture.h"
#include "Util.h"

Material::Material(Graphics& graphics, const aiMaterial& material, const std::filesystem::path& path)
{
	using namespace Bind;

	// 머테리얼 이름
	{
		aiString tempName;
		material.Get(AI_MATKEY_NAME, tempName);
		mName = tempName.C_Str();
	}

	int textureCounts[22] = { 0, };
	for (int i = 0; i < 22; ++i)
	{
		textureCounts[i] = material.GetTextureCount((aiTextureType)i);
	}

	// 랜더 테크닉 : pbr인지 non pbr인지 분기
	{
		Technique renderTech{ "RenderTechnique" };

		// 두개 다 추가한 후 나중에 직접 분기한다. 알파 텍스처는 안 받아쓸 예정
		Step step{ "renderPass" };
		Step stepHasAlpha{ "hasAlphaRenderPass" };

		auto alphaCbuf = std::make_shared<PixelCBufAlpha>(graphics, 2);
		step.AddBindable(alphaCbuf);
		stepHasAlpha.AddBindable(alphaCbuf);

		{
			// pbr 텍스처
			if (material.GetTextureCount(aiTextureType_METALNESS) > 0u || material.GetTextureCount(aiTextureType_SHININESS) > 0u)
			{
				// 이거 그냥 CbufPixel로 바꾸자 어차피 재정의할 일 없음
				Dcb::RawLayout PSLayout; // 그 정렬 안된 거
				PSLayout.Add<Dcb::Bool>("useDiffuse")
					.Add<Dcb::Bool>("useNormal")
					.Add<Dcb::Bool>("useEmissive")
					.Add<Dcb::Bool>("useAlpha")
					.Add<Dcb::Bool>("useMatalness")
					.Add<Dcb::Bool>("useRoughness");

				Dcb::Buffer PSBuffer{ std::move(PSLayout) }; // 상수버퍼

				auto diffuse = loadTexture(graphics, path, material, aiTextureType_DIFFUSE, 0);
				PSBuffer["useDiffuse"] = diffuse.get() != nullptr;
				if (diffuse)
				{
					step.AddBindable(diffuse);
					stepHasAlpha.AddBindable(diffuse);
				}
				else
				{
					auto baseColor = loadTexture(graphics, path, material, aiTextureType_BASE_COLOR, 0);
					PSBuffer["useDiffuse"] = baseColor.get() != nullptr;

					if (baseColor)
					{
						step.AddBindable(baseColor);
						stepHasAlpha.AddBindable(baseColor);
					}
				}

				auto normal = loadTexture(graphics, path, material, aiTextureType_NORMALS, 1);
				PSBuffer["useNormal"] = normal.get() != nullptr;
				if (normal != nullptr)
				{
					step.AddBindable(normal);
					stepHasAlpha.AddBindable(normal);
				}

				auto emissive = loadTexture(graphics, path, material, aiTextureType_EMISSIVE, 2);
				PSBuffer["useEmissive"] = emissive.get() != nullptr;
				if (emissive != nullptr)
				{
					step.AddBindable(emissive);
					stepHasAlpha.AddBindable(emissive);
				}

				auto alpha = loadTexture(graphics, path, material, aiTextureType_OPACITY, 3);
				PSBuffer["useAlpha"] = alpha.get() != nullptr;
				if (alpha != nullptr)
				{
					step.AddBindable(alpha);
					stepHasAlpha.AddBindable(alpha);
				}

				auto matalness = loadTexture(graphics, path, material, aiTextureType_METALNESS, 4);
				PSBuffer["useMatalness"] = matalness.get() != nullptr;
				if (matalness != nullptr)
				{
					step.AddBindable(matalness);
					stepHasAlpha.AddBindable(matalness);
				}

				auto roughness = loadTexture(graphics, path, material, aiTextureType_SHININESS, 5);
				PSBuffer["useRoughness"] = roughness.get() != nullptr;
				if (roughness != nullptr)
				{
					step.AddBindable(roughness);
					stepHasAlpha.AddBindable(roughness);
				}

				auto pixelCbuf1 = std::make_shared<Bind::CachingPixelConstantBufferEx>(graphics, std::move(PSBuffer), 1u);
				step.AddBindable(pixelCbuf1);
				stepHasAlpha.AddBindable(pixelCbuf1);

				auto pixelCubf2 = std::make_shared<PixelCBufAlpha>(graphics, 2u);
				step.AddBindable(pixelCubf2);
				stepHasAlpha.AddBindable(pixelCubf2);

				// 이 부분 패스로 이동시키는 게 좋아보이네
				// step.AddBindable(Bind::CubeTexture::Create(graphics, "../Resources/Texture/CubicFirstCubeMapDiffuseHDR.dds", 6));
				// step.AddBindable(Bind::CubeTexture::Create(graphics, "../Resources/Texture/CubicFirstCubeMapSpecularHDR.dds", 7));
				// step.AddBindable(Bind::Texture::Create(graphics, "../Resources/Texture/CubicFirstCubeMapBrdf.dds", 8));

				// 쉐이더 이름
				auto pixelShader = PixelShader::Create(graphics, "../Resources/Shader/ModelPBR_PS.hlsl");
				step.AddBindable(pixelShader);
				stepHasAlpha.AddBindable(pixelShader);
			}
			else
			{
				// 상수 버퍼
				Dcb::RawLayout PSLayout;
				PSLayout.Add<Dcb::Bool>("useDiffuse")
					.Add<Dcb::Bool>("useNormal")
					.Add<Dcb::Bool>("useSpecular")
					.Add<Dcb::Bool>("useAlpha");

				Dcb::Buffer PSBuffer{ std::move(PSLayout) };

				auto diffuse = loadTexture(graphics, path, material, aiTextureType_DIFFUSE, 0);
				PSBuffer["useDiffuse"] = diffuse.get() != nullptr;
				if (diffuse)
				{
					step.AddBindable(diffuse);
					stepHasAlpha.AddBindable(diffuse);
				}
				else
				{
					auto baseColor = loadTexture(graphics, path, material, aiTextureType_BASE_COLOR, 0);
					PSBuffer["useDiffuse"] = baseColor.get() != nullptr;

					if (baseColor)
					{
						step.AddBindable(baseColor);
						stepHasAlpha.AddBindable(baseColor);
					}

				}

				auto normal = loadTexture(graphics, path, material, aiTextureType_NORMALS, 1);
				PSBuffer["useNormal"] = normal.get() != nullptr;
				if (normal != nullptr)
				{
					step.AddBindable(normal);
					stepHasAlpha.AddBindable(normal);
				}

				auto specular = loadTexture(graphics, path, material, aiTextureType_SPECULAR, 2);
				PSBuffer["useSpecular"] = specular.get() != nullptr;
				if (specular != nullptr)
				{
					step.AddBindable(specular);
					stepHasAlpha.AddBindable(specular);
				}

				auto alpha = loadTexture(graphics, path, material, aiTextureType_OPACITY, 3);
				PSBuffer["useAlpha"] = alpha != nullptr;
				if (alpha != nullptr)
				{
					step.AddBindable(alpha);
					stepHasAlpha.AddBindable(alpha);
				}

				auto pixelCbuf1 = std::make_shared<Bind::CachingPixelConstantBufferEx>(graphics, std::move(PSBuffer), 1u);
				step.AddBindable(pixelCbuf1);
				stepHasAlpha.AddBindable(pixelCbuf1);

				auto pixelCubf2 = std::make_shared<PixelCBufAlpha>(graphics, 2u);
				step.AddBindable(pixelCubf2);
				stepHasAlpha.AddBindable(pixelCubf2);

				// 쉐이더 이름
				auto pixelShader = PixelShader::Create(graphics, "../Resources/Shader/Model_PS.hlsl");
				step.AddBindable(pixelShader);
				stepHasAlpha.AddBindable(pixelShader);
			}


			renderTech.AddStep(std::move(step));
			renderTech.AddStep(std::move(stepHasAlpha));
		}
		mTechniques.push_back(std::move(renderTech));
	}
}

std::shared_ptr<Bind::Texture> Material::loadTexture(Graphics& graphics, const std::filesystem::path& path, const aiMaterial& material, aiTextureType aitype, unsigned int bindIndex)
{
	std::vector<std::string> findPaths =
	{
		"/../Texture/",
		"/../TestTexture/"
	};

	for (auto findPath : findPaths)
	{
		const auto rootPath = path.parent_path().string() + findPath;

		aiString texturePath;

		if (material.GetTexture(aitype, 0, &texturePath) == AI_SUCCESS)
		{
			std::filesystem::path filePath = Util::ToWide(texturePath.C_Str());

			return Bind::Texture::Create(graphics, rootPath + filePath.filename().string(), bindIndex);

		}
	}

	return nullptr;
}

