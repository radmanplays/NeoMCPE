#ifndef NET_MINECRAFT_CLIENT_GUI__PauseScreen_H__
#define NET_MINECRAFT_CLIENT_GUI__PauseScreen_H__

//package net.minecraft.client.gui;

#include "../Screen.h"
#include "../components/ImageButton.h"
#include "../components/GuiElementContainer.h"
#include "../components/NinePatch.h"
#include <vector>
#include <string>

class Button;
class Player;
class NinePatchLayer;
class GuiElementContainer;

class PauseScreen: public Screen
{
	typedef Screen super;
public:
	PauseScreen(bool wasBackPaused);
	~PauseScreen();

	void init();
	void setupPositions();

	void tick();
	void render(int xm, int ym, float a);
	virtual void mouseWheel(int dx, int dy, int xm, int ym);
protected:
    void buttonClicked(Button* button);
private:
	void updateServerVisibilityText();
	bool playerListChanged();
	void rebuildPlayerList();
	void renderPlayerList(int xm, int ym);

	int saveStep;
	int visibleTime;
	bool wasBackPaused;

	Button* bContinue;
	Button* bQuit;
	Button* bQuitAndSaveLocally;
	Button* bServerVisibility;
	Button* bOptions;

	//	Button* bThirdPerson;

	// OptionButton bSound;
	OptionButton bThirdPerson;
    OptionButton bHideGui;

	std::vector<Player*> players;
	int playerListX;
	int playerListY;
	int playerListWidth;
	int playerListHeight;
	int playerListItemHeight;
	int playerListScroll;
	int playerListDragStartY;
	float playerListDragStartScroll;
	bool playerListDragging;
	float playerListScrollBarAlpha;
	bool playerListWasDown;
	GuiElementContainer* playerListContainer;
	NinePatchLayer* playerListBgPatch;
};

#endif /*NET_MINECRAFT_CLIENT_GUI__PauseScreen_H__*/
