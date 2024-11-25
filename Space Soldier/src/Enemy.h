#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"

struct SDL_Texture;

class Enemy : public Entity
{
public:

    Enemy();
    virtual ~Enemy();

    bool Awake();

    bool Start();

    bool Update(float dt);

    bool CleanUp();

    bool patrullando = true;
    int movimiento = 0;

    void SetParameters(pugi::xml_node parameters) {
        this->parameters = parameters;
    }

    void SetPosition(Vector2D pos);

    Vector2D GetPosition();

    void ResetPath();


public:

private:

    SDL_Texture* texture;
    const char* texturePath;
    int texW, texH;
    pugi::xml_node parameters;
    Animation* currentAnimation = nullptr;
    Animation idle, jumpR, jumpL, moveR, moveL, deathR, deathL, crouch;
    PhysBody* pbody;
    Pathfinding* pathfinding;
};