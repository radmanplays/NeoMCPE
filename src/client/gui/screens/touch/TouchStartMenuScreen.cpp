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
#include "../../../../util/Random.h"
#include "../../../../platform/time.h"

#include "../DialogDefinitions.h"
#include "../SimpleChooseLevelScreen.h"

#include <cmath>

namespace Touch {

// 
// Start menu screen implementation
//


const char* StartMenuScreen::gSplashes[] = {
	"Scientific!",
	"Cooler than Spock!",
	"Collaborate and listen!",
	"Never dig down!",
	"Take frequent breaks!",
	"Not linear!",
	"Han shot first!",
	"Nice to meet you!",
	"Buckets of lava!",
	"Ride the pig!",
	"Larger than Earth!",
	"sqrt(-1) love you!",
	"Phobos anomaly!",
	"Punching wood!",
	"Falling off cliffs!",
	"0% sugar!",
	"150% hyperbole!",
	"Synecdoche!",
	"Let's danec!",
	"Seecret Friday update!",
	"Ported implementation!",
	"Lewd with two dudes with food!",
	"Kiss the sky!",
	"20 GOTO 10!",
	"Verlet intregration!",
	"Peter Griffin!",
	"Do not distribute!",
	"Cogito ergo sum!",
	"4815162342 lines of code!",
	"A skeleton popped out!",
	"The Work of Notch!",
	"The sum of its parts!",
	"BTAF used to be good!",
	"I miss ADOM!",
	"umop-apisdn!",
	"OICU812!",
	"Bring me Ray Cokes!",
	"Finger-licking!",
	"Thematic!",
	"Pneumatic!",
	"Sublime!",
	"Octagonal!",
	"Une baguette!",
	"Gargamel plays it!",
	"Rita is the new top dog!",
	"SWM forever!",
	"Representing Edsbyn!",
	"Matt Damon!",
	"Supercalifragilisticexpialidocious!",
	"Consummate V's!",
	"Cow Tools!",
	"Double buffered!",
	"V-synched!",
	"Fan fiction!",
	"Flaxkikare!",
	"Jason! Jason! Jason!",
	"Hotter than the sun!",
	"Internet enabled!",
	"Autonomous!",
	"Engage!",
	"Fantasy!",
	"DRR! DRR! DRR!",
	"Kick it root down!",
	"Regional resources!",
	"Woo, facepunch!",
	"Woo, somethingawful!",
	"Woo, /v/!",
	"Woo, tigsource!",
	"Woo, minecraftforum!",
	"Woo, worldofminecraft!",
	"Woo, reddit!",
	"Woo, 2pp!",
	"Google anlyticsed!",
	"Give us Gordon!",
	"Tip your waiter!",
	"Very fun!",
	"12345 is a bad password!",
	"Vote for net neutrality!",
	"Lives in a pineapple under the sea!",
	"Omnipotent!",
	"Gasp!",
	"...!",
	"Bees, bees, bees, bees!",
	"Haha, LOL!",
	"Hampsterdance!",
	"Switches and ores!",
	"Menger sponge!",
	"idspispopd!",
	"Eple (original edit)!",
	"So fresh, so clean!",
	"Don't look directly at the bugs!",
	"Oh, ok, Pigmen!",
	"Scary!",
	"Play Minecraft, Watch Topgear, Get Pig!",
	"Twittered about!",
	"Jump up, jump up, and get down!",
	"Joel is neat!",
	"A riddle, wrapped in a mystery!",
	"Huge tracts of land!",
	"Welcome to your Doom!",
	"Stay a while, stay forever!",
	"Stay a while and listen!",
	"Treatment for your rash!",
	"\"Autological\" is!",
	"Information wants to be free!",
	"\"Almost never\" is an interesting concept!",
	"Lots of truthiness!",
	"The creeper is a spy!",
	"It's groundbreaking!",
	"Let our battle's begin!",
	"The sky is the limit!",
	"Jeb has amazing hair!",
	"Casual gaming!",
	"Undefeated!",
	"Kinda like Lemmings!",
	"Follow the train, CJ!",
	"Leveraging synergy!",
	"DungeonQuest is unfair!",
	"110813!",
	"90210!",
	"Tyrion would love it!",
	"Also try VVVVVV!",
	"Also try Super Meat Boy!",
	"Also try Terraria!",
	"Also try Mount And Blade!",
	"Also try Project Zomboid!",
	"Also try World of Goo!",
	"Also try Limbo!",
	"Also try Pixeljunk Shooter!",
	"Also try Braid!",
	"That's super!",
	"Bread is pain!",
	"Read more books!",
	"Khaaaaaaaaan!",
	"Less addictive than TV Tropes!",
	"More addictive than lemonade!",
	"Bigger than a bread box!",
	"Millions of peaches!",
	"Fnord!",
	"This is my true form!",
	"Totally forgot about Dre!",
	"Don't bother with the clones!",
	"Pumpkinhead!",
	"Hobo humping slobo babe!",
	"Endless!",
	"Feature packed!",
	"Boots with the fur!",
	"Stop, hammertime!",
	"Conventional!",
	"Homeomorphic to a 3-sphere!",
	"Doesn't avoid double negatives!",
	"Place ALL the blocks!",
	"Does barrel rolls!",
	"Meeting expectations!",
	"PC gaming since 1873!",
	"Ghoughpteighbteau tchoghs!",
	"Got your nose!",
	"Haley loves Elan!",
	"Afraid of the big, black bat!",
	"Doesn't use the U-word!",
	"Child's play!",
	"See you next Friday or so!",
	"150 bpm for 400000 minutes!",
	"Technologic!",
	"Funk soul brother!",
	"Pumpa kungen!",
	"Helo Cymru!",
	"My life for Aiur!",
	"Lennart lennart = new Lennart();",
	"I see your vocabulary has improved!",
	"Who put it there?",
	"You can't explain that!",
	"if not ok then return end",
	"SOPA means LOSER in Swedish!",
	"Big Pointy Teeth!",
	"Bekarton guards the gate!",
	"Mmmph, mmph!",
	"Don't feed avocados to parrots!",
	"Swords for everyone!",
	"Plz reply to my tweet!",
	".party()!",
	"Take her pillow!",
	"Put that cookie down!",
	"Pretty scary!",
	"I have a suggestion.",
	"Now with extra hugs!",
	"Almost C++11!",
	"Woah.",
	"HURNERJSGER?",
	"What's up, Doc?",
	"1 star! Deal with it notch!",
	"100% more yellow text!",
	"Glowing creepy eyes!",
	"Redstone free!",
	"Toilet friendly!",
	"Annoying touch buttons!",
	"Astronomically accurate!",
	"0xffff-1 chunks!",
	"Cubism!",
	"Mostly harmless!Pretty!",
	"!!!1!",
	"As seen on TV!",
	"Awesome!",
	"100% pure!",
	"May contain nuts!",
	"Better than Prey!",
	"Less polygons!",
	"Sexy!",
	"Limited edition!",
	"Flashing letters!",
	"Made by Mojang!",
	"It's here!",
	"Best in class!",
	"It's alpha!",
	"100% dragon free!",
	"Excitement!",
	"More than 500 sold!",
	"One of a kind!",
	"Heaps of hits on YouTube!",
	"Indev!",
	"Spiders everywhere!",
	"Check it out!",
	"Holy cow, man!",
	"It's a game!",
	"Made in Sweden!",
	"Uses C++!",
	"Reticulating splines!",
	"Minecraft!",
	"Yaaay!",
	"Multiplayer!",
	"Touch compatible!",
	"Undocumented!",
	"Ingots!",
	"Exploding creepers!",
	"That's no moon!",
	"l33t!",
	"Create!",
	"Survive!",
	"Dungeon!",
	"Exclusive!",
	"The bee's knees!",
	"Down with O.P.P.!",
	"Closed source!",
	"Classy!",
	"Wow!",
	"Not on steam!",
	"Oh man!",
	"Awesome community!",
	"Pixels!",
	"Teetsuuuuoooo!",
	"Kaaneeeedaaaa!",
	"Enhanced!",
	"90% bug free!",
	"12 herbs and spices!",
	"Fat free!",
	"Absolutely no memes!",
	"Free dental!",
	"Ask your doctor!",
	"Minors welcome!",
	"Cloud computing!",
	"Legal in Finland!",
	"Hard to label!",
	"Technically good!",
	"Bringing home the bacon!",
	"Quite Indie!",
	"GOTY!",
	"Euclidian!",
	"Now in 3D!",
	"Inspirational!",
	"Herregud!",
	"Complex cellular automata!",
	"Yes, sir!",
	"Played by cowboys!",
	"OpenGL ES 1.1!",
	"Thousands of colors!",
	"Try it!",
	"Age of Wonders is better!",
	"Try the mushroom stew!",
	"Sensational!",
	"Hot tamale, hot hot tamale!",
	"Play him off, keyboard cat!",
	"Guaranteed!",
	"Macroscopic!",
	"Bring it on!",
	"Random splash!",
	"Call your mother!",
	"Monster infighting!",
	"Loved by millions!",
	"Ultimate edition!",
	"Freaky!",
	"You've got a brand new key!",
	"Water proof!",
	"Uninflammable!",
	"Whoa, dude!",
	"All inclusive!",
	"Tell your friends!",
	"NP is not in P!",
	"Notch <3 ez!",
	"Livestreamed!",
	"Haunted!",
	"Polynomial!",
	"Terrestrial!",
	"All is full of love!",
	"Full of stars!",
	nullptr
};

// Some kind of default settings, might be overridden in ::init
StartMenuScreen::StartMenuScreen()
:	bHost(    2, "Play"),
	// bJoin(    3, "Join Game"),
	bOptions( 4, ""),
	bQuit(    5, "Quit Game"),
	currentSplash(-1),
	titleX(0),
	titleY(0),
	titleW(0),
	titleH(0)
{
	int numSplashes = 0;
	while (gSplashes[numSplashes]) numSplashes++;
	if (numSplashes > 0)
		currentSplash = Random(getRawTimeS()).nextInt(numSplashes);
	splash = gSplashes[currentSplash];
	splashStartTime = getTimeMs();
}

StartMenuScreen::~StartMenuScreen()
{
}

void StartMenuScreen::renderSplashText(float a)
{
	if (currentSplash < 0 || !gSplashes[currentSplash])
		return;

	float elapsed = (float)(getTimeMs() - splashStartTime) / 1000.0f;
	float splashScale = std::pow(std::sin(elapsed * 5.0f), 4.0f) * 0.1f + 1.2f;

	glPushMatrix();
	glTranslatef((titleX + titleW) * 0.71f, (titleY + titleH) - 15.0f, 0.0f);
	glRotatef(-20.0f, 0.0f, 0.0f, 1.0f);
	glScalef(splashScale, splashScale, splashScale);
	drawCenteredString(font, splash, 0, -4, 0xFFFFFF00);
	glPopMatrix();
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
	bHost.y = (height / 2) - 1;
	// bJoin.y =	 yBase;
	bOptions.y = height - bOptions.height - 4;
	//#endif

	// Center buttons
	// bJoin.x		= 0*buttonWidth + (int)(1*spacing);
	bHost.x		= (width/2) - 50;
	bOptions.x	= width - bOptions.width - 4;
    
	// quit icon top-right (use size assigned in init)
	bQuit.x = bHost.x;
	bQuit.y = bHost.y + bQuit.height + 10;

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
#else
	TextureId id = minecraft->textures->loadTexture("gui/title.png");
#endif
	const TextureData* data = minecraft->textures->getTemporaryTextureData(id);

	if (data) {
		minecraft->textures->bind(id);

		titleX = ((float)width - std::min((float)data->w, (float)width)) / 2;
		titleY = 14;
		titleW = std::min((float)data->w, (float)width);
		titleH = std::min((float)data->w, (float)width) * ((float)data->h / (float)data->w);

		// Render title text
		Tesselator& t = Tesselator::instance;
		glColor4f2(1, 1, 1, 1);
		t.begin();
			t.vertexUV(titleX, titleY+titleH, blitOffset, 0, 1);
			t.vertexUV(titleX+titleW, titleY+titleH, blitOffset, 1, 1);
			t.vertexUV(titleX+titleW, titleY+0, blitOffset, 1, 0);
			t.vertexUV(titleX, titleY+0, blitOffset, 0, 0);
		t.draw();

		drawString(font, version, 1, versionPosY, 0xffffff);
		drawString(font, copyright, 1, copyrightPosY, 0xffffff);
		glColor4f2(1, 1, 1, 1);
		// if (Textures::isTextureIdValid(minecraft->textures->loadAndBindTexture("gui/logo/github.png")))
		// 	blit(2, height - 10, 0, 0, 8, 8, 256, 256);
		// drawString(font, "Kolyah35/minecraft-pe-0.6.1", 12, height - 10, 0xffcccccc);
		//patch->draw(t, 0, 20);

		renderSplashText(a);
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

