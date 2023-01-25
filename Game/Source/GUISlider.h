#ifndef __GUISLIDER_H__
#define __GUISLIDER_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"
#include "Animation.h"

class GuiSlider : public GuiControl
{
public:

	GuiSlider(uint32 id, SDL_Rect bounds, const char* text, SDL_Rect sliderBounds);
	virtual ~GuiSlider();

	bool Start();

	bool Update(float dt);
	bool Draw(Render* render);

	int mouseX, mouseY;
	unsigned int click;

	bool canClick = true;
	bool drawBasic = false;

private:
	const char* texturePath;
	const char* focusAudioPath;
	const char* clickAudioPath;
	bool one, two, three, four;

	SDL_Rect sliderbounds;

	float boundsX, boundsY;

	uint clickaudioFxId;
	uint focusaudioFxId;
};

#endif // __GUISLIDER_H__