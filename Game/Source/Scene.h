#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"

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

public:

	//L02: DONE 3: Declare a Player attribute 
	Player* player;
	bool stopcamera;
	bool level1;

private:
	SDL_Texture* img;
	

};

#endif // __SCENE_H__