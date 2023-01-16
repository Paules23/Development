#ifndef __SCENE_INTRO_H__
#define __SCENE_INTRO_H__

#include "Module.h"
#include "Animation.h"
#include "GuiButton.h"

struct SDL_Texture;

class SceneIntro : public Module
{
public:

	SceneIntro(bool startEnabled);

	~SceneIntro();

	bool Start() override;

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp() override;


	// Define multiple Gui Event methods
	bool OnGuiMouseClickEvent(GuiControl* control);

public:
private:
	SDL_Texture* bgTexture = nullptr;
	const char* texturepath;
	int entersound;

	//intro buttons
	GuiButton* play;
	GuiButton* Continue;
	GuiButton* settings;
	GuiButton* credits;
	GuiButton* Exit;
	//settings
	GuiButton* musicVolume;
	GuiButton* fxVolume;
	GuiButton* fullscreenmode;
	GuiButton* Vsync;
};

#endif

