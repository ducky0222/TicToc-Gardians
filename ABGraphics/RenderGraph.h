#pragma once

class Graphics;

namespace Bind
{
	class RenderTarget;
	class DepthStencil;
}

namespace Rgph
{
	class Pass;
	class RenderQueuePass;
	class Source;
	class Sink;

	class RenderGraph
	{
	public:
		RenderGraph(Graphics& graphics);
		virtual ~RenderGraph() = default;

		virtual void Execute(Graphics& graphics);
		void Reset();

		RenderQueuePass& GetRenderQueue(const std::string& passName);

	protected:
		void finalize();

		void addGlobalSource(std::unique_ptr<Source>);
		void addGlobalSink(std::unique_ptr<Sink>);
		void appendPass(std::unique_ptr<Pass> pass);

		void setSinkTarget(const std::string& sinkName, const std::string& target);

		Pass* findPassByNameOrNull(const std::string& name);

	private:
		// 현재 싱크가 원하는 소스를 연결해주는 함수
		void linkSinks(Pass& pass);
		void linkGlobalSinks();

	protected:
		std::vector<std::unique_ptr<Pass>> mPasses;
		std::vector<std::unique_ptr<Source>> mGlobalSources;
		std::vector<std::unique_ptr<Sink>> mGlobalSinks;

		std::shared_ptr<Bind::RenderTarget> mBackBufferRenderTarget;
		std::shared_ptr<Bind::RenderTarget> mRenderTarget;
		std::shared_ptr<Bind::RenderTarget> mPostEffectRenderTarget;
		std::shared_ptr<Bind::DepthStencil> mMasterDepth;

		bool mbIsFinalized = false;
	};
}