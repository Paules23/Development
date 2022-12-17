#ifndef __GROUNDENEMY_H__
#define __GROUNDENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

#define NOTCHILLDISTANCE 500
#define ENEMYVELX 2



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
	bool GetDeadState();

public:

	bool dead;
	bool changedir;
	WalkState walkstate;
	iPoint target;

private:


	
	//body and movement parameters
	_moveState moveState;
	PhysBody* ebody;
	iPoint playerPos;

	// animations and textures
	Animation* currentEnemyAnimation;
	Animation iddle, run_left, run_right, walk_right, walk_left;

	SDL_Texture* texture;
	const char* texturePath;


};

#endif // __GROUNDENEMY_H__
