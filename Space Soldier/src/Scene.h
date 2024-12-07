#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"

struct SDL_Texture;

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

	Vector2D GetPlayerPosition();

	bool helpMenuVisible = false;
	SDL_Texture* helpMenuTexture = nullptr;

	Vector2D GuardarPosicion;

private:
	SDL_Texture* img;
	
	//L03: TODO 3b: Declare a Player attribute
	Player* player;
	pugi::xml_document configFile;
	std::vector<Enemy*> enemyList;
	bool once = false;
};