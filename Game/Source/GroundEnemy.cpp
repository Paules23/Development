#include "GroundEnemy.h"
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


GroundEnemy::GroundEnemy() : Entity(EntityType::PLAYER)
{
	name.Create("GroundEnemy");
}

GroundEnemy::~GroundEnemy() {

}

bool GroundEnemy::Awake() {

	//L02: DONE 5: Get Player parameters from XML


	return true;
}

bool GroundEnemy::Start() {
	
	
		
		position.x = parameters.attribute("x2").as_int();
		position.y = parameters.attribute("y2").as_int();
		
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



		//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
		/*dieFxId = app->audio->LoadFx("Assets/Audio/Fx/die.ogg");
		jumpFxId = app->audio->LoadFx("Assets/Audio/Fx/Jump-1.ogg");
		winFxId = app->audio->LoadFx("Assets/Audio/Fx/win.ogg");*/

	return true;
}

bool GroundEnemy::Update()
{

	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  


	app->render->DrawTexture(texture, position.x, position.y);
	return true;
}

bool GroundEnemy::CleanUp()
{
	ebody->body->GetWorld()->DestroyBody(ebody->body);
	ebody = NULL;
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
