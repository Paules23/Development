#ifndef __SCENE_INTRO_H__
#define __SCENE_INTRO_H__

#include "Module.h"
#include "Animation.h"

struct SDL_Texture;

class SceneIntro : public Module
{
public:

	SceneIntro(bool startEnabled);


	~SceneIntro();


	bool Start() override;

	bool Update(float dt);


	bool PostUpdate();


	bool CleanUp() override;

public:

	SDL_Texture* bgTexture = nullptr;
	const char* texturepath;

	int entersound;

private:
};

#endif

