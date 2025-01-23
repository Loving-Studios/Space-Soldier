#include "GuiControlButton.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"

GuiControlButton::GuiControlButton(int id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;
}

GuiControlButton::~GuiControlButton()
{

}

bool GuiControlButton::Update(float dt)
{
	if (state == GuiControlState::DISABLED) {
		return false; // No interactúa ni se dibuja
	}

	if (state == GuiControlState::INACTIVE) {
		// Dibujar el botón como inactivo y terminar la función
		Engine::GetInstance().render->DrawRectangle(bounds, 100, 100, 100, 150, true, false);
		Engine::GetInstance().render->DrawText(text.c_str(), bounds.x, bounds.y, bounds.w, bounds.h, { 128, 128, 128, 255 });
		return false;
	}

	Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

	if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w &&
		mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h)
	{
		state = GuiControlState::FOCUSED;

		if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
			state = GuiControlState::PRESSED;
		}

		if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
			NotifyObserver();
		}
	}
	else {
		state = GuiControlState::NORMAL;
	}

	switch (state)
	{
	case GuiControlState::NORMAL:
		Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 0, 255, true, false);
		Engine::GetInstance().render->DrawText(text.c_str(), bounds.x, bounds.y, bounds.w, bounds.h, { 255, 255, 255, 255 }); // Texto blanco
		break;
	case GuiControlState::FOCUSED:
		Engine::GetInstance().render->DrawRectangle(bounds, 150, 150, 150, 10, true, false);
		Engine::GetInstance().render->DrawText(text.c_str(), bounds.x, bounds.y, bounds.w, bounds.h, { 0, 0, 0, 255 }); // Texto negro
		break;
	case GuiControlState::PRESSED:
		Engine::GetInstance().render->DrawRectangle(bounds, 0, 255, 0, 255, true, false);
		Engine::GetInstance().render->DrawText(text.c_str(), bounds.x, bounds.y, bounds.w, bounds.h, { 255, 255, 255, 255 }); // Texto blanco
		break;
	}

	return false;
}

