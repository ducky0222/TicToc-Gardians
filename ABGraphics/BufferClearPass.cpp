#include "pch.h"

#include "BufferClearPass.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "Sink.h"
#include "Source.h"

namespace Rgph
{
	BufferClearPass::BufferClearPass(std::string name)
		: Pass(std::move(name))
	{
		registerSink(DirectBufferSink<Bind::BufferResource>::Make("buffer", buffer));
		registerSource(DirectBufferSource<Bind::BufferResource>::Make("buffer", buffer));
	}

	void BufferClearPass::Execute(Graphics& graphics) const
	{
		buffer->Clear(graphics);
	}
}