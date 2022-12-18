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
#include "Map2.h"


GroundEnemy::GroundEnemy() : Entity(EntityType::GROUND_ENEMY)
{
	name.Create("GroundEnemy");

	iddle.PushBack({ 1103,0,48,48 });
	iddle.PushBack({ 1055,0,48,48 });
	iddle.PushBack({ 1007,0,48,48 });
	iddle.PushBack({ 959,0,48,48 });
	iddle.speed = 0.1f;

	walk_right.PushBack({ 192,48,48,48 });
	walk_right.PushBack({ 240,48,48,48 });
	walk_right.PushBack({ 288,48,48,48 });
	walk_right.PushBack({ 336,48,48,48 });
	walk_right.PushBack({ 384,48,48,48 });
	walk_right.PushBack({ 432,48,48,48 });
	walk_right.PushBack({ 480,48,48,48 });
	walk_right.speed = 0.1f;

	walk_left.PushBack({ 912,0,48,48 });
	walk_left.PushBack({ 864,0,48,48 });
	walk_left.PushBack({ 816,0,48,48 });
	walk_left.PushBack({ 768,0,48,48 });
	walk_left.PushBack({ 720,0,48,48 });
	walk_left.PushBack({ 672,0,48,48 });
	walk_left.PushBack({ 624,0,48,48 });
	walk_left.speed = 0.1f;

	run_left.PushBack({ 0,0,48,48 });
	run_left.PushBack({ 48,0,48,48 });
	run_left.PushBack({ 96,0,48,48 });
	run_left.PushBack({ 144,0,48,48 });
	run_left.PushBack({ 192,0,48,48 });
	run_left.PushBack({ 240,0,48,48 });
	run_left.PushBack({ 288,0,48,48 }); 
	run_left.speed = 0.1f;

	run_right.PushBack({ 816,48,48,48 });
	run_right.PushBack({ 864,48,48,48 });
	run_right.PushBack({ 912,48,48,48 });
	run_right.PushBack({ 960,48,48,48 });
	run_right.PushBack({ 1008,48,48,48 });
	run_right.PushBack({ 1058,48,48,48 });
	run_right.PushBack({ 1106,48,48,48 });
	run_right.speed = 0.1;
	
	
}

GroundEnemy::~GroundEnemy() {

}

bool GroundEnemy::Awake() {

	return true;
}

bool GroundEnemy::Start() {
	position.x = parameters.attribute("posx").as_int();
	position.y = parameters.attribute("posy").as_int();

	texturePath = parameters.attribute("texturepath").as_string();
		
		dead = false;
		
		//initilize textures
		texture = app->tex->Load(texturePath);

		ebody = app->physics->CreateCircle(position.x + 16, position.y + 16, 15, bodyType::DYNAMIC);

		ebody->listener = this;

		ebody->ctype = ColliderType::GROUND_ENEMY;

		app->map2->enemies.Add(ebody);

		remainingJumps = 1;

		/*dieFxId = app->audio->LoadFx("Assets/Audio/Fx/die.ogg");
		jumpFxId = app->audio->LoadFx("Assets/Audio/Fx/Jump-1.ogg");
		winFxId = app->audio->LoadFx("Assets/Audio/Fx/win.ogg");*/

		deadFxId = app->audio->LoadFx("Assets/Audio/Fx/egDeath.ogg");

	return true;
}

bool GroundEnemy::Update()
{
	//takes player position
	playerPos = app->scene2->player->position;

	if (dead == true) {
		app->audio->PlayFx(deadFxId);
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
		app->render->camera.x = 0;
		dead = false;
	}

	if (playerPos.x -  position.x > NOTCHILLDISTANCE || playerPos.x - position.x < -NOTCHILLDISTANCE) {
		walkstate = WalkState::CHILL;
	}
	else {
		walkstate = WalkState::FOLLOWINGPLAYER;
	}

	//enemy movement 
 	b2Vec2 vel(0,-GRAVITY_Y);
	if (walkstate == WalkState::CHILL) {
		if (changedir == true) {
			vel.x = -ENEMYVELX;
			currentEnemyAnimation = &walk_left;
		}
		else {
			vel.x = ENEMYVELX;
			currentEnemyAnimation = &walk_right;
		}
		ebody->body->SetLinearVelocity(vel);
	}
	else if (walkstate == WalkState::FOLLOWINGPLAYER) {
		if (abs(target.x + PIXEL_TO_METERS(app->map2->mapData.tileWidth / 2) - ebody->body->GetPosition().x) <= PIXEL_TO_METERS(1) && jumpVel == false) {

			moveState = MS_STOP;
			currentEnemyAnimation = &iddle;
		}
		else if (target.x > (int)ebody->body->GetPosition().x) {
			moveState = MS_RIGHT;
			currentEnemyAnimation = &run_right;
		}
		else if (target.x  < (int)ebody->body->GetPosition().x) {
			moveState = MS_LEFT;
			currentEnemyAnimation = &run_left;
		}
		b2Vec2 vel = ebody->body->GetLinearVelocity();
		float desiredVelx = 0;
		float desiredVely = 0;
		switch (moveState)
		{
		case MS_LEFT:  if (jumpVel) { desiredVelx = -6; } else { desiredVelx = -4; }; break;
		case MS_STOP:  desiredVelx = 0; break;
		case MS_RIGHT: if (jumpVel) { desiredVelx = 6; }else { desiredVelx = 4; }; break;
		}
		float velChangex = desiredVelx - vel.x;
		float impulsex = ebody->body->GetMass() * velChangex; //disregard time factor
		ebody->body->ApplyLinearImpulse(b2Vec2(impulsex, 0), ebody->body->GetWorldCenter(), true);

		if (jump == true) {
			b2Vec2 xd(ebody->body->GetLinearVelocity().x, 0);
			ebody->body->SetLinearVelocity(xd);
			float impulsey = ebody->body->GetMass() * JUMPIMPULSEENEMYUP;
			ebody->body->ApplyLinearImpulse(b2Vec2(0, impulsey), ebody->body->GetWorldCenter(), false);
			jump = false;
		}
	}

	currentEnemyAnimation->Update();

	position.x = METERS_TO_PIXELS(ebody->body->GetTransform().p.x) - 24;
	position.y = METERS_TO_PIXELS(ebody->body->GetTransform().p.y) - 24;

	SDL_Rect rect = currentEnemyAnimation->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect);


	if (dead == true) {
		ebody->body->SetActive(false);
		this->Disable();
	}
	return true;
}

bool GroundEnemy::CleanUp()
{
	app->tex->UnLoad(texture);
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
		dead = true;
		
		break;
	case ColliderType::WIN:
		LOG("Collision WIN");
		
		break;
	case ColliderType::GROUND:
		LOG("Collision JUMPS RESTORED");
		remainingJumps = 1;
		jumpVel = false;
		break;
	case ColliderType::CHANGE_DIR:
		LOG("Collision ENEMY CHANGE DIR");
		if (walkstate == CHILL) {
			changedir = !changedir;
		}
		break;
	case ColliderType::ENEMY_JUMP:
		LOG("Collision ENEMY CHANGE DIR");
		if (walkstate == FOLLOWINGPLAYER) {
			if (remainingJumps > 0) {
				--remainingJumps;
				jump = true;
				jumpVel = true;
			}
		}
		break;
	case ColliderType::PLAYER:

		int playerY, enemyY;

		playerY = METERS_TO_PIXELS(physB->body->GetPosition().y);
		enemyY = METERS_TO_PIXELS(physA->body->GetPosition().y);

		if (enemyY > playerY + physB->height -2) {
			dead = true;
		}
		else {
			app->scene2->player->isdead = true;
		}		
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
bool GroundEnemy::GetDeadState() {
	return dead;
}
