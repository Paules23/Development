#include "ItemGem.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Map.h"

ItemGem::ItemGem() : Entity(EntityType::ITEMCOIN)
{
	name.Create("Gem");

	shine.PushBack({ 480, 480, 32, 32 });
	shine.PushBack({ 512, 480, 32, 32 });
	shine.PushBack({ 544, 480, 32, 32 });
	shine.PushBack({ 576, 480, 32, 32 });
	shine.PushBack({ 608, 480, 32, 32 });
	shine.PushBack({ 640, 480, 32, 32 });
	shine.PushBack({ 480, 480, 32, 32 });
	shine.PushBack({ 480, 480, 32, 32 });
	shine.PushBack({ 480, 480, 32, 32 });
	shine.PushBack({ 480, 480, 32, 32 });
	shine.PushBack({ 480, 480, 32, 32 });
	shine.PushBack({ 480, 480, 32, 32 });
	shine.PushBack({ 480, 480, 32, 32 });

	shine.speed = 0.125;

}

ItemGem::~ItemGem() {}

bool ItemGem::Awake() {
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	audioPath = parameters.attribute("audiopath").as_string();
	return true;
}

bool ItemGem::Start() {
	isPicked = false;
	activated = true;

	//initilize textures
	texture = app->tex->Load(texturePath);
	pickUpFx = app->audio->LoadFx(audioPath);

	// L07 DONE 4: Add a physics to an item - initialize the physics body

	ibody = app->physics->CreateRectangleSensor(position.x + 16, position.y + 16, 16, 16, STATIC);

	ibody->listener = this;

	ibody->ctype = ColliderType::ITEMGEM;

	app->map->Items.Add(ibody);

	return true;
}

bool ItemGem::Update()
{
	if (app->physics->getPause() == true) {
		return true;
	}
	if (!activated) {
		return true;
	}
	if (pickedUp) {
		app->audio->PlayFx(pickUpFx);
		ibody->body->SetActive(false);
		activated = false;
	}
	currentItemAnimation = &shine;

	currentItemAnimation->Update();
	return true;

}

bool ItemGem::PostUpdate() {
	if (currentItemAnimation == NULL)
		return true;
	if (!activated) {
		return true;
	}
	// draw textures and animations
	position.x = METERS_TO_PIXELS(ibody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(ibody->body->GetTransform().p.y) - 16;

	SDL_Rect rect = currentItemAnimation->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect);

	return true;
}

bool ItemGem::CleanUp()
{
	app->tex->UnLoad(texture);
	return true;
}

void ItemGem::OnCollision(PhysBody* physA, PhysBody* physB) {

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
		if (!app->physics->getGodMode()) {
			pickedUp = true;
		}
		break;
	}
}