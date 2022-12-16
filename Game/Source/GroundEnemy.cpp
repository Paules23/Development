#include "GroundEnemy.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Player.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Scene2.h"


GroundEnemy::GroundEnemy() : Entity(EntityType::GROUND_ENEMY)
{
	iddle.PushBack({ 0,0,48,48 });
	iddle.PushBack({ 48,0,48,48 });
	iddle.PushBack({ 96,0,48,48 });
	iddle.PushBack({ 142,0,48,48 });
	iddle.speed = 0.1f;



	name.Create("GroundEnemy");
}

GroundEnemy::~GroundEnemy() {

}

bool GroundEnemy::Awake() {

	//L02: DONE 5: Get Player parameters from XML


	return true;
}

bool GroundEnemy::Start() {
		position.x = parameters.attribute("x").as_int();
		position.y = parameters.attribute("y").as_int();
		
		texturePath = parameters.attribute("texturepath").as_string();
		dead = false;
		
		//initilize textures
		texture = app->tex->Load(texturePath);

		// L07 DONE 5: Add physics to the player - initialize physics body
		ebody = app->physics->CreateCircle(position.x + 16, position.y + 16, 15, bodyType::DYNAMIC);

		// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
		ebody->listener = this;

		// L07 DONE 7: Assign collider type
		ebody->ctype = ColliderType::GROUND_ENEMY;
		left = true;

		/*dieFxId = app->audio->LoadFx("Assets/Audio/Fx/die.ogg");
		jumpFxId = app->audio->LoadFx("Assets/Audio/Fx/Jump-1.ogg");
		winFxId = app->audio->LoadFx("Assets/Audio/Fx/win.ogg");*/

	return true;
}

bool GroundEnemy::Update()
{
	//takes player position
	playerPos = app->scene2->player->position;

	if (abs(playerPos.x - ebody->body->GetPosition().x) < NOTCHILLDISTANCE ) {
		walkstate = WalkState::FOLLOWINGPLAYER;
	}
	else {
		walkstate = WalkState::CHILL;
	}

	//enemy movement no pathfinding
	b2Vec2 vel(0, 0);
	ebody->body->SetGravityScale(1);

	if (walkstate == WalkState::CHILL) {
		if (changedir == true) {
			vel.x = -2;
			ebody->body->SetLinearVelocity(vel);
		}
		else {
			vel.x = 2;
			ebody->body->SetLinearVelocity(vel);
		}
	}

	if (dead == false) {
		if (left) {
			vel.x = 2;
			ebody->body->SetLinearVelocity(vel);
		}
		else {
			vel.x = -2;
			ebody->body->SetLinearVelocity(vel);
		}
		currentEnemyAnimation = &iddle;

		currentEnemyAnimation->Update();

		position.x = METERS_TO_PIXELS(ebody->body->GetTransform().p.x) - 16;
		position.y = METERS_TO_PIXELS(ebody->body->GetTransform().p.y) - 16;

		SDL_Rect rect = currentEnemyAnimation->GetCurrentFrame();
		app->render->DrawTexture(texture, position.x, position.y, &rect);
	}
	else {
		ebody->body->GetWorld()->DestroyBody(ebody->body);
		ebody = NULL;
		this->Disable();
	}
	









	
	return true;
}

bool GroundEnemy::CleanUp()
{

	return true;
}

void GroundEnemy::OnCollision(PhysBody* physA, PhysBody* physB) {

	// L07 DONE 7: Detect the type of collision

	iPoint physbpos;
	physB->GetPosition(physbpos.x, physbpos.y);

	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::DEATH:
		LOG("Collision DEATH");
		
		break;
	case ColliderType::WIN:
		LOG("Collision WIN");
		
		break;
	case ColliderType::GROUND:
		LOG("Collision JUMPS RESTORED");
		break;
	case ColliderType::CHANGE_DIR:
		LOG("Collision ENEMY CHANGE DIR");

		changedir = !changedir;

		break;
	case ColliderType::PLAYER:

		int playerY, enemyY;

		playerY = METERS_TO_PIXELS(physB->body->GetPosition().y);
		enemyY = METERS_TO_PIXELS(physA->body->GetPosition().y);

		if (enemyY > playerY + physB->height -2) {
			dead = true;
		}
		else {
			left = !left;
		}

		LOG("change direction");
		
		break;
	}
}


bool GroundEnemy::LoadState(pugi::xml_node& data)
{
	if (app->scene->level1) {
		position.x = parameters.attribute("x1").as_int();
		position.y = parameters.attribute("y1").as_int();
	}
	if (app->scene2->level2) {
		position.x = parameters.attribute("x2").as_int();
		position.y = parameters.attribute("y2").as_int();
	}

	b2Vec2 pos(position.x, position.y);
	ebody->body->SetTransform(PIXEL_TO_METERS(pos), 0);

	return true;
}

// L03: DONE 8: Create a method to save the state of the renderer
// using append_child and append_attribute
bool GroundEnemy::SaveState(pugi::xml_node& data)
{
	pugi::xml_node play = data.append_child("player");

	if (app->scene->level1) {
		play.append_attribute("x1") = position.x;
		play.append_attribute("y1") = position.y;
	}
	if (app->scene2->level2) {
		play.append_attribute("x2") = position.x;
		play.append_attribute("y2") = position.y;
	}

	return true;
}

//bool GroundEnemy::GetWinState() {
//	return win;
//}
//bool GroundEnemy::GetDeadState() {
//	return isdead;
//}
