#ifndef NET_MINECRAFT_CLIENT_GUI_SCREENS_TOUCH__TouchSelectWorldScreen_H__
#define NET_MINECRAFT_CLIENT_GUI_SCREENS_TOUCH__TouchSelectWorldScreen_H__

#include "../ConfirmScreen.h"
#include "../../Screen.h"
#include "../../TweenData.h"
#include "../../components/ImageButton.h"
#include "../../components/Button.h"
#include "../../components/RolledSelectionListV.h"
#include "../../components/NinePatch.h"
#include "../../../Minecraft.h"
#include "../../../../world/level/storage/LevelStorageSource.h"
#include "../../../../network/RakNetInstance.h"
#include <sstream>


class NinePatchLayer;

namespace Touch {

class SelectWorldScreen;

//
// Scrolling World selection list
//
class TouchWorldSelectionList : public RolledSelectionListV
{
public:
	TouchWorldSelectionList(Minecraft* _minecraft, int _width, int _height);
	virtual void tick();
	void stepLeft();
	void stepRight();

	void commit();
	void commitLanServers(const ServerList& lanServers);
	void setEditMode(bool edit);
	bool isEditMode() const { return _listEditMode; }
	int getEditButtonCount() const { return _listEditButtonCount; }
	int getEditButtonId(int i) const { return _listEditButtonIds[i]; }
	bool isEditButtonHovered(int i, int xm, int ym) const;
	std::string getLastPlayedString(int lastPlayedString);
protected:
	virtual int getNumberOfItems();
	virtual void selectItem(int item, bool doubleClick);
	virtual bool isSelectedItem(int item);

	virtual void renderBackground() {}
	virtual void renderItem(int i, int x, int y, int h, Tesselator& t);
	virtual float getPos(float alpha);
	virtual void touched() { mode = 0; }
	virtual bool capYPosition();
	virtual float getItemBgWidthOffset(int item);
	virtual void updateHoverItem(int xm, int ym);
	virtual int getItemAtPosition(int x, int y);

private:
	TweenData td;
	void tweenInited();

	int selectedItem;
	int _height;
	LevelSummaryList levels;
	std::vector<StringVector> _descriptions;
	std::vector<std::string> m_seedStrings;
	std::vector<float> m_seedWidths;

	ServerList lanServers;
	std::vector<StringVector> _lanDescriptions;
	bool hasPickedLanServer;
	int pickedLanIndex;

	bool hasPickedLevel;
	LevelSummary pickedLevel;
	int pickedIndex;

	int stoppedTick;
	int currentTick;
	float accRatio;
	int mode;
	bool _listEditMode;
	int _listEditButtonCount;
	int _listEditButtonIds[32];
	float _listEditButtonX[32];
	float _listEditButtonY[32];
	bool _listEditButtonHovered[32];
	NinePatchLayer* _editBtnNormal;
	NinePatchLayer* _editBtnPressed;
	
	friend class SelectWorldScreen;
};

//
// Delete World screen
//
class TouchDeleteWorldScreen: public ConfirmScreen
{
public:
	TouchDeleteWorldScreen(const LevelSummary& levelId);
protected:
	virtual void postResult(bool isOk);
private:
	LevelSummary _level;
};


//
// Select world screen
//
class SelectWorldScreen: public Screen
{
public:
	SelectWorldScreen();
	virtual ~SelectWorldScreen();

	virtual void init() override;
	virtual void setupPositions() override;

	virtual void tick() override;
	virtual void render(int xm, int ym, float a) override;

	virtual bool isIndexValid(int index);
	virtual bool handleBackEvent(bool isDown) override;
	virtual void buttonClicked(Button* button) override;
	virtual void keyPressed(int eventKey) override;
	virtual void mouseClicked(int x, int y, int buttonNum) override;

	// support for mouse wheel when desktop code uses touch variant
	virtual void mouseWheel(int dx, int dy, int xm, int ym) override;

	bool isInGameScreen() override;
private:
	void loadLevelSource();
	std::string getUniqueLevelName(const std::string& level);

	TButton bCreate;
	TButton bJoinByIp;
	TButton bEdit;
	Touch::THeader bHeader;
	TButton bBack;
	Button bWorldView;
	TouchWorldSelectionList* worldsList;
	LevelSummaryList levels;
	NinePatchLayer* guiBackground;
	bool _mouseHasBeenUp;
	bool _hasStartedLevel;
	bool _editMode;
	//LevelStorageSource* levels;
};
};

#endif /*NET_MINECRAFT_CLIENT_GUI_SCREENS_TOUCH__TouchSelectWorldScreen_H__*/
