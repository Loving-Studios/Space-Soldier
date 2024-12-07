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


Scene::Scene() : Module()
{
	name = "scene1";
	img = nullptr;
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

	for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	{
		Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
		enemy->SetParameters(enemyNode);
		enemyList.push_back(enemy);
	}

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());

	//Musica de fondo
	Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Fx/CelestialDrift.ogg", 0);
	saveFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Save.wav");
	loadFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Load.wav");
	PopOut = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/PopOut.wav");
	// Configura el volumen al 5%
	Mix_VolumeMusic(static_cast<int>(128 * 0.05));

	helpMenuTexture = Engine::GetInstance().textures->Load("Assets/Textures/HelpMenu.png"); // Cambia a la ruta correcta.


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
	// ...
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
	// ...

	//Saves the modifications to the XML
	loadFile.save_file("config.xml");

}

void Scene::GotoStart()
{
	//Player position
	Vector2D playerPos = Vector2D(490,450);
	player->SetPosition(playerPos);
}
// Called each loop iteration
bool Scene::Update(float dt)
{
	//L03 TODO 3: Make the camera movement independent of framerate
	float camSpeed = 1;
	float casimitad = 400.0f;

	// Obtener la posición del jugador
	float playerPosX = player->position.getX();

	// Calcular la nueva posición de la cámara
	Engine::GetInstance().render.get()->camera.x = playerPosX * -1.0f + casimitad;

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN) {
		helpMenuVisible = !helpMenuVisible;
		Engine::GetInstance().audio.get()->PlayFx(PopOut);
	}
	return true;
}

Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
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
		//Proximamente aquí hará que te mandé al inicio del primer nivel
		GotoStart();
		Engine::GetInstance().audio.get()->PlayFx(loadFxId);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		//Proximamente aquí hará que te mandé al inicio del segundo nivel
		GotoStart();
		Engine::GetInstance().audio.get()->PlayFx(loadFxId);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		GotoStart();
		Engine::GetInstance().audio.get()->PlayFx(loadFxId);
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

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	SDL_DestroyTexture(img);

	if (helpMenuTexture != nullptr) {
		Engine::GetInstance().textures->UnLoad(helpMenuTexture);
		helpMenuTexture = nullptr;
	}


	return true;
}
