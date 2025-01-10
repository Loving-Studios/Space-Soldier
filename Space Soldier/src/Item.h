#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"

struct SDL_Texture;
enum class ItemType { CURA, MONEDA, BALA };

class Item : public Entity
{
public:

	Item();
	virtual ~Item();
	ItemType type;

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();
	std::string GetName() const { return name; }

	bool IsAlive();

	void SetAlive(bool isAlive);

public:

	bool isPicked = false, alive;

private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	std::string name;
	//L08 TODO 4: Add a physics to an item
	PhysBody* pbody;
	pugi::xml_node parameters;
};
