#include "SimpleChooseLevelScreen.h"
#include "ProgressScreen.h"
#include "ScreenChooser.h"
#include "../components/Button.h"
#include "../components/ImageButton.h"
#include "../components/NinePatch.h"
#include "../../Minecraft.h"
#include "../../renderer/Tesselator.h"
#include "../../renderer/Textures.h"
#include "../../../world/level/LevelSettings.h"
#include "../../../platform/time.h"
#include "../../../platform/input/Keyboard.h"
#include "../../../platform/log.h"

SimpleChooseLevelScreen::SimpleChooseLevelScreen(const std::string& levelName)
:   bHeader(0),
    bGamemode(0),
    bCheats(0),
    bBack(0),
    bCreate(0),
    levelName(levelName),
    hasChosen(false),
    gamemode(GameType::Creative),
    cheatsEnabled(false),
    customGenerationEnabled(false),
    tLevelName(0, 0, 0, 100, 18, "Name"),
    tSeed(1, 0, 0, 100, 18,"Seed")
{
}

SimpleChooseLevelScreen::~SimpleChooseLevelScreen()
{
    if (bHeader) delete bHeader;
    delete bGamemode;
    delete bCheats;
    delete bBack;
    delete bCreate;
}

void SimpleChooseLevelScreen::init()
{
    // make sure the base class loads the existing level list; the
    // derived screen uses ChooseLevelScreen::getUniqueLevelName(), which
    // depends on `levels` being populated.  omitting this used to result
    // in duplicate IDs ("creating the second world would load the
    // first") when the name already existed.
    ChooseLevelScreen::init();

    tLevelName.text = "New world";

    // header + close button
    bHeader = new Touch::THeader(0, "Create a Local Game");
    // create the back/X button as ImageButton like CreditsScreen
    bBack = new Touch::TButton(2, 0, 0, 38, 18, "Back");
    NinePatchFactory builder(minecraft->textures, "gui/spritesheet.png");
    guiBackground = builder.createSymmetrical(IntRectangle(34, 43, 14, 14), 3, 3, 32, 32);
    if (/* minecraft->useTouchscreen() */ true) {
        bGamemode = new Touch::TButton(1, "Creative mode");
        bCheats  = new Touch::TButton(4, "Cheats: Off");
        bCustomGeneration = new Touch::TButton(5, "Custom Gen: Off");
        bCreate  = new Touch::TButton(3, "Generate World");
    } else {
        bGamemode = new Button(1, "Creative mode");
        bCheats  = new Button(4, "Cheats: Off");
        bCustomGeneration = new Button(5, "Custom Gen: Off");
        bCreate  = new Button(3, "Create");
    }

    buttons.push_back(bHeader);
    buttons.push_back(bBack);
    buttons.push_back(bGamemode);
    buttons.push_back(bCheats);
    buttons.push_back(bCustomGeneration);
    buttons.push_back(bCreate);

    tabButtons.push_back(bGamemode);
    tabButtons.push_back(bCheats);
    tabButtons.push_back(bCustomGeneration);
    tabButtons.push_back(bBack);
    tabButtons.push_back(bCreate);

    textBoxes.push_back(&tLevelName);
    textBoxes.push_back(&tSeed);
}

void SimpleChooseLevelScreen::setupPositions()
{
    bBack->x = 4;
    bBack->y = 4;
    bBack->width = 38;
    bBack->height = 18;

    if (bHeader) {
        bHeader->x = 0;
        bHeader->y = 0;
        bHeader->width = width;
        bHeader->height = bBack->height + 8;
    }

    bCreate->width = (int)(float)((float)width / 2.3);

    bGamemode->width = bCreate->width / 2 - 5;
    bCheats->width = bCreate->width / 2 - 5;

    bCustomGeneration->width = bGamemode->width;

    bGamemode->y = height / 2 + 10;
    bCheats->y = height / 2 + 10;
    bGamemode->x = width / 2 + width / 4 - (width / 2 - 12) / 2 - 3;
    bCheats->x = width / 2 + width / 4 - (width / 2 - 12) / 2 - 3 + (width / 2 - 12) - bCheats->width;

    bCreate->x = width / 2 - bCreate->width / 2;
    bCreate->y = bGamemode->y + bGamemode->height + 10;

    bCustomGeneration->x = width / 2 + width / 4 - (width / 2 - 12) / 2 - 3 + (width / 2 - 12) - bCustomGeneration->width;
    bCustomGeneration->y = bCreate->y;

    tLevelName.width = 100;
    tSeed.width = 100;
    tLevelName.x = 10;
    tSeed.x = 10;

    tLevelName.y = bHeader->height + 28;
    int seedLabelY = tLevelName.y + tLevelName.height + 13;
    tSeed.y = seedLabelY + 10;
}

void SimpleChooseLevelScreen::tick()
{
    // let any textboxes handle their own blinking/input
    for (auto* tb : textBoxes)
        tb->tick(minecraft);
}

void SimpleChooseLevelScreen::render( int xm, int ym, float a )
{
    renderMenuBackground(a);
    glEnable2(GL_BLEND);

    if (guiBackground) {
        float bgX = 5.0f;
        float bgY = (float)(bHeader->height + 10);
        float bgW = (float)width - 10.0f;
        float bgH = (float)(bCreate->y + bCreate->height) - bgY + 5.0f;
        guiBackground->setSize(bgW, bgH);
        guiBackground->draw(Tesselator::instance, bgX, bgY);
    }

    const char* modeDesc = NULL;
    if (gamemode == GameType::Survival) {
        modeDesc = "Limited resources, you'll need tools. You may get hurt. Watch out for \nMonsters.";
    } else if (gamemode == GameType::Creative) {
        modeDesc = "Easily destroy and place blocks. No damage, flying and other cool stuff.";
    }
    if (modeDesc) {
        float wrapWidth = (float)(width - 10) - ((width / 2) + 2);
        float startX = (width / 2) + 2;
        float startY = (float)(bHeader->height + 16);
        minecraft->font->drawWordWrap(modeDesc, startX, startY, wrapWidth, 0xffffff);
    }

    int seedLabelY = tLevelName.y + tLevelName.height + 13;
    drawString(minecraft->font, "Name", 11, bHeader->height + 16, 0xffffff);
    drawString(minecraft->font, "Seed", 11, seedLabelY, 0xffffff);

    Screen::render(xm, ym, a);
    glDisable2(GL_BLEND);
}

// mouse clicks should also manage textbox focus explicitly
void SimpleChooseLevelScreen::mouseClicked(int x, int y, int buttonNum)
{
    if (buttonNum == MouseAction::ACTION_LEFT) {
        // determine if the click landed on either textbox or its label above
        int lvlTop = tLevelName.y - (Font::DefaultLineHeight + 4);
        int lvlBottom = tLevelName.y + tLevelName.height;
        int lvlLeft = tLevelName.x;
        int lvlRight = tLevelName.x + tLevelName.width;
        bool clickedLevel = x >= lvlLeft && x < lvlRight && y >= lvlTop && y < lvlBottom;

        int seedTop = tSeed.y - (Font::DefaultLineHeight + 4);
        int seedBottom = tSeed.y + tSeed.height;
        int seedLeft = tSeed.x;
        int seedRight = tSeed.x + tSeed.width;
        bool clickedSeed  = x >= seedLeft && x < seedRight && y >= seedTop && y < seedBottom;

        if (clickedLevel) {
            LOGI("SimpleChooseLevelScreen: level textbox clicked (%d,%d)\n", x, y);
            tLevelName.setFocus(minecraft);
            tSeed.loseFocus(minecraft);
        } else if (clickedSeed) {
            LOGI("SimpleChooseLevelScreen: seed textbox clicked (%d,%d)\n", x, y);
            tSeed.setFocus(minecraft);
            tLevelName.loseFocus(minecraft);
        } else {
            // click outside both fields -> blur both
            tLevelName.loseFocus(minecraft);
            tSeed.loseFocus(minecraft);
        }
    }

    // allow normal button and textbox handling too
    Screen::mouseClicked(x, y, buttonNum);
}

void SimpleChooseLevelScreen::buttonClicked( Button* button )
{
    if (hasChosen)
        return;

    if (button == bGamemode) {
        gamemode ^= 1;
        bGamemode->msg = (gamemode == GameType::Survival) ? "Survival mode" : "Creative mode";
        return;
    }

    if (button == bCheats) {
        cheatsEnabled = !cheatsEnabled;
        bCheats->msg = cheatsEnabled ? "Cheats: On" : "Cheats: Off";
        return;
    }

    if (button == bCustomGeneration) {
        customGenerationEnabled = !customGenerationEnabled;
        bCustomGeneration->msg = customGenerationEnabled ? "Custom Gen: On" : "Custom Gen: Off";
        return;
    }

    if (button == bCreate && !tLevelName.text.empty()) {
        int seed = getEpochTimeS();
        if (!tSeed.text.empty()) {
            std::string seedString = Util::stringTrim(tSeed.text);
            int tmpSeed;
            if (sscanf(seedString.c_str(), "%d", &tmpSeed) > 0) {
                seed = tmpSeed;
            } else {
                seed = Util::hashCode(seedString);
            }
        }
        std::string levelId = getUniqueLevelName(tLevelName.text);
        LevelSettings settings(seed, gamemode, cheatsEnabled, customGenerationEnabled);
        minecraft->selectLevel(levelId, levelId, settings);
        minecraft->hostMultiplayer();
        minecraft->setScreen(new ProgressScreen());
        hasChosen = true;
        return;
    }

    if (button == bBack) {
        minecraft->screenChooser.setScreen(SCREEN_STARTMENU);
    }
}

void SimpleChooseLevelScreen::keyPressed(int eventKey)
{
    if (eventKey == Keyboard::KEY_ESCAPE) {
        minecraft->screenChooser.setScreen(SCREEN_STARTMENU);
        return;
    }
    // let base class handle navigation and text box keys
    Screen::keyPressed(eventKey);
}

bool SimpleChooseLevelScreen::handleBackEvent(bool isDown) {
	if (!isDown)
		minecraft->screenChooser.setScreen(SCREEN_STARTMENU);
	return true; 
}
