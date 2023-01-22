#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "ItemCoin.h"
#include "ItemHeart.h"
#include "GuiButton.h"

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
	SDL_Texture* img;
	
	//menu buttons
	GuiButton* resume;
	GuiButton* settings;
	GuiButton* backToTitle;
	GuiButton* Exit;
	//settings
	GuiButton* musicVolume;
	GuiButton* fxVolume;
	GuiButton* fullscreenmode;
	GuiButton* Vsync;
	//checkpoints
	bool checkpoint;

};

#endif // __SCENE_H__