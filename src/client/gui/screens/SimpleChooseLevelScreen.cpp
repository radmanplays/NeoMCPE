#include "SimpleChooseLevelScreen.h"
#include "ProgressScreen.h"
#include "ScreenChooser.h"
#include "../components/Button.h"
#include "../../Minecraft.h"
#include "../../../world/level/LevelSettings.h"
#include "../../../platform/time.h"
#include "client/gamemode/GameMode.h"

SimpleChooseLevelScreen::SimpleChooseLevelScreen(const std::string& levelName)
:	
	// bCreative(0),
	bGamemode(0),
	bBack(0),
	bCreate(0),
	levelName(levelName),
	hasChosen(false),
	gamemode(GameType::Survival),
	tLevelName(0, "World name"),
	tSeed(1, "World seed")
{
}

SimpleChooseLevelScreen::~SimpleChooseLevelScreen()
{
	// delete bCreative;
	delete bGamemode;
	delete bBack;
}

void SimpleChooseLevelScreen::init()
{
	if (minecraft->useTouchscreen()) {
		// bCreative = new Touch::TButton(1, "Creative mode");
		bGamemode = new Touch::TButton(2, "Survival mode");
		bBack	  = new Touch::TButton(3, "Back");
		bCreate	  = new Touch::TButton(4, "Create");
	} else {
		// bCreative = new Button(1, "Creative mode");
		bGamemode = new Button(2, "Survival mode");
		bBack	  = new Button(3, "Back");
		bCreate	  = new Button(4, "Create");
	}
	// buttons.push_back(bCreative);
	buttons.push_back(bGamemode);
	buttons.push_back(bBack);
	buttons.push_back(bCreate);

	textBoxes.push_back(&tLevelName);
	textBoxes.push_back(&tSeed);

	// tabButtons.push_back(bCreative);
	tabButtons.push_back(bGamemode);
	tabButtons.push_back(bBack);
	tabButtons.push_back(bCreate);
}

void SimpleChooseLevelScreen::setupPositions()
{
	const int padding = 5;

	/* bCreative->width = */ bGamemode->width = 120;
	tLevelName.width = tSeed.width = 120;
	bBack->width = bCreate->width = 60 - padding;
	// bCreative->x = (width - bCreative->width) / 2;
	// bCreative->y = height/3 - 40;
	bGamemode->x = (width - bGamemode->width) / 2;
	bGamemode->y = 2*height/3 - 30;
	bBack->x = bGamemode->x;
	bCreate->x = bGamemode->x + bGamemode->width - bCreate->width;
	bBack->y = bCreate->y = height - 40;

	tLevelName.x = tSeed.x = bGamemode->x;
	tLevelName.y = 20;
	tSeed.y = tLevelName.y + 30;
}

void SimpleChooseLevelScreen::render( int xm, int ym, float a )
{
	renderDirtBackground(0);
    glEnable2(GL_BLEND);

	const char* str = NULL;

	if (gamemode == GameType::Survival) {
		str = "Mobs, health and gather resources";
	} else if (gamemode == GameType::Creative) {
		str = "Unlimited resources and flying";
	}

	if (str) {
		drawCenteredString(minecraft->font, str, width/2, bGamemode->y + bGamemode->height + 4, 0xffcccccc);
	}
	
	drawString(minecraft->font, "World name:", tLevelName.x, tLevelName.y - Font::DefaultLineHeight - 2, 0xffcccccc);
	drawString(minecraft->font, "World seed:", tSeed.x, tSeed.y - Font::DefaultLineHeight - 2, 0xffcccccc);
	
	Screen::render(xm, ym, a);
    glDisable2(GL_BLEND);
}

void SimpleChooseLevelScreen::buttonClicked( Button* button )
{
	if (button == bBack) {
		minecraft->screenChooser.setScreen(SCREEN_STARTMENU);
		return;
	}
	if (hasChosen)
		return;

	if (button == bGamemode) {
		gamemode ^= 1;
		bGamemode->msg = (gamemode == GameType::Survival) ? "Survival mode" : "Creative mode";
	}

	if (button == bCreate) {
		int seed = getEpochTimeS();

		if (!tSeed.text.empty()) {
			std::string seedString = Util::stringTrim(tSeed.text);
			int tmpSeed;
			// Try to read it as an integer
			if (sscanf(seedString.c_str(), "%d", &tmpSeed) > 0) {
				seed = tmpSeed;
			} // Hash the "seed"
			else {
				seed = Util::hashCode(seedString);
			}
		}

		std::string levelId = getUniqueLevelName(tLevelName.text);
		LevelSettings settings(seed, gamemode);
		minecraft->selectLevel(levelId, levelId, settings);
		minecraft->hostMultiplayer();
		minecraft->setScreen(new ProgressScreen());
		hasChosen = true;
	}
}

bool SimpleChooseLevelScreen::handleBackEvent(bool isDown) {
	if (!isDown)
		minecraft->screenChooser.setScreen(SCREEN_STARTMENU);
	return true;
}
