#include "GuiButton.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Textures.h"
#include "GuiManager.h"
#include "Log.h"

GuiButton::GuiButton(uint32 id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;
}

GuiButton::~GuiButton()
{

}

bool GuiButton::Start() {

	texturePath = parameters.attribute("texturepath").as_string();
	clickAudioPath = parameters.attribute("clickaudiopath").as_string();
	focusAudioPath = parameters.attribute("focusaudiopath").as_string();

	clickaudioFxId = app->audio->LoadFx(clickAudioPath);
	focusaudioFxId = app->audio->LoadFx(focusAudioPath);
	texture = app->tex->Load(texturePath);

	if (this->id == 9 || this->id == 7 || this->id == 12 || this->id == 13 || this->id == 8 || this->id == 10 || this->id == 11) {
		canClick = false;
	}

	return true;
}

bool GuiButton::Update(float dt)
{
	if (canClick == false) {
		state = GuiControlState::DISABLED;
	}
	if (state != GuiControlState::DISABLED)
	{
		// L15: DONE 3: Update the state of the GUiButton according to the mouse position
		app->input->GetMousePosition(mouseX, mouseY);

		GuiControlState previousState = state;

		// I'm inside the limitis of the button
		if (mouseX >= bounds.x && mouseX <= bounds.x + bounds.w &&
			mouseY >= bounds.y && mouseY <= bounds.y + bounds.h) {

			state = GuiControlState::FOCUSED;
			

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT) {
				state = GuiControlState::PRESSED;
			}

			
			if (previousState == GuiControlState::FOCUSED && state == GuiControlState::PRESSED) {
				LOG("Change state from %d to %d", previousState, state);
				app->audio->PlayFx(clickaudioFxId);
			}
			else if (previousState == GuiControlState::NORMAL && state == GuiControlState::FOCUSED) {
				LOG("Change state from %d to %d", previousState, state);
				app->audio->PlayFx(focusaudioFxId);
			}

			//
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP) {
				NotifyObserver();
			}
		}
		else {
			state = GuiControlState::NORMAL;
		}
	}

	return false;
}

bool GuiButton::Draw(Render* render)
{
	//L15: DONE 4: Draw the button according the GuiControl State

	switch (state)
	{
	case GuiControlState::DISABLED:
		section = { 124,74,104,44 };
		if (app->guiManager->menuDebug) {
			app->render->DrawRectangle({ bounds.x,bounds.y,bounds.w,bounds.h }, 0, 0, 0, 255, false);
		}
		break;
	case GuiControlState::NORMAL:
		section = {124,10,104,44};
		if (app->guiManager->menuDebug) {
			app->render->DrawRectangle({ bounds.x,bounds.y,bounds.w,bounds.h }, 0, 0, 255, 255, false);
		}
		break;
	case GuiControlState::FOCUSED:
		section = { 12,10,104,44 };
		if (app->guiManager->menuDebug) {
			app->render->DrawRectangle({ bounds.x,bounds.y,bounds.w,bounds.h }, 0, 255, 0, 255, false);
		}
		break;
	case GuiControlState::PRESSED:
		section = { 12,74,104,44 };
		if (app->guiManager->menuDebug) {
			app->render->DrawRectangle({ bounds.x,bounds.y,bounds.w,bounds.h }, 255, 0, 0, 255, false);
		}
		break;
	}

	render->DrawTexture(texture, bounds.x, bounds.y, &section,0);
	app->render->DrawText(text.GetString(), bounds.x+10, bounds.y+10, bounds.w-20, bounds.h-20, { 0,0,0 });

	return false;
}

bool GuiButton::CleanUp()
{
	app->tex->UnLoad(texture);
	return true;
}