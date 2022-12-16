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
#include "FadeToBlack.h"
#include "Pathfinding.h"

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
	level2 = false;

	return true;
}

// Called before the first frame
bool Scene2::Start()
{

	LOG("Loading Scene2");

	

	// L12 Create walkability map
	

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	/*for (pugi::xml_node itemNode = app->LoadConfig2().child("scene").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}*/

	for (pugi::xml_node itemNode = app->LoadConfig2().child("scene2").child("groundenemy"); itemNode; itemNode = itemNode.next_sibling("groundenemy"))
	{
		GroundEnemy* groundEnemy = (GroundEnemy*)app->entityManager->CreateEntity(EntityType::GROUND_ENEMY);
		groundEnemy->parameters = itemNode;
		groundEnemies.Add(groundEnemy);
	}

	/*for (pugi::xml_node itemNode = app->LoadConfig2().child("scene").child("flyingenemy"); itemNode; itemNode = itemNode.next_sibling("flyingenemy"))
	{
		GroundEnemy* groundEnemy = (GroundEnemy*)app->entityManager->CreateEntity(EntityType::FLYING_ENEMY);
		groundEnemy->parameters = itemNode;
	}*/

	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = app->LoadConfig2().child("scene").child("player");
	app->entityManager->Enable();
	godmode = false;

	app->audio->PlayMusic("Assets/Audio/Music/song.ogg");
	mouseTileTex = app->tex->Load("Assets/Textures/path_square.png");
	originTex = app->tex->Load("Assets/Textures/x_square.png");

	// L03: DONE: Load map
	bool retLoad = app->map2->Load();
	if (retLoad) {
		int w, h;
		uchar* data = NULL;

		bool retWalkMap = app->map2->CreateWalkabilityMap(w, h, &data);
		if (retWalkMap) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);

	}

	// L04: DONE 7: Set the window title with map/tileset info
	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map2->mapData.width,
		app->map2->mapData.height,
		app->map2->mapData.tileWidth,
		app->map2->mapData.tileHeight,
		app->map2->mapData.tilesets.Count());

	app->win->SetTitle(title.GetString());

	stopcamera = true;
	level2 = true;
	app->render->camera.x = 0;

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

	if (player->GetWinState() == true) {
		app->map2->Disable();
		app->fade->FadeToBlack1((Module*)app->entityManager, (Module*)app->scenewin, 20);
		app->render->camera.x = 0;
		level2 = false;
	}
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
	if (app->render->camera.x < -3340) {
		app->render->camera.x = -3340;
	}
	if (player->position.x >= 3340 && stopcamera == true) {
		app->render->camera.x = -3040;
	}

	if (player->position.x >= 300 && player->position.x < 3340 && stopcamera == true) {
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
	if (level2) {
		app->map2->Draw();
	}
	//pathfinding
	ListItem<GroundEnemy*>* groundEnemyItem = groundEnemies.start;

	while (groundEnemyItem != NULL) {

		if (groundEnemyItem->data->walkstate == WalkState::FOLLOWINGPLAYER)
		{
			app->pathfinding->ClearLastPath();
			destination = player->position;
			origin = groundEnemyItem->data->position;
			
			app->pathfinding->CreatePath(origin, destination);

			const DynArray<iPoint>* path = app->pathfinding->GetLastPath();
			for (uint i = 0; i < path->Count(); ++i)
			{
				iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
				app->render->DrawTexture(mouseTileTex, pos.x, pos.y);
			}

			// L12: Debug pathfinding
			iPoint originScreen = app->map->MapToWorld(origin.x, origin.y);
			app->render->DrawTexture(originTex, originScreen.x, originScreen.y);

		}
		groundEnemyItem = groundEnemyItem->next;
	}


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