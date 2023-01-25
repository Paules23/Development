#ifndef __GUIBUTTON_H__
#define __GUIBUTTON_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"
#include "Animation.h"

class GuiButton : public GuiControl
{
public:

	GuiButton(uint32 id, SDL_Rect bounds, const char* text);
	virtual ~GuiButton();

	bool Start();

	bool Update(float dt);
	bool Draw(Render* render);

	bool CleanUp();

	int mouseX, mouseY;
	unsigned int click;

	bool canClick = true;
	bool drawBasic = false;

private:
	const char* texturePath;
	const char* focusAudioPath;
	const char* clickAudioPath;

	uint clickaudioFxId;
	uint focusaudioFxId;
};

#endif // __GUIBUTTON_H__