#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene2.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Map2.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"


Scene2::Scene2(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene2");
}

// Destructor
Scene2::~Scene2()
{}

// Called before render is available
bool Scene2::Awake(pugi::xml_node& config)
{
	//LOG("Loading Scene");
	//bool ret = true;

	//// iterate all objects in the scene
	//// Check https://pugixml.org/docs/quickstart.html#access
	//for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	//{
	//	Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
	//	item->parameters = itemNode;
	//}

	////L02: DONE 3: Instantiate the player using the entity manager
	//player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	//player->parameters = config.child("player");

	//app->entityManager->Enable();
	//app->map->Enable();


	return true;
}

// Called before the first frame
bool Scene2::Start()
{

	LOG("Loading Scene2");

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = app->LoadConfig2().child("scene").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = app->LoadConfig2().child("scene").child("player");
	app->entityManager->Enable();
	godmode = false;

	app->audio->PlayMusic("Assets/Audio/Music/song.ogg");

	// L03: DONE: Load map
	app->map2->Load();

	// L04: DONE 7: Set the window title with map/tileset info
	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map2->mapData.width,
		app->map2->mapData.height,
		app->map2->mapData.tileWidth,
		app->map2->mapData.tileHeight,
		app->map2->mapData.tilesets.Count());

	app->win->SetTitle(title.GetString());

	stopcamera = true;

	return true;
}

// Called each loop iteration
bool Scene2::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene2::Update(float dt)
{
	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();


	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		app->LoadGameRequest();
		if (player->position.x <= 953 && player->position.x >= 943) {
			app->render->camera.x = 0;
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) {
		app->render->camera.y += CAMERASPEED;
		stopcamera = false;
	}

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
		app->render->camera.y -= CAMERASPEED;
		stopcamera = false;
	}
	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
		app->render->camera.x += CAMERASPEED;
		stopcamera = false;
	}
	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
		app->render->camera.x -= CAMERASPEED;
		stopcamera = false;
	}


	//camera limits
	app->render->camera.y = 0;
	if (app->render->camera.x > 0) {
		app->render->camera.x = 0;
	}
	if (app->render->camera.x < -2020) {
		app->render->camera.x = -2020;
	}

	//camera update with the player
	/*if (player->position.x >= 954 && player->position.x < 964) {
		app->render->camera.x = -954;

	}
	if (player->position.x <= 953 && player->position.x >= 943) {
		app->render->camera.x = 0;
	}*/

	/*if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		app->render->camera.x -= CAMERASPEED;
	}
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		app->render->camera.x += CAMERASPEED;
	}*/

	if (player->position.x >= 2470 && stopcamera == true) {
		app->render->camera.x = -2170;
	}

	if (player->position.x >= 300 && player->position.x < 2470 && stopcamera == true) {
		app->render->camera.x = -player->position.x + 300;
	}


	//godmode
	ListItem<PhysBody*>* colliderItem;
	colliderItem = app->map2->mapColliders.start;
	while (colliderItem != NULL)
	{
		if (godmode) {
			colliderItem->data->body->SetActive(false);

		}
		else {
			colliderItem->data->body->SetActive(true);
		}
		colliderItem = colliderItem->next;
	}



	// Draw map
	app->map2->Draw();

	return true;
}

// Called each loop iteration
bool Scene2::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene2::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
