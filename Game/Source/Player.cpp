#include "Player.h"
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
#include "FlyingEnemy.h"
#include "GroundEnemy.h"


Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");

	iddle.PushBack({ 32, 0, 32, 32 });
	iddle.PushBack({ 32, 55, 32, 32 });
	iddle.PushBack({ 32, 107, 32, 32 });
	iddle.PushBack({ 32, 159, 32, 35 });
	iddle.PushBack({ 32, 216, 32, 32 });
	iddle.PushBack({ 32, 267, 32, 32 });
	iddle.speed = 0.1f;

	movement.PushBack({ 64, 0, 32, 32 });
	movement.PushBack({ 64, 52, 32, 33 });
	movement.PushBack({ 64, 137, 32, 32 });
	movement.PushBack({ 64, 194, 32, 32 });
	movement.PushBack({ 64, 250, 32, 32 });
	movement.PushBack({ 64, 384, 32, 32 });
	movement.PushBack({ 64, 438, 32, 32 });
	movement.speed = 0.1f;

	jump.PushBack({ 0, 0, 32, 32 });
	jump.PushBack({ 0, 53, 32, 35 });
	jump.PushBack({ 0, 111, 32, 32 });
	jump.PushBack({ 0, 159, 32, 39 });
	jump.PushBack({ 0, 333, 32, 36 });
	jump.PushBack({ 0, 420, 32, 34 });
	jump.PushBack({ 0, 505, 32, 32 });
	jump.PushBack({ 0, 557, 32, 34 });
	jump.PushBack({ 0, 614, 32, 32 });
	jump.PushBack({ 0, 668, 32, 32 });
	jump.PushBack({ 0, 719, 32, 32 });
	jump.speed = 0.2f;
	


	movementLeft.PushBack({ 128, 0, 32, 32 });
	movementLeft.PushBack({ 128, 52, 32, 33 });
	movementLeft.PushBack({ 128, 137, 32, 32 });
	movementLeft.PushBack({ 128, 194, 32, 32 });
	movementLeft.PushBack({ 128, 250, 32, 32 });
	movementLeft.PushBack({ 128, 384, 32, 32 });
	movementLeft.PushBack({ 128, 438, 32, 32 });
	movementLeft.speed = 0.1f;

	die.PushBack({ 96, 0, 32, 32 });
	die.PushBack({ 96, 55, 32, 32 });
	die.PushBack({ 96, 107, 32, 32 });
	die.speed = 0.3f;

}

Player::~Player() {

}

bool Player::Awake() {

	//L02: DONE 5: Get Player parameters from XML
	

	return true;
}

bool Player::Start() {
	if (app->scene->level1) {
		position.x = parameters.attribute("x1").as_int();
		position.y = parameters.attribute("y1").as_int();
	}
	if (app->scene2->level2) {
		position.x = parameters.attribute("x2").as_int();
		position.y = parameters.attribute("y2").as_int();
	}
	texturePath = parameters.attribute("texturepath").as_string();
	audioDie = parameters.attribute("audioDie").as_string();
	audioWin = parameters.attribute("audioWin").as_string();
	audioJump = parameters.attribute("audioJump").as_string();
	isdead = false;
	win = false;
	//initilize textures
	texture = app->tex->Load(texturePath);

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x+16, position.y+16, 15, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	dieFxId = app->audio->LoadFx("Assets/Audio/Fx/die.ogg");
	jumpFxId = app->audio->LoadFx("Assets/Audio/Fx/Jump-1.ogg");
	winFxId = app->audio->LoadFx("Assets/Audio/Fx/win.ogg");

	return true;
}

bool Player::Update()
{
	//win and death conditions
	if (isdead == true) {
		app->audio->PlayFx(dieFxId);
		if (app->scene->level1) {
			position.x = parameters.attribute("x1").as_int();
			position.y = parameters.attribute("y1").as_int();
			
			
		}
		if (app->scene2->level2) {
			position.x = parameters.attribute("x2").as_int();
			position.y = parameters.attribute("y2").as_int();

			//enemies will respawn if the player dies
			ListItem<GroundEnemy*>* groundEnemyItem = app->scene2->groundEnemies.start;
			while (groundEnemyItem != NULL) {
				
				groundEnemyItem->data->dead = false;
				groundEnemyItem->data->ebody->body->SetActive(true);
				groundEnemyItem->data->position.x = groundEnemyItem->data->parameters.attribute("posx").as_int();
				groundEnemyItem->data->position.y = groundEnemyItem->data->parameters.attribute("posy").as_int();
				b2Vec2 pos(groundEnemyItem->data->position.x, groundEnemyItem->data->position.y);
				groundEnemyItem->data->ebody->body->SetTransform(PIXEL_TO_METERS(pos), 0);
				
				groundEnemyItem = groundEnemyItem->next;
			}
			ListItem<FlyingEnemy*>* flyingEnemyItem = app->scene2->flyingEnemies.start;
			while (flyingEnemyItem != NULL) {
				flyingEnemyItem->data->dead = false;
				flyingEnemyItem->data->ebody->body->SetActive(true);
				flyingEnemyItem->data->position.x = flyingEnemyItem->data->parameters.attribute("posx").as_int();
				flyingEnemyItem->data->position.y = flyingEnemyItem->data->parameters.attribute("posy").as_int();
				b2Vec2 pos(flyingEnemyItem->data->position.x, flyingEnemyItem->data->position.y);
				flyingEnemyItem->data->ebody->body->SetTransform(PIXEL_TO_METERS(pos), 0);
				flyingEnemyItem = flyingEnemyItem->next;
			}

		}

		b2Vec2 pos(position.x, position.y);
		pbody->body->SetTransform(PIXEL_TO_METERS(pos), 0);
		app->render->camera.x = 0;
		isdead = false;
	}
	if (win == true) {
		app->audio->PlayMusic("");
	}
	//Movement and jump of the player
	if (remainingJumps < 1) {
		currentPlayerAnimation = &jump;
	}
	else {
		currentPlayerAnimation = &iddle;
	}
	if (app->scene->godmode || app->scene2->godmode) {
		b2Vec2 vel(0, 0);
		pbody->body->SetGravityScale(0);
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			currentPlayerAnimation = &iddle;
			vel.y = GODMODESPEED;
			app->scene->stopcamera = true;
			app->scene2->stopcamera = true;
		}
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			currentPlayerAnimation = &iddle;
			vel.y = -GODMODESPEED;
			app->scene->stopcamera = true;
			app->scene2->stopcamera = true;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {

			currentPlayerAnimation = &movement;
			vel.x = GODMODESPEED;
			app->scene->stopcamera = true;
			app->scene2->stopcamera = true;
		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			currentPlayerAnimation = &movementLeft;
			vel.x = -GODMODESPEED;
			app->scene->stopcamera = true;
			app->scene2->stopcamera = true;
		}
		pbody->body->SetLinearVelocity(vel);
		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;
	}
	else {
		pbody->body->SetGravityScale(1);
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
			if (remainingJumps > 0) {
				b2Vec2 xd(pbody->body->GetLinearVelocity().x, 0);
				pbody->body->SetLinearVelocity(xd);
				currentPlayerAnimation = &jump;
				app->audio->PlayFx(jumpFxId);
				float impulse = pbody->body->GetMass() * JUMPIMPULSE;
				pbody->body->ApplyLinearImpulse(b2Vec2(0, impulse), pbody->body->GetWorldCenter(), false);
				--remainingJumps;
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {

			currentPlayerAnimation = &movementLeft;
			moveState = MS_LEFT;
			app->scene->stopcamera = true;
			app->scene2->stopcamera = true;
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {
			moveState = MS_STOP;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			currentPlayerAnimation = &movement;
			moveState = MS_RIGHT;
			app->scene->stopcamera = true;
			app->scene2->stopcamera = true;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_UP) {
			moveState = MS_STOP;
		}
		b2Vec2 vel = pbody->body->GetLinearVelocity();
		float desiredVelx = 0;
		float desiredVely = 0;
		switch (moveState)
		{
		case MS_LEFT:  desiredVelx = -7; break;
		case MS_STOP:  desiredVelx = 0; desiredVely = 0; break;
		case MS_RIGHT: desiredVelx = 7; break;
		}
		float velChangex = desiredVelx - vel.x;
		float impulsex = pbody->body->GetMass() * velChangex; //disregard time factor
		pbody->body->ApplyLinearImpulse(b2Vec2(impulsex, 0), pbody->body->GetWorldCenter(), true);
		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;
	}

	//debug options
	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		if (app->scene->level1) {
			pbody->body->SetTransform(PIXEL_TO_METERS(INIT_POSITION1), 0);
		}
		else if (app->scene2->level2) {
			pbody->body->SetTransform(PIXEL_TO_METERS(INIT_POSITION2), 0);
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		if (app->scene->level1) {
			win = true;
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		if (app->scene->level1) {
			app->scene->godmode = !app->scene->godmode;
		}
		else if (app->scene2->level2){
			app->scene2->godmode = !app->scene2->godmode;
		}
		
	}
	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {
		win = true;
	}
	if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) {
		isdead = true;
	}
	
	
	// draw textures and animations
	currentPlayerAnimation->Update();

	SDL_Rect rect = currentPlayerAnimation->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect);
	return true;
}

bool Player::CleanUp()
{
	pbody->body->GetWorld()->DestroyBody(pbody->body);
	pbody = NULL;
	app->tex->UnLoad(texture);
	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

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
		if (app->scene2->godmode) {
			break;
		}
		LOG("Collision DEATH");
		isdead = true;
		break;
	case ColliderType::WIN:
		LOG("Collision WIN");
		win = true;
		app->audio->PlayFx(winFxId);
		break;
	case ColliderType::GROUND:
		LOG("Collision JUMPS RESTORED");
		remainingJumps = 2;
		break;
	}
}


bool Player::LoadState(pugi::xml_node& data)
{
	//player loadstate
	if (app->scene->level1) {
		position.x = data.child("player").attribute("x1").as_int();
		position.y = data.child("player").attribute("y1").as_int();
		remainingJumps = data.child("player").attribute("remainingJumps").as_int();
	}
	if (app->scene2->level2) {
		position.x = data.child("player").attribute("x2").as_int();
		position.y = data.child("player").attribute("y2").as_int();
		remainingJumps = data.child("player").attribute("remainingJumps").as_int();

		////ground enemies loadstate
		//ListItem<GroundEnemy*>* groundEnemyItem = app->scene2->groundEnemies.start;
		//pugi::xml_node GE = data.child("groundEnemy");

		//while (groundEnemyItem != NULL)
		//{
		//	groundEnemyItem->data->position.x = GE.attribute("x").as_int();
		//	groundEnemyItem->data->position.x = GE.attribute("y").as_int();

		//	if (groundEnemyItem->data->dead == true && GE.attribute("dead").as_bool() == false)
		//	{
		//		groundEnemyItem->data->dead = false;
		//		groundEnemyItem->data->ebody->body->SetActive(true);
		//		groundEnemyItem->data->Enable();
		//	}
		//	else if (groundEnemyItem->data->dead == false && GE.attribute("dead").as_bool() == true)
		//	{
		//		groundEnemyItem->data->dead = true;
		//	}
		//	b2Vec2 pos(groundEnemyItem->data->position.x, groundEnemyItem->data->position.y);
		//	pbody->body->SetTransform(PIXEL_TO_METERS(pos), 0);
		//	GE = GE.next_sibling("groundEnemy");
		//	groundEnemyItem = groundEnemyItem->next;
		//}

		////flying enemies loadstate
		//ListItem<FlyingEnemy*>* flyingEnemyItem = app->scene2->flyingEnemies.start;
		//pugi::xml_node FE = data.child("flyingEnemy");

		//while (flyingEnemyItem != NULL)
		//{
		//	flyingEnemyItem->data->position.x = FE.attribute("x").as_int();
		//	flyingEnemyItem->data->position.x = FE.attribute("y").as_int();

		//	if (flyingEnemyItem->data->dead == true && FE.attribute("dead").as_bool() == false)
		//	{
		//		flyingEnemyItem->data->dead = false;
		//		flyingEnemyItem->data->ebody->body->SetActive(true);
		//		flyingEnemyItem->data->Enable();
		//	}
		//	else if (flyingEnemyItem->data->dead == false && FE.attribute("dead").as_bool() == true)
		//	{
		//		flyingEnemyItem->data->dead = true;
		//	}
		//	b2Vec2 pos(flyingEnemyItem->data->position.x, flyingEnemyItem->data->position.y);
		//	pbody->body->SetTransform(PIXEL_TO_METERS(pos), 0);
		//	FE = FE.next_sibling("flyingEnemy");
		//	flyingEnemyItem = flyingEnemyItem->next;
		//}
	}

	b2Vec2 pos (position.x, position.y);
	pbody->body->SetTransform(PIXEL_TO_METERS(pos), 0);

	

	return true;
}

// L03: DONE 8: Create a method to save the state of the renderer
// using append_child and append_attribute
bool Player::SaveState(pugi::xml_node& data)
{
	//player savestate
	pugi::xml_node play = data.append_child("player");

	if (app->scene->level1) {
		play.append_attribute("x1") = position.x;
		play.append_attribute("y1") = position.y;
		play.append_attribute("remainingJumps") = remainingJumps;
	}
	if (app->scene2->level2) {
		play.append_attribute("x2") = position.x;
		play.append_attribute("y2") = position.y;
		play.append_attribute("remainingJumps") = remainingJumps;

		////ground enemies savestate
		//ListItem<GroundEnemy*>* groundEnemyItem = app->scene2->groundEnemies.start;
		//pugi::xml_node GE = data.append_child("groundEnemy");

		//while (groundEnemyItem != NULL)
		//{
		//	GE.append_attribute("x") = groundEnemyItem->data->position.x;
		//	GE.append_attribute("y") = groundEnemyItem->data->position.y;
		//	GE.append_attribute("dead") = groundEnemyItem->data->dead;

		//	GE = GE.next_sibling("groundEnemy");
		//	groundEnemyItem = groundEnemyItem->next;
		//}
		////flying enemies savestate
		//ListItem<FlyingEnemy*>* flyingEnemyItem = app->scene2->flyingEnemies.start;
		//pugi::xml_node FE = data.append_child("flyingEnemy");

		//while (flyingEnemyItem != NULL)
		//{
		//	FE.append_attribute("x") = flyingEnemyItem->data->position.x;
		//	FE.append_attribute("y") = flyingEnemyItem->data->position.y;
		//	FE.append_attribute("dead") = flyingEnemyItem->data->dead;
		//	FE = FE.next_sibling("flyingEnemy");
		//	flyingEnemyItem = flyingEnemyItem->next;
		//}
	}

	

	return true;
}

bool Player::GetWinState() {
	return win;
}
bool Player::GetDeadState() {
	return isdead;
}
PhysBody* Player::GetBody() {
	return pbody;
}
void Player::SetJumps(int jumps) {
	remainingJumps = jumps;
}

