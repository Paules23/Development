#ifndef __SCENE_INTRO_H__
#define __SCENE_INTRO_H__

#include "Module.h"
#include "Animation.h"
#include "GuiButton.h"
#include "GUISlider.h"
#include "GUICheckBox.h"

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
	SDL_Texture* creditsTex = nullptr;
	const char* texturepath;
	const char* creditstexturepath;
	int entersound;

	//intro buttons
	//menu buttons
	GuiButton* play;
	GuiButton* settings;
	GuiButton* Continue;
	GuiButton* Credits;
	GuiButton* Exit;
	//settings
	GuiSlider* musicVolumeSlider;
	GuiButton* musicVolume;
	GuiButton* fxVolume;
	GuiSlider* fxVolumeSlider;
	GuiButton* fullscreen;
	GuiCheckBox* fullscreenCheck;
	GuiButton* vsync;
	GuiCheckBox* VsyncCheck;
	GuiButton* goBack;

	bool Play;
	bool exit;
	bool resume;
	bool credits;
};

#endif

