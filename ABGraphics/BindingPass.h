#pragma once

#include "Pass.h"
#include "Sink.h"

#include "IBindable.h"

namespace Rgph
{
	class BindingPass : public Pass
	{
	protected:
		BindingPass(std::string name, std::vector<std::shared_ptr<IBindable>> binds = {});
		virtual ~BindingPass() = default;

		void AddBind(std::shared_ptr<IBindable> bind);
		void BindAll(Graphics& graphics) const;
		void Finalize() override;

	protected:
		template<class T>
		void addBindSink(std::string name);

	private:
		void bindBufferResources(Graphics& graphics) const;

	protected:
		std::shared_ptr<Bind::RenderTarget> mRenderTarget;
		std::shared_ptr<Bind::DepthStencil> mDepthStencil;

	private:
		std::vector<std::shared_ptr<IBindable>> binds;
	};

	template<class T>
	void BindingPass::addBindSink(std::string name)
	{
		const auto index = binds.size();
		binds.emplace_back();
		registerSink(std::make_unique<ContainerBindableSink<T>>(std::move(name), binds, index));
	}
}