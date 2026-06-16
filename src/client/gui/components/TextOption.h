#pragma once
#include "TextBox.h"
#include <client/Options.h>

class TextOption : public TextBox {
public:
    TextOption(Minecraft* minecraft, OptionId optId);

    virtual void setFocus(Minecraft* minecraft);
    virtual bool loseFocus(Minecraft* minecraft);
};