#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

#define JUMPIMPULSE -9
#define GODMODESPEED 10



struct SDL_Texture;

enum _moveState {
	MS_STOP,
	MS_LEFT,
	MS_RIGHT,
};

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

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
	
private:



	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	SDL_Texture* run;
	const char* texturePath;

	// L07 DONE 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;

	_moveState moveState;

	int remainingJumps;
	int timerbetweenjumps;
	bool isdead;
	bool win;


	int dieFxId;
	int jumpFxId;
	int winFxId;
	Animation* currentPlayerAnimation;
	Animation movement, jump, die, iddle, movementLeft;

};

#endif // __PLAYER_H__