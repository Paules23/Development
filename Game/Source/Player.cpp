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
		}

		b2Vec2 pos(position.x, position.y);
		pbody->body->SetTransform(PIXEL_TO_METERS(pos), 0);
		app->render->camera.x = 0;
		isdead = false;
	}
	if (win == true) {
		app->audio->PlayMusic("");
		app->audio->PlayFx(winFxId);
	}
	//Movement and jump of the player
	if (remainingJumps < 1) {
		currentPlayerAnimation = &jump;
	}
	else {
		currentPlayerAnimation = &iddle;
	}
	if (app->scene->godmode) {
		b2Vec2 vel(0, 0);
		pbody->body->SetGravityScale(0);
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			currentPlayerAnimation = &iddle;
			vel.y = GODMODESPEED;
			app->scene->stopcamera = true;
		}
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			currentPlayerAnimation = &iddle;
			vel.y = -GODMODESPEED;
			app->scene->stopcamera = true;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {

			currentPlayerAnimation = &movement;
			vel.x = GODMODESPEED;
			app->scene->stopcamera = true;
		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			currentPlayerAnimation = &movementLeft;
			vel.x = -GODMODESPEED;
			app->scene->stopcamera = true;
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
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {
			moveState = MS_STOP;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			currentPlayerAnimation = &movement;
			moveState = MS_RIGHT;
			app->scene->stopcamera = true;
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
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		pbody->body->SetTransform(PIXEL_TO_METERS(INIT_POSITION), 0);
	}
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		app->scene->godmode = !app->scene->godmode;
	}
	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {
		win = true;
	}
	if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) {
		isdead = true;
	}
	
	
	//Update player position in pixels
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
	app->tex->UnLoad(run);
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
		LOG("Collision DEATH");
		isdead = true;
		break;
	case ColliderType::WIN:
		LOG("Collision WIN");
		win = true;
		break;
	case ColliderType::GROUND:
		LOG("Collision JUMPS RESTORED");
		remainingJumps = 2;
		break;
	}
}


bool Player::LoadState(pugi::xml_node& data)
{
	if (app->scene->level1) {
		position.x = parameters.attribute("x1").as_int();
		position.y = parameters.attribute("y1").as_int();
	}
	if (app->scene2->level2) {
		position.x = parameters.attribute("x2").as_int();
		position.y = parameters.attribute("y2").as_int();
	}

	b2Vec2 pos (position.x, position.y);
	pbody->body->SetTransform(PIXEL_TO_METERS(pos), 0);

	return true;
}

// L03: DONE 8: Create a method to save the state of the renderer
// using append_child and append_attribute
bool Player::SaveState(pugi::xml_node& data)
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

bool Player::GetWinState() {
	return win;
}
bool Player::GetDeadState() {
	return isdead;
}
