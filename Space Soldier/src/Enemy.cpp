#include "Enemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"

Enemy::Enemy() : Entity(EntityType::ENEMY), isDead(false)
{
	showPath = false;
}

Enemy::~Enemy() {
	delete pathfinding;
}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();
	movimiento = 2;
	lados = 2;
	patrullando = false;
	jump = false;
	Giro = false;
	Lado = false;
	encontrado = false;
	muerto = true;

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	jumpR.LoadAnimations(parameters.child("animations").child("jumpR"));
	jumpL.LoadAnimations(parameters.child("animations").child("jumpL"));
	moveR.LoadAnimations(parameters.child("animations").child("moveR"));
	moveL.LoadAnimations(parameters.child("animations").child("moveL"));
	deathR.LoadAnimations(parameters.child("animations").child("deathR"));
	deathL.LoadAnimations(parameters.child("animations").child("deathL"));
	crouch.LoadAnimations(parameters.child("animations").child("crouch"));
	currentAnimation = &idle;

	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

	pbody->listener = this;

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	std::string typeStr = parameters.attribute("type").as_string();

	if (typeStr == "terrestre") {
		type = EnemyType::TERRESTRE;
	}
	else if (typeStr == "volador") {
		type = EnemyType::VOLADOR;
	}

	deathFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Death.wav");
	killMonsterFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/KillMonster.wav");


	

	return true;
}

bool Enemy::Update(float dt)
{
	if (isDead) {
		currentAnimation->Update();
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());

		// Si está marcado para eliminación, elimina el cuerpo físico
		if (pendingToDelete && pbody != nullptr) {
			Engine::GetInstance().physics->DestroyBody(pbody->body);
			pbody = nullptr;
		}
		return true; // Salir temprano si el enemigo está muerto
	}

	b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);
	// Pathfinding testing inputs
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
		Vector2D pos = GetPosition();
		Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
		pathfinding->ResetPath(tilePos);
	}

	// L13: TODO 3:	Add the key inputs to propagate the A* algorithm with different heuristics (Manhattan, Euclidean, Squared) 

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
		encontrado = !encontrado;
	}

		pathfinding->PropagateAStar(SQUARED);

		if(encontrado == true) {//una vez encontrado el player 
			patrullando = true;
			if (position.getX() < ultimaTile){//comparar si la posicion del enemigo es más pequeña que la proxima posicion, moverse hacia esa posicion
				velocity.x = 0.2 * 8;
				currentAnimation = &moveR;
				ResetPath();
			}
			else if (position.getX() > ultimaTile) {//comparar si la posicion del enemigo es más grande que la proxima posicion, moverse hacia esa posicion
				velocity.x = -0.2 * 8;
				currentAnimation = &moveL;
				ResetPath();
			}
		}
if (type == EnemyType::VOLADOR) {
	if (!patrullando && Giro) {
		if (movimiento == 2) {//si se mete en un switch se vuelve loco
			movimiento = 1;
			Giro = false;
		}
		else {
			movimiento = 2;
			Giro = false;
		}
	}
	if (!patrullando && Lado) {
		if (lados == 2) {//si se mete en un switch se vuelve loco
			lados = 1;
			Lado = false;
		}
		else {
			lados = 2;
			Lado = false;
		}
	}
	if (!patrullando) {
		switch (movimiento)
		{
		case 1:
			velocity.y = -2;
			currentAnimation = &moveR;
			break;
		case 2:
			velocity.y = 2;
			currentAnimation = &moveL;
			break;
		default:
			break;
		}
		switch (lados)
		{
		case 1:
			velocity.x = 0.2 * 5;
			currentAnimation = &moveR;
			break;
		case 2:
			velocity.x = -0.2 * 3;
			currentAnimation = &moveL;
			break;
		default:
			break;
		}
	}
}
	if (type == EnemyType::TERRESTRE) {
	if (!patrullando && Giro) {
		if (movimiento == 2) {//si se mete en un switch se vuelve loco
			movimiento = 1;
			Giro = false;
		}
		else {
			movimiento = 2;
			Giro = false;
		}	
	} 

	if (!patrullando) {
		switch (movimiento)
		{
		case 1:
			if (jump == true) {
				velocity.y = -4;
				velocity.x = 15;
				currentAnimation = &moveR;
			}
			velocity.x = 0.2 * 5;
			currentAnimation = &moveR;
			break;
			
		case 2:
			if (jump == true) {
				velocity.y = -4;
				velocity.x = -1 * 15;
				currentAnimation = &moveL;
			}
			velocity.x = -0.2 * 5;
			currentAnimation = &moveL;
			break;
		default:
			break;
		}
	}
}
	pbody->body->SetLinearVelocity(velocity);

	// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.  
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		showPath = !showPath;


	if (showPath)
	{
		// Draw pathfinding 
		pathfinding->DrawPath();
	}
	return true;
}
void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	if (isDead) return;
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Enemy Collision PLATFORM");
		break;
	case ColliderType::PLAYER: {
		Vector2D playerPos = ((Player*)physB->listener)->GetPosition();

		if (playerPos.getY() < position.getY()) {
			LOG("Enemy killed by Player from above");
			isDead = true;
			currentAnimation = &deathR; // Cambia a deathL si corresponde
			Engine::GetInstance().audio.get()->PlayFx(killMonsterFxId);

			// Marca para eliminar el cuerpo físico
			pendingToDelete = true;
		}
		else {
			LOG("Player killed by Enemy");
			((Player*)physB->listener)->Die();
			if (muerto == true) {
				Engine::GetInstance().audio.get()->PlayFx(deathFxId);
				muerto = !muerto;
			}
		}
		break;
	}
	case ColliderType::JUMP:
		LOG("Enemy Collision JUMP");
		jump = true;
		break;
	case ColliderType::FIN:
		LOG("Enemy Collision Giro");
		Giro = true;
		break;
	case ColliderType::LADOS:
		LOG("Enemy volador Giro");
		Lado = true;
		break;
	case ColliderType::UNKNOWN:
		LOG("Enemy Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Enemy Collision PLATFORM");
		break;
	case ColliderType::JUMP:
		LOG("End Enemy Collision JUMP");
		jump = false;
		break;
	case ColliderType::PLAYER:
		LOG("End Enemy Collision Player");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Enemy Collision UNKNOWN");
		break;
	default:
		break;
	}
}

bool Enemy::CleanUp()
{
	return true;
}

void Enemy::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Enemy::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}