#include "SceneIntro.h"
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

	return ret;
}

bool SceneIntro::Update(float dt)
{
	bool ret = true;
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		app->audio->PlayFx(entersound);
		app->fade->FadeToBlack1(this, (Module*)app->scene, 20);
	}

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		ret = false;
	}
		

	return ret;
}

// Update: draw background
bool SceneIntro::PostUpdate()
{
	app->render->DrawTexture(bgTexture,0,0);
	return true;
}

bool SceneIntro::CleanUp()
{
	app->tex->UnLoad(bgTexture);
	bgTexture = nullptr;
	return true;
}