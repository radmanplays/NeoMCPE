#include "TouchSelectWorldScreen.h"
#include "../../Gui.h"
#include "../StartMenuScreen.h"
#include "../ProgressScreen.h"
#include "../DialogDefinitions.h"
#include "../../components/ImageButton.h" //weird!
#include "../../components/NinePatch.h"
#include "../../../renderer/Textures.h"
#include "../../../renderer/Tesselator.h"
#include "../../../renderer/gles.h"
#include "../../../../world/level/LevelSettings.h"
#include "../../../../AppPlatform.h"
#include "../../../../util/StringUtils.h"
#include "../../../../util/Mth.h"
#include "../../../../platform/input/Mouse.h"
#include "../../../../Performance.h"

#include <sstream>
#include <algorithm>
#include <set>
#include "../SimpleChooseLevelScreen.h"

namespace Touch {

//
// World Selection List
//
TouchWorldSelectionList::TouchWorldSelectionList( Minecraft* minecraft, int width, int height )
:	_height(height),
	hasPickedLevel(false),
	pickedIndex(-1),
	hasPickedLanServer(false),
	pickedLanIndex(-1),
	currentTick(0),
	stoppedTick(-1),
	mode(0),
	selectedItem(-1),
	_listEditMode(false),
	_listEditButtonCount(0),
	_editBtnNormal(nullptr),
	_editBtnPressed(nullptr),
	RolledSelectionListV(minecraft, width - 22, height - 6, 12, width - 12, 28, height - 6, 32)
{
	_renderTopBorder = false;
	_renderBottomBorder = false;
	//setRenderSelection(false);
}

void TouchWorldSelectionList::setEditMode(bool edit) {
	_listEditMode = edit;
	if (edit && _editBtnNormal == nullptr) {
		NinePatchFactory factory(minecraft->textures, "gui/spritesheet.png");
		_editBtnNormal = factory.createSymmetrical(IntRectangle(112, 0, 8, 67), 2, 2, 30.0f, 30.0f);
		_editBtnPressed = factory.createSymmetrical(IntRectangle(120, 0, 8, 67), 2, 2, 30.0f, 30.0f);
	}
}

bool TouchWorldSelectionList::isEditButtonHovered(int i, int xm, int ym) const {
	if (i < 0 || i >= _listEditButtonCount) return false;
	return xm >= _listEditButtonX[i] && xm < _listEditButtonX[i] + 30 &&
		   ym >= _listEditButtonY[i] && ym < _listEditButtonY[i] + 30;
}

float TouchWorldSelectionList::getItemBgWidthOffset(int item) {
	if (!_listEditMode) return 0.0f;
	int lanCount = (int)lanServers.size();
	if (item < lanCount) return 0.0f;
	return 32.0f;
}

int TouchWorldSelectionList::getItemAtPosition(int x, int y) {
	if (_listEditMode) {
		int editBtnLeft = (int)x1 - 30;
		if (x >= editBtnLeft && x <= (int)x1 && y >= y0 && y <= y1) {
			return -1;
		}
	}
	return RolledSelectionListV::getItemAtPosition(x, y);
}

void TouchWorldSelectionList::updateHoverItem(int xm, int ym) {
	if (_listEditMode) {
		int hoverExRight = (int)x1 - 30;
		if (xm >= x0 && xm < hoverExRight && ym >= y0 && ym <= y1) {
			_hoverItem = getItemAtPosition(xm, ym);
		} else {
			_hoverItem = -1;
		}
	} else {
		RolledSelectionListV::updateHoverItem(xm, ym);
	}
}

int TouchWorldSelectionList::getNumberOfItems() {
    if (_listEditMode)
        return (int)levels.size();
    return (int)levels.size() + (int)lanServers.size();
}

void TouchWorldSelectionList::selectItem( int item, bool doubleClick ) {
	if (item < 0 || item >= getNumberOfItems())
		return;

	selectedItem = item;

	int lanCount = (int)lanServers.size();
	if (item < lanCount) {
		if (!hasPickedLanServer) {
			hasPickedLanServer = true;
			pickedLanIndex = item;
		}
		return;
	}

	int localIndex = item - lanCount;
	if (!hasPickedLevel) {
		hasPickedLevel = true;
		pickedIndex = localIndex;
		if (localIndex < (int)levels.size())
			pickedLevel = levels[localIndex];
	}
}

bool TouchWorldSelectionList::isSelectedItem( int item ) {
	return item == selectedItem;
}

std::string TouchWorldSelectionList::getLastPlayedString( int lastPlayed ) {
	int32_t diff = getEpochTimeS() - lastPlayed;
	char s[32];
	if(diff > 86399) {
		if(diff > 172799) {
			if(diff > 604799) {
				if(diff > 2419199) {
					return "long ago";
				}
				sprintf(s, "%d weeks ago", diff / 604800);
			} else {
				sprintf(s, "%d days ago", diff / 86400);
			}
			return s;
		} else {
			return "Yesterday";
		}
	} else {
		return "Today";
	}
}

void TouchWorldSelectionList::renderItem( int i, int x, int y, int h, Tesselator& t ) {
	int centerx = x + 48;
	int centerY = y + h/2;
	float a0 = Mth::Max(1.1f - std::abs( _height / 2 - centerY ) * 0.0055f, 0.2f);
	if (a0 > 1) a0 = 1;
	int textColor =  (int)(255.0f) * 0x010101;
	int textColor2 = 0xFFBBBBBB;
	const int TX = x + 5;
	const int TY = y + 8; //@kindle-res:42

	int lanCount = (int)lanServers.size();

	if (i < lanCount) {
		if (i >= 0 && i < (int)_lanDescriptions.size()) {
			StringVector& v = _lanDescriptions[i];
			int lanIconColor = 0x4080FF;
			fill(TX, TY + 2, TX + 6, TY + 10, lanIconColor);
			drawString(minecraft->font, v[0].c_str(), TX - 2, TY - 3, 0xBABAFE);
			drawString(minecraft->font, std::string("World on wifi: ") + v[1].c_str(), TX - 2, TY + 8, textColor2);

			// yoink https://github.com/oldminecraftcommunity/MCPE-0.8.1/blob/master/minecraftpe/impl/gui/elements/LocalServerListItemElement.cpp
			// wifi
			minecraft->textures->loadAndBindTexture("gui/spritesheet.png");
			glColor4f2(1.0f, 1.0f, 1.0f, 1.0f);
			blit((float)x1 - 14.0f, (float)y + 9.0f, 192, -24 * (currentTick / 4 % 3) + 48, 12.0f, 12.0f, 24, 24);
		}
		return;
	}

	// Draw the worlds
	int localIndex = i - lanCount;
	StringVector v = _descriptions[localIndex];
	drawString(minecraft->font, v[0].c_str(), TX - 2, TY - 3, textColor);
	drawString(minecraft->font, v[3].c_str(), TX  -2, TY + 8, textColor2);
	std::string lastPlayedStr = getLastPlayedString(levels[localIndex].lastPlayed);
	drawString(minecraft->font, lastPlayedStr.c_str(), TX + minecraft->font->width(v[3].c_str()) + 8, TY + 8, textColor2);

	// yoinked from https://github.com/oldminecraftcommunity/MCPE-0.8.1/blob/master/minecraftpe/impl/gui/elements/LocalServerListItemElement.cpp
	// (i did change it a bit cuz the original was obfuscated)
	if (_listEditMode) {
		drawString(minecraft->font, "Seed: ", ((x1 - 30) - minecraft->font->width("Seed:")) - 5.0, y + 5.0, 0xFFBBBBBB);
		drawString(minecraft->font, m_seedStrings[localIndex], ((x1 - 30) - m_seedWidths[localIndex]) - 5.0, y + 16.0, 0xFFBBBBBB);
	}

	if (_listEditMode && i < _listEditButtonCount && _editBtnNormal && _editBtnPressed) {
		int btnX = (int)x1 - 30;
		int btnY = y + (h - 31) / 2;
		_listEditButtonX[i] = (float)btnX;
		_listEditButtonY[i] = (float)btnY;
		bool hovered = isEditButtonHovered(i, _lastxm, _lastym);
		_listEditButtonHovered[i] = hovered;
		NinePatchLayer* patch = hovered ? _editBtnPressed : _editBtnNormal;
		patch->setSize(32.0f, 32.0f);
		patch->draw(t, (float)btnX, (float)btnY);
		minecraft->textures->loadAndBindTexture("gui/gui.png");
		glColor4f2(1.0f, 1.0f, 1.0f, 1.0f);
		blit(btnX + 11, btnY + 11, 182, 10, 11, 11);
	}
}

void TouchWorldSelectionList::stepLeft() {
	if (selectedItem > 0) {
		int yoffset = (int)(yo - ((float)(selectedItem * itemHeight) + ((float)(itemHeight-height)) * 0.5f));
		td.start = yo;
		td.stop = yo - itemHeight - yoffset;
		td.cur = 0;
		td.dur = 8;
		mode = 1;
		tweenInited();
	}
}

void TouchWorldSelectionList::stepRight() {
	if (selectedItem >= 0 && selectedItem < getNumberOfItems()-1) {
		int yoffset = (int)(yo - ((float)(selectedItem * itemHeight) + ((float)(itemHeight-height)) * 0.5f));
		td.start = yo;
		td.stop = yo + itemHeight - yoffset;
		td.cur = 0;
		td.dur = 8;
		mode = 1;
		tweenInited();
	}
}

void TouchWorldSelectionList::commit() {
	m_seedStrings.clear();
    m_seedWidths.clear();

	for (unsigned int i = 0; i < levels.size(); ++i) {
		LevelSummary& level = levels[i];

		// std::stringstream ss;
		// ss << level.name << "/preview.png";
		// TextureId id = Textures::InvalidId;//minecraft->textures->loadTexture(ss.str(), false);

		// if (id != Textures::InvalidId) {
		// 	_imageNames.push_back( ss.str() );
		// } else {
		// 	_imageNames.push_back("gui/default_world.png");
		// }

		std::stringstream ss;
        ss << levels[i].seed;
        std::string seedStr = ss.str();
        m_seedStrings.push_back(seedStr);
        float width = minecraft->font->width(seedStr);
        m_seedWidths.push_back(width);

		StringVector lines;
		lines.push_back(levels[i].name);
		lines.push_back(minecraft->platform()->getDateString(levels[i].lastPlayed));
		lines.push_back(levels[i].id);
		lines.push_back(LevelSettings::gameTypeToString(level.gameType));
		_descriptions.push_back(lines);
	}
}

void TouchWorldSelectionList::commitLanServers(const ServerList& lanServers)
{
	this->lanServers = lanServers;
	_lanDescriptions.clear();
	for (unsigned int i = 0; i < lanServers.size(); ++i) {
		StringVector lines;
		lines.push_back(std::string(lanServers[i].name.C_String()));
		lines.push_back(std::string(lanServers[i].address.ToString(false)));
		_lanDescriptions.push_back(lines);
	}
}

static float quadraticInOut(float t, float dur, float start, float stop) {
	const float delta = stop - start;
	const float T = (t / dur) * 2.0f;
	if (T < 1) return 0.5f*delta*T*T + start;
	return -0.5f*delta * ((T-1)*(T-3) - 1) + start;
}

void TouchWorldSelectionList::tick()
{
	yo += yInertia;

	// friction
    yInertia *= 0.85f;
	
    if (std::abs(yInertia) < 0.01f) {
        yInertia = 0;
    }

	RolledSelectionListV::tick();

	++currentTick;
}

float TouchWorldSelectionList::getPos( float alpha )
{
	if (mode != 1) return RolledSelectionListV::getPos(alpha);

	float y0 = quadraticInOut(td.cur, td.dur, td.start, td.stop);
	float y1 = quadraticInOut(td.cur+1, td.dur, td.start, td.stop);
	return y0 + (y1-y0)*alpha;
}

bool TouchWorldSelectionList::capYPosition() {
	bool capped = RolledSelectionListV::capYPosition();
	if (capped) mode = 0;
	return capped;
}

void TouchWorldSelectionList::tweenInited() {
	float y0 = quadraticInOut(td.cur,   td.dur, td.start, td.stop);
	float y1 = quadraticInOut(td.cur+1, td.dur, td.start, td.stop);
	_yinertia = 0;
	yInertia = y0-y1; // yes, it's all backwards and messed up..
}

//
// Select World Screen
//
SelectWorldScreen::SelectWorldScreen()
:	bHeader (0, "Play"),
	bCreate (2, 0, 0, 38, 18, "New"),
	bJoinByIp(5, 0, 0, 54, 18, "Join By IP"),
	bEdit   (6, 0, 0, 38, 18, "Edit"),
	bBack   (3, 0, 0, 38, 18, "Back"),
	bWorldView(4, ""),
	worldsList(NULL),
	guiBackground(NULL),
	_hasStartedLevel(false),
	_editMode(false)
{
}

SelectWorldScreen::~SelectWorldScreen()
{
	delete worldsList;
	if (guiBackground) delete guiBackground;
}

void SelectWorldScreen::init()
{
	worldsList = new TouchWorldSelectionList(minecraft, width, height);
	loadLevelSource();
	worldsList->commit();
	NinePatchFactory builder(minecraft->textures, "gui/spritesheet.png");
	guiBackground = builder.createSymmetrical(IntRectangle(34, 43, 14, 14), 3, 3, 32, 32);

	buttons.push_back(&bHeader);
	buttons.push_back(&bCreate);
	buttons.push_back(&bJoinByIp);
	buttons.push_back(&bEdit);
	buttons.push_back(&bBack);

	_mouseHasBeenUp = !Mouse::getButtonState(MouseAction::ACTION_LEFT);

	tabButtons.push_back(&bWorldView);
	tabButtons.push_back(&bCreate);
	tabButtons.push_back(&bJoinByIp);
	tabButtons.push_back(&bBack);

	minecraft->raknetInstance->clearServerList();
	minecraft->locateMultiplayer();
}

void SelectWorldScreen::setupPositions() {
	//#ifdef ANDROID
	bCreate.y =	4;
	bJoinByIp.y = 4;
	bBack.y   = 4;
	bEdit.y   = 4;
	bHeader.y = 0;

	// Center buttons
	bHeader.x   = 0;
	bHeader.width   = width;
	bHeader.height   = 26;
	bEdit.x     = width - bEdit.width - 4;
	bCreate.x   = bEdit.x - bCreate.width - 4;
	bJoinByIp.x = bCreate.x - bJoinByIp.width - 4;
	bBack.x     =  4;
	if (guiBackground) {
		const int listX = 10;
		const int listY = 32;
		const int listW = width - 20;
		const int listH = height - 38;
		guiBackground->setSize((float)listW + 6.0f, (float)listH + 6.0f);
	}
}

void SelectWorldScreen::buttonClicked(Button* button)
{
	if (button->id == bEdit.id) {
		if (!_editMode) {
			_editMode = true;
			bEdit.msg = "Cancel";
			bEdit.selected = true;
			bCreate.visible = false;
			bCreate.active = false;
			bJoinByIp.visible = false;
			bJoinByIp.active = false;
			bBack.visible = false;
			bBack.active = false;
			bHeader.visible = false;
			bHeader.active = false;
			bWorldView.visible = false;
			bWorldView.active = false;
			worldsList->_listEditButtonCount = 0;
			for (unsigned int i = 0; i < worldsList->levels.size() && i < 32; ++i) {
				worldsList->_listEditButtonIds[worldsList->_listEditButtonCount] = 100 + i;
				worldsList->_listEditButtonHovered[worldsList->_listEditButtonCount] = false;
				worldsList->_listEditButtonCount++;
			}
			worldsList->setEditMode(true);
		} else {
			_editMode = false;
			bEdit.msg = "Edit";
			bEdit.selected = false;
			setupPositions();
			bCreate.visible = true;
			bCreate.active = true;
			bJoinByIp.visible = true;
			bJoinByIp.active = true;
			bBack.visible = true;
			bBack.active = true;
			bHeader.visible = true;
			bHeader.active = true;
			bWorldView.visible = true;
			bWorldView.active = true;
			worldsList->setEditMode(false);
			worldsList->_listEditButtonCount = 0;
		}
		return;
	}
	if (button->id == bCreate.id) {
		if (!_hasStartedLevel) {
			std::string name = getUniqueLevelName("World");
			minecraft->setScreen(new SimpleChooseLevelScreen(name));
		}
	}
	if (button->id == bJoinByIp.id) {
		minecraft->screenChooser.setScreen(SCREEN_JOINBYIP);
	}
	if (button->id == bBack.id) {
		minecraft->cancelLocateMultiplayer();
		minecraft->screenChooser.setScreen(SCREEN_STARTMENU);
	}
	if (button->id == bWorldView.id) {
		// Try to "click" the item in the middle
		worldsList->selectItem( worldsList->getItemAtPosition(width/2, height/2), false );
	}
}

bool SelectWorldScreen::handleBackEvent(bool isDown)
{
	if (!isDown)
	{
		minecraft->cancelLocateMultiplayer();
		minecraft->screenChooser.setScreen(SCREEN_STARTMENU);
	}
	return true;
}

bool SelectWorldScreen::isIndexValid( int index )
{
	return worldsList && index >= 0 && index < worldsList->getNumberOfItems();
}

static char ILLEGAL_FILE_CHARACTERS[] = {
	'/', '\n', '\r', '\t', '\0', '\f', '`', '?', '*', '\\', '<', '>', '|', '\"', ':'
};

void SelectWorldScreen::mouseWheel(int dx, int dy, int xm, int ym)
{
	if (!worldsList) return;
	if (dy == 0) return;

    // worldsList->yo -= dy * worldsList->itemHeight;
    // worldsList->yoo = worldsList->yo;

	//really scuffed scrolling isnt accurate to real 0.7.0
	worldsList->yInertia += (dy * 5.0f);
	worldsList->mode = 0;
}

void SelectWorldScreen::mouseClicked(int x, int y, int buttonNum)
{
	if (_editMode && buttonNum == MouseAction::ACTION_LEFT) {
		for (int i = 0; i < worldsList->_listEditButtonCount; ++i) {
			if (worldsList->isEditButtonHovered(i, x, y)) {
				minecraft->setScreen(new TouchDeleteWorldScreen(worldsList->levels[i]));
				return;
			}
		}
	}
	Screen::mouseClicked(x, y, buttonNum);
}

void SelectWorldScreen::tick()
{
#if 0
	if (_state == _STATE_CREATEWORLD) {
		#if defined(RPI)
			std::string levelId = getUniqueLevelName("World");
			//int seed = Util::hashCode("/r/Minecraft");
			LevelSettings settings(getEpochTimeS(), GameType::Creative);
			minecraft->selectLevel(levelId, levelId, settings);
			minecraft->hostMultiplayer();
			minecraft->setScreen(new ProgressScreen());
			_hasStartedLevel = true;
		#elif defined(PLATFORM_DESKTOP)
			std::string name = getUniqueLevelName("World");
			minecraft->setScreen(new SimpleChooseLevelScreen(name));
		#else
			int status = minecraft->platform()->getUserInputStatus();
            //LOGI("Status is: %d\n", status);
			if (status > -1) {
				if (status == 1) {
					StringVector sv = minecraft->platform()->getUserInput();
					
					// Read the level name.
					// 1) Trim name 2) Remove all bad chars 3) Append '-' chars 'til the name is unique
					std::string levelName = Util::stringTrim(sv[0]);
					std::string levelId = levelName;

					for (int i = 0; i < sizeof(ILLEGAL_FILE_CHARACTERS) / sizeof(char); ++i)
						levelId = Util::stringReplace(levelId, std::string(1, ILLEGAL_FILE_CHARACTERS[i]), "");
                    if ((int)levelId.length() == 0) {
                        levelId = "no_name";
                    }
					levelId = getUniqueLevelName(levelId);

					// Read the seed
					int seed = getEpochTimeS();
					if (sv.size() >= 2) {
						std::string seedString = Util::stringTrim(sv[1]);
						if (seedString.length() > 0) {
							int tmpSeed;
							// Try to read it as an integer
							if (sscanf(seedString.c_str(), "%d", &tmpSeed) > 0) {
								seed = tmpSeed;
							} // Hash the "seed"
							else {
								seed = Util::hashCode(seedString);
							}
						}
					}
					// Read the game mode
					bool isCreative = true;
					if (sv.size() >= 3 && sv[2] == "survival")
						isCreative = false;

					// Start a new level with the given name and seed
					LOGI("Creating a level with id '%s', name '%s' and seed '%d'\n", levelId.c_str(), levelName.c_str(), seed);
					LevelSettings settings(seed, isCreative? GameType::Creative : GameType::Survival);
					minecraft->selectLevel(levelId, levelName, settings);
					minecraft->hostMultiplayer();
					minecraft->setScreen(new ProgressScreen());
					_hasStartedLevel = true;
				}
				_state = _STATE_DEFAULT;
				// Reset the world list
				worldsList->hasPickedLevel = false;
				worldsList->pickedIndex = -1;
			}
		#endif

		worldsList->hasPickedLevel = false;
		return;
	}
#endif

	worldsList->tick();

	const ServerList& currentLanServers = minecraft->raknetInstance->getServerList();
	ServerList filteredLanServers;
	for (unsigned int i = 0; i < currentLanServers.size(); ++i) {
		if (currentLanServers[i].name.GetLength() > 0)
			filteredLanServers.push_back(currentLanServers[i]);
	}
	worldsList->commitLanServers(filteredLanServers);

	if (worldsList->hasPickedLanServer) {
		int lanIdx = worldsList->pickedLanIndex;
		if (lanIdx >= 0 && lanIdx < (int)worldsList->lanServers.size()) {
			const PingedCompatibleServer& server = worldsList->lanServers[lanIdx];
			minecraft->joinMultiplayer(server);
			minecraft->setScreen(new ProgressScreen());
			_hasStartedLevel = true;
			worldsList->hasPickedLanServer = false;
			return;
		}
		worldsList->hasPickedLanServer = false;
	}

	if (worldsList->hasPickedLevel) {
		if (worldsList->pickedIndex == worldsList->levels.size()) {
			worldsList->hasPickedLevel = false;
			std::string name = getUniqueLevelName("World");
			minecraft->setScreen(new SimpleChooseLevelScreen(name));
		} else {
			minecraft->selectLevel(worldsList->pickedLevel.id, worldsList->pickedLevel.name, LevelSettings::None());
			minecraft->hostMultiplayer();
			minecraft->setScreen(new ProgressScreen());
			_hasStartedLevel = true;
			return;
		}
	}

	// copy the currently selected item
	LevelSummary selectedWorld;
	//bool hasSelection = false;
	if (isIndexValid(worldsList->selectedItem))
	{
		selectedWorld = worldsList->levels[worldsList->selectedItem];
		//hasSelection = true;
	}

}

void SelectWorldScreen::render( int xm, int ym, float a )
{
	//Performance::watches.get("sws-full").start();
	//Performance::watches.get("sws-renderbg").start();
	renderMenuBackground(a);
	//Performance::watches.get("sws-renderbg").stop();
	//Performance::watches.get("sws-worlds").start();

	worldsList->setComponentSelected(bWorldView.selected);

	Tesselator& t = Tesselator::instance;
	if (guiBackground) {
		const int listX = 10;
		const int listY = 32;
		const int listW = width - 20;
		const int listH = height - 37;
		guiBackground->draw(t, (float)(listX - 3), (float)(listY - 3));
		// i love scissoring
		glEnable2(GL_SCISSOR_TEST);
		int scissorX = (int)((listX - 1) * Gui::GuiScale);
		int scissorY = (int)((height - (listY + listH)) * Gui::GuiScale);
		int scissorW = (int)((listW + 2) * Gui::GuiScale);
		int scissorH = (int)((listH + 2) * Gui::GuiScale);
		glScissor(scissorX, scissorY, scissorW, scissorH);
	}

	if (_mouseHasBeenUp)
		worldsList->render(xm, ym, a);
	else {
		worldsList->render(0, 0, a);
		_mouseHasBeenUp = !Mouse::getButtonState(MouseAction::ACTION_LEFT);
	}

	if (guiBackground) {
		glDisable2(GL_SCISSOR_TEST);
	}

	glEnable2(GL_BLEND);
	glBlendFunc2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//minecraft->textures->loadAndBindTexture("gui/selectworld/trash.png");

	//Performance::watches.get("sws-string").start();
	//Performance::watches.get("sws-string").stop();
	Screen::render(xm, ym, a);
	//Performance::watches.get("sws-full").stop();
	//Performance::watches.printEvery(128);
}

void SelectWorldScreen::loadLevelSource()
{
	LevelStorageSource* levelSource = minecraft->getLevelSource();
	levelSource->getLevelList(levels);
	std::sort(levels.begin(), levels.end());

	for (unsigned int i = 0; i < levels.size(); ++i) {
		if (levels[i].id != LevelStorageSource::TempLevelId)
			worldsList->levels.push_back( levels[i] );
	}
}

std::string SelectWorldScreen::getUniqueLevelName( const std::string& level )
{
	std::set<std::string> Set;
	for (unsigned int i = 0; i < levels.size(); ++i)
		Set.insert(levels[i].id);

	std::string s = level;
	while ( Set.find(s) != Set.end() )
		s += "-";
	return s;
}

bool SelectWorldScreen::isInGameScreen() { return true;  }

void SelectWorldScreen::keyPressed( int eventKey )
{
	if (bWorldView.selected) {
		if (eventKey == minecraft->options.getIntValue(OPTIONS_KEY_LEFT))
			worldsList->stepLeft();
		if (eventKey == minecraft->options.getIntValue(OPTIONS_KEY_RIGHT))
			worldsList->stepRight();
	}

	Screen::keyPressed(eventKey);
}

//
// Delete World Screen
//
TouchDeleteWorldScreen::TouchDeleteWorldScreen(const LevelSummary& level)
:	ConfirmScreen(NULL, "Are you sure you want to delete this world?",
						"'" + level.name + "' will be lost forever!",
						"Delete", "Cancel", 0),
	_level(level)
{
	tabButtonIndex = 1;
}

void TouchDeleteWorldScreen::postResult( bool isOk )
{
	if (isOk) {
		LevelStorageSource* storageSource = minecraft->getLevelSource();
		storageSource->deleteLevel(_level.id);
	}
	minecraft->screenChooser.setScreen(SCREEN_SELECTWORLD);
}

};
