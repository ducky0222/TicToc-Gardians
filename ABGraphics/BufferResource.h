#pragma once

class Graphics;

namespace Bind
{
	class BufferResource
	{
	public:
		virtual ~BufferResource() = default;

		virtual void BindBuffer(Graphics&) = 0;
		virtual void BindBuffer(Graphics&, BufferResource*) = 0;
		virtual void Clear(Graphics&) = 0;
	};
}