#ifndef __HUD_H__
#define __HUD_H__

#include "Module.h"
#include "Animation.h"
#include "GuiButton.h"
#include "Timer.h"

struct SDL_Texture;

class HUD : public Module
{
public:

	HUD(bool startEnabled);

	~HUD();

	bool Start() override;

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp() override;

public:
	float playtime = 0.0f;
private:
	SDL_Texture* coinTex = nullptr;
	SDL_Texture* heartTex = nullptr;
	const char* texturepath;
	Animation* CurrentHeartAnim;
	Animation* CurrentCoinAnim;
	Animation staticCoin, staticHeart, coinAnim, heartAnim;

	Timer PlayTime;

	short coinAnimCounter;
	short heartAnimCounter;

	char totalHearts[10] = { "\0" };
	char totalCoins[11] = { "\0" };
	char totalPlaytime[11] = { "\0" };
};

#endif


