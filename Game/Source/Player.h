#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

#define JUMPIMPULSE -9
#define KILLENEMYIMPULSE -7
#define PLAYERHITIMPULSE 7
#define GODMODESPEED 10




struct SDL_Texture;



class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool PreUpdate();

	bool Update();

	bool PostUpdate();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	bool LoadState(pugi::xml_node&);
	bool SaveState(pugi::xml_node&);
	bool GetWinState();
	bool GetDeadState();
	void SetJumps(int jumps);
	PhysBody* GetBody();

public:
	uint hp;
	uint coinCount;
	iPoint position;
	bool isdead;
	bool playerhitRight;
	bool playerhitLeft;

	//hud anims and item pickups
	bool heartHUDAnim, coinHUDAnim, easterEgg;

private:

	//movement and physics
	PhysBody* pbody;
	_moveState moveState;
	int remainingJumps;
	int timerbetweenjumps;
	bool hitImpulse;
	
	//win lose
	bool win;
	int dieFxId;
	int jumpFxId;
	int winFxId;
	int ouchFxId;
	bool playerfallen;
	//anims
	Animation* currentPlayerAnimation;
	Animation movement, jump, die, iddle, movementLeft;
	SDL_Texture* texture;
	SDL_Texture* credits;
	const char* texturePath;
	const char* creditstexturepath;
	const char* audioDie; 
	const char* audioJump; 
	const char* audioWin;
	const char* audioOuch;

	

};

#endif // __PLAYER_H__