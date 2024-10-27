#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
	GodMode = false;
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: TODO 2: Initialize Player parameters
	position = Vector2D(96, 96);
	return true;
}

bool Player::Start() {

	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

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

	/*
	switch (position)
	{
	case:

	default:
		break;
	}
	*/

	// L08 TODO 5: Add physics to the player - initialize physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect
	//pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");
	pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Faro.wav");
	return true;
}

bool Player::Update(float dt)
{
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		death = !death;
	}
	// Cambia el estado de GodMode al presionar F10
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		GodMode = !GodMode;

		// Si se activa GodMode, permite saltos infinitos reseteando isJumping
		if (GodMode)
		{
			isJumping = true;
		}
	}

	// L08 TODO 5: Add physics to the player - updated player position using physics
	b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);
	if (death == false)
	{
		currentAnimation = &idle;
		//momento = 0;

		// Move left
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			velocity.x = -0.2 * 16;
			if (!isJumping) {
				currentAnimation = &moveL;
			}
		}

		// Move right
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			velocity.x = 0.2 * 16;
			if (!isJumping) {
				currentAnimation = &moveR;
			}
		}

		//Jump
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && (GodMode || !isJumping)) {
			// Apply an initial upward force
			pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
			if (!GodMode) isJumping = true;  // En modo normal, se activa el flag de salto
		}

		// If the player is jumpling, we don't want to apply gravity, we use the current velocity prduced by the jump
		if (isJumping == true)
		{
			velocity.y = pbody->body->GetLinearVelocity().y;

			// Compruebo si se est� moviendo hacia la izquierda mientras saltas
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				currentAnimation = &jumpL;
			}
			else {
				currentAnimation = &jumpR;
			}
		}
	}
	else {
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			currentAnimation = &deathL;
			//momento += 1;
		}
		else
		{
			currentAnimation = &deathR;
			//momento += 1;
		}
	}

	// Apply the velocity to the player
	pbody->body->SetLinearVelocity(velocity);

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	if (position.getY() > 800) {
		// Reinicio posici�n del player cuando cae m�s de 800px
		pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(490), PIXEL_TO_METERS(450)), 0);
	}

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();
	return true;

}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		// Resetea isJumping solo si no est� en GodMode
		if (!GodMode) isJumping = false;
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	default:
		break;
	}
}