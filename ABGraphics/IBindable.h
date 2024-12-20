#pragma once

#include "GraphicsResource.h"

class Graphics;
class TechniqueProbe;

class IBindable : public GraphicsResource
{
public:
	IBindable() = default;
	virtual ~IBindable() = default;

	virtual void Bind(Graphics& graphics) = 0;
	virtual void InitReference(const class IDrawable& drawable) {};
	virtual void Accept(TechniqueProbe&) {};

	virtual inline std::string GetUID() const;
};

std::string IBindable::GetUID() const
{
	assert(false);
	return "";
}

/*
* 모든 IBindable은 UID를 키로 리소스매니저에서 관리된다.
* IBindable만 friend Class로 지정하여 Device와 Context 접근 권한을 제한적으로 열어둔다.
*/