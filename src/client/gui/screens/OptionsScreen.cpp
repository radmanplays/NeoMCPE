#include "OptionsScreen.h"

#include "StartMenuScreen.h"
#include "UsernameScreen.h"
#include "DialogDefinitions.h"
#include "../../Minecraft.h"
#include "../../../AppPlatform.h"
#include "CreditsScreen.h"
#include "../../renderer/Textures.h"

#include "../components/ImageButton.h"
#include "../components/OptionsGroup.h"
#include "../components/OptionsItem.h"
#include "../components/TextBox.h"
#include "platform/input/Keyboard.h"

OptionsScreen::OptionsScreen()
	: btnClose(NULL),
	bHeader(NULL),
	btnCredits(NULL),
	selectedCategory(0) {
}

OptionsScreen::~OptionsScreen() {
	if (btnClose != NULL) {
		delete btnClose;
		btnClose = NULL;
	}

	if (bHeader != NULL) {
		delete bHeader;
		bHeader = NULL;
	}

	if (btnCredits != NULL) {
		delete btnCredits;
		btnCredits = NULL;
	}

	for (std::vector<Touch::TButton*>::iterator it = categoryButtons.begin(); it != categoryButtons.end(); ++it) {
		if (*it != NULL) {
			delete* it;
			*it = NULL;
		}
	}

	for (std::vector<OptionsGroup*>::iterator it = optionPanes.begin(); it != optionPanes.end(); ++it) {
		if (*it != NULL) {
			delete* it;
			*it = NULL;
		}
	}

	categoryButtons.clear();
}

struct IconRect {
    int u, v, w, h;
};

static const IconRect categoryIcons[] = {
    { 134, 0, 28, 28 },  // Game
    { 106, 0, 28, 28 },  // Controls
    { 134, 28, 28, 28 },  // Graphics
    { 106, 28, 28, 28 },  // Sound
    { 134, 56, 28, 28 },  // Tweaks
};

void OptionsScreen::init() {
	bHeader = new Touch::THeader(0, "Options");

	btnClose = new Touch::TButton(2, 0, 0, 38, 18, "Back");

	categoryButtons.push_back(new Touch::TButton(2, 0, 0, 28, 28, ""));
	categoryButtons.push_back(new Touch::TButton(3, 0, 0, 28, 28, ""));
	categoryButtons.push_back(new Touch::TButton(4, 0, 0, 28, 28, ""));
	categoryButtons.push_back(new Touch::TButton(5, 0, 0, 28, 28, ""));
	categoryButtons.push_back(new Touch::TButton(6, 0, 0, 28, 28, ""));

	btnCredits = new Touch::TButton(11, 0, 0, 38, 14, "Credits");

	buttons.push_back(bHeader);
	buttons.push_back(btnClose);
	buttons.push_back(btnCredits);

	for (std::vector<Touch::TButton*>::iterator it = categoryButtons.begin(); it != categoryButtons.end(); ++it) {
		buttons.push_back(*it);
		tabButtons.push_back(*it);
	}

	generateOptionScreens();
	// start with first category selected
	selectCategory(0);
}

void OptionsScreen::setupPositions() {
    int headerHeight = bHeader->height;
    int totalButtons = categoryButtons.size();

    int usedHeight = headerHeight + 3;
    int bottomMargin = (btnCredits != NULL) ? btnCredits->height + 2 : 0;
    int buttonsTotalHeight = totalButtons * 28 + (totalButtons - 1);
    int v3 = (height - usedHeight - bottomMargin - buttonsTotalHeight) / 2;
    if (v3 < 0) v3 = 0;

    for (size_t i = 0; i < categoryButtons.size(); ++i) {
        Touch::TButton* btn = categoryButtons[i];
        btn->x = 5;
        btn->y = headerHeight + 3 + v3 + 29 * i;
    }

    btnClose->x = 4;
    btnClose->y = 4;

    bHeader->x = 0;
    bHeader->y = 0;
    bHeader->width = width;
    bHeader->height = headerHeight;

    if (btnCredits != NULL) {
        btnCredits->x = 0;
        btnCredits->y = height - btnCredits->height - 2;
    }

    for (std::vector<OptionsGroup*>::iterator it = optionPanes.begin(); it != optionPanes.end(); ++it) {
        if (categoryButtons.size() > 0 && categoryButtons[0] != NULL) {
            (*it)->x = categoryButtons[0]->width + 20;
            (*it)->y = bHeader->height + 3;
            (*it)->width = width - (*it)->x;
            (*it)->height = height - bHeader->height - 3;
            (*it)->setupPositions();
        }
    }

    selectCategory(0);

	// don't override user selection on resize
}


void OptionsScreen::render(int xm, int ym, float a) {

	renderBackground(a);

	int xmm = xm * width / minecraft->width;
	int ymm = ym * height / minecraft->height - 1;

	if (currentOptionsGroup != NULL)
		currentOptionsGroup->render(minecraft, xmm, ymm);

	fill(0, 0, 38, this->height, 0xFF958782);// the thing behind the category buttons

	super::render(xm, ym, a);
	if (!isTextBoxEditing()) {
		for (size_t i = 0; i < categoryButtons.size(); ++i) {
			if (i < sizeof(categoryIcons) / sizeof(categoryIcons[0])) {
				const IconRect& icon = categoryIcons[i];
				drawCategoryIcon(categoryButtons[i], icon.u, icon.v, icon.w, icon.h);
			}
		}
	}
}

void OptionsScreen::drawCategoryIcon(Touch::TButton* button, int u, int v, int iconW, int iconH) {
    int iconX = button->x + (button->width - iconW) / 2;
    int iconY = button->y + (button->height - iconH) / 2;
    
    minecraft->textures->loadAndBindTexture("gui/touchgui2.png");
    blit(iconX, iconY, u, v, iconW, iconH);
}

void OptionsScreen::removed() {
}

void OptionsScreen::buttonClicked(Button* button) {
	if (button == btnClose) {
		minecraft->options.save();
		if (minecraft->screen != NULL) {
			minecraft->setScreen(NULL);
		} else {
			minecraft->screenChooser.setScreen(SCREEN_STARTMENU);
		}
	}
	else if (button->id > 1 && button->id < 7) {
		int categoryButton = button->id - categoryButtons[0]->id;
		selectCategory(categoryButton);
	}
	else if (button == btnCredits) {
		minecraft->setScreen(new CreditsScreen());
	}
}

static void collectTextboxesFromGroup(OptionsGroup* group, std::vector<TextBox*>& out) {
	for (size_t i = 0; i < group->size(); i++) {
		GuiElement* child = group->get(i);
		OptionsItem* item = dynamic_cast<OptionsItem*>(child);
		if (item == NULL) continue;
		for (size_t j = 0; j < item->size(); j++) {
			TextBox* tb = dynamic_cast<TextBox*>(item->get(j));
			if (tb != NULL) {
				out.push_back(tb);
			}
		}
	}
}

void OptionsScreen::selectCategory(int index) {
	int currentIndex = 0;

	for (int i = (int)textBoxes.size() - 1; i >= 0; i--) {
		if (optionCategoryMap.count(textBoxes[i]->id)) {
			textBoxes.erase(textBoxes.begin() + i);
		}
	}

	if (index < (int)optionPanes.size()) {
		std::vector<TextBox*> categoryBoxes;
		collectTextboxesFromGroup(optionPanes[index], categoryBoxes);
		for (auto* tb : categoryBoxes) {
			textBoxes.push_back(tb);
		}
	}

	for (std::vector<Touch::TButton*>::iterator it = categoryButtons.begin(); it != categoryButtons.end(); ++it) {

		if (index == currentIndex)
			(*it)->selected = true;
		else
			(*it)->selected = false;

		currentIndex++;
	}

	if (index < (int)optionPanes.size())
		currentOptionsGroup = optionPanes[index];
}

void OptionsScreen::generateOptionScreens() {
	// how the fuck it works

	optionPanes.push_back(new OptionsGroup("options.group.game"));
	optionPanes.push_back(new OptionsGroup("options.group.input"));
	optionPanes.push_back(new OptionsGroup("options.group.graphics"));
	optionPanes.push_back(new OptionsGroup("options.group.audio"));
	optionPanes.push_back(new OptionsGroup("options.group.tweaks"));

	// Game Pane
	optionCategoryMap[OPTIONS_USERNAME] = 0;
	optionCategoryMap[OPTIONS_DIFFICULTY] = 0;
	optionCategoryMap[OPTIONS_SERVER_VISIBLE] = 0;
	optionCategoryMap[OPTIONS_THIRD_PERSON_VIEW] = 0;
	optionCategoryMap[OPTIONS_WINDOW_SCALE] = 0;
	optionCategoryMap[OPTIONS_GUI_SCALE] = 0;
	optionCategoryMap[OPTIONS_SMOOTH_CAMERA] = 0;
	optionPanes[0]->addOptionItem(OPTIONS_USERNAME, minecraft)
		.addOptionItem(OPTIONS_DIFFICULTY, minecraft)
		.addOptionItem(OPTIONS_SERVER_VISIBLE, minecraft)
		.addOptionItem(OPTIONS_THIRD_PERSON_VIEW, minecraft)
		.addOptionItem(OPTIONS_WINDOW_SCALE, minecraft)
		.addOptionItem(OPTIONS_GUI_SCALE, minecraft)
		.addOptionItem(OPTIONS_SMOOTH_CAMERA, minecraft);

	// Controls Pane
	optionCategoryMap[OPTIONS_SENSITIVITY] = 1;
	optionCategoryMap[OPTIONS_INVERT_Y_MOUSE] = 1;
	optionCategoryMap[OPTIONS_IS_LEFT_HANDED] = 1;
	optionCategoryMap[OPTIONS_USE_TOUCHSCREEN] = 1;
	optionCategoryMap[OPTIONS_IS_JOY_TOUCH_AREA] = 1;
	optionCategoryMap[OPTIONS_DPAD_SIZE] = 1;
	optionCategoryMap[OPTIONS_DESTROY_VIBRATION] = 1;
	optionCategoryMap[OPTIONS_AUTOJUMP] = 1;
	for (int i = OPTIONS_KEY_FORWARD; i <= OPTIONS_KEY_USE; i++) {
		optionCategoryMap[i] = 1;
	}
	optionPanes[1]->addOptionItem(OPTIONS_SENSITIVITY, minecraft)
		.addOptionItem(OPTIONS_INVERT_Y_MOUSE, minecraft)
		.addOptionItem(OPTIONS_IS_LEFT_HANDED, minecraft)
		.addOptionItem(OPTIONS_USE_TOUCHSCREEN, minecraft)
		.addOptionItem(OPTIONS_IS_JOY_TOUCH_AREA, minecraft)
		.addOptionItem(OPTIONS_DPAD_SIZE, minecraft)
		.addOptionItem(OPTIONS_DESTROY_VIBRATION, minecraft)
		.addOptionItem(OPTIONS_AUTOJUMP, minecraft);

	for (int i = OPTIONS_KEY_FORWARD; i <= OPTIONS_KEY_USE; i++) {
		optionPanes[1]->addOptionItem((OptionId)i, minecraft);
	}

	// Graphics Pane
	optionCategoryMap[OPTIONS_VIEW_DISTANCE] = 2;
	optionCategoryMap[OPTIONS_FANCY_GRAPHICS] = 2;
	optionCategoryMap[OPTIONS_BEAUTIFUL_SKY] = 2;
	optionCategoryMap[OPTIONS_RENDER_DEBUG] = 2;
	optionCategoryMap[OPTIONS_ANAGLYPH_3D] = 2;
	optionCategoryMap[OPTIONS_VIEW_BOBBING] = 2;
	optionCategoryMap[OPTIONS_AMBIENT_OCCLUSION] = 2;
	optionCategoryMap[OPTIONS_NORMAL_LIGHTING] = 2;
	optionCategoryMap[OPTIONS_LIMIT_FRAMERATE] = 2;
	optionCategoryMap[OPTIONS_VSYNC] = 2;
	optionCategoryMap[OPTIONS_VIGNETTE] = 2;
	optionCategoryMap[OPTIONS_BLOCK_OUTLINE] = 2;
	optionPanes[2]->addOptionItem(OPTIONS_VIEW_DISTANCE, minecraft)
		// .addOptionItem(&Option::VIEW_BOBBING, minecraft)
		// .addOptionItem(&Option::AMBIENT_OCCLUSION, minecraft)
		// .addOptionItem(&Option::ANAGLYPH, minecraft)
		.addOptionItem(OPTIONS_FANCY_GRAPHICS, minecraft)
		.addOptionItem(OPTIONS_BEAUTIFUL_SKY, minecraft)
		// .addOptionItem(OPTIONS_ANIMATED_WATER, minecraft)
		.addOptionItem(OPTIONS_RENDER_DEBUG, minecraft)
		.addOptionItem(OPTIONS_ANAGLYPH_3D, minecraft)
		.addOptionItem(OPTIONS_VIEW_BOBBING, minecraft)
		.addOptionItem(OPTIONS_AMBIENT_OCCLUSION, minecraft)
		.addOptionItem(OPTIONS_NORMAL_LIGHTING, minecraft)
		.addOptionItem(OPTIONS_LIMIT_FRAMERATE, minecraft)
		.addOptionItem(OPTIONS_VSYNC, minecraft)
		.addOptionItem(OPTIONS_VIGNETTE, minecraft)
		.addOptionItem(OPTIONS_BLOCK_OUTLINE, minecraft);

	// Sound Pane
	optionCategoryMap[OPTIONS_MUSIC_VOLUME] = 3;
	optionCategoryMap[OPTIONS_SOUND_VOLUME] = 3;
	optionPanes[3]->addOptionItem(OPTIONS_MUSIC_VOLUME, minecraft)
		.addOptionItem(OPTIONS_SOUND_VOLUME, minecraft);

	// Tweaks Pane
	optionCategoryMap[OPTIONS_ALLOW_SPRINT] = 4;
	optionCategoryMap[OPTIONS_BAR_ON_TOP] = 4;
	optionCategoryMap[OPTIONS_MENU_STYLE] = 4;
	optionCategoryMap[OPTIONS_RPI_CURSOR] = 4;
	optionCategoryMap[OPTIONS_FOLIAGE_TINT] = 4;
	optionCategoryMap[OPTIONS_TINTED_SIDE] = 4;
	optionCategoryMap[OPTIONS_JAVA_HUD] = 4;
	optionCategoryMap[OPTIONS_FOG_TYPE] = 4;
	optionCategoryMap[OPTIONS_BETA_SKY] = 4;
	optionCategoryMap[OPTIONS_RESTORED_ANIMS] = 4;
	optionCategoryMap[OPTIONS_DEBUG_STYLE] = 4;
	optionPanes[4]->addOptionItem(OPTIONS_ALLOW_SPRINT, minecraft)
		.addOptionItem(OPTIONS_BAR_ON_TOP, minecraft)
		.addOptionItem(OPTIONS_MENU_STYLE, minecraft)
		.addOptionItem(OPTIONS_RPI_CURSOR, minecraft)
		.addOptionItem(OPTIONS_FOLIAGE_TINT, minecraft)
		.addOptionItem(OPTIONS_TINTED_SIDE, minecraft)
		.addOptionItem(OPTIONS_JAVA_HUD, minecraft)
		.addOptionItem(OPTIONS_FOG_TYPE, minecraft)
		.addOptionItem(OPTIONS_BETA_SKY, minecraft)
		.addOptionItem(OPTIONS_RESTORED_ANIMS, minecraft)
		.addOptionItem(OPTIONS_DEBUG_STYLE, minecraft);
	
		
}

void OptionsScreen::mouseClicked(int x, int y, int buttonNum) {
	if (!isTextBoxEditing() && currentOptionsGroup != NULL)
		currentOptionsGroup->mouseClicked(minecraft, x, y, buttonNum);

	super::mouseClicked(x, y, buttonNum);
}

void OptionsScreen::mouseReleased(int x, int y, int buttonNum) {
	if (!isTextBoxEditing() && currentOptionsGroup != NULL)
		currentOptionsGroup->mouseReleased(minecraft, x, y, buttonNum);

	super::mouseReleased(x, y, buttonNum);
}

void OptionsScreen::mouseWheel(int dx, int dy, int xm, int ym) {
	if (!isTextBoxEditing() && currentOptionsGroup != NULL && currentOptionsGroup->pointInside(xm, ym) && dy != 0) {
		currentOptionsGroup->scrollByPixels((float)dy * 18.0f);
	}
}

void OptionsScreen::keyPressed(int eventKey) {
	if (currentOptionsGroup != NULL)
		currentOptionsGroup->keyPressed(minecraft, eventKey);
	if (eventKey == Keyboard::KEY_ESCAPE) {
		minecraft->options.save();
		minecraft->setScreen(NULL);
	}
}

void OptionsScreen::charPressed(char inputChar) {
	if (currentOptionsGroup != NULL)
		currentOptionsGroup->charPressed(minecraft, inputChar);
}

void OptionsScreen::tick() {

	if (currentOptionsGroup != NULL)
		currentOptionsGroup->tick(minecraft);

	super::tick();
}