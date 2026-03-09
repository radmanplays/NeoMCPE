#ifndef NET_MINECRAFT_CLIENT_GUI_SCREENS_TOUCH__TouchStartMenuScreen_H__
#define NET_MINECRAFT_CLIENT_GUI_SCREENS_TOUCH__TouchStartMenuScreen_H__

#include "../../Screen.h"
#include "../../components/LargeImageButton.h"
#include "../../components/TextBox.h"

namespace Touch {

class StartMenuScreen: public Screen
{
public:
	StartMenuScreen();
	virtual ~StartMenuScreen();
	
	void init();
	void setupPositions();
	
	void tick();
	void render(int xm, int ym, float a);

	void buttonClicked(Button* button);
	bool handleBackEvent(bool isDown);
	bool isInGameScreen();
private:
	void _updateLicense();
	
	LargeImageButton bHost;
	LargeImageButton bJoin;
	LargeImageButton bOptions;

	std::string copyright;
	int copyrightPosX;

	std::string version;
	int versionPosX;
};
};

#endif /*NET_MINECRAFT_CLIENT_GUI_SCREENS_TOUCH__TouchStartMenuScreen_H__*/
