#pragma once

class Graphics;
class IDrawable;
class Step;

namespace Rgph
{
	class Job
	{
	public:
		Job(const Step* pStep, IDrawable* pDrawable);
		~Job() = default;

		void Execute(Graphics& graphics, bool hasAlpha, size_t layer) const;

	private:
		IDrawable* mpDrawable;
		const Step* mpStep;
	};
}