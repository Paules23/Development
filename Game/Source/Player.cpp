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

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");

	iddle.PushBack({120,115,57,103});
	iddle.PushBack({ 446,115,57,103 });
	iddle.PushBack({ 774,115,57,103 });
	iddle.PushBack({ 1099,115,57,103 });
	iddle.PushBack({ 1426,115,57,103 });
	iddle.PushBack({ 1752,115,57,103 });
	iddle.PushBack({ 2079,115,57,103 });
	iddle.PushBack({ 2405,115,57,103 });
	iddle.PushBack({ 2732,115,57,103 });
	iddle.PushBack({ 3058,115,57,103 });
	iddle.speed = 0.1f;


	
}

Player::~Player() {

}

bool Player::Awake() {

	//L02: DONE 5: Get Player parameters from XML
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	isdead = false;
	win = false;

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x+16, position.y+16, 15, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update()
{

	// L07 DONE 5: Add physics to the player - updated player position using physics

	int speed = 4;
	timerbetweenjumps--;
	currentPlayerAnimation = &iddle;
	//L02: DONE 4: modify the position of the player using arrow keys and render the texture
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
		if (remainingJumps == 2) {
			float impulse = pbody->body->GetMass() * -8;
			pbody->body->ApplyLinearImpulse(b2Vec2(0, impulse), pbody->body->GetWorldCenter(), false);
			--remainingJumps;
		}
		else if (remainingJumps == 1 && timerbetweenjumps < 0) {
			float impulse = pbody->body->GetMass() * -8;
			pbody->body->ApplyLinearImpulse(b2Vec2(0, impulse), pbody->body->GetWorldCenter(), false);
			--remainingJumps;
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
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

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		pbody->body->SetTransform(PIXEL_TO_METERS(INIT_POSITION), 0);
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
	pbody->body->ApplyLinearImpulse(b2Vec2(impulse, 0), pbody->body->GetWorldCenter(),true);
	
	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	currentPlayerAnimation->Update();

	if (isdead == true) {
		isdead = false;
	}
	if (win == true) {
		return false;
	}

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

	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		remainingJumps = 2;
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
