#ifndef __SCENE_DEATH_H__
#define __SCENE_DEATH_H__

#include "Module.h"
#include "Animation.h"
#include "GuiButton.h"

struct SDL_Texture;

class SceneDeath : public Module
{
public:

	SceneDeath(bool startEnabled);

	~SceneDeath();

	bool Start() override;

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp() override;


	// Define multiple Gui Event methods for buttons in the future
	/*bool OnGuiMouseClickEvent(GuiControl* control);*/

public:
private:
	SDL_Texture* bgTexture = nullptr;
	const char* texturepath;
	int entersound;
};

#endif

