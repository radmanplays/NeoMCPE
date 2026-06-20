#ifndef NET_MINECRAFT_CLIENT_GUI_SCREENS__StartMenuScreen_H__
#define NET_MINECRAFT_CLIENT_GUI_SCREENS__StartMenuScreen_H__

#include "../Screen.h"
#include "../components/Button.h"
#include "../components/ImageButton.h"

class Textures;

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
	virtual void mouseClicked(int x, int y, int buttonNum);
	bool handleBackEvent(bool isDown);
	bool isInGameScreen();
private:

	Button* bHost;
	// Button* bJoin;
	Button* bOptions;
	Button* bQuit; 

	std::string copyright;
	int copyrightPosY;

	std::string version;
	int versionPosY;

	std::string username;

	static const char* gSplashes[];
	int currentSplash;
	const char* splash;
	int splashStartTime;

	float titleX;
	float titleY;
	float titleW;
	float titleH;

	void renderSplashText(float a);
};

#endif /*NET_MINECRAFT_CLIENT_GUI_SCREENS__StartMenuScreen_H__*/
