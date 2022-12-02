#ifndef __SCENE2_H__
#define __SCENE2_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"

struct SDL_Texture;


#define CAMERASPEED 15

class Scene2 : public Module
{
public:

	Scene2(bool startEnabled);

	// Destructor
	virtual ~Scene2();

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

public:

	//L02: DONE 3: Declare a Player attribute 
	Player* player;
	bool stopcamera;
	bool godmode;

private:
	SDL_Texture* img;


};

#endif // __SCENE2_H__