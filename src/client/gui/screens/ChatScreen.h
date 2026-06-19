#ifndef NET_MINECRAFT_CLIENT_GUI_SCREENS__ChatScreen_H__
#define NET_MINECRAFT_CLIENT_GUI_SCREENS__ChatScreen_H__

#include "../Screen.h"
#include "../components/Button.h"
#include "../components/ImageButton.h"
#include "../components/NinePatch.h"
#include "../Gui.h"

class NinePatchLayer;

class NinePatchButton : public ImageButton
{
public:
	NinePatchButton(int id, const std::string& msg);
	virtual ~NinePatchButton();
	void init(Textures* textures);
	void setSize(float w, float h);
	virtual void render(Minecraft* minecraft, int xm, int ym) override;
	virtual void renderBg(Minecraft* minecraft, int xm, int ym) override;
	virtual void renderFace(Minecraft* minecraft, int xm, int ym) override;
	bool isSecondImage(bool hovered) override { return false; }
private:
	NinePatchLayer* bg;
	NinePatchLayer* bgSelected;
};

class ChatScreen: public Screen
{
public:
	ChatScreen();
	virtual ~ChatScreen();

	void init();
	void setupPositions();
	void render(int xm, int ym, float a);
	void tick();
	void removed();
	void buttonClicked(Button* button);
	void keyPressed(int key);
	void charPressed(char inputChar) override;
	void keyboardNewChar(const std::string& text, bool composing);
	void setTextboxText(const std::string& text);

	bool handleBackEvent(bool isDown);
	bool renderGameBehind();
	bool isPauseScreen();
	bool isErrorScreen();
	bool isInGameScreen();
	bool closeOnPlayerHurt();

private:
	int getBaseY();
	void closeWindow();
	void drawChatMessages(int baseY);
	bool guiMessagesUpdated();
	void sendChatMessage();
	std::string processCommand(const std::string& cmd);
	void updateGuiMessages();
	void updateKeyboardVisibility();
	void updateToggleKeyboardButton();
	void syncKeyboardState();

	bool m_keyboardVisible;
	bool m_composing;
	Touch::TButton* m_closeButton;
	BlankButton* m_textBoxButton;
	NinePatchButton* m_keyboardButton;
	NinePatchButton* m_sendButton;
	std::string m_textBoxText;
	std::vector<GuiMessage> m_chatMessages;
	std::vector<std::string> m_chatHistory;
	int m_historyIndex;
};

#endif
