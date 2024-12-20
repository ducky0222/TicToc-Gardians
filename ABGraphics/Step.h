#pragma once

#include "IBindable.h"
#include "Graphics.h"

class TechniqueProbe;
class IDrawable;

namespace Rgph
{
	class RenderQueuePass;
	class RenderGraph;
}
/*
Step은 기법(technique)을 수행하기 위해 필요한 여러 단계를 정의하는 클래스이다.
*/

class Step
{
public:
	Step(std::string targetPassName);
	~Step() = default;
	Step(const Step& src);
	Step& operator=(const Step&) = delete;
	Step(Step&&) = default;
	Step& operator=(Step&&) = delete;

	void Bind(Graphics& graphics) const;

	void Submit(IDrawable& drawable) const;
	void InitReferences(const IDrawable& parent);
	void Link(Rgph::RenderGraph& renderGraph);

	void AddBindable(std::shared_ptr<IBindable> bind_in);

private:
	std::vector<std::shared_ptr<IBindable>> mBindables;
	Rgph::RenderQueuePass* mpTargetPass = nullptr;
	std::string mTargetPassName;
};