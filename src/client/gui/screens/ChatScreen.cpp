#include "ChatScreen.h"
#include "../Gui.h"
#include "../components/NinePatch.h"
#include "../../Minecraft.h"
#include "../../player/LocalPlayer.h"
#include "../../../AppPlatform.h"
#include "../../../platform/log.h"
#include "../../../network/RakNetInstance.h"
#include "../../../network/packet/MessagePacket.h"
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cctype>
#include "../../../world/level/Level.h"

NinePatchButton::NinePatchButton(int id, const std::string& msg)
	: ImageButton(id, msg),
	  bg(nullptr),
	  bgSelected(nullptr)
{
}

NinePatchButton::~NinePatchButton() {
	delete bg;
	delete bgSelected;
}

void NinePatchButton::init(Textures* textures) {
	NinePatchFactory factory(textures, "gui/spritesheet.png");
	bg = factory.createSymmetrical(IntRectangle(112, 0, 8, 67), 2, 2);
	bgSelected = factory.createSymmetrical(IntRectangle(120, 0, 8, 67), 2, 2);
}

void NinePatchButton::setSize(float w, float h) {
	this->width = (int)w;
	this->height = (int)h;
	if (bg) bg->setSize(w, h);
	if (bgSelected) bgSelected->setSize(w, h);
}

void NinePatchButton::render(Minecraft* minecraft, int xm, int ym) {
	if (!visible) return;
	glColor4f2(1, 1, 1, 1);
	renderBg(minecraft, xm, ym);
	renderFace(minecraft, xm, ym);
	if (!active) {
		drawCenteredString(minecraft->font, msg, x + width / 2, y + 16, 0xffa0a0a0);
	} else {
		bool hovered = active && (minecraft->useTouchscreen() ?
			(_currentlyDown && xm >= x && ym >= y && xm < x + width && ym < y + height) :
			isInside(xm, ym));
		if (hovered || selected)
			drawCenteredString(minecraft->font, msg, x + width / 2, y + 17, 0xffffa0);
		else
			drawCenteredString(minecraft->font, msg, x + width / 2, y + 16, 0xe0e0e0);
	}
}

void NinePatchButton::renderBg(Minecraft* minecraft, int xm, int ym) {
	if (!bg || !bgSelected) return;
	bool hovered = active && (minecraft->useTouchscreen() ?
		(_currentlyDown && xm >= x && ym >= y && xm < x + width && ym < y + height) :
		isInside(xm, ym));
	if (hovered || selected)
		bgSelected->draw(Tesselator::instance, (float)x, (float)y);
	else
		bg->draw(Tesselator::instance, (float)x, (float)y);
}

void NinePatchButton::renderFace(Minecraft* minecraft, int xm, int ym) {
	if (_imageDef.name.empty()) return;
	TextureId texId = minecraft->textures->loadAndBindTexture(_imageDef.name);
	if (!Textures::isTextureIdValid(texId)) return;

	const ImageDef& d = _imageDef;
	Tesselator& t = Tesselator::instance;

	float iconW = d.width;
	float iconH = d.height;
	float iconX = x + ((float)width - iconW) / 2.0f;
	float iconY = y + ((float)height - iconH) / 2.0f;

	const IntRectangle* src = _imageDef.getSrc();
	if (src) {
		const TextureData* td = minecraft->textures->getTemporaryTextureData(texId);
		if (td) {
			float u0 = (float)src->x / (float)td->w;
			float u1 = (float)(src->x + src->w) / (float)td->w;
			float v0 = (float)src->y / (float)td->h;
			float v1 = (float)(src->y + src->h) / (float)td->h;
			t.begin();
			if (!active) t.color(0xff808080);
			else t.color(0xffffffff);
			t.vertexUV(iconX, iconY, blitOffset, u0, v0);
			t.vertexUV(iconX, iconY + iconH, blitOffset, u0, v1);
			t.vertexUV(iconX + iconW, iconY + iconH, blitOffset, u1, v1);
			t.vertexUV(iconX + iconW, iconY, blitOffset, u1, v0);
			t.draw();
		}
	}
}

ChatScreen::ChatScreen()
	: m_keyboardVisible(false),
	  m_composing(false),
	  m_closeButton(nullptr),
	  m_textBoxButton(nullptr),
	  m_keyboardButton(nullptr),
	  m_sendButton(nullptr),
	  m_historyIndex(0)
{
}

ChatScreen::~ChatScreen() {
	delete m_closeButton;
	delete m_textBoxButton;
	delete m_keyboardButton;
	delete m_sendButton;
}

int ChatScreen::getBaseY() {
	if (!m_keyboardVisible) {
		return height;
	}
	if (minecraft->platform()->supportsTouchscreen()) {
		int rawKbHeight = minecraft->platform()->getKeyboardHeight();
		int screenHeight = minecraft->platform()->getScreenHeight();
		if (rawKbHeight > 0 && screenHeight > 0) {
			int scaledKbHeight = (rawKbHeight * height) / screenHeight;
			int btnHeight = m_keyboardButton ? m_keyboardButton->height : 0;
			int offset = scaledKbHeight + btnHeight;
			if (offset < height) {
				return height - offset;
			}
		}
	}
	return height / 2;
}

void ChatScreen::closeWindow() {
	minecraft->platform()->hideKeyboard();
	minecraft->setScreen(nullptr);
}

void ChatScreen::drawChatMessages(int baseY) {
	int yPos = baseY;
	for (auto& msg : m_chatMessages) {
		int lineCount = 1;
		int textW = width;
		if (textW > 0) {
			int msgLen = font->width(msg.message);
			if (msgLen > textW) {
				lineCount = (msgLen + textW - 1) / textW;
				if (lineCount < 1) lineCount = 1;
			}
		}
		yPos -= 9 * lineCount;
		if (msg.message[0] == '/') {
			font->drawWordWrap(msg.message, 2.0f, (float)yPos, (float)width, 0xFFA0A0A0);
		} else {
			font->drawWordWrap(msg.message, 2.0f, (float)yPos, (float)width, 0xFFFFFFFF);
		}
	}
}

bool ChatScreen::guiMessagesUpdated() {
	int sz = (int)m_chatMessages.size();
	const auto& guiMsgs = minecraft->gui.getGuiMessages();
	if (sz != (int)guiMsgs.size()) {
		return true;
	}
	for (int i = 0; i < sz; ++i) {
		if (!(guiMsgs[i].message == m_chatMessages[i].message)) {
			return true;
		}
	}
	return false;
}

static std::string trim(const std::string& s) {
	size_t a = s.find_first_not_of(" \t");
	if (a == std::string::npos) return "";
	size_t b = s.find_last_not_of(" \t");
	return s.substr(a, b - a + 1);
}

std::string ChatScreen::processCommand(const std::string& raw)
{
	std::string line = raw;
	if (!line.empty() && line[0] == '/') line = line.substr(1);
	line = trim(line);

	std::vector<std::string> args;
	{
		std::istringstream ss(line);
		std::string tok;
		while (ss >> tok) args.push_back(tok);
	}

	if (args.empty()) return "";

	Level* level = minecraft->level;
	if (!level) return "No level loaded.";

	if (args[0] == "time") {
		if (args.size() < 2)
			return "Usage: /time <add|set|query> ...";

		const std::string& sub = args[1];

		if (sub == "add") {
			if (args.size() < 3) return "Usage: /time add <value>";
			long delta = std::atol(args[2].c_str());
			long newTime = level->getTime() + delta;
			level->setTime(newTime);
			std::ostringstream out;
			out << "Set the time to " << (newTime % Level::TICKS_PER_DAY);
			return out.str();
		}

		if (sub == "set") {
			if (args.size() < 3) return "Usage: /time set <value|day|night|noon|midnight>";
			const std::string& val = args[2];

			long t = -1;
			if      (val == "day")      t = 1000;
			else if (val == "noon")     t = 6000;
			else if (val == "night")    t = 13000;
			else if (val == "midnight") t = 18000;
			else {
				bool numeric = true;
				for (char c : val)
					if (!std::isdigit((unsigned char)c)) { numeric = false; break; }
				if (!numeric) return std::string("Unknown value: ") + val;
				t = std::atol(val.c_str());
			}

			long dayCount = level->getTime() / Level::TICKS_PER_DAY;
			long newTime  = dayCount * Level::TICKS_PER_DAY + (t % Level::TICKS_PER_DAY);
			level->setTime(newTime);
			std::ostringstream out;
			out << "Set the time to " << t;
			return out.str();
		}

		if (sub == "query") {
			if (args.size() < 3) return "Usage: /time query <daytime|gametime|day>";
			const std::string& what = args[2];

			long total   = level->getTime();
			long daytime = total % Level::TICKS_PER_DAY;
			long day     = total / Level::TICKS_PER_DAY;

			std::ostringstream out;
			if      (what == "daytime")  { out << "The time of day is " << daytime; }
			else if (what == "gametime") { out << "The game time is "   << total;   }
			else if (what == "day")      { out << "The day is "         << day;     }
			else return std::string("Unknown query: ") + what;
			return out.str();
		}

		return "Unknown sub-command. Usage: /time <add|set|query> ...";
	}

	return std::string("Unknown command: /") + args[0];
}

void ChatScreen::sendChatMessage() {
	if (m_textBoxText.empty()) return;

	if (m_textBoxText[0] == '/') {
		std::string result = processCommand(m_textBoxText);
		if (!result.empty())
			minecraft->gui.addMessage(result);
	} else {
		std::string username = minecraft->player ? minecraft->player->name : "";

		if (minecraft->raknetInstance) {
			MessagePacket msg(RakNet::RakString(username.c_str()), RakNet::RakString(m_textBoxText.c_str()));
			minecraft->raknetInstance->send(msg);
		}
		minecraft->gui.addMessage(username, m_textBoxText, 200);
	}
	m_chatHistory.push_back(m_textBoxText);
	m_historyIndex = (int)m_chatHistory.size();
	m_textBoxText = "";
}

void ChatScreen::updateGuiMessages() {
	m_chatMessages = minecraft->gui.getGuiMessages();
}

void ChatScreen::updateKeyboardVisibility() {
	if (m_keyboardVisible) {
		minecraft->platform()->showKeyboard();
		updateToggleKeyboardButton();
		if (m_sendButton) {
			m_sendButton->active = true;
			m_sendButton->visible = true;
		}
		if (m_textBoxButton) {
			m_textBoxButton->active = false;
		}
	} else {
		minecraft->platform()->hideKeyboard();
		updateToggleKeyboardButton();
		if (m_sendButton) {
			m_sendButton->active = false;
			m_sendButton->visible = false;
		}
		if (m_textBoxButton) {
			m_textBoxButton->active = true;
		}
	}
}

void ChatScreen::updateToggleKeyboardButton() {
	if (!m_keyboardButton) return;

	ImageDef def;
	def.name = "gui/spritesheet.png";
	def.x = 36;
	def.y = 1;
	if (m_keyboardVisible) {
		def.setSrc(IntRectangle(36, 9, 12, 8));
		def.width = 12.0f;
		def.height = 8.0f;
	} else {
		def.setSrc(IntRectangle(36, 0, 13, 11));
		def.width = 13.0f;
		def.height = 11.0f;
	}
	m_keyboardButton->setImageDef(def, false);

	int btnH = m_keyboardButton->height;
	m_keyboardButton->y = getBaseY() - btnH;
}

void ChatScreen::init() {
	m_keyboardVisible = false;

	m_textBoxButton = new BlankButton(1);

	m_keyboardButton = new NinePatchButton(10, "");
	m_keyboardButton->init(minecraft->textures);
	{
		ImageDef def;
		def.name = "gui/spritesheet.png";
		def.setSrc(IntRectangle(36, 0, 13, 11));
		def.width = 13.0f;
		def.height = 11.0f;
		m_keyboardButton->setImageDef(def, true);
	}

	m_sendButton = new NinePatchButton(2, "");
	m_sendButton->init(minecraft->textures);
	{
		ImageDef def;
		def.name = "gui/spritesheet.png";
		def.setSrc(IntRectangle(48, 4, 12, 12));
		def.width = 12.0f;
		def.height = 12.0f;
		m_sendButton->setImageDef(def, true);
	}

	m_closeButton = new Touch::TButton(2, "Back");
	m_closeButton->width = 38;
	m_closeButton->height = 18;

	buttons.push_back(m_closeButton);
	buttons.push_back(m_textBoxButton);
	buttons.push_back(m_keyboardButton);
	buttons.push_back(m_sendButton);

	updateKeyboardVisibility();
	m_historyIndex = 0;
}

void ChatScreen::setupPositions() {
	if (m_closeButton) {
		m_closeButton->x = width - m_closeButton->width - 4;
		m_closeButton->y = 8;
	}
	if (m_keyboardButton) {
		m_keyboardButton->setSize(20.0f, 20.0f);
		m_keyboardButton->x = width - m_keyboardButton->width;
	}
	if (m_sendButton) {
		m_sendButton->width = 0;
		m_sendButton->height = 20;
		if (m_keyboardButton) {
			m_sendButton->x = m_keyboardButton->x;
		}
		m_sendButton->y = getBaseY() - m_sendButton->height;
	}
	updateToggleKeyboardButton();
	m_keyboardButton->setSize((float)m_keyboardButton->width, (float)m_keyboardButton->height);
	m_sendButton->setSize(20.0f, 20.0f);
	if (m_sendButton && m_keyboardButton) {
		m_sendButton->x = m_keyboardButton->x - m_sendButton->width;
	}
}

void ChatScreen::render(int a2, int a3, float a4) {
	int textBoxWidth;

	renderBackground(0);

	if (m_keyboardVisible) {
		textBoxWidth = width - m_keyboardButton->width - (m_sendButton && m_sendButton->visible ? m_sendButton->width : 0);
	} else {
		textBoxWidth = width - m_keyboardButton->width;
	}

	int textBoxHeight = font->height(m_textBoxText);
	if (textBoxHeight < m_keyboardButton->height) {
		textBoxHeight = m_keyboardButton->height;
	}

	int textAreaY = getBaseY() - textBoxHeight;

	drawChatMessages(textAreaY);

	Screen::render(a2, a3, a4);

	if (m_textBoxButton) {
		m_textBoxButton->x = 0;
		m_textBoxButton->y = height - textBoxHeight;
		m_textBoxButton->width = textBoxWidth;
		m_textBoxButton->height = textBoxHeight;
	}

	fill(0, textAreaY, textBoxWidth, height, 0xFF000000);

	std::string displayText = m_textBoxText;
	displayText += '_';
	font->drawWordWrap(displayText, 2.0f, (float)textAreaY + 2.0f, (float)(textBoxWidth - 4), 0xFFFFFFFF);
}

bool ChatScreen::handleBackEvent(bool a2) {
	if (!a2) {
		if (m_keyboardVisible) {
			m_keyboardVisible = false;
			updateKeyboardVisibility();
		} else {
			closeWindow();
		}
	}
	return true;
}

void ChatScreen::syncKeyboardState() {
	bool kbVisible = minecraft->platform()->isKeyboardVisible();
	if (kbVisible != m_keyboardVisible) {
		m_keyboardVisible = kbVisible;
		updateKeyboardVisibility();
	}
}

void ChatScreen::tick() {
	Screen::tick();
	if (guiMessagesUpdated()) {
		updateGuiMessages();
	}
	syncKeyboardState();

	if (m_keyboardButton) m_keyboardButton->y = getBaseY() - m_keyboardButton->height;
	if (m_sendButton) m_sendButton->y = getBaseY() - m_sendButton->height;
}

void ChatScreen::removed() {
	m_keyboardVisible = false;
	updateKeyboardVisibility();
}

bool ChatScreen::renderGameBehind() {
	return true;
}

bool ChatScreen::isPauseScreen() {
	return false;
}

bool ChatScreen::isErrorScreen() {
	return false;
}

bool ChatScreen::isInGameScreen() {
	return true;
}

bool ChatScreen::closeOnPlayerHurt() {
	return true;
}

void ChatScreen::charPressed(char inputChar) {
	if (!m_keyboardVisible) return;
	std::string s(1, inputChar);
	keyboardNewChar(s, false);
}

void ChatScreen::keyboardNewChar(const std::string& newText, bool composing) {
	std::string candidate = m_textBoxText;
	if (m_composing) {
		int len = (int)candidate.length();
		int i = len - 1;
		while (i > 0 && (candidate[i] & 0xC0) == 0x80) i--;
		candidate = candidate.substr(0, i);
	}
	candidate += newText;
	m_composing = composing;
	if ((int)candidate.length() <= 160) {
		m_textBoxText = candidate;
	}
}

void ChatScreen::setTextboxText(const std::string& text) {
	m_textBoxText = text;
}

void ChatScreen::buttonClicked(Button* button) {
	if (button == m_closeButton) {
		closeWindow();
	} else if (button == m_textBoxButton || button == m_keyboardButton) {
		m_keyboardVisible = !m_keyboardVisible;
		updateKeyboardVisibility();
	} else if (button == m_sendButton) {
		sendChatMessage();
	}
}

void ChatScreen::keyPressed(int key) {
	switch (key) {
	case 8:
		if (m_textBoxText.empty()) return;
		{
			int len = (int)m_textBoxText.length();
			int i = len - 1;
			while (i > 0 && (m_textBoxText[i] & 0xC0) == 0x80) i--;
			m_textBoxText = m_textBoxText.substr(0, i);
		}
		return;
	case 13:
		sendChatMessage();
		return;
	case 38:
	{
		int sz = (int)m_chatHistory.size();
		int idx = m_historyIndex - 1;
		int clamped = sz - 1;
		if (clamped >= idx) clamped = idx;
		m_historyIndex = clamped & ~(clamped >> 31);
		if (sz == 0) return;
		m_textBoxText = m_chatHistory[m_historyIndex];
		return;
	}
	case 40:
	{
		int sz = (int)m_chatHistory.size();
		int idx = m_historyIndex + 1;
		if (sz < idx) idx = sz;
		m_historyIndex = idx & ~(idx >> 31);
		if (m_historyIndex == sz) {
			m_textBoxText = "";
			return;
		}
		if (sz == 0) return;
		m_textBoxText = m_chatHistory[m_historyIndex];
		return;
	}
	}
	Screen::keyPressed(key);
}
