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
	muerto = true;
	Estavivo = true;

	name = parameters.attribute("name").as_string();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	jumpR.LoadAnimations(parameters.child("animations").child("jumpR"));
	jumpL.LoadAnimations(parameters.child("animations").child("jumpL"));
	moveR.LoadAnimations(parameters.child("animations").child("moveR"));
	moveL.LoadAnimations(parameters.child("animations").child("moveL"));
	deathR.LoadAnimations(parameters.child("animations").child("deathR"));
	deathL.LoadAnimations(parameters.child("animations").child("deathL"));
	crouch.LoadAnimations(parameters.child("animations").child("crouch"));
	awake.LoadAnimations(parameters.child("animations").child("awake"));
	cargarR.LoadAnimations(parameters.child("animations").child("cargarR"));
	cargarL.LoadAnimations(parameters.child("animations").child("cargarL"));
	damageR.LoadAnimations(parameters.child("animations").child("damageR"));
	damageL.LoadAnimations(parameters.child("animations").child("damageL"));
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
	else if (typeStr == "boss") {
		type = EnemyType::BOSS;
	}

	deathFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Death.wav");
	killMonsterFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/KillMonster.wav");

	return true;
}

bool Enemy::Update(float dt)
{
	if (Engine::GetInstance().scene->GetCurrentState() != SceneState::GAMEPLAY)
	{
		return true;
	}

	if (Estavivo == true) { isDead = false; }
	else { isDead = true; }
	if (isDead) {
		currentAnimation->Update();
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());

		// Si esta marcado para eliminacion, elimina el cuerpo fisico
		if (pendingToDelete && pbody != nullptr) {
			Engine::GetInstance().physics->DestroyBody(pbody->body);
			pbody = nullptr;
		}
		return true; // Salir temprano si el enemigo esta muerto
	}

	b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);
	// Pathfinding testing inputs
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
		Vector2D pos = GetPosition();
		Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
		pathfinding->ResetPath(tilePos);
	}

	Vector2D currentPos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(currentPos.getX(), currentPos.getY());

	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
	enemyTilePos = Engine::GetInstance().map->WorldToMap(GetPosition().getX(), GetPosition().getY());//sacar posicion enemigo
	Vector2D playerTilePos = Engine::GetInstance().map->WorldToMap(playerPos.getX(), playerPos.getY());//sacar posicion player

	int distance = abs(enemyTilePos.getX() - playerTilePos.getX()) + abs(enemyTilePos.getY() - playerTilePos.getY()); //calcular distancia entre enemigo y player

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
		//encontrado = !encontrado;
	}
		while (pathfinding->pathTiles.empty()) {
			pathfinding->PropagateAStar(SQUARED);
		}

		

		if (type == EnemyType::VOLADOR) {

			if (distance <= 6) {//una vez encontrado el player 
				patrullando = true;
			}
			else { patrullando = false; }

			if (!patrullando && Giro) {//Subir y bajar duranmte el modo patrullar
				Giro = false;
				if (movimiento == 2) {//si se mete en un switch se vuelve loco
					movimiento = 1;
				}
				else {
					movimiento = 2;
				}
			}
			if (!patrullando && Lado) { //Girar de derecha a izquierda en el modo patrullar
				if (lados == 2) {//si se mete en un switch se vuelve loco
					lados = 1;
					Lado = false;
				}
				else {
					lados = 2;
					Lado = false;
				}
			}
			if (patrullando){
					if (enemyTilePos.getY() < playerTilePos.getY()) {//mirar si la posicion del player es mas alta que la del enemigo
						velocity.y = 2;  // Abajo
						currentAnimation = &moveR;  // Cambiar si tienes animaciones especificas
					}
					else if (enemyTilePos.getY() > playerTilePos.getY()) {//mirar si la posicion del player es mas baja que la del enemigo
						velocity.y = -2;  // Arriba
						currentAnimation = &moveL;  // Cambiar si tienes animaciones especificas
					}
					if (enemyTilePos.getX() < playerTilePos.getX()) {//mirar si la posicion del player es mas alta que la del enemigo
						velocity.x = 0.2 * 5;  // derecha
						currentAnimation = &moveR;  // Cambiar si tienes animaciones especificas
					}
					else if (enemyTilePos.getX() > playerTilePos.getX()) {//mirar si la posicion del player es mas baja que la del enemigo
						velocity.x = -0.2 * 5; // Izquierda
						currentAnimation = &moveL;  // Cambiar si tienes animaciones especificas
					}
			}else{
				switch (movimiento)//movimiento enemigo volador en modo patrullar
				{
					case 1://arriba
						velocity.y = -2;
						currentAnimation = &moveR;
					break;
					case 2://abajo
						velocity.y = 2;
						currentAnimation = &moveL;
					break;
					default:
					break;
				}
				switch (lados)
				{
					case 1://Derecha
						velocity.x = 0.2 * 5;
						currentAnimation = &moveR;
					break;
					case 2://Izquierda
						velocity.x = -0.2 * 5;
						currentAnimation = &moveL;
					break;
					default:
					break;
				}
		}
}
	if (type == EnemyType::TERRESTRE) {

		if (distance <= 4) {//una vez encontrado el player 
			patrullando = true;
		}
		else { patrullando = false; }

		if (position.getY() > 650) {
			Engine::GetInstance().scene.get()->Valoresenemigos();
		}
	if (!patrullando && Giro) {
		Giro = false;
		if (movimiento == 2) {//si se mete en un switch se vuelve loco
			movimiento = 1;
		}
		else {
			movimiento = 2;
		}	
	} 

	if (patrullando) {
		if (suelo) {
			if (enemyTilePos.getX() < playerTilePos.getX()) {
				velocity.x = 0.2 * 5;  // Derecha
				if (jump) {//saltar cuando el terreno se eleve
					velocity.y = -4;
				}
				currentAnimation = &moveR;
			}
			else if (enemyTilePos.getX() > playerTilePos.getX()) {
				velocity.x = -0.2 * 5;  // Izquierda
				if (jump) {//saltar cuando el terreno se eleve
					velocity.y = -4;
				}
				currentAnimation = &moveL;
			}
		}
	}else{
		switch (movimiento)
		{
		case 1:
			if (jump) {//saltar cuando el terreno se eleve
				velocity.y = -4;
				velocity.x = 15;
				currentAnimation = &moveR;
			}
			velocity.x = 0.2 * 5;
			currentAnimation = &moveR;
			break;
			
		case 2:
			if (jump) {//saltar cuando el terreno se eleve
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
	if (type == EnemyType::BOSS) {//si se descomenta al cerrar el juego no deja volverlo a abrir, peta por todos lados
		/*if (vidasB == 0) { Estavivo = false; isDead = true; currentAnimation = &deathR; }
		if (distance <= 10) { patrullando = true; }else { patrullando = false; }
		if(patrullando){
			if (distance <= 5) {
				if (enemyTilePos.getX() < playerTilePos.getX()) {
					currentAnimation = &cargarR;
					//crear bala
					// llamar a la funcion que inicializa la entity bala aņadiendole velocidad hacia la derecha y cuerpo dinamico, si colisiona con le jugador llama a la funcion del jugador que le baja la vida
				}
				else if (enemyTilePos.getX() > playerTilePos.getX()) {
					currentAnimation = &cargarL;
					//temporizador drante la animacion antes del disparo de la bala
					//crear bala
					//llamar a la funcion que inicializa la entity bala aņadiendole velocidad hacia la izquierda y cuerpo dinamico, si colisiona con le jugador llama a la funcion del jugador que le baja la vida
				}
			}else{
				if (enemyTilePos.getX() < playerTilePos.getX()) {//perseguir al player
					velocity.x = 0.2 * 5;  // Derecha
					currentAnimation = &moveR;
				}
				else if (enemyTilePos.getX() > playerTilePos.getX()) {
					velocity.x = -0.2 * 5;  // Izquierda

					currentAnimation = &moveL;
				}
			}
		}*/
		
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

	if (distance <= 200)
	{
		pathfinding->ResetPath(tilePos);
	}
	return true;
}
void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	if (isDead) return;
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		suelo = true;
		break;
	case ColliderType::PLAYER: {
		Vector2D playerPos = ((Player*)physB->listener)->GetPosition();

		if (playerPos.getY() < position.getY()) {
			

			if(type == EnemyType::TERRESTRE){ Estavivo = false; isDead = true; currentAnimation = &deathR; }
			if (type == EnemyType::VOLADOR) { Estavivo = false; isDead = true; currentAnimation = &deathR; }
			if (type == EnemyType::BOSS) { vidasB--;/* quitarle una de las vidas al boss final*/ }
			
			Engine::GetInstance().audio.get()->PlayFx(killMonsterFxId);
			// Marca para eliminar el cuerpo fisico
			//pendingToDelete = true;
		}
		else{
			((Player*)physB->listener)->Die();
			if (muerto == true) {
				Engine::GetInstance().audio.get()->PlayFx(deathFxId);
				muerto = !muerto;
			}
		}
		break;
	}
	case ColliderType::JUMP:
		jump = true;
		break;
	case ColliderType::FIN:
		Giro = true;
		break;
	case ColliderType::LADOS:
		Lado = true;
		break;
	case ColliderType::UNKNOWN:
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
		suelo = false;
		break;
	case ColliderType::JUMP:
		jump = false;
		break;
	case ColliderType::PLAYER:
		break;
	case ColliderType::UNKNOWN:
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

void Enemy::VIVO(bool v) { //llega desde scene si el enemigo esta vivo o no, para mantener los muertos muertos despues de cargar y revivir los que murieron despues del guardado
	Estavivo = v;
}
bool Enemy::SetVIVO() { //Le manda al scene si el enemigo esta vivo o no
	
	return Estavivo;
}

void Enemy::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}