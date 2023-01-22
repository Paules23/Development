#include "SceneDeath.h"
#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Audio.h"
#include "Input.h"
#include "FadeToBlack.h"
#include "Log.h"
#include "GuiManager.h"
#include "Window.h"
#include "Map.h"
#include "HUD.h"
#include <stdio.h>

SceneDeath::SceneDeath(bool startEnabled) : Module(startEnabled)
{

}

SceneDeath::~SceneDeath()
{

}

// Load assets
bool SceneDeath::Start()
{
	bool ret = true;
	texturepath = app->LoadConfig2().child("sceneDeath").attribute("texturepath").as_string();
	bgTexture = app->tex->Load(texturepath);

	return ret;
}

bool SceneDeath::Update(float dt)
{
	app->render->camera.x = 0;
	bool ret = true;
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		app->audio->PlayFx(entersound);
		app->hud->Enable();
		app->map->Enable();
		app->fade->FadeToBlack1(this, (Module*)app->scene, 20);
	}

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		ret = false;
	}

	return ret;
}

// Update: draw background
bool SceneDeath::PostUpdate()
{
	app->render->DrawTexture(bgTexture, 0, 0);
	return true;
}

bool SceneDeath::CleanUp()
{
	app->tex->UnLoad(bgTexture);
	bgTexture = nullptr;

	return true;
}

//for buttons in the future if i have time
//bool SceneDeath::OnGuiMouseClickEvent(GuiControl* control)
//{
//	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
//	LOG("Event by %d ", control->id);
//
//	switch (control->id)
//	{
//	case 1:
//		LOG("Button 1 click");
//		break;
//	case 2:
//		LOG("Button 2 click");
//		break;
//	case 3:
//		LOG("Button 3 click");
//		break;
//	case 4:
//		LOG("Button 4 click");
//		break;
//	case 5:
//		LOG("Button 5 click");
//		break;
//	case 6:
//		LOG("Button 6 click");
//		break;
//	case 7:
//		LOG("Button 7 click");
//		break;
//	case 8:
//		LOG("Button 8 click");
//		break;
//	case 9:
//		LOG("Button 9 click");
//		break;
//	}
//
//	return true;
//}