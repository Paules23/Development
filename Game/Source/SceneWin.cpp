#include "SceneWin.h"

#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Audio.h"
#include "Input.h"
#include "FadeToBlack.h"
#include "Player.h"
#include "Log.h"
#include "EntityManager.h"
#include <stdio.h>

SceneWin::SceneWin(bool startEnabled) : Module(startEnabled)
{

}

SceneWin::~SceneWin()
{

}

// Load assets
bool SceneWin::Start()
{
	bool ret = true;
	texturepath = app->LoadConfig2().child("sceneWin").attribute("texturepath").as_string();
	winTexture = app->tex->Load(texturepath);
	app->render->camera.x = 0;

	return ret;
}

bool SceneWin::Update(float dt)
{
	bool ret = true;
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		ret = false;
	}
	return ret;
}

// Update: draw background
bool SceneWin::PostUpdate()
{
	app->render->DrawTexture(winTexture, 0, 0);
	return true;
}

bool SceneWin::CleanUp()
{
	app->tex->UnLoad(winTexture);
	winTexture = nullptr;
	//App->textures->Unload(bgTexture);
	//App->fonts->UnLoad(introFont);
	//bgTexture = nullptr;
	return true;
}