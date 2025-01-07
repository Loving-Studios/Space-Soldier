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
	/*Cargar items como cargar enemigos Falta escribirlo bien
	
	for (pugi::xml_node itemNode = configParameters.child("entities").child("items").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
		item->SetParameters(itemNode);
		itemList.push_back(item);
	}
	*/
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
			LOG("Error: Atributos inválidos para enemigo %s.", enemy->GetName().c_str());
			continue;
		}
		Vector2D enemyPos(x, y);
		enemy->SetPosition(enemyPos);
		enemy->VIVO(vivo);

		LOG("Cargando enemigo: %s, Posición: (%f, %f), Vivo: %s",
			enemy->GetName().c_str(),
			enemyPos.getX(),
			enemyPos.getY(),
			vivo ? "true" : "false");
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
	

	//Saves the modifications to the XML
	loadFile.save_file("config.xml");

}
void Scene::Valoresenemigos(){
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
			LOG("Error: Atributos inválidos para enemigo %s.", enemy->GetName().c_str());
			continue;
		}
		Vector2D enemyPos(x, y);
		enemy->SetPosition(enemyPos);

	}
}

void Scene::GotoStart()
{
	//Player position
	Vector2D playerPos = Vector2D(490,450);
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
			LOG("Error: Atributos inválidos para enemigo %s.", enemy->GetName().c_str());
			continue;
		}
		Vector2D enemyPos(x, y);
		enemy->SetPosition(enemyPos);
	
	}
	
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
