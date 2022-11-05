#ifndef __SCENE_Win_H__
#define __SCENE_Win_H__

#include "Module.h"
#include "Animation.h"

struct SDL_Texture;

class SceneWin : public Module
{
public:

	SceneWin(bool startEnabled);


	~SceneWin();


	bool Start() override;

	bool Update(float dt);


	bool PostUpdate();


	bool CleanUp() override;

public:

	SDL_Texture* winTexture = nullptr;
	const char* texturepath;

	int winsound;

private:
};

#endif


