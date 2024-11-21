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
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());


	//Musica de fondo
	Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Fx/CelestialDrift.ogg", 0);
	// Configura el volumen al 5%
	Mix_VolumeMusic(static_cast<int>(128 * 0.05));

	helpMenuTexture = Engine::GetInstance().textures->Load("Assets/Textures/HelpMenu.png"); // Cambia a la ruta correcta.


	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

bool Scene::SaveState()
{
	//Coger posicion player
	float playerPosX = player->position.getX();
	float playerPosY = player->position.getY();
	GuardarPosicion = Vector2D(playerPosX, playerPosY);
	//guardarla en el xml

	return true;
}

bool Scene::LoadState()
{
	//coger posicion del xml

	//darsela al player
	//L04: TODO 3b: Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->SetParameters(configParameters.child("entities").child("player"));

	return true;
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
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		name = "scene1";
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		name = "scene2";
	}

	/*
	* MOVER CAMARA MANUALMENTE
	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.y -= ceil(camSpeed * dt);

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.y += ceil(camSpeed * dt);

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.x -= ceil(camSpeed * dt);

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.x += ceil(camSpeed * dt);
		*/
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		SaveState();
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		LoadState();
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
