#ifndef __ITEMHEART_H__
#define __ITEMHEART_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class ItemHeart : public Entity
{
public:

	ItemHeart();
	virtual ~ItemHeart();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	bool PostUpdate();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

	bool isPicked = false;

private:

	//Animation
	Animation* currentItemAnimation;
	Animation heart;
	SDL_Texture* texture;
	const char* texturePath;
	//audio
	const char* audioPath;
	int pickUpFx;

	//DONE 4: Add a physics to an item
	PhysBody* ibody;
	bool pickedUp;
	bool activated;



};

#endif // __ITEMHEART_H__