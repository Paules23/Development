#ifndef __FLYINGENEMY_H__
#define __FLYINGENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class FlyingEnemy : public Entity
{
public:

	FlyingEnemy();

	virtual ~FlyingEnemy();

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
	WalkState walkstate;
	iPoint target;

private:


	//body and movement parameters
	PhysBody* ebody;
	iPoint playerPos;
	iPoint bodyPos;
	_moveState moveStateX;
	_moveState moveStateY;
	bool typeOfMovement;
	iPoint position;

	//sound
	int deadFxId;

	//animations and textures
	Animation* currentEnemyAnimation;
	Animation fly_left, fly_right, iddle;
	SDL_Texture* texture;
	const char* texturePath;

};

#endif // __FlyLYINGENEMY_H__
