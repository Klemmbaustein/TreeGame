#include "MenuObject.h"
#include "UI/MenuUI.h"
#include "Engine/Input.h"
#include "Objects/Components/CameraComponent.h"

void MenuObject::Begin()
{
	UICanvas::CreateNewCanvas<MenuUI>();
	Input::CursorVisible = true;
	CameraComponent* cam = new CameraComponent();
	Attach(cam);
	cam->Use();
}

void MenuObject::Update()
{
}

void MenuObject::Destroy()
{
}
