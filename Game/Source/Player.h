#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"




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

public:

private:



	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	const char* texturePath;

	// L07 DONE 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;

	_moveState moveState;

	int remainingJumps;
	bool isdead, win;

	int pickCoinFxId;
	Animation* currentPlayerAnimation;
	Animation movement, jump, die, iddle;

};

#endif // __PLAYER_H__