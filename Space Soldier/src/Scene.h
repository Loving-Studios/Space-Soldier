#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
#include <vector>
#include "GuiControlButton.h"

struct SDL_Texture;

enum class SceneState
{
	INTRO_SCREEN,
	TITLE_SCREEN,
	GAMEPLAY,
	PAUSE_MENU,
	DIE_SCREEN,
	END_SCREEN
};

class Scene : public Module
{
public:

	Scene();

	int saveFxId;
	int loadFxId;
	int PopOut;

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Called in PostUpdate
	void SaveState();

	// Called in PostUpdate
	void LoadState();

	//Called in PostUpdate
	void GotoStart();

	// Called in PostUpdate
	void GotoStartLevel2();

	void Valoresenemigos();

	Vector2D GetPlayerPosition();

	SceneState GetCurrentState() const;

	void SetCurrentState(SceneState state);

	bool helpMenuVisible = false;
	SDL_Texture* helpMenuTexture = nullptr;

	SDL_Texture* introScreenTexture = nullptr;
	SDL_Texture* titleScreenTexture = nullptr;
	SDL_Texture* pauseMenuTexture = nullptr;
	SDL_Texture* dieScreenTexture = nullptr;
	SDL_Texture* winScreenTexture = nullptr;

	Vector2D GuardarPosicion;

	// Handles multiple Gui Event methods
	bool OnGuiMouseClickEvent(GuiControl* control);

	bool isGameplayMusicPlaying = false;
	bool isTitleScreenMusicPlaying = false;

	//Buttons Lists
	std::vector<GuiControlButton*> TitleButtons;
	std::vector<GuiControlButton*> PauseButtons;
	std::vector<GuiControlButton*> SettingsButtons;

private:

	SDL_Texture* img;

	//L03: TODO 3b: Declare a Player attribute
	Player* player;
	Enemy* enemy;
	Item* item;
	pugi::xml_document configFile;
	std::vector<Enemy*> enemyList;
	std::vector<Item*> itemList;
	bool once = false;
	SceneState currentState = SceneState::INTRO_SCREEN;
	float introTimer = 0.0f;


	//Buttons:
	//First number is horizontal position of rectangle, distance between left border of window and left border of rectangle
	//Second is vertical position, distance between top border of window and top border of rectangle
	//Third is width, determines how bigger the rectangle is horizontally
	//Fourth is height, determines how bigger the rectangle is vertically
	
	//Como la pantalla es de 1280x 720, el centro es 640 x 360. Si el boton tiene 200 de ancho, el centro del boton es 640 - 100  = 540

	// Title Screen Main Menu:
	GuiControlButton* playBt = nullptr;
	SDL_Rect playBtPos = { 540, 60, 200, 40 };
	GuiControlButton* continueBt = nullptr;
	SDL_Rect continueBtPos = { 540, 120, 200, 40 };
	GuiControlButton* settingsBt = nullptr;
	SDL_Rect settingsBtPos = { 540, 180, 200, 40 };
	GuiControlButton* creditsBt = nullptr;
	SDL_Rect creditsBtPos = { 540, 240, 200, 40 };
	GuiControlButton* exitTitleBt = nullptr;
	SDL_Rect exitTitleBtPos = { 540, 300, 200, 40 };
	//Settings:

	//Pause Menu:
	GuiControlButton* resumeBt = nullptr;
	SDL_Rect resumeBtPos = { 540, 60, 200, 40 };
	GuiControlButton* settingsPauseBt = nullptr;
	SDL_Rect settingsPauseBtPos = { 540, 120, 200, 40 };
	GuiControlButton* backtoTitleBt = nullptr;
	SDL_Rect backtoTitleBtPos = { 540, 180, 200, 40 };
	GuiControlButton* exitBt = nullptr;
	SDL_Rect exitBtPos = { 540, 240, 200, 40 };

};