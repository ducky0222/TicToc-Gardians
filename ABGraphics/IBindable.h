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
* ��� IBindable�� UID�� Ű�� ���ҽ��Ŵ������� �����ȴ�.
* IBindable�� friend Class�� �����Ͽ� Device�� Context ���� ������ ���������� ����д�.
*/