#ifndef __ITEMGEM_H__
#define __ITEMGEM_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class ItemGem : public Entity
{
public:

	ItemGem();
	virtual ~ItemGem();

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
	Animation shine;
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

#endif // __ITEMCOIN_H__