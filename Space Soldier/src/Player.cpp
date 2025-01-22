#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Item.h"
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
	vidas = parameters.attribute("vidas").as_int();
	muerto = false;
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

	// L08 TODO 5: Add physics to the player - initialize physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER; 

	
	saveFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Save.wav");
	loadFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Load.wav"); 
	deathFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Death.wav");
	jumpFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Jump.wav");
	//walkFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Walking.wav");
	return true;
}

bool Player::Update(float dt)
{
	if (Engine::GetInstance().scene->GetCurrentState() != SceneState::GAMEPLAY)
	{
		return true;
	}
	// L08 TODO 5: Add physics to the player - updated player position using physics
	b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		
		death = !death;
		muerto = !muerto;
		if (muerto == true) {
			Engine::GetInstance().audio.get()->PlayFx(deathFxId);
		}
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

	if (death == false)
	{
		currentAnimation = &idle;

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
			Engine::GetInstance().audio.get()->PlayFx(jumpFxId);
			if (!GodMode) isJumping = true;  // En modo normal, se activa el flag de salto
			
		}

		// If the player is jumpling, we don't want to apply gravity, we use the current velocity prduced by the jump
		if (isJumping == true)
		{
			velocity.y = pbody->body->GetLinearVelocity().y;
			// Compruebo si se esta moviendo hacia la izquierda mientras saltas
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
		}
		else
		{
			currentAnimation = &deathR;
		}
	}

	// Apply the velocity to the player
	pbody->body->SetLinearVelocity(velocity);

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	if (position.getY() > 800) {
		// Reinicio posicion del player cuando cae mas de 900px
		//Volver al inicio
		Engine::GetInstance().scene.get()->LoadState();
		Engine::GetInstance().audio.get()->PlayFx(loadFxId);
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
		// Resetea isJumping solo si no esta en GodMode
		if (!GodMode) isJumping = false;
		//Engine::GetInstance().audio.get()->PlayFx(walkFxId);
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");

		break;
	case ColliderType::CHECKPOINT:
		LOG("Collision CHECKPOINT");
		Engine::GetInstance().scene.get()->SaveState();
		Engine::GetInstance().audio.get()->PlayFx(saveFxId);
		break;
<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
=======
>>>>>>> Stashed changes
	case ColliderType::WIN:
		LOG("Collision WIN SENSOR");
		// Comprobar si se ha derrotado también al boss final
		//if (true)
		//{
			// Cambiar a estado de pantalla de victoria
			Engine::GetInstance().scene.get()->SetCurrentState(SceneState::END_SCREEN);
		//}
		break;
<<<<<<< Updated upstream
=======
	case ColliderType::TEPE:
		LOG("Collision TP");
		// Tp al segundo nivel
		Engine::GetInstance().scene.get()->GotoStartLevel2();
		break;
>>>>>>> Stashed changes
>>>>>>> Stashed changes
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
	case ColliderType::CHECKPOINT:
		LOG("End Collision CHECKPOINT");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::Die() {

	vidas--;
	//actualizar interfaz de vidas

	if(vidas == 0){
		LOG("Player dies");
		death = true;
		currentAnimation = (position.getX() < 0) ? &deathL : &deathR; // Ajustar direccion
		Engine::GetInstance().scene->SetCurrentState(SceneState::DIE_SCREEN);

		// Logica adicional para reiniciar o mostrar pantalla de fin de juego
	}
}

void Player::recVidas() {
	if (vidas > 3) {
		vidas++;
	}
	//actualizar interfaz de vidas
}


void Player::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Player::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}