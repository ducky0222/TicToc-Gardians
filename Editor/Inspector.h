#pragma once
#include "IWindow.h"

class TypeInfo;
class Property;
class GameObjectManager;
struct Component;

class Inspector : public IWindow
{
public:
	Inspector(GameObjectManager* goManger);
	~Inspector() override = default;

	void OnImGuiRender() override;
	

private:
	void RenderMember(const Property* property, Component* component);

	GameObjectManager* m_GameObjectManager;
	Property* selectedProperty = nullptr;

	string inspectorInput;
	string input;

	Component* rightClickComponent = nullptr;

};