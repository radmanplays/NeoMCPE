#ifndef NET_MINECRAFT_CLIENT_GUI_SCREENS_TOUCH__TouchStartMenuScreen_H__
#define NET_MINECRAFT_CLIENT_GUI_SCREENS_TOUCH__TouchStartMenuScreen_H__

#include "../../Screen.h"
#include "../../components/LargeImageButton.h"
#include "../../components/ImageButton.h"
#include "../../components/TextBox.h"
#include "../../components/Button.h"

class NinePatchLayer;
class Textures;

namespace Touch {

	
class OptionsButton : public ImageButton
{
public:
        OptionsButton(int id, const std::string& msg);
        virtual ~OptionsButton();
        void init(Textures* textures);
        void setSize(float w, float h);
        virtual void renderBg(Minecraft* minecraft, int xm, int ym) override;
		virtual void renderFace(Minecraft* minecraft, int xm, int ym) override;
private:
        NinePatchLayer* bg;
        NinePatchLayer* bgSelected;
};

class StartMenuScreen: public Screen
{
public:
	StartMenuScreen();
	virtual ~StartMenuScreen();
	
	void init();
	void setupPositions();
	void render(int xm, int ym, float a);

	void buttonClicked(Button* button);
	virtual void mouseClicked(int x, int y, int buttonNum);
	bool handleBackEvent(bool isDown);
	bool isInGameScreen();
private:
	void renderSplashText(float a);
	
	Touch::TButton bHost;
	// Touch::TButton bJoin;
	Touch::OptionsButton bOptions;
	Touch::TButton bQuit; // X close icon

	std::string copyright;
	int copyrightPosY;

	std::string version;
	int versionPosY;

	std::string username;

	// Splash text system
	static const char* gSplashes[];
	int currentSplash;
	const char* splash;

	float titleX;
	float titleY;
	float titleW;
	float titleH;
	int splashStartTime;
};
};

#endif /*NET_MINECRAFT_CLIENT_GUI_SCREENS_TOUCH__TouchStartMenuScreen_H__*/
