#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

Item::Item() : Entity(EntityType::ITEM)
{
	name.Create("Coin");

	spinning.PushBack({ 32, 32, 32, 32 });
	spinning.PushBack({ 32, 64, 32, 32 });
	spinning.PushBack({ 32, 96, 32, 32 });
	spinning.PushBack({ 32, 128, 32, 35 });
	spinning.PushBack({ 32, 160, 32, 32 });
	spinning.PushBack({ 32, 192, 32, 32 });
	spinning.speed = 0.1f;

}

Item::~Item() {}

bool Item::Awake() {

	

	return true;
}

bool Item::Start() {


	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	isPicked = false;

	//initilize textures
	texture = app->tex->Load(texturePath);
	
	// L07 DONE 4: Add a physics to an item - initialize the physics body

	ibody = app->physics->CreateRectangleSensor(position.x + 16, position.y + 16,16,16,STATIC);

	ibody->listener = this;

	ibody->ctype = ColliderType::ITEM;

	return true;
}

bool Item::Update()
{
	if (pickedUp) {
		ibody->body->SetActive(false);
		return true;
	}


	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	position.x = METERS_TO_PIXELS(ibody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(ibody->body->GetTransform().p.y) - 16;

	app->render->DrawTexture(texture, position.x, position.y);

	// draw textures and animations
	currentItemAnimation->Update();
	return true;

}

bool Item::CleanUp()
{
	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB) {

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