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

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	showPath = true;
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
	patrullando = false;
	Tocado = false;
	jump = false;

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

	return true;
}

bool Enemy::Update(float dt)
{

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

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_B) == KEY_DOWN) {
		pathfinding->PropagateAStar(MANHATTAN);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_B) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateAStar(MANHATTAN);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_N) == KEY_DOWN) {
		pathfinding->PropagateAStar(EUCLIDEAN);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_N) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateAStar(EUCLIDEAN);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_M) == KEY_DOWN) {
		pathfinding->PropagateAStar(SQUARED);
	}

	/*if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_M) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {*/
		pathfinding->PropagateAStar(SQUARED);
	//}
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

	if (!patrullando && (int)position.getX() < 800) {
			
		movimiento = 1;
		
	}else if (!patrullando && Tocado == true && (int)position.getX() >= 900) {
			
		movimiento = 2;

	}

	if (!patrullando) {
		switch (movimiento)
		{
		case 1:
			if (Tocado == true) {
				if (jump = true) {

				}
				else {
					velocity.x = 0.2 * 5;
					currentAnimation = &moveR;
				}
				
			}
			break;
			
		case 2:
			if (Tocado == true) {
				if (jump = true) {
					velocity.x = -0.2 * 5;
					currentAnimation = &moveL;
				}
				
				
			}
			break;
		default:
			break;
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
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Enemy Collision PLATFORM");
		Tocado = true;
		break;
	case ColliderType::PLAYER:
		LOG("Enemy Collision Player");
		break;
	/*case ColliderType::JUMP:
		LOG("Enemy Collision JUMP");
		jump = true;
		break;*/
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
		Tocado = false;
		break;
	case ColliderType::JUMP:
		LOG("End Enemy Collision JUMP");
		jump = false;
		break;
	/*case ColliderType::PLAYER:
		LOG("End Enemy Collision Player");
		break;*/
	case ColliderType::ITEM:
		LOG("End Enemy Collision PLAYER");
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