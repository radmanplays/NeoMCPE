#include "TextBox.h"
#include "Button.h"
#include "../Gui.h"
#include "../Screen.h"
#include "../../Minecraft.h"
#include "../../../AppPlatform.h"
#include "../../../platform/input/Mouse.h"
#include "../../../platform/time.h"

// delegate constructors
TextBox::TextBox(int id, const std::string& msg)
 : TextBox(id, 0, 0, msg)
{
}

TextBox::TextBox(int id, int x, int y, const std::string& msg)
 : TextBox(id, x, y, 24, Font::DefaultLineHeight + 4, msg)
{
}

TextBox::TextBox(int id, int x, int y, int w, int h, const std::string& msg)
 : GuiElement(true, true, x, y, w, h),
   id(id), hint(msg), focused(false), blink(false), blinkTicks(0),
   editing(false), maxChars(256), doneButton(NULL), pressedButton(NULL),
   mouseDown(0), callbackScreen(NULL), callback(NULL), callbackKey(0)
{
}

void TextBox::setFocus(Minecraft* minecraft) {
    if (!editing) {
        editing = true;
        focused = true;
        blinkTicks = 0;
        blink = false;
        mouseDown = 0;
        minecraft->platform()->showKeyboard();

        if (!doneButton) {
            int screenW = (int)(minecraft->width * Gui::InvGuiScale);
            doneButton = new Touch::TButton(1, screenW - 66, 0, 66, 38, "Done");
        }
    }
}

bool TextBox::loseFocus(Minecraft* minecraft) {
    if (editing) {
        editing = false;
        focused = false;
        minecraft->platform()->hideKeyboard();

        if (callbackScreen && callback) {
            (callbackScreen->*callback)(callbackKey);
        }
        return true;
    }
    return false;
}

bool TextBox::suppressOtherGUI() {
    return editing;
}

void TextBox::mouseClicked(Minecraft* minecraft, int x, int y, int buttonNum) {
    if (editing) {
        if (doneButton && doneButton->clicked(minecraft, x, y)) {
            doneButton->setPressed();
            pressedButton = doneButton;
        }
        mouseDown = 0;
        return;
    }
    if (buttonNum == MouseAction::ACTION_LEFT) {
        if (pointInside(x, y)) {
            mouseDown = 1;
        } else {
            mouseDown = 0;
        }
    }
}

void TextBox::mouseReleased(Minecraft* minecraft, int x, int y, int buttonNum) {
    if (editing) {
        if (pressedButton && pressedButton == doneButton && doneButton->clicked(minecraft, x, y)) {
            pressedButton->released(x, y);
            loseFocus(minecraft);
        }
        pressedButton = NULL;
        mouseDown = 0;
        return;
    }
    if (mouseDown && buttonNum == MouseAction::ACTION_LEFT) {
        if (pointInside(x, y)) {
            setFocus(minecraft);
        }
    }
    mouseDown = 0;
}

void TextBox::charPressed(Minecraft* minecraft, char c)  {
    if (editing && c >= 32 && c < 127 && (int)text.size() < maxChars) {
        text.push_back(c);
    }
}

void TextBox::keyPressed(Minecraft* minecraft, int key) {
    if (editing) {
        if (key == Keyboard::KEY_BACKSPACE) {
            if (!text.empty()) {
                text.pop_back();
            }
        } else if (key == Keyboard::KEY_RETURN) {
            loseFocus(minecraft);
        }
    }
}

void TextBox::tick(Minecraft* minecraft) {
    if (editing) {
        blinkTicks++;
        if (blinkTicks >= 5) {
            blink = !blink;
            blinkTicks = 0;
        }
    }
}

void TextBox::render(Minecraft* minecraft, int xm, int ym) {
    // textbox like in beta 1.7.3
    // change appearance when focused so the user can tell it's active
    // active background darker gray with a subtle border
    uint32_t bgColor = 0xFF373535;
    uint32_t borderColor = 0xFF676060;
    fill(x, y, x + width, y + height, bgColor);

    // draw border
    fill(x, y, x + width, y + 1, borderColor);
    fill(x, y + height - 1, x + width, y + height, borderColor);
    fill(x, y, x + 1, y + height, borderColor);
    fill(x + width - 1, y, x + width, y + height, borderColor);

    glEnable2(GL_SCISSOR_TEST);
    glScissor(
        Gui::GuiScale * (x + 2),
        minecraft->height - Gui::GuiScale * (y + height - 2),
        Gui::GuiScale * (width - 2),
        Gui::GuiScale * (height - 2)
    );

    int _y = y + (height - Font::DefaultLineHeight) / 2;

    minecraft->font->draw(text, (float)(x + 5), (float)_y + 1, 0xFFFFFFFF, false);

    glDisable2(GL_SCISSOR_TEST);
}

void TextBox::topRender(Minecraft* minecraft, int xm, int ym) {
    if (!editing) return;

    int screenW = (int)(minecraft->width * Gui::InvGuiScale);
    int screenH = (int)(minecraft->height * Gui::InvGuiScale);

    fill(0, 0, screenW, screenH, 0xFF000000);

    minecraft->font->draw(hint, 5.0f, 5.0f, 0xFFCCCCCC, false);

    int boxX = 5;
    int boxY = 20;
    int boxW = screenW - 76;
    int boxH = 17;

    fill(boxX, boxY, boxX + boxW, boxY + boxH, 0xff000000);
    fill(boxX, boxY, boxX + boxW, boxY + 1, 0xFFFFFFFF);
    fill(boxX, boxY + boxH - 1, boxX + boxW, boxY + boxH, 0xFFFFFFFF);
    fill(boxX, boxY, boxX + 1, boxY + boxH, 0xFFFFFFFF);
    fill(boxX + boxW - 1, boxY, boxX + boxW, boxY + boxH, 0xFFFFFFFF);

    std::string displayText = text;
    if ((getEpochTimeS() & 1) != 0) {
        displayText += '_';
    }
    minecraft->font->draw(displayText, (float)(boxX + 5), (float)(boxY + 5), 0xFFFFFFFF, false);

    if (doneButton) {
        doneButton->x = screenW - 66;
        doneButton->y = 0;
        doneButton->render(minecraft, xm, ym);
    }
}
