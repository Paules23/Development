#ifndef __SCENE2_H__
#define __SCENE2_H__

#include "Module.h"
#include "Player.h"
#include "ItemCoin.h"
#include "ItemHeart.h"
#include "GroundEnemy.h"
#include "FlyingEnemy.h"
#include "GuiButton.h"
#include "GUICheckBox.h"
#include "GUISlider.h"

struct SDL_Texture;


#define CAMERASPEED 1.0f

class Scene2 : public Module
{
public:

	Scene2(bool startEnabled);

	// Destructor
	virtual ~Scene2();

	List<GroundEnemy*> groundEnemies;
	List<FlyingEnemy*> flyingEnemies;

	List<Entity*> itemsEnemies;
	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Define multiple Gui Event methods
	bool OnGuiMouseClickEvent(GuiControl* control);

public:

	//L02: DONE 3: Declare a Player attribute 
	Player* player;


	bool stopcamera;
	bool level2;
	bool checkpoint1, checkpoint2, checkpoint3;

private:
	//tex for pathfinding
	SDL_Texture* img;
	SDL_Texture* mouseTileTex = nullptr;
	SDL_Texture* originTex = nullptr;
	iPoint origin;
	//checkpoints
	SDL_Texture* Checkpoint = nullptr;
	SDL_Texture* CheckpointTaken = nullptr;
	int checkpointFxId;

	//BUTTONS
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
	bool returnToIntro;
	bool exit;

};

#endif // __SCENE2_H__