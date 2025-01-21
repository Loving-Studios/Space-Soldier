#include "Item.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"

Item::Item() : Entity(EntityType::ITEM), isPicked(false)
{
	name = "item";
}

Item::~Item() {}

bool Item::Awake() {
	return true;
}

bool Item::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();
	name = parameters.attribute("name").as_string();
	alive = parameters.attribute("Alive").as_bool();
	gravedad = parameters.attribute("gravity").as_bool();

	pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Faro.wav");

		// L08 TODO 4: Add a physics to an item - initialize the physics body
		Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
		if (gravedad == true) {
			pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2 - 500, texH / 2, bodyType::DYNAMIC);
			pbody->listener = this;
		}
		else {
			pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2 - 500, texH / 2, bodyType::STATIC);
			pbody->listener = this;
		}


	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::ITEM;

	std::string typeStr = parameters.attribute("name").as_string();

	if (typeStr == "Coin") {
		type = ItemType::MONEDA;
	}
	else if (typeStr == "botiquin") {
		type = ItemType::CURA;
	}
	else if (typeStr == "bala") {
		type = ItemType::BALA;
	}

	return true;
}

bool Item::Update(float dt)
{
	if (Engine::GetInstance().scene->GetCurrentState() != SceneState::GAMEPLAY)
	{
		return true;
	}
	// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.

	if (!alive) {
		// Si esta marcado para eliminacion, elimina el cuerpo fisico

		if (pendingToDelete) {
			if (pbody != nullptr) {
				Engine::GetInstance().physics->DestroyBody(pbody->body);
				pbody = nullptr;
			}
			return false; // Detener la ejecucion del item
		}
		return true; // Salir temprano si el enemigo esta muerto
	}


	if (!alive) return false;

	b2Transform pbodyPos = pbody->body->GetTransform();

	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	if (alive == true) {
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY());
	}

	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Item Collision Player");
		if (name == "Coin") {
			LOG("---------------------MONEDA-----------------------------------");
			Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
			alive = false;
			pendingToDelete = true;

		}
		else if (name == "botiquin") {

			alive = false;
			pendingToDelete = true;
		}
		else if (name == "bala") {

			alive = false;
			pendingToDelete = true;
		}
		break;
	case ColliderType::UNKNOWN:
		LOG("End Enemy Collision UNKNOWN");
		break;
	default:
		break;
	}
}

bool Item::IsAlive() { return alive; } //Le manda al scene si el el item ha sido cogido o no

void Item::SetAlive(bool isAlive) { alive = isAlive; } //llega desde scene si el item ha sido cogido o no

Vector2D Item::GetPosition()
{
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	return Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
}

void Item::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

bool Item::CleanUp()
{
	if (pbody != nullptr) {
		Engine::GetInstance().physics->DestroyBody(pbody->body);
		pbody = nullptr;
	}

	if (texture != nullptr) {
		Engine::GetInstance().textures->UnLoad(texture);
		texture = nullptr;
	}

	LOG("Item cleaned up: %s", name.c_str());
	return true;
}