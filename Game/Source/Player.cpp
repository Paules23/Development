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
#include "FadeToBlack.h"

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
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	isdead = false;
	win = false;
	godmode = false;
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
	if (isdead == true) {
		app->audio->PlayFx(dieFxId);
		position.x = parameters.attribute("x").as_int();
		position.y = parameters.attribute("y").as_int();

		b2Vec2 pos(position.x, position.y);
		pbody->body->SetTransform(PIXEL_TO_METERS(pos), 0);
		app->render->camera.x = 0;
		isdead = false;
	}
	if (win == true) {
		app->audio->PlayMusic("");
		app->audio->PlayFx(winFxId);
		app->scene->Disable();
		app->fade->FadeToBlack1((Module*)app->entityManager, (Module*)app->scenewin, 20);
		app->render->camera.x = 0;
	}

	if (godmode == true) {
		pbody->body->SetActive(false);
	}
	else {
		pbody->body->SetActive(true);
	}


	if (remainingJumps < 1) {
		currentPlayerAnimation = &jump;
	}
	else {
		currentPlayerAnimation = &iddle;
	}
	
	//Movement of the player
	if (godmode) {
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			position.y -= 10;
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			position.y += 10;
		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			position.x -= 10;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			position.x += 10;
		}
	}
	else {
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
			if (remainingJumps > 0) {
				currentPlayerAnimation = &jump;
				app->audio->PlayFx(jumpFxId);
				float impulse = pbody->body->GetMass() * -7;
				pbody->body->ApplyLinearImpulse(b2Vec2(0, impulse), pbody->body->GetWorldCenter(), false);
				--remainingJumps;
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {

			currentPlayerAnimation = &movementLeft;
			moveState = MS_LEFT;
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {
			moveState = MS_STOP;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			currentPlayerAnimation = &movement;
			moveState = MS_RIGHT;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_UP) {
			moveState = MS_STOP;
		}
		b2Vec2 vel = pbody->body->GetLinearVelocity();
		float desiredVel = 0;
		switch (moveState)
		{
		case MS_LEFT:  desiredVel = -7; break;
		case MS_STOP:  desiredVel = 0; break;
		case MS_RIGHT: desiredVel = 7; break;
		}
		float velChange = desiredVel - vel.x;
		float impulse = pbody->body->GetMass() * velChange; //disregard time factor
		pbody->body->ApplyLinearImpulse(b2Vec2(impulse, 0), pbody->body->GetWorldCenter(), true);
		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;
	}
	//debug options

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		pbody->body->SetTransform(PIXEL_TO_METERS(INIT_POSITION), 0);
	}
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		godmode = !godmode;
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
		LOG("Collision DEATH");
		win = true;
		break;
	case ColliderType::GROUND:
		LOG("Collision DEATH");
		remainingJumps = 2;
		break;
	}
}


bool Player::LoadState(pugi::xml_node& data)
{
	position.x = data.child("player").attribute("x").as_int();
	position.y = data.child("player").attribute("y").as_int();

	b2Vec2 pos (position.x, position.y);
	pbody->body->SetTransform(PIXEL_TO_METERS(pos), 0);

	return true;
}

// L03: DONE 8: Create a method to save the state of the renderer
// using append_child and append_attribute
bool Player::SaveState(pugi::xml_node& data)
{
	pugi::xml_node play = data.append_child("player");

	play.append_attribute("x") = position.x;
	play.append_attribute("y") = position.y;

	return true;
}

bool Player::GetWinState() {
	return win;
}
bool Player::GetDeadState() {
	return isdead;
}
