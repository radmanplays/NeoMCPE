#include "StartMenuScreen.h"
#include "UsernameScreen.h"
#include "SelectWorldScreen.h"
#include "ProgressScreen.h"
#include "JoinGameScreen.h"
#include "OptionsScreen.h"
#include "PauseScreen.h"
#include "PrerenderTilesScreen.h" // test button
#include "../components/ImageButton.h"

#include "../../../util/Mth.h"
#include "../../../util/Random.h"
#include "../../../platform/time.h"

#include <cmath>

#include "../Font.h"
#include "../components/ScrolledSelectionList.h"

#include "../../Minecraft.h"
#include "../../renderer/Tesselator.h"
#include "../../../AppPlatform.h"
#include "../../../LicenseCodes.h"
#include "SimpleChooseLevelScreen.h"
#include "../../renderer/Textures.h"
#include "../../../SharedConstants.h"

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
	: currentSplash(-1)
	, titleX(0)
	, titleY(0)
	, titleW(0)
	, titleH(0)
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
	delete bHost;
	// delete bJoin;
	delete bOptions;
	delete bQuit;
}

void StartMenuScreen::init()
{
	if (minecraft->options.getIntValue(OPTIONS_MENU_STYLE) == 0) {
		bHost = new Touch::TButton(    2, 0, 0, 60, 24, "Singleplayer");
		// bJoin = new Touch::TButton(    3, 0, 0, 200, 20, "Multiplayer");
		bOptions = new Button( 4, 0, 0, 200, 20, "Options...");
		bQuit = new Touch::TButton( 5, 0, 0, 200, 20, "Ouit Game");
	} else if (minecraft->options.getIntValue(OPTIONS_MENU_STYLE) == 2){
		bHost = new Button(    2, 0, 0, 200, 20, "Singleplayer");
		// bJoin = new Button(    3, 0, 0, 200, 20, "Multiplayer");
		bOptions = new Button( 4, 0, 0, 200, 20, "Options...");
		bQuit = new Button( 5, 0, 0, 200, 20, "Ouit Game");
	} else {
		bHost = new Button(    2, 0, 0, 160, 24, "Start Game");
		// bJoin = new Button(    3, 0, 0, 160, 24, "Join Game");
		bOptions = new Button( 4, 0, 0, 160, 24, "Options");
		bQuit = new Button( 5, 0, 0, 160, 24, "Ouit Game");
	}
	// bJoin->active =
	bHost->active = bOptions->active = true;

	if (minecraft->options.getStringValue(OPTIONS_USERNAME).empty()) {
		return; // tick() will redirect to UsernameScreen
	}

	buttons.push_back(bHost);
	// buttons.push_back(bJoin);
	//buttons.push_back(&bTest);
	buttons.push_back(bQuit);

	tabButtons.push_back(bHost);
	// tabButtons.push_back(bJoin);
	tabButtons.push_back(bQuit);

	#ifndef RPI
		buttons.push_back(bOptions);
		tabButtons.push_back(bOptions);
	#endif

    //// add quit button (top right X icon) – match OptionsScreen style
    //{
    //    ImageDef def;
    //    def.name = "gui/touchgui.png";
    //    def.width = 34;
    //    def.height = 26;
    //    def.setSrc(IntRectangle(150, 0, (int)def.width, (int)def.height));
    //    bQuit.setImageDef(def, true);
    //    bQuit.scaleWhenPressed = false;
    //    buttons.push_back(&bQuit);
    //    // don't include in tab navigation
    //}

	copyright = "\xffMojang AB";//. Do not distribute!";

	// always show base version string, suffix was previously added for Android builds
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
		#ifdef RPI
			version = "v0.1.1 alpha";//(MCPE " + versionString + " compatible)";
		#else
			version = versionString;
		#endif
	#endif

	copyrightPosY = height - 10;
	versionPosY = height - 20;
}

void StartMenuScreen::setupPositions() {
	if (minecraft->options.getIntValue(OPTIONS_MENU_STYLE) == 2){
		int yBase = (height / 2) - 20;

		bHost->y =	 yBase;
		// bJoin->y =	 bHost->y + 24;
		bOptions->y = bHost->y + 24;
		bQuit->y = bOptions->y + 24;
	} else {
		int yBase = height / 2;
		bHost->y =	 yBase;
		// bJoin->y =	 bHost->y + 24 + 4;
		bOptions->y = bHost->y + 24 + 4;
		bQuit->y = bOptions->y + 24 + 4;
	}

	// Center buttons
	bHost->x = (width - bHost->width) / 2;
	// bJoin->x = (width - bJoin->width) / 2;
	bOptions->x = (width - bOptions->width) / 2;
	bQuit->x = (width - bQuit->width) / 2;

    //// position quit icon at top-right (use image-defined size)
    //bQuit.x = width - bQuit.width;
    //bQuit.y = 0;
}

void StartMenuScreen::tick() {
}

void StartMenuScreen::buttonClicked(Button* button) {

	if (button->id == bHost->id)
	{
        #if defined(DEMO_MODE) || defined(APPLE_DEMO_PROMOTION)
			minecraft->setScreen( new SimpleChooseLevelScreen("_DemoLevel") );
		#else
			minecraft->screenChooser.setScreen(SCREEN_SELECTWORLD);
		#endif
	}
	// if (button->id == bJoin->id)
	// {
	// 	minecraft->locateMultiplayer();
	// 	minecraft->screenChooser.setScreen(SCREEN_JOINGAME);
	// }
	if (button->id == bOptions->id)
	{
		minecraft->setScreen(new OptionsScreen());
	}
	if (button->id == bQuit->id)
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

		titleX = ((float)width - (std::min)((float)data->w, (float)width)) / 2;
		titleY = 14;
		titleW = (std::min)((float)data->w, (float)width);
		titleH = (std::min)((float)data->w, (float)width) * ((float)data->h / (float)data->w);

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

		renderSplashText(a);
	}
	Screen::render(xm, ym, a);
	glDisable2(GL_BLEND);
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
