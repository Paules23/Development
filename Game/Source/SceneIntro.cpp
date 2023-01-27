#include "SceneIntro.h"
#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Audio.h"
#include "Input.h"
#include "FadeToBlack.h"
#include "Log.h"
#include "GuiManager.h"
#include "Window.h"
#include "HUD.h"
#include "Map.h"
#include "Map2.h"
#include "Scene.h"
#include "Scene2.h"
#include <stdio.h>

SceneIntro::SceneIntro(bool startEnabled) : Module(startEnabled)
{

}

SceneIntro::~SceneIntro()
{

}

// Load assets
bool SceneIntro::Start()
{
	bool ret = true;

	entersound = app->audio->LoadFx("Assets/Audio/Fx/Entergame.ogg");
	texturepath = app->LoadConfig2().child("sceneIntro").attribute("texturepath").as_string();
	bgTexture = app->tex->Load(texturepath);

	//buttons creation
	//menu buttons
	uint w, h;
	app->win->GetWindowSize(w, h);
	Continue = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Continue", { (int)w / 2 - 50,(int)h / 2 + 100,104,44 }, this);
	Continue->parameters = app->LoadConfig2().child("scene").child("button");

	settings = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Settings", { (int)w / 2 - 50,(int)h / 2+ 200,104,44 }, this);
	settings->parameters = app->LoadConfig2().child("scene").child("button");

	play = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "Play", { (int)w / 2 - 50,(int)h / 2 + 150,104,44 }, this);
	play->parameters = app->LoadConfig2().child("scene").child("button");

	Exit = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "Exit", { (int)w / 2 - 50,(int)h / 2 + 250,104,44 }, this);
	Exit->parameters = app->LoadConfig2().child("scene").child("button");
	//settings buttons

	musicVolume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 11, "Music volume", { (int)w / 2 - 50,(int)h / 2 + 100,104,44 }, this);
	musicVolume->parameters = app->LoadConfig2().child("scene").child("button");

	musicVolumeSlider = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 5, "", { (int)w / 2 + 50,(int)h / 2 + 110,30,27 }, this, { (int)w / 2 + 50,(int)h / 2 + 110,100,44 });
	musicVolumeSlider->parameters = app->LoadConfig2().child("scene").child("slider");

	fxVolume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 12, "Fx Volume", { (int)w / 2 - 50,(int)h / 2 + 150,104,44 }, this);
	fxVolume->parameters = app->LoadConfig2().child("scene").child("button");

	fxVolumeSlider = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 6, "", { (int)w / 2 + 50,(int)h / 2 + 160,30,27 }, this, { (int)w / 2 + 50,(int)h / 2 + 160,100,44 });
	fxVolumeSlider->parameters = app->LoadConfig2().child("scene").child("slider");

	fullscreen = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "Fullscreen", { (int)w / 2 - 50,(int)h / 2+200,104,44 }, this);
	fullscreen->parameters = app->LoadConfig2().child("scene").child("button");

	fullscreenCheck = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 10, "", { (int)w / 2 - 50 + 104,(int)h / 2+200,104,36 }, this);
	fullscreenCheck->parameters = app->LoadConfig2().child("scene").child("checkbox");

	vsync = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, "Vsync", { (int)w / 2 - 50,(int)h / 2 + 250,104,44 }, this);
	vsync->parameters = app->LoadConfig2().child("scene").child("button");

	VsyncCheck = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 8, "", { (int)w / 2 - 50 + 104,(int)h / 2 + 250,40,36 }, this);
	VsyncCheck->parameters = app->LoadConfig2().child("scene").child("checkbox");

	goBack = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 13, "Return", { (int)w / 2 - 50,(int)h / 2 + 300,104,44 }, this);
	goBack->parameters = app->LoadConfig2().child("scene").child("button");


	app->guiManager->Enable();
	Play = false;
	resume = false;

	ListItem<GuiControl*>* control = app->guiManager->guiControlsList.start;

	while (control != nullptr)
	{
		for (int i = 1; i < 5; ++i) {
			if (control->data->id == i) {
				control->data->enabled = true;
			}
		}
		control = control->next;
	}

	return ret;
}

bool SceneIntro::Update(float dt)
{
	app->render->camera.x = 0;
	bool ret = true;
	if (exit == true) {
		return false;
	}

	if (Play == true) {
		app->audio->PlayFx(entersound);
		app->guiManager->Disable();
		app->hud->Enable();
		app->map->Enable();
		app->fade->FadeToBlack1(this, (Module*)app->scene, 20);
		Play = false;
	}
	if (resume) {
		if (app->scene->level1) {
			app->audio->PlayFx(entersound);
			app->guiManager->Disable();
			app->hud->Enable();
			app->map->Enable();
			app->fade->FadeToBlack1(this, (Module*)app->scene, 20);
		}
		else {
			app->audio->PlayFx(entersound);
			app->guiManager->Disable();
			app->hud->Enable();
			app->map2->Enable();
			app->fade->FadeToBlack1(this, (Module*)app->scene2, 20);
		}
		resume = false;
	}
	if (app->guiManager->settings == true) {
		ListItem<GuiControl*>* control = app->guiManager->guiControlsList.start;

		while (control != nullptr)
		{
			for (int i = 1; i < 5; ++i) {
				if (control->data->id == i) {
					control->data->enabled = false;
				}
			}
			control = control->next;
		}

		control = app->guiManager->guiControlsList.start;

		while (control != nullptr)
		{
			for (int i = 5; i < 14; ++i) {
				if (control->data->id == i) {
					control->data->enabled = true;
				}
			}
			control = control->next;
		}
	}

	if (app->guiManager->settings == false) {
		ListItem<GuiControl*>* control = app->guiManager->guiControlsList.start;

		while (control != nullptr)
		{
			for (int i = 5; i < 14; ++i) {
				if (control->data->id == i) {
					control->data->enabled = false;
				}
			}
			control = control->next;
		}
		control = app->guiManager->guiControlsList.start;

		while (control != nullptr)
		{
			for (int i = 1; i < 5; ++i) {
				if (control->data->id == i) {
					control->data->enabled = true;
				}
			}
			control = control->next;
		}
	}


	return ret;
}

// Update: draw background
bool SceneIntro::PostUpdate()
{
	app->render->DrawTexture(bgTexture, app->render->camera.x, app->render->camera.y);
	app->guiManager->Draw();

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		if (app->guiManager->settings == true) {
			app->guiManager->activateSettings();
		}
	}
	return true;
}

bool SceneIntro::CleanUp()
{
	app->tex->UnLoad(bgTexture);
	bgTexture = nullptr;
	return true;
}

bool SceneIntro::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Event by %d ", control->id);

	switch (control->id)
	{
	case 1:
		resume = true;
		LOG("Button 1 click");
		break;
	case 2:
		app->guiManager->settings = true;
		LOG("Button 2 click");
		break;
	case 3:
		Play = true;
		LOG("Button 3 click");
		break;
	case 4:
		exit = true;
		LOG("Button 4 click");
		break;
	case 5:
		LOG("Button 5 click");
		break;
	case 6:
		LOG("Button 6 click");
		break;
	case 7:
		LOG("Button 7 click");
		break;
	case 8:
		LOG("Button 8 click");
		/*app->render->Vsync = !app->render->Vsync;
		SDL_GL_SetSwapInterval(app->render->Vsync);*/
		/*if (app->render->Vsync == true) {

		}
		else {
			SDL_GL_SetSwapInterval(app->render->Vsync);
		}*/
		break;
	case 9:
		LOG("Button 9 click");
		break;
	case 10:
		LOG("Button fullscreen click");
		app->win->fullscreen = !app->win->fullscreen;
		if (app->win->fullscreen) {
			SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN);
		}
		else {
			SDL_SetWindowFullscreen(app->win->window, 0);
		}
		break;
	case 13:
		app->guiManager->settings = false;
		break;
	}

	return true;
}