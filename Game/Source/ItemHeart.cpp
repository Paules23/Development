#include "ItemHeart.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

ItemHeart::ItemHeart() : Entity(EntityType::ITEMHEART)
{
	name.Create("Heart");

	/*spinning.PushBack({ 32, 32, 32, 32 });
	spinning.PushBack({ 64, 32, 32, 32 });
	spinning.PushBack({ 96, 32, 32, 32 });
	spinning.PushBack({ 128, 32, 32, 35 });
	spinning.PushBack({ 160, 32, 32, 32 });
	spinning.PushBack({ 192, 32, 32, 32 });
	spinning.speed = 0.1f;*/

}

ItemHeart::~ItemHeart() {}

bool ItemHeart::Awake() {
	return true;
}

bool ItemHeart::Start() {


	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	audioPath = parameters.attribute("audiopath").as_string();

	isPicked = false;
	activated = true;

	//initilize textures
	texture = app->tex->Load(texturePath);
	pickUpFx = app->audio->LoadFx(audioPath);

	// L07 DONE 4: Add a physics to an item - initialize the physics body

	ibody = app->physics->CreateRectangleSensor(position.x + 16, position.y + 16, 16, 16, STATIC);

	ibody->listener = this;

	ibody->ctype = ColliderType::ITEM;

	return true;
}

bool ItemHeart::Update()
{
	if (!activated) {
		return true;
	}
	if (pickedUp) {
		app->audio->PlayFx(pickUpFx);
		ibody->body->SetActive(false);
		activated = false;
	}
	/*currentItemAnimation = &spinning;*/

	currentItemAnimation->Update();

	// draw textures and animations
	position.x = METERS_TO_PIXELS(ibody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(ibody->body->GetTransform().p.y) - 16;

	SDL_Rect rect = currentItemAnimation->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect);


	return true;

}

bool ItemHeart::CleanUp()
{
	return true;
}

void ItemHeart::OnCollision(PhysBody* physA, PhysBody* physB) {

	// L07 DONE 7: Detect the type of collision

	iPoint physbpos;
	physB->GetPosition(physbpos.x, physbpos.y);

	switch (physB->ctype)
	{
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::PLAYER:
		LOG("Coin picked up");
		pickedUp = true;
		break;
	}
}