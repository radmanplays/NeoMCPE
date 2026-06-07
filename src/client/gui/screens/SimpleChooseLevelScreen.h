#ifndef NET_MINECRAFT_CLIENT_GUI_SCREENS__DemoChooseLevelScreen_H__
#define NET_MINECRAFT_CLIENT_GUI_SCREENS__DemoChooseLevelScreen_H__

#include "ChooseLevelScreen.h"
#include "../components/TextBox.h"
#include "../components/Button.h"    // for Touch::THeader
#include "../components/NinePatch.h"
class Button;
class ImageButton;
class NinePatchLayer;

class SimpleChooseLevelScreen: public ChooseLevelScreen
{
public:
	SimpleChooseLevelScreen(const std::string& levelName);

	virtual ~SimpleChooseLevelScreen();

	void init();
	void setupPositions();
	void tick();

	void render(int xm, int ym, float a);

	void buttonClicked(Button* button);
	bool handleBackEvent(bool isDown);
	virtual void keyPressed(int eventKey);
	virtual void mouseClicked(int x, int y, int buttonNum);

private:
	Touch::THeader* bHeader;
	Button* bGamemode;
	Button* bCheats;
	Button* bCustomGeneration;
	Button* bBack;
	Button* bCreate;
	bool hasChosen;

	std::string levelName;
	int gamemode;
	bool cheatsEnabled;
	bool customGenerationEnabled;

	TextBox tLevelName;
	TextBox tSeed;
	NinePatchLayer* guiBackground;
	int descY;
};

#endif /*NET_MINECRAFT_CLIENT_GUI_SCREENS__DemoChooseLevelScreen_H__*/
