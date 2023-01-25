#ifndef __GUICHECKBOX_H__
#define __GUICHECKBOX_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"
#include "Animation.h"

class GuiCheckBox : public GuiControl
{
public:

	GuiCheckBox(uint32 id, SDL_Rect bounds, const char* text);
	virtual ~GuiCheckBox();

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

	bool checked;

	uint clickaudioFxId;
	uint focusaudioFxId;
};

#endif // __GUICHECKBOX_H__