#include "PauseScreen.h"
#include "StartMenuScreen.h"
#include "../components/ImageButton.h"
#include "../components/NinePatch.h"
#include "../../Minecraft.h"
#include "../../../util/Mth.h"
#include "../../../network/RakNetInstance.h"
#include "../../../network/ServerSideNetworkHandler.h"
#include "../../../world/entity/player/Player.h"
#include "../../player/LocalPlayer.h"
#include "../../../world/level/Level.h"
#include "../../renderer/Tesselator.h"
#include "../Gui.h"
#include "client/Options.h"
#include "client/gui/components/Button.h"
#include "client/gui/screens/OptionsScreen.h"

PauseScreen::PauseScreen(bool wasBackPaused)
	:	saveStep(0),
	visibleTime(0),
	bContinue(0),
	bQuit(0),
	bOptions(0),
	bQuitAndSaveLocally(0),
	bServerVisibility(0),
	//	bThirdPerson(0),
	wasBackPaused(wasBackPaused),
	// bSound(OPTIONS_SOUND_VOLUME, 1, 0),
	bThirdPerson(OPTIONS_THIRD_PERSON_VIEW),
	bHideGui(OPTIONS_HIDEGUI),
	playerListX(0),
	playerListY(0),
	playerListWidth(0),
	playerListHeight(0),
	playerListItemHeight(15),
	playerListScroll(0),
	playerListDragStartY(0),
	playerListDragStartScroll(0.0f),
	playerListDragging(false),
	playerListScrollBarAlpha(0.0f),
	playerListWasDown(false),
	playerListContainer(NULL),
	playerListBgPatch(NULL)
{
	ImageDef def;
	def.setSrc(IntRectangle(160, 144, 39, 31));
	def.name = "gui/touchgui.png";
	IntRectangle& defSrc = *def.getSrc();

	def.width = defSrc.w * 0.666667f;
	def.height = defSrc.h * 0.666667f;

	// bSound.setImageDef(def, true);
	defSrc.y += defSrc.h;
	bThirdPerson.setImageDef(def, true);
	bHideGui.setImageDef(def, true);
	//void setImageDef(ImageDef& imageDef, bool setButtonSize);
}

PauseScreen::~PauseScreen() {
	delete bContinue;
	delete bQuit;
	delete bQuitAndSaveLocally;
	delete bServerVisibility;
	delete bOptions;
	//	delete bThirdPerson;
	delete playerListContainer;
	delete playerListBgPatch;
}

void PauseScreen::init() {
	if (minecraft->options.getIntValue(OPTIONS_MENU_STYLE) == 0) {
		bContinue = new Touch::TButton(1, "Back to game");
		bOptions = new Touch::TButton(5, "Options");
		bQuit = new Touch::TButton(2, "Quit to title");
		bQuitAndSaveLocally = new Touch::TButton(3, "Quit and copy map");
		bServerVisibility = new Touch::TButton(4, "");
		//		bThirdPerson = new Touch::TButton(5, "Toggle 3:rd person view");
	} else if (minecraft->options.getIntValue(OPTIONS_MENU_STYLE) == 1) {
		bContinue = new Button(1, "Back to game");
		bOptions = new Button(5, "Options");
		bQuit = new Button(2, "Quit to title");
		bQuitAndSaveLocally = new Button(3, "Quit and copy map");
		bServerVisibility = new Button(4, "");
		//		bThirdPerson = new Button(5, "Toggle 3:rd person view");
	} else {
		bContinue = new Button(1, 0, 0, 200, 20, "Back to game");
		bServerVisibility = new Button(4, 0, 0, 200, 20, "");
		bOptions = new Button(5, 0, 0, 200, 20, "Options...");
		bQuit = new Button(2, 0, 0, 200, 20, "Save and quit to title");
		bQuitAndSaveLocally = new Button(3, 0, 0, 200, 20, "Copy and quit map");
		//		bThirdPerson = new Button(5, "Toggle 3:rd person view");
	}

	buttons.push_back(bContinue);
	buttons.push_back(bQuit);
	buttons.push_back(bOptions);
	// bSound.updateImage(&minecraft->options);
	bThirdPerson.updateImage(&minecraft->options);
	bHideGui.updateImage(&minecraft->options);
	// buttons.push_back(&bSound);
	buttons.push_back(&bThirdPerson);
	//buttons.push_back(&bHideGui);

	// If Back wasn't pressed, set up additional items (more than Quit to menu
	// and Back to game) here

#if !defined(APPLE_DEMO_PROMOTION) && !defined(RPI)
	if (true || !wasBackPaused) {
		if (minecraft->raknetInstance) {
			if (minecraft->raknetInstance->isServer()) {
				updateServerVisibilityText();
				buttons.push_back(bServerVisibility);
			}
			else {
#if !defined(DEMO_MODE)
				buttons.push_back(bQuitAndSaveLocally);
#endif
			}
		}
	}
#endif
	//	buttons.push_back(bThirdPerson);

	for (unsigned int i = 0; i < buttons.size(); ++i) {
		// if (buttons[i] == &bSound) continue;
		if (buttons[i] == &bThirdPerson) continue;
		if (buttons[i] == &bHideGui) continue;
		tabButtons.push_back(buttons[i]);
	}

	if (minecraft->options.getIntValue(OPTIONS_MENU_STYLE) != 2) {
		playerListContainer = new GuiElementContainer(1, 0, 1);
	}
}

void PauseScreen::setupPositions() {
	saveStep = 0;

	if (minecraft->options.getIntValue(OPTIONS_MENU_STYLE) == 2){
		int yBase = 50;

		bContinue->width = bOptions->width = bQuit->width = /*bThirdPerson->w =*/ 200;
		bQuitAndSaveLocally->width = bServerVisibility->width = 200;

		bContinue->x = (width - bContinue->width) / 2;
		bContinue->y = yBase + 24 * 1;

		bQuitAndSaveLocally->x = bServerVisibility->x = (width - bQuitAndSaveLocally->width) / 2;
		bQuitAndSaveLocally->y = bServerVisibility->y = yBase + 24 * 2;

		bOptions->x = (width - bOptions->width) / 2;
		bOptions->y = yBase + 24 * 3 + 24;

		bQuit->x = (width - bQuit->width) / 2;
		bQuit->y = yBase + 24 * 4 + 24;
	} else {
		bContinue->width = bOptions->width = bQuit->width = (width / 20) * 8;
		bQuitAndSaveLocally->width = bServerVisibility->width = (width / 20) * 8;

		bContinue->x = width / 20;
		bContinue->y = 48;

		bOptions->x = width / 20;
		bOptions->y = 80;

		bQuit->x = width / 20;
		bQuit->y = 112;

#if APPLE_DEMO_PROMOTION
		bQuit->y += 16;
#endif

		bQuitAndSaveLocally->x = bServerVisibility->x = width / 20;
		bQuitAndSaveLocally->y = bServerVisibility->y = 144;

		// bSound.y = bThirdPerson.y = 8;
		// bSound.x = 4;
		// bThirdPerson.x = bSound.x + 4 + bSound.width;
		// bHideGui.x = bThirdPerson.x + 4 + bThirdPerson.width;
		//bThirdPerson->x = (width - bThirdPerson->w) / 2;
		//bThirdPerson->y = yBase + 32 * 4;

		playerListWidth = (width / 20) * 8;
		playerListHeight = (height / 10) * 8;
		playerListX = width - playerListWidth - width / 20;
		playerListY = height / 10;

		if (playerListContainer) {
			playerListContainer->width = playerListWidth;
			playerListContainer->height = playerListHeight;
			playerListContainer->x = playerListX;
			playerListContainer->y = playerListY;
		}
	}



	// bSound.y = bThirdPerson.y = 8;
	// bSound.x = 4;
	// bThirdPerson.x = bSound.x + 4 + bSound.width;
	// bHideGui.x = bThirdPerson.x + 4 + bThirdPerson.width;

	//bThirdPerson->x = (width - bThirdPerson->w) / 2;
	//bThirdPerson->y = yBase + 32 * 4;
}

void PauseScreen::tick() {
	super::tick();
	visibleTime++;

	if (minecraft->options.getIntValue(OPTIONS_MENU_STYLE) != 2) {
		if (minecraft->level) {
			if (playerListChanged()) {
				rebuildPlayerList();
				players = minecraft->level->players;
				playerListScroll = 0;
			}
		}
	}

	//bool isSaving = !minecraft->level.pauseSave(saveStep++);
	//if (isSaving || visibleTime < 20) {
	//	float col = ((visibleTime % 10) + a) / 10.0f;
	//	col = Mth::sin(col * Mth::PI * 2) * 0.2f + 0.8f;
	//	int br = (int) (255 * col);
	//	drawString(font, "Saving level..", 8, height - 16, br << 16 | br << 8 | br);
	//}

	if (minecraft->options.getIntValue(OPTIONS_MENU_STYLE) != 2) {
		int totalHeight = 0;
		if (minecraft->player) totalHeight += playerListItemHeight;
		totalHeight += playerListContainer->size() * playerListItemHeight;

		bool isDown = Mouse::isButtonDown(MouseAction::ACTION_LEFT);
		int mx = Mouse::getX() * width / minecraft->width;
		int my = Mouse::getY() * height / minecraft->height;
		bool inside = mx >= playerListX && mx < playerListX + playerListWidth && my >= playerListY && my < playerListY + playerListHeight;

		if (isDown && !playerListWasDown && inside) {
			playerListDragging = true;
			playerListDragStartY = my;
			playerListDragStartScroll = (float)playerListScroll;
		}

		if (playerListDragging) {
			if (isDown) {
				if (totalHeight > playerListHeight) {
					int deltaY = playerListDragStartY - my;
					int maxScroll = totalHeight - playerListHeight;
					playerListScroll = (int)(playerListDragStartScroll + deltaY);
					if (playerListScroll < 0) playerListScroll = 0;
					if (playerListScroll > maxScroll) playerListScroll = maxScroll;
				}
				playerListScrollBarAlpha = 1.0f;
			} else {
				playerListDragging = false;
			}
		}

		if (!playerListDragging) {
			if (playerListScrollBarAlpha > 0.0f) {
				playerListScrollBarAlpha -= 0.1f;
				if (playerListScrollBarAlpha < 0.0f) playerListScrollBarAlpha = 0.0f;
			}
		}

		playerListWasDown = isDown;
	}
}

void PauseScreen::mouseWheel(int dx, int dy, int xm, int ym) {
	if (playerListContainer && minecraft->level) {
		int totalHeight = 0;
		if (minecraft->player) totalHeight += playerListItemHeight;
		totalHeight += playerListContainer->size() * playerListItemHeight;
		if (totalHeight > playerListHeight) {
			playerListScroll -= dy * 3;
			if (playerListScroll < 0) playerListScroll = 0;
			int maxScroll = totalHeight - playerListHeight;
			if (playerListScroll > maxScroll) playerListScroll = maxScroll;
			playerListScrollBarAlpha = 1.0f;
		}
	}
}

void PauseScreen::buttonClicked(Button* button) {
	if (button->id == bContinue->id) {
		minecraft->setScreen(NULL);
		//minecraft->grabMouse();
	}
	if (button->id == bQuit->id) {
		minecraft->leaveGame();
	}
	if (button->id == bQuitAndSaveLocally->id) {
		minecraft->leaveGame(true);
	}
	if (button->id == bOptions->id) {
		minecraft->setScreen(new OptionsScreen());
	}
	if (button->id == bServerVisibility->id) {
		if (minecraft->raknetInstance && minecraft->netCallback && minecraft->raknetInstance->isServer()) {
			ServerSideNetworkHandler* ss = (ServerSideNetworkHandler*) minecraft->netCallback;
			bool allows = !ss->allowsIncomingConnections();
			ss->allowIncomingConnections(allows);

			updateServerVisibilityText();
		}
	}

	if (button->id == OptionButton::ButtonId) {
		((OptionButton*)button)->toggle(&minecraft->options);
	}
}

	//if (button->id == bThirdPerson->id) {
	//	minecraft->options.thirdPersonView = !minecraft->options.thirdPersonView;
	//}
bool PauseScreen::playerListChanged() {
	if (!minecraft->level) return false;
	std::vector<Player*>& currentPlayers = minecraft->level->players;
	if (players.size() != currentPlayers.size()) return true;
	for (size_t i = 0; i < players.size(); ++i) {
		if (players[i] != currentPlayers[i]) return true;
	}
	return false;
}

void PauseScreen::rebuildPlayerList() {
	if (!playerListContainer) return;

	playerListContainer->clearAll();

	std::vector<Player*>& currentPlayers = minecraft->level->players;
	int yOffset = 0;

	for (size_t i = 0; i < currentPlayers.size(); ++i) {
		Player* p = currentPlayers[i];
		if (minecraft->player && p == minecraft->player) continue;

		GuiElementContainer* entry = new GuiElementContainer(0, 1, 0, yOffset, playerListWidth, playerListItemHeight);

		NinePatchFactory factory(minecraft->textures, "gui/spritesheet.png");
		NinePatchLayer* bg = factory.createSymmetrical(IntRectangle(0, 54, 3, 3), 1, 1, (float)playerListWidth, (float)playerListItemHeight);
		bg->x = 0;
		bg->y = 0;
		entry->addChild(bg);

		playerListContainer->addChild(entry);
		yOffset += playerListItemHeight;
	}

	playerListContainer->setupPositions();
}

void PauseScreen::render(int xm, int ym, float a) {
	renderBackground();

	if (minecraft->options.getIntValue(OPTIONS_MENU_STYLE) == 2) {
		drawCenteredString(font, "Game menu", width / 2, 24, 0xffffff);
	} else {
		int titleX = bContinue->x + bContinue->width / 2 - font->width("Game menu") / 2;
		int titleY = bContinue->y - 17;
		drawCenteredString(font, "Game menu", titleX - 2 + font->width("Game menu") / 2, titleY, 0xffffff);
	}

	if (minecraft->options.getIntValue(OPTIONS_MENU_STYLE) != 2) {
		if (minecraft->level && !minecraft->level->players.empty()) {
			renderPlayerList(xm, ym);
		}
	}

	super::render(xm, ym, a);
}

void PauseScreen::renderPlayerList(int xm, int ym) {
	if (!playerListContainer) return;

	Tesselator& t = Tesselator::instance;
	std::vector<Player*>& currentPlayers = minecraft->level->players;

	if (!playerListBgPatch) {
		NinePatchFactory factory(minecraft->textures, "gui/spritesheet.png");
		playerListBgPatch = factory.createSymmetrical(IntRectangle(0, 53, 3, 4), 1, 1, (float)playerListWidth, (float)playerListHeight);
	}

	glEnable2(GL_BLEND);
	glBlendFunc2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (playerListBgPatch) {
		playerListBgPatch->draw(t, (float)playerListX, (float)playerListY);
	}

	glEnable2(GL_SCISSOR_TEST);
	glScissor(playerListX * Gui::GuiScale, (height - playerListY - playerListHeight) * Gui::GuiScale, playerListWidth * Gui::GuiScale, playerListHeight * Gui::GuiScale);

	int yOffset = 0;

	if (minecraft->player) {
		bool found = false;
		for (size_t i = 0; i < currentPlayers.size(); ++i) {
			if (currentPlayers[i] == minecraft->player) {
				found = true;
				break;
			}
		}
		if (found) {
			int localDrawY = playerListY + yOffset - playerListScroll;
			if (localDrawY + playerListItemHeight >= playerListY && localDrawY < playerListY + playerListHeight) {
				NinePatchFactory factory(minecraft->textures, "gui/spritesheet.png");
				NinePatchLayer* bg = factory.createSymmetrical(IntRectangle(0, 54, 3, 3), 1, 1, (float)playerListWidth, (float)playerListItemHeight);
				bg->draw(t, (float)playerListX, (float)localDrawY);
				delete bg;
				drawString(font, minecraft->player->name, playerListX + 3, localDrawY + 4, 0xff777777);
			}
			yOffset += playerListItemHeight;
		}
	}

	for (size_t i = 0; i < playerListContainer->size(); ++i) {
		GuiElementContainer* entry = static_cast<GuiElementContainer*>(playerListContainer->get(i));

		int drawY = playerListY + yOffset - playerListScroll;
		if (drawY + playerListItemHeight < playerListY) {
			yOffset += playerListItemHeight;
			continue;
		}
		if (drawY >= playerListY + playerListHeight) break;

		if (entry->size() > 0) {
			NinePatchLayer* bg = static_cast<NinePatchLayer*>(entry->get(0));
			bg->setSize((float)playerListWidth, (float)playerListItemHeight);
			bg->draw(t, (float)playerListX, (float)drawY);
		}

		Player* p = NULL;
		int idx = 0;
		for (size_t j = 0; j < currentPlayers.size(); ++j) {
			if (minecraft->player && currentPlayers[j] == minecraft->player) continue;
			if (idx == (int)i) {
				p = currentPlayers[j];
				break;
			}
			idx++;
		}
		if (p) {
			drawString(font, p->name, playerListX + 3, drawY + 4, 0xffffffff);
		}

		yOffset += playerListItemHeight;
	}

	glDisable2(GL_SCISSOR_TEST);
	glDisable2(GL_BLEND);

	int totalHeight = 0;
	if (minecraft->player) totalHeight += playerListItemHeight;
	totalHeight += playerListContainer->size() * playerListItemHeight;
	if (totalHeight > playerListHeight && playerListScrollBarAlpha > 0.0f) {
		float sbX = (float)(playerListX + playerListWidth - 3);
		float sbY = (float)playerListY;
		float sbW = 2.0f;
		float sbH = (float)playerListHeight;
		float thumbH = (float)playerListHeight / (float)totalHeight * sbH;
		if (thumbH < 10.0f) thumbH = 10.0f;
		float maxScroll = (float)(totalHeight - playerListHeight);
		float thumbY = sbY;
		if (maxScroll > 0) thumbY = sbY + ((float)playerListScroll / maxScroll) * (sbH - thumbH);
		int alpha = (int)(playerListScrollBarAlpha * 255.0f);
		int trackColor = (alpha << 24) | 0x404040;
		int thumbColor = (alpha << 24) | 0xffffff;
		glEnable2(GL_BLEND);
		glBlendFunc2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		fill(sbX, sbY, sbX + sbW, sbY + sbH, trackColor);
		fill(sbX, thumbY, sbX + sbW, thumbY + thumbH, thumbColor);
		glDisable2(GL_BLEND);
	}
}

void PauseScreen::updateServerVisibilityText()
{
	if (!minecraft->raknetInstance || !minecraft->raknetInstance->isServer())
		return;

	ServerSideNetworkHandler* ss = (ServerSideNetworkHandler*) minecraft->netCallback;
	bServerVisibility->msg = ss->allowsIncomingConnections()?
		"Server is visible"
		:   "Server is invisible";
}
