#ifndef __GROUNDENEMY_H__
#define __GROUNDENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

#define JUMPIMPULSE -9
#define GODMODESPEED 10



struct SDL_Texture;



class GroundEnemy : public Entity
{
public:

	GroundEnemy();

	virtual ~GroundEnemy();

	bool Awake();

	bool Start();
	 
	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	bool LoadState(pugi::xml_node&);
	bool SaveState(pugi::xml_node&);
	bool GetWinState();
	bool GetDeadState();

public:

	bool dead;

private:



	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	SDL_Texture* run;
	const char* texturePath;

	//DONE 4: Add a physics to an item
	PhysBody* ebody;

};

#endif // __GROUNDENEMY_H__
