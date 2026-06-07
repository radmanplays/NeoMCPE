#include "Button.h"
#include "../../Minecraft.h"
#include "../../renderer/Textures.h"
#include "NinePatch.h"
#include "../../renderer/Tesselator.h"

Button::Button(int id, const std::string& msg)
:	GuiElement(true, true, 0, 0, 200, 24),
    id(id),
	msg(msg),
	selected(false),
    _currentlyDown(false)
{
}

Button::Button( int id, int x, int y, const std::string& msg )
:	GuiElement(true, true, x, y, 200, 24),
    id(id),
	msg(msg),
	selected(false),
    _currentlyDown(false)
{
}

Button::Button( int id, int x, int y, int w, int h, const std::string& msg )
:	GuiElement(true, true, x, y, w, h),
    id(id),
	msg(msg),
	selected(false),
    _currentlyDown(false)
{
}

void Button::render( Minecraft* minecraft, int xm, int ym )
{
	if (!visible) return;

	/*
	minecraft->textures->loadAndBindTexture("gui/gui.png");
	glColor4f2(1, 1, 1, 1);

	//printf("ButtonId: %d - Hovered? %d (cause: %d, %d, %d, %d, <> %d, %d)\n", id, hovered, x, y, x+w, y+h, xm, ym);
	int yImage = getYImage(hovered || selected);

	blit(x, y, 0, 46 + yImage * 20, w / 2, h, 0, 20);
	blit(x + w / 2, y, 200 - w / 2, 46 + yImage * 20, w / 2, h, 0, 20);
	*/

	renderBg(minecraft, xm, ym);
	renderFace(minecraft, xm , ym);
}

void Button::released( int mx, int my ) {
    _currentlyDown = false;
}

bool Button::clicked( Minecraft* minecraft, int mx, int my )
{
	return active && mx >= x && my >= y && mx < x + width && my < y + height;
}

void Button::setPressed() {
    _currentlyDown = true;
}

int Button::getYImage( bool hovered )
{
	int res = 1;
	if (!active) res = 0;
	else if (hovered) res = 2;
	return res;
}

void Button::renderFace(Minecraft* mc, int xm, int ym) {
	Font* font = mc->font;
	if (!active) {
		drawCenteredString(font, msg, x + width / 2, y + (height - 8) / 2, 0xffa0a0a0);
	} else {
		if (hovered(mc, xm, ym) || selected) {
			drawCenteredString(font, msg, x + width / 2, y + (height - 8) / 2, 0xffffa0);
		} else {
			drawCenteredString(font, msg, x + width / 2, y + (height - 8) / 2, 0xe0e0e0);
		}
	}
}

void Button::renderBg( Minecraft* minecraft, int xm, int ym )
{
	minecraft->textures->loadAndBindTexture("gui/gui.png");
	glColor4f2(1, 1, 1, 1);

	//printf("ButtonId: %d - Hovered? %d (cause: %d, %d, %d, %d, <> %d, %d)\n", id, hovered, x, y, x+w, y+h, xm, ym);
	int yImage = getYImage(selected || hovered(minecraft, xm, ym));;

	blit(x, y, 0, 46 + yImage * 20, width / 2, height, 0, 20);
	blit(x + width / 2, y, 200 - width / 2, 46 + yImage * 20, width / 2, height, 0, 20);
}

bool Button::hovered(Minecraft* minecraft, int xm , int ym) {
	return minecraft->useTouchscreen()? (_currentlyDown && isInside(xm, ym)) : isInside(xm, ym);
}

bool Button::isInside( int xm, int ym ) {
	return xm >= x && ym >= y && xm < x + width && ym < y + height;
}

//
// BlankButton
//
BlankButton::BlankButton(int id)
:	super(id, "")
{
	visible = false;
}

BlankButton::BlankButton(int id, int x, int y, int w, int h)
:	super(id, x, y, w, h, "")
{
	visible = false;
}

//
// The Touch-interface button
//
namespace Touch {

TButton::TButton(int id, const std::string& msg)
:	super(id, msg)
{
	width = 66;
	height = 26;
}

TButton::TButton( int id, int x, int y, const std::string& msg )
:	super(id, x, y, msg)
{
	width = 66;
	height = 26;
}

TButton::TButton( int id, int x, int y, int w, int h, const std::string& msg )
:	super(id, x, y, w, h, msg)
{
}

void TButton::renderBg( Minecraft* minecraft, int xm, int ym )
{
	bool hovered = active && (minecraft->useTouchscreen()? (_currentlyDown && xm >= x && ym >= y && xm < x + width && ym < y + height) : isInside(xm, ym));
	// bool hovered = active && (_currentlyDown && isInside(xm, ym));

	//printf("ButtonId: %d - Hovered? %d (cause: %d, %d, %d, %d, <> %d, %d)\n", id, hovered, x, y, x+w, y+h, xm, ym);
	if (active)
		glColor4f2(1, 1, 1, 1);
	else
		glColor4f2(0.5f, 0.5f, 0.5f, 1);

	static NinePatchLayer* normalPatch = nullptr;
	static NinePatchLayer* pressedPatch = nullptr;

	if (!normalPatch) {
		NinePatchFactory factory(minecraft->textures, "gui/spritesheet.png");
		IntRectangle normalRect = {8, 32, 8, 8};
		normalPatch = factory.createSymmetrical(normalRect, 3, 3, (float)width, (float)height);
		IntRectangle pressedRect = {0, 32, 8, 8};
		pressedPatch = factory.createSymmetrical(pressedRect, 3, 3, (float)width, (float)height);
	}

	Tesselator& t = Tesselator::instance;
	if (hovered || selected) {
		pressedPatch->setSize((float)width, (float)height);
		pressedPatch->draw(t, (float)x, (float)y);
	} else {
		normalPatch->setSize((float)width, (float)height);
		normalPatch->draw(t, (float)x, (float)y);
	}
}

void TButton::renderFace(Minecraft* mc, int xm, int ym) {
	Font* font = mc->font;
	bool isHovered = hovered(mc, xm, ym);
	int yOffset = isHovered ? 1 : 0;

	if (!active) {
		drawCenteredString(font, msg, x + width / 2, y + (height - 8) / 2 + yOffset, 0xffa0a0a0);
	} else {
		if (isHovered) {
			drawCenteredString(font, msg, x + width / 2, y + (height - 8) / 2 + yOffset, 0xffffa0);
		} else {
			drawCenteredString(font, msg, x + width / 2, y + (height - 8) / 2 + yOffset, 0xe0e0e0);
		}
	}
}

//
// Header spacing in Touchscreen mode
//
THeader::THeader(int id, const std::string& msg)
:	super(id, msg),
	xText(-99999)
{
	active = false;
	width = 66;
	height = 26;
}

THeader::THeader( int id, int x, int y, const std::string& msg )
:	super(id, x, y, msg),
	xText(-99999)
{
	active = false;
	width = 66;
	height = 26;
}

THeader::THeader( int id, int x, int y, int w, int h, const std::string& msg )
:	super(id, x, y, w, h, msg),
	xText(-99999)
{
	active = false;
}

void THeader::render( Minecraft* minecraft, int xm, int ym ) {
	Font* font = minecraft->font;
	renderBg(minecraft, xm, ym);
	
	int xx = x + width/2;
	if (xText != -99999)
		xx = xText;
	drawCenteredString(font, msg, xx, y + (height - 8) / 2, 0xe0e0e0);
}

void THeader::renderBg( Minecraft* minecraft, int xm, int ym )
{
	//printf("ButtonId: %d - Hovered? %d (cause: %d, %d, %d, %d, <> %d, %d)\n", id, hovered, x, y, x+w, y+h, xm, ym);
	glEnable(GL_BLEND);
	minecraft->textures->loadAndBindTexture("gui/touchgui.png");
	glColor4f(1.0, 1.0, 1.0, 1.0);
	blit(x, y, 150, 26, 2, height - 1, 2, 25);
	blit(x + 2, y, 153, 26, width - 3, height - 1, 8, 25);
	blit(x+width-2, y, 162, 26, 2, height-1, 2, 25);
	blit(x, y+height-1, 153, 52, width, 3, 8, 3);
	
	glDisable(GL_BLEND);
}

};