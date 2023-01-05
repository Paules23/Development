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
#include "Scene.h"

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

	for (pugi::xml_node itemNode = app->LoadConfig2().child("scene2").child("flyingenemy"); itemNode; itemNode = itemNode.next_sibling("flyingenemy"))
	{
		FlyingEnemy* flyingEnemy = (FlyingEnemy*)app->entityManager->CreateEntity(EntityType::FLYING_ENEMY);
		flyingEnemy->parameters = itemNode;
		flyingEnemies.Add(flyingEnemy);
	}

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
	
	//winstate
	if (player->GetWinState() == true) {
		app->map2->Disable();
		app->fade->FadeToBlack1((Module*)app->entityManager, (Module*)app->scenewin, 20);
		app->render->camera.x = 0;
		level2 = false;
	}
	// debug keys
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();


	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		app->LoadGameRequest();
		if (player->position.x <= 953 && player->position.x >= 943) {
			app->render->camera.x = 0;
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		app->map2->Disable();
		app->fade->FadeToBlack1((Module*)app->entityManager, (Module*)app->scene, 20);
		app->map->Enable();
		app->render->camera.x = 0;
		level2 = false;
		app->scene->level1 = true;
		app->scene2->Disable();
	}

	//camera movement
	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) {
		app->render->camera.y += ceil(CAMERASPEED * dt);
		stopcamera = false;
	}

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
		app->render->camera.y -= ceil(CAMERASPEED * dt);
		stopcamera = false;
	}
	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
		app->render->camera.x += ceil(CAMERASPEED * dt);
		stopcamera = false;
	}
	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
		app->render->camera.x -= ceil(CAMERASPEED * dt);
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

	// Draw map
	if (level2) {
		app->map2->Draw();
	}
	//pathfinding
	ListItem<PhysBody*>* enemyBodyItem = app->map2->enemies.start;
	ListItem<GroundEnemy*>* groundEnemyItem = groundEnemies.start;

	while (enemyBodyItem != NULL && groundEnemyItem != NULL) {
		if (enemyBodyItem->data->ctype != ColliderType::GROUND_ENEMY) {
			enemyBodyItem = enemyBodyItem->next;
		}
		else if (groundEnemyItem->data->dead == true) {
			groundEnemyItem = groundEnemyItem->next;
			enemyBodyItem = enemyBodyItem->next;
		}
		else if (enemyBodyItem->data->body->IsActive() && enemyBodyItem->data->ctype == ColliderType::GROUND_ENEMY
			&& groundEnemyItem->data->walkstate == WalkState::FOLLOWINGPLAYER) {
			origin.x = enemyBodyItem->data->body->GetPosition().x;
			origin.y = enemyBodyItem->data->body->GetPosition().y;
			iPoint destination;
			destination.x = player->GetBody()->body->GetPosition().x;
			destination.y = player->GetBody()->body->GetPosition().y;
			app->pathfinding->ClearLastPath();
			app->pathfinding->CreatePath(origin, destination);
			const DynArray<iPoint>* path = app->pathfinding->GetLastPath();
			for (uint i = 0; i < path->Count(); ++i)
			{
				iPoint pos = app->map2->MapToWorld(path->At(i)->x, path->At(i)->y);
				if (i == 1) {
					groundEnemyItem->data->target.x = PIXEL_TO_METERS(pos.x);
					groundEnemyItem->data->target.y = PIXEL_TO_METERS(pos.y);
				}

				if (app->physics->debug) app->render->DrawTexture(mouseTileTex, pos.x, pos.y);
			}
			// L12: Debug pathfinding didn't change the names cause xd
			iPoint originScreen = app->map2->MapToWorld(destination.x, destination.y);
			if (app->physics->debug) app->render->DrawTexture(originTex, originScreen.x, originScreen.y);
			groundEnemyItem = groundEnemyItem->next;
			enemyBodyItem = enemyBodyItem->next;
		}
		else {
			groundEnemyItem = groundEnemyItem->next;
			enemyBodyItem = enemyBodyItem->next;
		}
		
	}
	enemyBodyItem = app->map2->enemies.start;
	ListItem<FlyingEnemy*>* flyingEnemyItem = flyingEnemies.start;

	while (enemyBodyItem != NULL && flyingEnemyItem != NULL ) {
		if (enemyBodyItem->data->ctype != ColliderType::FLYING_ENEMY) {
			enemyBodyItem = enemyBodyItem->next;
		}
		else if (flyingEnemyItem->data->dead == true) {
			flyingEnemyItem = flyingEnemyItem->next;
			enemyBodyItem = enemyBodyItem->next;
		}
		else if ( enemyBodyItem->data->ctype == ColliderType::FLYING_ENEMY 
			&& flyingEnemyItem->data->walkstate == WalkState ::FOLLOWINGPLAYER) {
			origin.x = enemyBodyItem->data->body->GetPosition().x;
			origin.y = enemyBodyItem->data->body->GetPosition().y;
			iPoint destination;
			destination.x = player->GetBody()->body->GetPosition().x;
			destination.y = player->GetBody()->body->GetPosition().y;
			app->pathfinding->ClearLastPath();
			app->pathfinding->CreatePath(origin, destination);
			const DynArray<iPoint>* path = app->pathfinding->GetLastPath();
			for (uint i = 0; i < path->Count(); ++i)
			{
				iPoint pos = app->map2->MapToWorld(path->At(i)->x, path->At(i)->y);
				if (i == 1) {
					flyingEnemyItem->data->target.x = PIXEL_TO_METERS(pos.x);
					flyingEnemyItem->data->target.y = PIXEL_TO_METERS(pos.y);
				}

				if (app->physics->debug) app->render->DrawTexture(mouseTileTex, pos.x, pos.y);
			}
			// L12: Debug pathfinding didn't change the names cause xd
			iPoint originScreen = app->map2->MapToWorld(destination.x, destination.y);
			if (app->physics->debug) app->render->DrawTexture(originTex, originScreen.x, originScreen.y);
			flyingEnemyItem = flyingEnemyItem->next;
			enemyBodyItem = enemyBodyItem->next;
		}
		else {
			enemyBodyItem = enemyBodyItem->next;
			flyingEnemyItem = flyingEnemyItem->next;
		}
	}
	
	
	return true;
}

// Called each loop iteration
bool Scene2::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	//debug of the path basically shows a fking red line xdd
	if (app->physics->debug )
	{
		PhysBody* pbody = player->GetBody();
		ListItem<PhysBody*>* enemyBodyItem = app->map2->enemies.start;
		ListItem<GroundEnemy*>* groundEnemyItem = groundEnemies.start;

		while (enemyBodyItem != NULL && groundEnemyItem != NULL) {
			if (enemyBodyItem->data->ctype != ColliderType::GROUND_ENEMY) {
				enemyBodyItem = enemyBodyItem->next;
			}
			else if (groundEnemyItem->data->dead == true) {
				groundEnemyItem = groundEnemyItem->next;
				enemyBodyItem = enemyBodyItem->next;
			}
			else if (enemyBodyItem->data->body->IsActive() && enemyBodyItem->data->ctype == ColliderType::GROUND_ENEMY
				&& groundEnemyItem->data->walkstate == WalkState::FOLLOWINGPLAYER) {
				app->render->DrawLine(METERS_TO_PIXELS(enemyBodyItem->data->body->GetPosition().x),
					METERS_TO_PIXELS(enemyBodyItem->data->body->GetPosition().y),
					METERS_TO_PIXELS(pbody->body->GetPosition().x),
					METERS_TO_PIXELS(pbody->body->GetPosition().y),
					255, 0, 0);
				groundEnemyItem = groundEnemyItem->next;
				enemyBodyItem = enemyBodyItem->next;
			}
			else {
				groundEnemyItem = groundEnemyItem->next;
				enemyBodyItem = enemyBodyItem->next;
			}
		}
		enemyBodyItem = app->map2->enemies.start;
		ListItem<FlyingEnemy*>* flyingEnemyItem = flyingEnemies.start;

		while (enemyBodyItem != NULL && flyingEnemyItem != NULL) {
			if (enemyBodyItem->data->ctype != ColliderType::FLYING_ENEMY) {
				enemyBodyItem = enemyBodyItem->next;
			}
			else if (flyingEnemyItem->data->dead == true) {
				flyingEnemyItem = flyingEnemyItem->next;
				enemyBodyItem = enemyBodyItem->next;
			}
			else if (enemyBodyItem->data->ctype == ColliderType::FLYING_ENEMY
				&& flyingEnemyItem->data->walkstate == WalkState::FOLLOWINGPLAYER) {
				app->render->DrawLine(METERS_TO_PIXELS(enemyBodyItem->data->body->GetPosition().x),
					METERS_TO_PIXELS(enemyBodyItem->data->body->GetPosition().y),
					METERS_TO_PIXELS(pbody->body->GetPosition().x),
					METERS_TO_PIXELS(pbody->body->GetPosition().y),
					255, 0, 0);
				flyingEnemyItem = flyingEnemyItem->next;
				enemyBodyItem = enemyBodyItem->next;
			}
			else {
				enemyBodyItem = enemyBodyItem->next;
				flyingEnemyItem = flyingEnemyItem->next;
			}
		}
	}
	return ret;
}

// Called before quitting
bool Scene2::CleanUp()
{
	LOG("Freeing scene");
	groundEnemies.Clear();
	flyingEnemies.Clear();
	app->tex->UnLoad(img);
	app->tex->UnLoad(mouseTileTex);
	app->tex->UnLoad(originTex);
	
	return true;
}
