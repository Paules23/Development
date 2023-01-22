#ifndef __ITEMCOIN_H__
#define __ITEMCOIN_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class ItemCoin : public Entity
{
public:

	ItemCoin();
	virtual ~ItemCoin();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

	bool isPicked = false;

private:

	//Animation
	Animation* currentItemAnimation;
	Animation spinning;
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