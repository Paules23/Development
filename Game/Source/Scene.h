#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "ItemCoin.h"
#include "ItemHeart.h"
#include "ItemGem.h"
#include "GuiButton.h"
#include "GUICheckBox.h"
#include "GUISlider.h"

struct SDL_Texture;


#define CAMERASPEED 1.0f

class Scene : public Module
{
public:

	Scene(bool startEnabled);

	virtual ~Scene();

	bool Awake(pugi::xml_node& config);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	// Define multiple Gui Event methods
	bool OnGuiMouseClickEvent(GuiControl* control);

public:

	//L02: DONE 3: Declare a Player attribute 
	Player* player;
	bool stopcamera;
	bool level1;

private:
	bool exit;


	//menu buttons
	GuiButton* resume;
	GuiButton* settings;
	GuiButton* backToTitle;
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
	//checkpoints
	bool checkpoint;
	bool returnToIntro;
	int checkpointFxId;
	SDL_Texture* Checkpoint = nullptr;
	SDL_Texture* CheckpointTaken = nullptr;

	List<Entity*> items;
};

#endif // __SCENE_H__