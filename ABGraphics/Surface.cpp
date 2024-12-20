#include "pch.h"

#include <sstream>

#include "Surface.h"
#include "Util.h"

Surface::Surface(unsigned int width, unsigned int height)
{
	HRESULT hr = scratch.Initialize2D(
		format,
		width, height, 1u, 1u
	);

	assert(SUCCEEDED(hr));
}

void Surface::Clear(Color fillValue)
{
	const auto width = GetWidth();
	const auto height = GetHeight();
	auto& imgData = *scratch.GetImage(0, 0, 0);
	for (size_t y = 0u; y < height; y++)
	{
		auto rowStart = reinterpret_cast<Color*>(imgData.pixels + imgData.rowPitch * y);
		std::fill(rowStart, rowStart + imgData.width, fillValue);
	}
}

void Surface::PutPixel(unsigned int x, unsigned int y, Color c)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < GetWidth());
	assert(y < GetHeight());
	auto& imgData = *scratch.GetImage(0, 0, 0);
	reinterpret_cast<Color*>(&imgData.pixels[y * imgData.rowPitch])[x] = c;
}

Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < GetWidth());
	assert(y < GetHeight());
	auto& imgData = *scratch.GetImage(0, 0, 0);
	return reinterpret_cast<Color*>(&imgData.pixels[y * imgData.rowPitch])[x];
}

unsigned int Surface::GetWidth() const
{
	return (unsigned int)scratch.GetMetadata().width;
}

unsigned int Surface::GetHeight() const
{
	return (unsigned int)scratch.GetMetadata().height;
}

Surface::Color* Surface::GetBufferPtr()
{
	return reinterpret_cast<Color*>(scratch.GetPixels());
}

const Surface::Color* Surface::GetBufferPtr() const
{
	return const_cast<Surface*>(this)->GetBufferPtr();
}

const Surface::Color* Surface::GetBufferPtrConst() const
{
	return const_cast<Surface*>(this)->GetBufferPtr();
}

Surface Surface::FromFile(const std::string& name)
{
	DirectX::ScratchImage scratch;
	
	std::wstring wideName = Util::ToWide(name).c_str();

	HRESULT hr = DirectX::LoadFromWICFile(wideName.c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, nullptr, scratch);

	if (FAILED(hr))
	{
		hr = LoadFromTGAFile(wideName.c_str(), nullptr, scratch);
	}
	if (FAILED(hr))
	{
		hr = LoadFromDDSFile(wideName.c_str(), DDS_FLAGS_NONE, nullptr, scratch);
	}
	if (FAILED(hr))
	{
		hr = LoadFromHDRFile(wideName.c_str(), nullptr, scratch);
	}
	assert(SUCCEEDED(hr));

	if (scratch.GetImage(0, 0, 0)->format != format)
	{
		DirectX::ScratchImage converted;
		hr = DirectX::Convert(
			*scratch.GetImage(0, 0, 0),
			format,
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			converted
		);

		assert(SUCCEEDED(hr));

		return Surface(std::move(converted));
	}

	return Surface(std::move(scratch));
}

void Surface::Save(const std::string& filename) const
{
	const auto GetCodecID = [](const std::string& filename) {
		const std::filesystem::path path = filename;
		const auto ext = path.extension().string();

		if (ext == ".png")
		{
			return DirectX::WIC_CODEC_PNG;
		}
		else if (ext == ".jpg")
		{
			return DirectX::WIC_CODEC_JPEG;
		}
		else if (ext == ".bmp")
		{
			return DirectX::WIC_CODEC_BMP;
		}

		assert(false);
	};

	HRESULT hr = DirectX::SaveToWICFile(
		*scratch.GetImage(0, 0, 0),
		DirectX::WIC_FLAGS_NONE,
		GetWICCodec(GetCodecID(filename)),
		Util::ToWide(filename).c_str()
	);

	assert(SUCCEEDED(hr));
}

bool Surface::AlphaLoaded() const
{
	return !scratch.IsAlphaAllOpaque();
}

Surface::Surface(DirectX::ScratchImage scratch)
	:
	scratch(std::move(scratch))
{
}
