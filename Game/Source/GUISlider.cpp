#include "GuiSlider.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Textures.h"
#include "Log.h"
#include "SDL/include/SDL.h"
#include "SDL_mixer/include/SDL_mixer.h"

GuiSlider::GuiSlider(uint32 id, SDL_Rect bounds, const char* text, SDL_Rect sliderBounds) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;
	this->sliderbounds = sliderBounds;

	canClick = true;
	drawBasic = false;
}

GuiSlider::~GuiSlider()
{

}

bool GuiSlider::Start() {

	texturePath = parameters.attribute("texturepath").as_string();
	clickAudioPath = parameters.attribute("clickaudiopath").as_string();
	focusAudioPath = parameters.attribute("focusaudiopath").as_string();

	clickaudioFxId = app->audio->LoadFx(clickAudioPath);
	focusaudioFxId = app->audio->LoadFx(focusAudioPath);
	texture = app->tex->Load(texturePath); 

	bounds.x = sliderbounds.x + sliderbounds.w;


	return true;
}

bool GuiSlider::Update(float dt)
{
	if (canClick == false) {
		state = GuiControlState::NORMAL;
		return false;
	}
	if (state != GuiControlState::DISABLED)
	{
		// L15: DONE 3: Update the state of the GUiButton according to the mouse position
		app->input->GetMousePosition(mouseX, mouseY);

		GuiControlState previousState = state;
		state = GuiControlState::NORMAL;

		// I'm inside the limitis of the button
		if (mouseX >= bounds.x && mouseX <= bounds.x + bounds.w &&
			mouseY >= bounds.y && mouseY <= bounds.y + bounds.h) {

			/*state = GuiControlState::FOCUSED;*/


			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT) {
				state = GuiControlState::PRESSED;
			}


			if (previousState == GuiControlState::PRESSED && state == GuiControlState::NORMAL) {
				/*if (mouseX > sliderbounds.x + 12.5f && mouseX < sliderbounds.x + 37.5f) {
					bounds.x = sliderbounds.x + 25 - bounds.w / 2;
				}else if (mouseX > sliderbounds.x + 37.5f && mouseX < sliderbounds.x + 62.5f) {
					bounds.x = sliderbounds.x + 50 - bounds.w/2;
				}
				else if (mouseX > sliderbounds.x + 62.5f && mouseX < sliderbounds.x + 87.5f) {
					bounds.x = sliderbounds.x + 75 - bounds.w / 2;
				}
				else if(mouseX < sliderbounds.x + 12.5f && mouseX > sliderbounds.x -1){
					bounds.x = sliderbounds.x;
				}
				else if (mouseX > sliderbounds.x + 87.5f && mouseX < sliderbounds.x +sliderbounds.w +1) {
					bounds.x = sliderbounds.x + 100 - bounds.w/2;
				}
				else if (mouseX > sliderbounds.x + 100.0f) {
					bounds.x = sliderbounds.x + sliderbounds.w;
				}*/
			}
			else if (previousState == GuiControlState::NORMAL && state == GuiControlState::FOCUSED) {
				LOG("Change state from %d to %d", previousState, state);
				app->audio->PlayFx(focusaudioFxId);
			}
			if (state == GuiControlState::PRESSED) {
				bounds.x = mouseX - (bounds.w / 2);
				if (bounds.x > sliderbounds.x + sliderbounds.w) {
					bounds.x = sliderbounds.x + sliderbounds.w;
				}
				else if (bounds.x < sliderbounds.x) {
					bounds.x = sliderbounds.x;
				}
			}

			if (this->id == 5) {
				Mix_VolumeMusic(bounds.x-sliderbounds.x);
			}
			if (this->id == 6) {
				Mix_Volume(-1,bounds.x - sliderbounds.x);
			}
		}
	}

	return false;
}

bool GuiSlider::Draw(Render* render)
{
	//L15: DONE 4: Draw the button according the GuiControl State
	app->input->GetMousePosition(mouseX, mouseY);
	switch (state)
	{
	case GuiControlState::DISABLED:
		break;
	case GuiControlState::NORMAL:
		section = { 18,140,119,21 };
		render->DrawTexture(texture, sliderbounds.x, sliderbounds.y, &section,0);
		//section of the slider
		section = { 147,133,30,27 };
		render->DrawTexture(texture, bounds.x, bounds.y, &section,0);
		break;
	//case GuiControlState::FOCUSED:
	//	section = { 18,140,119,21 };
	//	render->DrawTexture(texture, bounds.x, bounds.y, &section);
	//	//section of the slider
	//	section = { 147,133,30,27 };
	//	render->DrawTexture(texture, mouseX, bounds.y, &section);
	//	break;
	case GuiControlState::PRESSED:
		
		//section of the rectangle
		section = { 18,140,119,21 };
		render->DrawTexture(texture, sliderbounds.x, sliderbounds.y, &section,0);
		//section of the slider
		section = { 197,133,30,27 };
		render->DrawTexture(texture, bounds.x, sliderbounds.y, &section,0);
		break;
	}
	//app->render->DrawText(text.GetString(), bounds.x + 10, bounds.y + 10, bounds.w - 20, bounds.h - 20, { 0,0,0 });

	return false;
}
bool GuiSlider::CleanUp()
{
	app->tex->UnLoad(texture);
	return true;
}