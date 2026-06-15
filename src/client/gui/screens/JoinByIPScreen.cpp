#include "JoinByIPScreen.h"

#include "JoinGameScreen.h"
#include "StartMenuScreen.h"
#include "ProgressScreen.h"
#include "../Font.h"
#include "../../../network/RakNetInstance.h"
#include "client/Options.h"
#include "client/gui/Screen.h"
#include "client/gui/components/TextBox.h"
#include "network/ClientSideNetworkHandler.h"
#include "../components/NinePatch.h"
#include "../../renderer/Tesselator.h"

JoinByIPScreen::JoinByIPScreen() :
    tIP(0, "Server IP"),
    bHeader(1, "Join on server"),
	bJoin(  2, "Join Game"),
	bBack(  3, 0, 0, 38, 18, "Back")
{
	bJoin.active = false;
	//gamesList->yInertia = 0.5f;
}

JoinByIPScreen::~JoinByIPScreen()
{
}

void JoinByIPScreen::buttonClicked(Button* button)
{
	if (button->id == bJoin.id)
	{            
        minecraft->isLookingForMultiplayer = true;
	    minecraft->netCallback = new ClientSideNetworkHandler(minecraft, minecraft->raknetInstance);

        minecraft->joinMultiplayerFromString(tIP.text);
        {
			minecraft->options.set(OPTIONS_LAST_IP, tIP.text);
            bJoin.active = false;
            bBack.active = false;
            minecraft->setScreen(new ProgressScreen());
        }
	}
	if (button->id == bBack.id)
	{
		minecraft->cancelLocateMultiplayer();
		minecraft->screenChooser.setScreen(SCREEN_STARTMENU);
	}
}

bool JoinByIPScreen::handleBackEvent(bool isDown)
{
	if (!isDown)
	{
		minecraft->screenChooser.setScreen(SCREEN_STARTMENU);
	}
	return true;
}

void JoinByIPScreen::tick()
{
	Screen::tick();
	bJoin.active = !tIP.text.empty();
}

void JoinByIPScreen::init()
{
	NinePatchFactory builder(minecraft->textures, "gui/spritesheet.png");
	guiBackground = builder.createSymmetrical(IntRectangle(34, 43, 14, 14), 3, 3, 32, 32);

	buttons.push_back(&bHeader);
	buttons.push_back(&bJoin);
	buttons.push_back(&bBack);
    
    textBoxes.push_back(&tIP);
#ifdef ANDROID
    tabButtons.push_back(&bHeader);
	tabButtons.push_back(&bJoin);
	tabButtons.push_back(&bBack);
#endif

	tIP.text = minecraft->options.getStringValue(OPTIONS_LAST_IP);
}

void JoinByIPScreen::setupPositions() {
    int tIpDiff = 40;
    
	bJoin.y   = height * 2 / 3;
	bBack.y   = 4;
	bHeader.y = 0;

	// Center buttons
	//bJoin.x = width / 2 - 4 - bJoin.w;
	bBack.x = 4;//width / 2 + 4;
    
    bJoin.x = (width - bJoin.width) / 2;
	
    bHeader.x = 0;
	bHeader.width = width;

    tIP.width = bJoin.width + tIpDiff;
    tIP.height = 16;
    tIP.x = bJoin.x - tIpDiff / 2;
    tIP.y     = ((height - bJoin.height) / 2) - tIP.height - 4;

	if (guiBackground) {
		const int listX = 10;
		const int listY = 32;
		const int listW = width - 20;
		const int listH = height /2 + 30;
		guiBackground->setSize((float)listW + 6.0f, (float)listH + 6.0f);
	}
}

void JoinByIPScreen::render( int xm, int ym, float a )
{
	renderMenuBackground(a);
	Tesselator& t = Tesselator::instance;
	if (guiBackground) {
		const int listX = 10;
		const int listY = height / 4;
		const int listW = width - 20;
		const int listH = height /2;
		guiBackground->draw(t, (float)(listX - 3), (float)(listY - 3));
	}
	Screen::render(xm, ym, a);
}

void JoinByIPScreen::keyPressed(int eventKey)
{
    if (eventKey == Keyboard::KEY_ESCAPE) {
        minecraft->screenChooser.setScreen(SCREEN_SELECTWORLD);
        return;
    }
    // let base class handle navigation and text box keys
    Screen::keyPressed(eventKey);
}