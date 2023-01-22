#include "GuiCheckBox.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Textures.h"
#include "Log.h"

GuiCheckBox::GuiCheckBox(uint32 id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::CHECKBOX, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;
}

GuiCheckBox::~GuiCheckBox()
{

}

bool GuiCheckBox::Start() {

	texturePath = parameters.attribute("texturepath").as_string();
	clickAudioPath = parameters.attribute("clickaudiopath").as_string();
	focusAudioPath = parameters.attribute("focusaudiopath").as_string();

	clickaudioFxId = app->audio->LoadFx(clickAudioPath);
	focusaudioFxId = app->audio->LoadFx(focusAudioPath);
	texture = app->tex->Load(texturePath);
	checked = false;

	return true;
}

bool GuiCheckBox::Update(float dt)
{
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
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN) {
				checked = !checked;
			}


			if (previousState == GuiControlState::FOCUSED && state == GuiControlState::PRESSED) {
				LOG("Change state from %d to %d", previousState, state);
				app->audio->PlayFx(clickaudioFxId);
			}
			else if (previousState == GuiControlState::NORMAL && state == GuiControlState::FOCUSED) {
				LOG("Change state from %d to %d", previousState, state);
				app->audio->PlayFx(focusaudioFxId);
			}
		}
		else {
			state = GuiControlState::NORMAL;
		}
	}

	return false;
}

bool GuiCheckBox::Draw(Render* render)
{
	//L15: DONE 4: Draw the button according the GuiControl State

	switch (state)
	{
	case GuiControlState::DISABLED:
		render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
		break;
	case GuiControlState::NORMAL:
		if (checked) {
			section = { 140,10,40,44 };
		}
		else {
			section = { 140,74,40,44 };
		}
		break;
	case GuiControlState::FOCUSED:
		if (checked) {
			section = { 12,74,40,44 };
		}
		else {
			section = { 12,10,40,44 };
		}
		break;
	case GuiControlState::PRESSED:
		if (checked) {
			section = { 76,74,40,44 };
		}
		else {
			section = { 76,10,40,44 };
		}
		break;
	}
	render->DrawTexture(texture, bounds.x, bounds.y, &section);
	/*app->render->DrawText(text.GetString(), bounds.x + 10, bounds.y + 10, bounds.w - 20, bounds.h - 20, { 0,0,0 });*/

	return false;
}