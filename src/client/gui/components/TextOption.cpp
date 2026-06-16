#include "TextOption.h"
#include "Button.h"
#include <client/Minecraft.h>
#include "../../../locale/I18n.h"

TextOption::TextOption(Minecraft* minecraft, OptionId optId) 
    : TextBox((int)optId, I18n::get(minecraft->options.getOpt(optId)->getStringId()))
{
    text = minecraft->options.getStringValue(optId);
}

void TextOption::setFocus(Minecraft* minecraft) {
    TextBox::setFocus(minecraft);
    if (doneButton) {
        doneButton->height = 26;
    }
}

bool TextOption::loseFocus(Minecraft* minecraft) {
    if (TextBox::loseFocus(minecraft)) {
        minecraft->options.set((OptionId)id, text);
        return true;
    }

    return false;
}