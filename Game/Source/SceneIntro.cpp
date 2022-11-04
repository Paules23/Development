#include "SceneIntro.h"

#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Audio.h"
#include "Input.h"
#include "FadeToBlack.h"
#include "Player.h"
#include "Log.h"
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
	LOG("Loading background assets");

	bool ret = true;

	
	//App->audio->PlayMusic("Assets/Music/introTitle.ogg", 1.0f);

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	
	return ret;
}

bool SceneIntro::Update()
{
	//if (App->input->keys[SDL_SCANCODE_SPACE] == Key_State::KEY_DOWN) {
	//	credits = 1;
	//	int audioCredit = App->audio->LoadFx("Assets/gwar-191.wav");
	//	App->audio->PlayFx(audioCredit);

	//}

	//if (App->input->keys[SDL_SCANCODE_RETURN] == Key_State::KEY_DOWN && credits > 0) {
	//	App->fade->FadeToBlack(this, (Module*)App->scenePhoto, 20);


	//	credits--;
	//}
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		app->SaveGameRequest();

	return true;
}

// Update: draw background
bool SceneIntro::PostUpdate()
{
	//// Draw everything --------------------------------------
	//sprintf_s(creditsText, 10, "%d", credits);

	//App->render->Blit(bgTexture, 0, 30, NULL);
	//App->fonts->BlitText(315, 475, introFont, "CREDIT ");
	//App->fonts->BlitText(370, 475, introFont, creditsText);
	return true;
}

bool SceneIntro::CleanUp()
{
	/*App->textures->Unload(bgTexture);
	App->fonts->UnLoad(introFont);
	bgTexture = nullptr;*/
	return true;
}