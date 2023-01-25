#include "HUD.h"
#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Audio.h"
#include "Input.h"
#include "FadeToBlack.h"
#include "Scene.h"
#include "Scene2.h"
#include "Log.h"
#include "GuiManager.h"
#include "Window.h"
#include "Map.h"
#include "Physics.h"
#include <stdio.h>

HUD::HUD(bool startEnabled) : Module(startEnabled)
{
	heartAnim.PushBack({ 960, 224, 32, 32 });
	heartAnim.PushBack({ 928, 224, 32, 32 });
	heartAnim.PushBack({ 992, 224, 32, 32 });
	heartAnim.PushBack({ 992, 224, 32, 35 });
	heartAnim.PushBack({ 928, 224, 32, 32 });
	heartAnim.PushBack({ 960, 224, 32, 32 });
	heartAnim.speed = 0.1f;

	coinAnim.PushBack({ 32, 32, 32, 32 });
	coinAnim.PushBack({ 64, 32, 32, 32 });
	coinAnim.PushBack({ 96, 32, 32, 32 });
	coinAnim.PushBack({ 128, 32, 32, 35 });
	coinAnim.PushBack({ 160, 32, 32, 32 });
	coinAnim.PushBack({ 192, 32, 32, 32 });
	coinAnim.speed = 1.0f;

	staticHeart.PushBack({ 960, 224, 32, 32 });
	staticCoin.PushBack({ 32, 32, 32, 32 });
}

HUD::~HUD()
{

}

// Load assets
bool HUD::Start()
{
	bool ret = true;

	texturepath = app->LoadConfig2().child("HUD").attribute("texturepath").as_string();

	coinTex = app->tex->Load(texturepath);
	heartTex = app->tex->Load(texturepath);

	PlayTime.Start();

	return ret;
}

bool HUD::Update(float dt)
{
	bool ret = true;
	CurrentCoinAnim = &staticCoin;
	CurrentHeartAnim = &staticHeart;
	if (app->scene->player == nullptr) {
		return ret;
	}
	if (app->scene->player->coinHUDAnim) {
		coinAnimCounter = 6;
		app->scene->player->coinHUDAnim = false;
	}
	if (app->scene->player->heartHUDAnim) {
		heartAnimCounter = 65;
		app->scene->player->heartHUDAnim = false;
	}

	if (coinAnimCounter > 0 && coinAnimCounter < 7) {
		CurrentCoinAnim = &coinAnim;
		if (coinAnimCounter == 1) {
			coinAnim.Reset();
		}
		--coinAnimCounter;
	}
	if (heartAnimCounter > 7 && heartAnimCounter < 66) {
		CurrentHeartAnim = &heartAnim;
		if (heartAnimCounter == 8) {
			heartAnim.Reset();
		}
		--heartAnimCounter;
	}
	CurrentCoinAnim->Update();
	CurrentHeartAnim->Update();

	return ret;
}

// Update: draw background
bool HUD::PostUpdate()
{
	if (app->scene->player == nullptr) {
		return true;
	}
	playtime = PlayTime.ReadSec();
	SDL_Rect rect = CurrentCoinAnim->GetCurrentFrame();
	app->render->DrawTexture(coinTex, 100, 70,&rect, 0);
	rect = CurrentHeartAnim->GetCurrentFrame();
	app->render->DrawTexture(heartTex, 160, 70, &rect, 0);

	sprintf_s(totalHearts, 4, "%d", app->scene->player->hp);
	sprintf_s(totalPlaytime, 11, "%.0f ", playtime);
	sprintf_s(totalCoins, 4, "%d", app->scene->player->coinCount);


	app->render->DrawText(totalCoins,70,70,32,32, { 255,255,255 });
	app->render->DrawText(totalHearts, 130, 70, 32, 32, { 255,255,255 });

	app->render->DrawText("Total playtime: ", 200, 70, 130, 32, { 255,255,255 });
	app->render->DrawText(totalPlaytime, 330, 70, 32, 32, { 255,255,255 });
	

	return true;
}

bool HUD::CleanUp()
{
	app->tex->UnLoad(heartTex);
	app->tex->UnLoad(coinTex);
	heartTex = nullptr;
	coinTex = nullptr;

	return true;
}
