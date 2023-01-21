#include "FlyingEnemy.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Scene2.h"
#include "Map2.h"


FlyingEnemy::FlyingEnemy() : Entity(EntityType::PLAYER)
{
	name.Create("FlyingEnemy");

	iddle.PushBack({0,0,48,48});
	iddle.PushBack({ 48,0,48,48 });
	iddle.PushBack({ 96,0,48,48 });
	iddle.PushBack({ 144,0,48,48 });
	iddle.speed = 0.1f;

	fly_right.PushBack({ 0,48,48,48 });
	fly_right.PushBack({ 48,48,48,48 });
	fly_right.PushBack({ 96,48,48,48 });
	fly_right.PushBack({ 144,48,48,48 });
	fly_right.speed = 0.1f;

	fly_left.PushBack({ 0,144,48,48 });
	fly_left.PushBack({ 48,144,48,48 });
	fly_left.PushBack({ 96,144,48,48 });
	fly_left.PushBack({ 144,144,48,48 });
	fly_left.speed = 0.1f;

}

FlyingEnemy::~FlyingEnemy() {

}

bool FlyingEnemy::Awake() {

	


	return true;
}

bool FlyingEnemy::Start() {

	position.x = parameters.attribute("posx").as_int();
	position.y = parameters.attribute("posy").as_int();

	texturePath = parameters.attribute("texturepath").as_string();
	audioPath = parameters.attribute("audiopath").as_string();
	dead = false;

	//initilize textures
	texture = app->tex->Load(texturePath);

	ebody = app->physics->CreateCircle(position.x + 16, position.y + 16, 15, bodyType::DYNAMIC);

	ebody->listener = this;

	ebody->ctype = ColliderType::FLYING_ENEMY;
	app->map2->enemies.Add(ebody);

	typeOfMovement = false;


	/*dieFxId = app->audio->LoadFx("Assets/Audio/Fx/die.ogg");
	jumpFxId = app->audio->LoadFx("Assets/Audio/Fx/Jump-1.ogg");
	winFxId = app->audio->LoadFx("Assets/Audio/Fx/win.ogg");*/

	deadFxId = app->audio->LoadFx(audioPath);

	return true;
}

bool FlyingEnemy::PreUpdate() 
{
	return true;
}

bool FlyingEnemy::Update()
{
	if (app->physics->getPause()) {
		return true;
	}
	if (dead == true) {
		ebody->body->SetActive(false);
		return true;
	}
	currentEnemyAnimation = &iddle;
	playerPos = app->scene2->player->position;

	if (playerPos.x - position.x > NOTCHILLDISTANCE || playerPos.x - position.x < -NOTCHILLDISTANCE) {
		walkstate = WalkState::CHILL;
	}
	else {
		walkstate = WalkState::FOLLOWINGPLAYER;
	}

	//enemy movement 
	b2Vec2 vel(0, 0);
	ebody->body->SetGravityScale(0);
	if (walkstate == WalkState::CHILL) {
		currentEnemyAnimation = &iddle;
		ebody->body->SetLinearVelocity(vel);
	}
	else if (walkstate == WalkState::FOLLOWINGPLAYER) {

		if (target.x > (int)ebody->body->GetPosition().x) {
			moveStateX = MS_RIGHT;
			vel.x = 4;
			currentEnemyAnimation = &fly_right;
		}
		else if (target.x < (int)ebody->body->GetPosition().x) {
			moveStateX = MS_LEFT;
			vel.x = -4;
			currentEnemyAnimation = &fly_left;
		}

		if (target.y > (int)ebody->body->GetPosition().y) {
			moveStateY = MS_DOWN;
			vel.y = 4;
		}
		else if (target.y < (int)ebody->body->GetPosition().y) {
			moveStateY = MS_UP;
			vel.y = -4;
		}
		if (typeOfMovement) {
			ebody->body->SetLinearVelocity(vel);
		}
		else {
			b2Vec2 vel = ebody->body->GetLinearVelocity();
			float desiredVelx = 0;
			float desiredVely = 0;
			switch (moveStateX)
			{
			case MS_LEFT:  desiredVelx = -4; break;
			case MS_STOP:  desiredVelx = 0; break;
			case MS_RIGHT: desiredVelx = 4; break;
			}
			float velChangex = desiredVelx - vel.x;
			float impulsex = ebody->body->GetMass() * velChangex; //disregard time factor
			switch (moveStateY)
			{
			case MS_DOWN: desiredVely = 4; break;
			case MS_STOP: desiredVely = 0; break;
			case MS_UP: desiredVely = -4; break;
			}
			float velChangey = desiredVely - vel.y;
			float impulsey = ebody->body->GetMass() * velChangey;
			ebody->body->ApplyLinearImpulse(b2Vec2(impulsex, impulsey), ebody->body->GetWorldCenter(), true);
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {
		typeOfMovement = !typeOfMovement;
	} 
	currentEnemyAnimation->Update();
	return true;
}

bool FlyingEnemy::PostUpdate() 
{
	if (currentEnemyAnimation == NULL) {
		return true;
	}
	if (dead == true) {
		return true;
	}
	
	position.x = METERS_TO_PIXELS(ebody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(ebody->body->GetTransform().p.y) - 16;

	SDL_Rect rect = currentEnemyAnimation->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect);
	return true;
}

bool FlyingEnemy::CleanUp()
{
	app->tex->UnLoad(texture);
	return true;
}

void FlyingEnemy::OnCollision(PhysBody* physA, PhysBody* physB) {

	// L07 DONE 7: Detect the type of collision

	iPoint physbpos;
	physB->GetPosition(physbpos.x, physbpos.y);

	switch (physB->ctype)
	{
	case ColliderType::ITEMCOIN:
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
	case ColliderType::PLAYER:
		if (app->physics->getGodMode()) {
			break;
		}
		int playerY, enemyY;

		playerY = METERS_TO_PIXELS(physB->body->GetPosition().y);
		enemyY = METERS_TO_PIXELS(physA->body->GetPosition().y);

		if (enemyY > playerY + physB->height - 2) {
			b2Vec2 xd(app->scene2->player->GetBody()->body->GetLinearVelocity().x, 0);
			app->scene2->player->GetBody()->body->SetLinearVelocity(xd);
			float impulse = app->scene2->player->GetBody()->body->GetMass() * KILLENEMYIMPULSE;
			app->scene2->player->GetBody()->body->ApplyLinearImpulse(b2Vec2(0, impulse), app->scene2->player->GetBody()->body->GetWorldCenter(), false);
			app->scene2->player->SetJumps(2);
			dead = true;
		}
		else {
			/*app->scene2->player->playerhit = true;*/
		}
		app->audio->PlayFx(deadFxId);
		break;
	}
	
}


bool FlyingEnemy::GetDeadState() {
	return dead;
}
