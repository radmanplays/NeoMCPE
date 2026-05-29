#include "TouchStartMenuScreen.h"
#include "../ProgressScreen.h"
#include "../OptionsScreen.h"
#include "../PauseScreen.h"

#include "../../Font.h"
#include "../../components/ScrolledSelectionList.h"
#include "../../components/GuiElement.h"
#include "../../components/NinePatch.h"

#include "../../../Minecraft.h"
#include "../../../renderer/Tesselator.h"
#include "../../../renderer/Textures.h"
#include "../../../renderer/TextureData.h"
#include "../../../../SharedConstants.h"
#include "../../../../AppPlatform.h"
#include "../../../../LicenseCodes.h"
#include "../../../../util/Mth.h"

#include "../DialogDefinitions.h"
#include "../SimpleChooseLevelScreen.h"

namespace Touch {

// 
// Start menu screen implementation
//

// Some kind of default settings, might be overridden in ::init
StartMenuScreen::StartMenuScreen()
:	bHost(    2, "Play"),
	// bJoin(    3, "Join Game"),
	bOptions( 4, ""),
	bQuit(    5, "Quit Game")
{
}

StartMenuScreen::~StartMenuScreen()
{
}

void StartMenuScreen::init()
{
	bOptions.init(minecraft->textures);

	buttons.push_back(&bHost);
	// buttons.push_back(&bJoin);
	buttons.push_back(&bOptions);
	buttons.push_back(&bQuit);

	tabButtons.push_back(&bHost);
	// tabButtons.push_back(&bJoin);
	tabButtons.push_back(&bOptions);
	tabButtons.push_back(&bQuit);

	#ifdef DEMO_MODE
		buttons.push_back(&bBuy);
		tabButtons.push_back(&bBuy);
	#endif

	copyright = "\xffMojang AB";//. Do not distribute!";

	// always show base version string
	std::string versionString = Common::getGameVersionString();

	std::string _username = minecraft->options.getStringValue(OPTIONS_USERNAME);
	if (_username.empty()) _username = "unknown";

	username = "Username: " + _username;

	#ifdef DEMO_MODE
        #ifdef __APPLE__
            version = versionString + " (Lite)";
        #else
            version = versionString + " (Demo)";
        #endif
	#else
		version = versionString;
	#endif
    
    #ifdef APPLE_DEMO_PROMOTION
        version = versionString + " (Demo)";
    #endif

	// bJoin.active = 
	bHost.active = bOptions.active = bQuit.active = true;
}

void StartMenuScreen::setupPositions() {
	bHost.width = 100;
	bHost.height = 30;
	bOptions.width = 30;
	bOptions.height = 30;
	bQuit.width = 100;
	bQuit.height = 30;
	int buttonWidth = bHost.width;
	float spacing = (width - (3.0f * buttonWidth)) / 4;

	//#ifdef ANDROID
	bHost.y =	 80 - bHost.height;
	// bJoin.y =	 yBase;
	bOptions.y = height - bOptions.height - 4;
	//#endif

	// Center buttons
	// bJoin.x		= 0*buttonWidth + (int)(1*spacing);
	bHost.x		= (width/2) - 50;
	bOptions.x	= width - bOptions.width - 4;
    
	// quit icon top-right (use size assigned in init)
	bQuit.x = bHost.x;
	bQuit.y = bHost.y + bQuit.height + 15;

	copyrightPosY = height - 10;
	versionPosY = height - 20;
}

void StartMenuScreen::buttonClicked(::Button* button) {

	if (button->id == bHost.id)
	{
		#if defined(DEMO_MODE) || defined(APPLE_DEMO_PROMOTION)
			minecraft->setScreen( new SimpleChooseLevelScreen("_DemoLevel") );
		#else
			minecraft->screenChooser.setScreen(SCREEN_SELECTWORLD);
		#endif
	}
	// if (button->id == bJoin.id)
	// {
    //     #ifdef APPLE_DEMO_PROMOTION
    //         minecraft->platform()->createUserInput(DialogDefinitions::DIALOG_DEMO_FEATURE_DISABLED);
    //     #else
    //         minecraft->locateMultiplayer();
    //         minecraft->screenChooser.setScreen(SCREEN_JOINGAME);
    //     #endif
	// }
	if (button->id == bOptions.id)
	{
		minecraft->setScreen(new OptionsScreen());
	}
	if (button == &bQuit)
	{
		minecraft->quit();
	}
}

bool StartMenuScreen::isInGameScreen() { return false; }

void StartMenuScreen::render( int xm, int ym, float a )
{
	renderMenuBackground(a);

	// Show current username in the top-left corner
	drawString(font, username, 2, 2, 0xffffffff);
    
    glEnable2(GL_BLEND);

#if defined(RPI)
	TextureId id = minecraft->textures->loadTexture("gui/pi_title.png");
#elif defined(_WIN32)
	TextureId id = minecraft->textures->loadTexture("gui/win_title.png");
#else
	TextureId id = minecraft->textures->loadTexture("gui/title.png");
#endif
	const TextureData* data = minecraft->textures->getTemporaryTextureData(id);

	if (data) {
		minecraft->textures->bind(id);

		const float x = (float)width / 2;
		const float y = 4;
		const float wh = 0.5f * Mth::Min((float)width/2.0f, (float)data->w / 2);
		const float scale = 2.0f * wh / (float)data->w;
		const float h = scale * (float)data->h;

		// Render title text
		Tesselator& t = Tesselator::instance;
		glColor4f2(1, 1, 1, 1);
		t.begin();
			t.vertexUV(x-wh, y+h, blitOffset, 0, 1);
			t.vertexUV(x+wh, y+h, blitOffset, 1, 1);
			t.vertexUV(x+wh, y+0, blitOffset, 1, 0);
			t.vertexUV(x-wh, y+0, blitOffset, 0, 0);
		t.draw();

		drawString(font, version, 1, versionPosY, 0xffffff);
		drawString(font, copyright, 1, copyrightPosY, 0xffffff);
		glColor4f2(1, 1, 1, 1);
		// if (Textures::isTextureIdValid(minecraft->textures->loadAndBindTexture("gui/logo/github.png")))
		// 	blit(2, height - 10, 0, 0, 8, 8, 256, 256);
		// drawString(font, "Kolyah35/minecraft-pe-0.6.1", 12, height - 10, 0xffcccccc);
		//patch->draw(t, 0, 20);
	}
	Screen::render(xm, ym, a);
    glDisable2(GL_BLEND);
}


void StartMenuScreen::mouseClicked(int x, int y, int buttonNum) {
	// const int logoX = 2;
	// const int logoW = 8 + 2 + font->width("Kolyah35/minecraft-pe-0.6.1");
	// const int logoY = height - 10;
	// const int logoH = 10;
	// if (x >= logoX && x <= logoX + logoW && y >= logoY && y <= logoY + logoH)
	// 	minecraft->platform()->openURL("https://gitea.sffempire.ru/Kolyah35/minecraft-pe-0.6.1");
	// else
		Screen::mouseClicked(x, y, buttonNum);
}

bool StartMenuScreen::handleBackEvent( bool isDown ) {
	minecraft->quit();
	return true;
}

OptionsButton::OptionsButton(int id, const std::string& msg)
:        ImageButton(id, msg),
        bg(NULL),
        bgSelected(NULL)
{
}
OptionsButton::~OptionsButton()
{
        delete bg;
        delete bgSelected;
}
void OptionsButton::init(Textures* textures)
{
        NinePatchFactory builder(textures, "gui/spritesheet.png");
        bg = builder.createSymmetrical(IntRectangle(112, 0, 8, 67), 2, 2);
        bgSelected = builder.createSymmetrical(IntRectangle(120, 0, 8, 67), 2, 2);
}
void OptionsButton::setSize(float w, float h)
{
        this->width = (int)w;
        this->height = (int)h;
        if (bg && bgSelected) {
                bg->setSize(w, h);
                bgSelected->setSize(w, h);
        }
}
void OptionsButton::renderBg(Minecraft* minecraft, int xm, int ym)
{
        if (!bg || !bgSelected)
                return;
        bool hovered = active && (minecraft->useTouchscreen()?
                (_currentlyDown && xm >= x && ym >= y && xm < x + width && ym < y + height) : isInside(xm, ym));
        if (hovered || selected)
                bgSelected->draw(Tesselator::instance, (float)x, (float)y);
        else
                bg->draw(Tesselator::instance, (float)x, (float)y);
		OptionsButton::renderFace(minecraft, xm, ym);
}
	
void OptionsButton::renderFace(Minecraft* minecraft, int xm, int ym)
{
        minecraft->textures->loadAndBindTexture("gui/touchgui.png");
        
        if (!active) {
                glColor4f2(0.5f, 0.5f, 0.5f, 1.0f);
        } else {
                glColor4f2(1.0f, 1.0f, 1.0f, 1.0f);
        }
        int iconX = x + (width - 20) / 2;
        int iconY = y + (height - 19) / 2;
        
        blit(iconX, iconY, 218, 0, 22, 21);
}


} // namespace Touch

