#ifndef __GROUNDENEMY_H__
#define __GROUNDENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

#define NOTCHILLDISTANCE 7



struct SDL_Texture;

enum WalkState
{
	CHILL,
	FOLLOWINGPLAYER
};

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
	bool right, left;
	bool changedir;
	WalkState walkstate;

private:


	

	SDL_Texture* texture;
	const char* texturePath;

	PhysBody* ebody;
	iPoint playerPos;
	Animation* currentEnemyAnimation;
	Animation iddle, run_left, run_right, jump;


};

#endif // __GROUNDENEMY_H__
