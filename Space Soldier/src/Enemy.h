#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"

struct SDL_Texture;

enum class EnemyType { TERRESTRE, VOLADOR };

class Enemy : public Entity
{
public:

    Enemy();
    virtual ~Enemy();
    EnemyType type;

    bool Awake();

    bool Start();

    bool Update(float dt);

    bool CleanUp();

    bool patrullando = true, encontrado = false, jump = false, Lado = false;
    int movimiento = 0, lados = 0;

    void SetParameters(pugi::xml_node parameters) {
        this->parameters = parameters;
    }

    void SetPosition(Vector2D pos);

    Vector2D GetPosition();

    void ResetPath();

    void OnCollision(PhysBody* physA, PhysBody* physB);

    void OnCollisionEnd(PhysBody* physA, PhysBody* physB);


public:

private:

    bool isDead = false;
    bool pendingToDelete = false;
    bool showPath, Giro;
    SDL_Texture* texture;
    const char* texturePath;
    int texW, texH, ultimaTile;
    pugi::xml_node parameters;
    Animation* currentAnimation = nullptr;
    Animation idle, jumpR, jumpL, moveR, moveL, deathR, deathL, crouch;
    PhysBody* pbody;
    Pathfinding* pathfinding;
};