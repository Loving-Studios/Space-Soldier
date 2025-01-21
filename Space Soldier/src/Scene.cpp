#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"
#include "Enemy.h"
#include "GuiControl.h"
#include "GuiManager.h"


Scene::Scene() : Module()
{
	name = "scene1";
	img = nullptr;
	currentState = SceneState::INTRO_SCREEN;
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	//L04: TODO 3b: Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->SetParameters(configParameters.child("entities").child("player"));
	
	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	Item* item = (Item*) Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
	item->position = Vector2D(200, 672);
	
	for (pugi::xml_node itemNode = configParameters.child("entities").child("items").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
		item->SetParameters(itemNode);
		itemList.push_back(item);
	}

	for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	{
		Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
		enemy->SetParameters(enemyNode);
		enemyList.push_back(enemy);
	}


	//guiBt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "MyButton", btPos, this);
	// El 1 es el id del boton

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());

	//Musica de fondo
	//Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Fx/CelestialDrift.ogg", 0);
	saveFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Save.wav");
	loadFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Load.wav");
	PopOut = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/PopOut.wav");
	// Configura el volumen al 5%
	Mix_VolumeMusic(static_cast<int>(128 * 0.05));

	helpMenuTexture = Engine::GetInstance().textures->Load("Assets/Textures/HelpMenu.png");

	introScreenTexture = Engine::GetInstance().textures->Load("Assets/Textures/IntroScreen.png");
	titleScreenTexture = Engine::GetInstance().textures->Load("Assets/Textures/TitleScreen.png");
	pauseMenuTexture = Engine::GetInstance().textures->Load("Assets/Textures/PauseScreen.png");
	dieScreenTexture = Engine::GetInstance().textures->Load("Assets/Textures/DieScreen.png");
	winScreenTexture = Engine::GetInstance().textures->Load("Assets/Textures/WinScreen.png");

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	if (once == false) {

		GotoStart();
		once = true;
	}
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{

	switch (currentState)
	{
	case SceneState::INTRO_SCREEN:
		if (introScreenTexture != nullptr)
		{
			Engine::GetInstance().render->DrawTexture(introScreenTexture, 0, 0);
		}
		introTimer += dt;
		if (introTimer >= 4000.0f) // 4 seconds later
		{
			currentState = SceneState::TITLE_SCREEN;
			introTimer = 0.0f; // Reset el temporizador
		}
		break;

	case SceneState::TITLE_SCREEN:
		if (titleScreenTexture != nullptr)
		{
			Engine::GetInstance().render->DrawTexture(titleScreenTexture, 0, 0);
			if (!isTitleScreenMusicPlaying)
			{
				Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Fx/TitleScreenMusic2.ogg", -1);
				isTitleScreenMusicPlaying = true;
			}
		}
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			Engine::GetInstance().audio.get()->StopMusic();
			isTitleScreenMusicPlaying = false;
			currentState = SceneState::GAMEPLAY; // Cambiar a gameplayD
		}
		break;

	case SceneState::GAMEPLAY:
		isTitleScreenMusicPlaying = false;
		//Move camera to the player position
		Engine::GetInstance().render.get()->camera.x = player->position.getX() * -1.0f + 400.0f;
		//Music
		if (!isGameplayMusicPlaying)
		{
			Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Fx/CelestialDrift.ogg", -1); // -1 for infinite loop
			isGameplayMusicPlaying = true;
		}
		//Gameplay directions
		Engine::GetInstance().map->Update(dt);
		Engine::GetInstance().entityManager->Update(dt);

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN) {
			helpMenuVisible = !helpMenuVisible;
			Engine::GetInstance().audio.get()->PlayFx(PopOut);
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
			currentState = SceneState::PAUSE_MENU;
		}
		break;

	case SceneState::PAUSE_MENU:
		isTitleScreenMusicPlaying = false;
		if (pauseMenuTexture != nullptr)
		{
			Engine::GetInstance().render->DrawTexture(pauseMenuTexture, player->position.getX() - 400.0f, 0);
			//Buttons
			resumeBt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "RESUME", resumeBtPos, this);
			PauseButtons.push_back(resumeBt);
			settingsPauseBt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "SETTINGS", settingsPauseBtPos, this);
			PauseButtons.push_back(settingsPauseBt);
			backtoTitleBt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "BACK TO TITLE", backtoTitleBtPos, this);
			PauseButtons.push_back(backtoTitleBt);
			exitBt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "EXIT", exitBtPos, this);
			PauseButtons.push_back(exitBt);
		}
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		{
			for (auto button : PauseButtons)
			{
				if (button != nullptr)
				{
					button->state = GuiControlState::DISABLED;
				}
			}
			currentState = SceneState::GAMEPLAY; // Salir del menú de pausa
		}
		break;

	case SceneState::DIE_SCREEN:
		isTitleScreenMusicPlaying = false;
		if (dieScreenTexture != nullptr)
		{
			Engine::GetInstance().render->DrawTexture(dieScreenTexture, player->position.getX() - 400.0f, 0);
			isGameplayMusicPlaying = false;
		}
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			currentState = SceneState::GAMEPLAY; // Reiniciar el juego
		}
		break;

	case SceneState::END_SCREEN:
		isTitleScreenMusicPlaying = false;
		if (winScreenTexture != nullptr)
		{
			Engine::GetInstance().render->DrawTexture(winScreenTexture, player->position.getX() - 400.0f, 0);
			isGameplayMusicPlaying = false;
		}
		break;
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
		ret = false;
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		SaveState();
		Engine::GetInstance().audio.get()->PlayFx(saveFxId);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		LoadState();
		Engine::GetInstance().audio.get()->PlayFx(loadFxId);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		GotoStart();
		Engine::GetInstance().audio.get()->PlayFx(loadFxId);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		GotoStartLevel2();
		Engine::GetInstance().audio.get()->PlayFx(loadFxId);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		GotoStart();
		Engine::GetInstance().audio.get()->PlayFx(loadFxId);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		Engine::GetInstance().audio.get()->PlayFx(PopOut);
	}
	if (helpMenuVisible && helpMenuTexture != nullptr) {
		int width, height;
		Engine::GetInstance().textures->GetSize(helpMenuTexture, width, height);
		int windowWidth, windowHeight;
		Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);
		
		//Imagen en la esquina superior derecha
		SDL_Rect dstRect = { windowWidth - width - 10, 10, width, height };
		//Imagen en el centro de la pantalla
		//SDL_Rect dstRect = { (windowWidth - width) / 2, (windowHeight - height) / 2, width, height };

		SDL_RenderCopy(Engine::GetInstance().render->renderer, helpMenuTexture, nullptr, &dstRect);
	}

	return ret;
}

Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}

void Scene::GotoStart()
{
	//Player position
	Vector2D playerPos = Vector2D(490, 450);
	player->SetPosition(playerPos);
	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");
	pugi::xml_node sceneNode = loadFile.child("config").child("scene1");
	for (Enemy* enemy : enemyList)
	{
		if (!enemy) {
			LOG("Error: Puntero enemigo nulo en enemyList.");
			continue;
		}
		pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies").find_child_by_attribute("name", enemy->GetName().c_str());

		if (!enemyNode) {
			LOG("Error: Nodo para enemigo %s no encontrado.", enemy->GetName().c_str());
			continue;
		}
		bool vivo = enemyNode.attribute("Alive").as_bool();
		float x = enemyNode.attribute("xo").as_float(-1.0f); // Valor por defecto en caso de error
		float y = enemyNode.attribute("yo").as_float(-1.0f);
		//Vector2D enemyPos(enemyNode.attribute("x").as_float(), enemyNode.attribute("y").as_float());

		if (x < 0 || y < 0) {
			LOG("Error: Atributos invalidos para enemigo %s.", enemy->GetName().c_str());
			continue;
		}
		Vector2D enemyPos(x, y);
		enemy->SetPosition(enemyPos);

	}
}

void Scene::GotoStartLevel2()
{
	//Player position del inicio del segundo nivel
	Vector2D playerPos = Vector2D(9125, 513);
	player->SetPosition(playerPos);
	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");
	pugi::xml_node sceneNode = loadFile.child("config").child("scene1");
	for (Enemy* enemy : enemyList)
	{
		if (!enemy) {
			LOG("Error: Puntero enemigo nulo en enemyList.");
			continue;
		}
		pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies").find_child_by_attribute("name", enemy->GetName().c_str());

		if (!enemyNode) {
			LOG("Error: Nodo para enemigo %s no encontrado.", enemy->GetName().c_str());
			continue;
		}
		bool vivo = enemyNode.attribute("Alive").as_bool();
		float x = enemyNode.attribute("xo").as_float(-1.0f); // Valor por defecto en caso de error
		float y = enemyNode.attribute("yo").as_float(-1.0f);
		//Vector2D enemyPos(enemyNode.attribute("x").as_float(), enemyNode.attribute("y").as_float());

		if (x < 0 || y < 0) {
			LOG("Error: Atributos invalidos para enemigo %s.", enemy->GetName().c_str());
			continue;
		}
		Vector2D enemyPos(x, y);
		enemy->SetPosition(enemyPos);

	}
}

void Scene::LoadState()
{
	//coger posicion del xml
	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL) {
		LOG("error loading config.xml", result.description());
		return;
	}
	/*HACER CODIGO PARA DETECTAR EN QUE ESCENA ESTAS Y SUSTITUIRLO POR scene1 ES DECIR METER UNA VARIABLE
	sceneplaying = name
	*/
	pugi::xml_node sceneNode = loadFile.child("config").child("scene1");

	//Read XML and restore information

	//Player position
	Vector2D playerPos = Vector2D(sceneNode.child("entities").child("player").attribute("x").as_int(),
		sceneNode.child("entities").child("player").attribute("y").as_int());
	player->SetPosition(playerPos);

	//enemies
	//mirar si esta vivo o no
	for (Enemy* enemy : enemyList)
	{
		if (!enemy) {
			LOG("Error: Puntero enemigo nulo en enemyList.");
			continue;
		}
		pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies").find_child_by_attribute("name", enemy->GetName().c_str());

		if (!enemyNode) {
			LOG("Error: Nodo para enemigo %s no encontrado.", enemy->GetName().c_str());
			continue;
		}

		bool vivo = enemyNode.attribute("Alive").as_bool();
		float x = enemyNode.attribute("x").as_float(-1.0f); // Valor por defecto en caso de error
		float y = enemyNode.attribute("y").as_float(-1.0f);
		//Vector2D enemyPos(enemyNode.attribute("x").as_float(), enemyNode.attribute("y").as_float());

		if (x < 0 || y < 0) {
			LOG("Error: Atributos invalidos para enemigo %s.", enemy->GetName().c_str());
			continue;
		}
		Vector2D enemyPos(x, y);
		enemy->SetPosition(enemyPos);
		enemy->VIVO(vivo);

		LOG("Cargando enemigo: %s, Posicion: (%f, %f), Vivo: %s",
			enemy->GetName().c_str(),
			enemyPos.getX(),
			enemyPos.getY(),
			vivo ? "true" : "false");
	}

	for (Item* item : itemList)
	{
		if (!item) {
			LOG("Error: Puntero nulo en itemList.");
			continue;
		}
		pugi::xml_node itemNode = sceneNode.child("entities").child("items").find_child_by_attribute("name", item->GetName().c_str());

		if (!itemNode) {
			LOG("Error: Nodo para item %s no encontrado.", item->GetName().c_str());
			continue;
		}
		bool alive = itemNode.attribute("Alive").as_bool();
		float x = itemNode.attribute("x").as_float(-1.0f); // Valor por defecto en caso de error
		float y = itemNode.attribute("y").as_float(-1.0f);

		if (x < 0 || y < 0) {
			LOG("Error: Atributos invalidos para enemigo %s.", item->GetName().c_str());
			continue;
		}

		Vector2D itemPos(x, y);
		item->SetPosition(itemPos);
		item->SetAlive(alive); // Metodo para configurar si el item esta activo

		LOG("Cargando enemigo: %s, Posicion: (%f, %f), Vivo: %s",
			item->GetName().c_str(),
			itemPos.getX(),
			itemPos.getY(),
			alive ? "true" : "false");
	}
}

void Scene::SaveState()
{
	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL) {
		LOG("error loading config.xml", result.description());
		return;
	}
	/*HACER CODIGO PARA DETECTAR EN QUE ESCENA ESTAS Y SUSTITUIRLO POR scene1 ES DECIR METER UNA VARIABLE
	sceneplaying = name
	*/
	pugi::xml_node sceneNode = loadFile.child("config").child("scene1");

	//Save info to XML 

	//Player position
	sceneNode.child("entities").child("player").attribute("x").set_value(player->GetPosition().getX());
	sceneNode.child("entities").child("player").attribute("y").set_value(player->GetPosition().getY());

	//enemies
	//guardar si esta vivo o no
	for (Enemy* enemy : enemyList)
	{
		if (!enemy) {
			LOG("Error: Puntero enemigo nulo en enemyList.");
			continue;
		}
		pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies").find_child_by_attribute("name", enemy->GetName().c_str());

		if (!enemyNode) {
			LOG("Error: Nodo para enemigo %s no encontrado.", enemy->GetName().c_str());
			continue;
		}

		enemyNode.attribute("x").set_value(enemy->GetPosition().getX());
		enemyNode.attribute("y").set_value(enemy->GetPosition().getY());
		enemyNode.attribute("Alive").set_value(enemy->SetVIVO());
	}
	//Items
	//Guardar si se han cogido o estan en el mapa
	for (Item* item : itemList)
	{
		if (!item) {
			LOG("Error: Puntero nulo en itemList.");
			continue;
		}
		pugi::xml_node itemNode = sceneNode.child("entities").child("items").find_child_by_attribute("name", item->GetName().c_str());

		if (!itemNode) {
			LOG("Error: Nodo para item %s no encontrado.", item->GetName().c_str());
			continue;
		}

		itemNode.attribute("Alive").set_value(item->IsAlive());
	}


	//Saves the modifications to the XML
	loadFile.save_file("config.xml");

}

void Scene::Valoresenemigos() {
	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");
	pugi::xml_node sceneNode = loadFile.child("config").child("scene1");
	for (Enemy* enemy : enemyList)
	{
		if (!enemy) {
			LOG("Error: Puntero enemigo nulo en enemyList.");
			continue;
		}
		pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies").find_child_by_attribute("name", enemy->GetName().c_str());

		if (!enemyNode) {
			LOG("Error: Nodo para enemigo %s no encontrado.", enemy->GetName().c_str());
			continue;
		}
		float x = enemyNode.attribute("xo").as_float(-1.0f); // Valor por defecto en caso de error
		float y = enemyNode.attribute("yo").as_float(-1.0f);
		//Vector2D enemyPos(enemyNode.attribute("x").as_float(), enemyNode.attribute("y").as_float());

		if (x < 0 || y < 0) {
			LOG("Error: Atributos invalidos para enemigo %s.", enemy->GetName().c_str());
			continue;
		}
		Vector2D enemyPos(x, y);
		enemy->SetPosition(enemyPos);

	}
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	Engine::GetInstance().entityManager->CleanUp();

	if (img != nullptr) {
		SDL_DestroyTexture(img);
		img = nullptr;
	}

	if (helpMenuTexture != nullptr) {
		Engine::GetInstance().textures->UnLoad(helpMenuTexture);
		helpMenuTexture = nullptr;
	}

	if (introScreenTexture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(introScreenTexture);
		introScreenTexture = nullptr;
	}
	if (titleScreenTexture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(titleScreenTexture);
		titleScreenTexture = nullptr;
	}
	if (pauseMenuTexture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(pauseMenuTexture);
		pauseMenuTexture = nullptr;
	}
	if (dieScreenTexture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(dieScreenTexture);
		dieScreenTexture = nullptr;
	}
	if (winScreenTexture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(winScreenTexture);
		winScreenTexture = nullptr;
	}

	return true;
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	if (control == nullptr) {
		LOG("Control is nullptr");
		return false;
	}
	LOG("Press Gui Control: %d", control->id);
	//Aquí hacer un switch con el control->id para saber que boton se ha pulsado y se hace cada caso, segun botones
	switch (control->id)
	{
	case 1:

		break;

	case 2:

		break;

	case 3:

		break;
	case 4:
		LOG("Closing application by Exit Button...");
		exit(0);
		break;
	case 5:

	case 6:

	case 7:

	case 8:

		break;
	case 9:

		break;
	case 10:

		break;
	case 11:

		break;
	case 12:

		break;
	case 13:

		break;
	case 14:

		break;
	case 15:

		break;
	case 16:

		break;
	case 17:

		break;
	case 18:

		break;
	case 19:

		break;
	case 20:

		break;
	}
	return true;
}

SceneState Scene::GetCurrentState() const
{
	return currentState;
}

void Scene::SetCurrentState(SceneState state)
{
	currentState = state;
}

