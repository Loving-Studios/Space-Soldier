#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	void Die();
	void recVidas();

	Vector2D GetPosition();

public:
	int GetVidas() const { return vidas; }


public:

	//Declare player parameters
	float speed = 5.0f;
	SDL_Texture* texture = NULL;
	int texW, texH, vidas;

	//Audio fx
	int saveFxId;
	int loadFxId;
	int deathFxId;
	int jumpFxId;
	int walkFxId;
	bool muerto;
	bool pendingtomakeTEPE = false;


	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	float jumpForce = 5.0f; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the player is currently jumping
	bool death = false;
	bool GodMode;

	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle, jumpR, jumpL, moveR, moveL, deathR, deathL, crouch;
};