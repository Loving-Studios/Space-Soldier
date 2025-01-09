#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"

struct SDL_Texture;

enum class EnemyType { TERRESTRE, VOLADOR, BOSS };

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

    bool patrullando = true, suelo = false, jump = false, Lado = false;
    int movimiento = 0, lados = 0;

    void SetParameters(pugi::xml_node parameters) {
        this->parameters = parameters;
    }

    void SetPosition(Vector2D pos);

    Vector2D GetPosition();
    Vector2D enemyTilePos;
    
    void VIVO (bool v);
    bool SetVIVO();
    std::string GetName() const { return name; };

    void ResetPath();

    void OnCollision(PhysBody* physA, PhysBody* physB);


    void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

    int killMonsterFxId;
    int deathFxId;

    bool muerto;

public:

private:

    bool isDead = false, Estavivo;
    bool pendingToDelete = false;
    bool showPath, Giro;
    SDL_Texture* texture;
    std::string name;
    const char* texturePath;
    int texW, texH, ultimaTile;
    pugi::xml_node parameters;
    Animation* currentAnimation = nullptr;
    Animation idle, jumpR, jumpL, moveR, moveL, deathR, deathL, crouch, awake, cargarR, cargarL, damageR, damageL;
    PhysBody* pbody;
    Pathfinding* pathfinding;
};