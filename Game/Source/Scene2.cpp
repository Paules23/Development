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
#include "HUD.h"
#include "GuiManager.h"

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
	level2 = false;
	return true;
}

// Called before the first frame
bool Scene2::Start()
{
	LOG("Loading Scene2");
	for (pugi::xml_node itemNode = app->LoadConfig2().child("scene2").child("itemCoin"); itemNode; itemNode = itemNode.next_sibling("itemCoin"))
	{
		ItemCoin* item = (ItemCoin*)app->entityManager->CreateEntity(EntityType::ITEMCOIN);
		item->parameters = itemNode;
		item->Awake();
		itemsEnemies.Add(item);
	}

	for (pugi::xml_node itemNode = app->LoadConfig2().child("scene2").child("itemHeart"); itemNode; itemNode = itemNode.next_sibling("itemHeart"))
	{
		ItemHeart* item = (ItemHeart*)app->entityManager->CreateEntity(EntityType::ITEMHEART);
		item->parameters = itemNode;
		item->Awake();
		itemsEnemies.Add(item);
	}

	for (pugi::xml_node itemNode = app->LoadConfig2().child("scene2").child("itemGem"); itemNode; itemNode = itemNode.next_sibling("itemGem"))
	{
		ItemGem* item = (ItemGem*)app->entityManager->CreateEntity(EntityType::ITEMGEM);
		item->parameters = itemNode;
		item->Awake();
		itemsEnemies.Add(item);
	}

	for (pugi::xml_node itemNode = app->LoadConfig2().child("scene2").child("groundenemy"); itemNode; itemNode = itemNode.next_sibling("groundenemy"))
	{
		GroundEnemy* groundEnemy = (GroundEnemy*)app->entityManager->CreateEntity(EntityType::GROUND_ENEMY);
		groundEnemy->parameters = itemNode;
		groundEnemies.Add(groundEnemy);
		itemsEnemies.Add(groundEnemy);
	}

	for (pugi::xml_node itemNode = app->LoadConfig2().child("scene2").child("flyingenemy"); itemNode; itemNode = itemNode.next_sibling("flyingenemy"))
	{
		FlyingEnemy* flyingEnemy = (FlyingEnemy*)app->entityManager->CreateEntity(EntityType::FLYING_ENEMY);
		flyingEnemy->parameters = itemNode;
		flyingEnemies.Add(flyingEnemy);
		itemsEnemies.Add(flyingEnemy);
	}

	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = app->LoadConfig2().child("scene").child("player");
	app->entityManager->Enable();

	app->audio->PlayMusic("Assets/Audio/Music/song2.ogg");
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

	stopcamera = true;
	level2 = true;
	app->render->camera.x = 0;
	//checkpoints
	checkpoint1 = true;
	checkpoint2 = true;
	checkpoint3 = true;
	CheckpointTaken = app->tex->Load("Assets/Textures/flag.png");
	Checkpoint = app->tex->Load("Assets/Textures/flag_red.png");
	checkpointFxId = app->audio->LoadFx("Assets/Audio/Fx/checkpoint.ogg");

	//menu buttons
	uint w, h;
	app->win->GetWindowSize(w, h);
	resume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Resume", { (int)w / 2 - 50,(int)h / 2 - 80,104,44 }, this);
	resume->parameters = app->LoadConfig2().child("scene").child("button");
	settings = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Settings", { (int)w / 2 - 50,(int)h / 2 - 40,104,44 }, this);
	settings->parameters = app->LoadConfig2().child("scene").child("button");
	backToTitle = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "Back to title", { (int)w / 2 - 50,(int)h / 2,104,44 }, this);
	backToTitle->parameters = app->LoadConfig2().child("scene").child("button");
	Exit = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "Exit", { (int)w / 2 - 50,(int)h / 2 + 40,104,44 }, this);
	Exit->parameters = app->LoadConfig2().child("scene").child("button");
	//settings buttons

	musicVolume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 11, "Music volume", { (int)w / 2 - 50,(int)h / 2 - 80,104,44 }, this);
	musicVolume->parameters = app->LoadConfig2().child("scene").child("button");

	musicVolumeSlider = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 5, "", { (int)w / 2 + 50,(int)h / 2 - 70,30,27 }, this, { (int)w / 2 + 50,(int)h / 2 - 70,100,44 });
	musicVolumeSlider->parameters = app->LoadConfig2().child("scene").child("slider");

	fxVolume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 12, "Fx Volume", { (int)w / 2 - 50,(int)h / 2 - 40,104,44 }, this);
	fxVolume->parameters = app->LoadConfig2().child("scene").child("button");

	fxVolumeSlider = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 6, "", { (int)w / 2 + 50,(int)h / 2 - 30,30,27 }, this, { (int)w / 2 + 50,(int)h / 2 - 30,100,44 });
	fxVolumeSlider->parameters = app->LoadConfig2().child("scene").child("slider");

	fullscreen = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "Fullscreen", { (int)w / 2 - 50,(int)h / 2,104,44 }, this);
	fullscreen->parameters = app->LoadConfig2().child("scene").child("button");

	fullscreenCheck = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 10, "", { (int)w / 2 - 50 + 104,(int)h / 2,104,36 }, this);
	fullscreenCheck->parameters = app->LoadConfig2().child("scene").child("checkbox");

	vsync = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, "Vsync", { (int)w / 2 - 50,(int)h / 2 + 40,104,44 }, this);
	vsync->parameters = app->LoadConfig2().child("scene").child("button");

	VsyncCheck = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 8, "", { (int)w / 2 - 50 + 104,(int)h / 2 + 40,40,36 }, this);
	VsyncCheck->parameters = app->LoadConfig2().child("scene").child("checkbox");


	//activate gui
	app->guiManager->Enable();

	app->SaveGameRequest();

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
	if (exit) {
		return false;
	}
	//if 5 mins pass u lose
	if (app->hud->playtime > 300) {
		player->isdead = true;
		app->hud->playtime = 0;
	}
	//winstate
	if (player->GetWinState() == true) {
		app->map2->Disable();
		app->hud->Disable();
		app->fade->FadeToBlack1((Module*)app->entityManager, (Module*)app->scenewin, 20);
		app->render->camera.x = 0;
		level2 = false;
	}

	if (returnToIntro) {
		app->hud->Disable();
		app->fade->FadeToBlack1((Module*)app->entityManager, (Module*)app->intro, 20);
		app->scene2->Disable();
		app->map2->Disable();
		app->guiManager->Disable();
		app->render->camera.x = 0;
		app->audio->PlayMusic("");
		app->physics->pause = false;
		returnToIntro = false;
		app->SaveGameRequest();
	}

	if (player->isdead) {
		app->map2->Disable();
		app->hud->Disable();
		app->fade->FadeToBlack1((Module*)app->entityManager, (Module*)app->scenedeath, 20);
		app->scene2->Disable();
		app->render->camera.x = 0;
		app->render->camera.y = 0;
		app->audio->PlayMusic("");
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
		app->guiManager->Disable();
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

	if (app->render->camera.x < 300 && stopcamera == true) {
		app->render->camera.x = 0;
	}

	if (player->position.x >= 300 && player->position.x < 3340 && stopcamera == true) {
		app->render->camera.x = -player->position.x + 300;
	}

	//buttons
	if (app->guiManager->menu == true) {
		ListItem<GuiControl*>* control = app->guiManager->guiControlsList.start;

		while (control != nullptr)
		{
			for (int i = 1; i < 5; ++i) {
				if (control->data->id == i) {
					control->data->enabled = true;
				}
			}
			control = control->next;
		}
	}

	if (app->guiManager->menu == false) {
		ListItem<GuiControl*>* control = app->guiManager->guiControlsList.start;

		while (control != nullptr)
		{
			for (int i = 1; i < 5; ++i) {
				if (control->data->id == i) {
					control->data->enabled = false;
				}
			}
			control = control->next;
		}
	}

	if (app->guiManager->settings == true) {
		ListItem<GuiControl*>* control = app->guiManager->guiControlsList.start;

		while (control != nullptr)
		{
			for (int i = 5; i < 13; ++i) {
				if (control->data->id == i) {
					control->data->enabled = true;
				}
			}
			control = control->next;
		}
	}

	if (app->guiManager->settings == false) {
		ListItem<GuiControl*>* control = app->guiManager->guiControlsList.start;

		while (control != nullptr)
		{
			for (int i = 5; i < 13; ++i) {
				if (control->data->id == i) {
					control->data->enabled = false;
				}
			}
			control = control->next;
		}
	}

	// Draw map
	if (level2) {
		app->map2->Draw();
	}
	if (app->physics->getPause()) {
		return true;
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

				if (app->physics->getDebug()) app->render->DrawTexture(mouseTileTex, pos.x, pos.y);
			}
			// L12: Debug pathfinding 
			iPoint originScreen = app->map2->MapToWorld(destination.x, destination.y);
			if (app->physics->getDebug()) app->render->DrawTexture(originTex, originScreen.x, originScreen.y);
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

				if (app->physics->getDebug()) app->render->DrawTexture(mouseTileTex, pos.x, pos.y);
			}
			// L12: Debug pathfinding didn't change the names cause xd
			iPoint originScreen = app->map2->MapToWorld(destination.x, destination.y);
			if (app->physics->getDebug()) app->render->DrawTexture(originTex, originScreen.x, originScreen.y);
			flyingEnemyItem = flyingEnemyItem->next;
			enemyBodyItem = enemyBodyItem->next;
		}
		else {
			enemyBodyItem = enemyBodyItem->next;
			flyingEnemyItem = flyingEnemyItem->next;
		}
	}

	//checkpoints
	if (checkpoint1 == true) {
		if (player->position.x >= 1225 && player->position.x < 1300 && player->position.y == 352) {
			app->SaveGameRequest();
			checkpoint1 = false;
			app->audio->PlayFx(checkpointFxId);
		}
		app->render->DrawTexture(Checkpoint, 1225, 320);
	}

	if (checkpoint1 == false) {
		app->render->DrawTexture(CheckpointTaken, 1225, 320);
	}

	if (checkpoint2 == true) {
		if (player->position.x >= 1909 && player->position.x < 1915 && player->position.y == 448) {
			app->SaveGameRequest();
			checkpoint2 = false;
			app->audio->PlayFx(checkpointFxId);
		}
		app->render->DrawTexture(Checkpoint, 1915, 416);
	}

	if (checkpoint2 == false) {
		app->render->DrawTexture(CheckpointTaken, 1915, 416);
	}

	if (checkpoint3 == true) {
		if (player->position.x >= 2945 && player->position.x < 2950 && player->position.y == 384) {
			app->SaveGameRequest();
			checkpoint3 = false;
			app->audio->PlayFx(checkpointFxId);
		}
		app->render->DrawTexture(Checkpoint, 2950, 352);
	}

	if (checkpoint3 == false) {
		app->render->DrawTexture(CheckpointTaken, 2950, 352);
	}	
	return true;
}

// Called each loop iteration
bool Scene2::PostUpdate()
{
	bool ret = true;
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		if (app->guiManager->settings == false) {
			app->guiManager->activateMenu();
			app->physics->Pause();
		}
		else {
			app->guiManager->settings = false;
			app->guiManager->menu = true;
		}
	}
	app->guiManager->Draw();
	//debug of the path basically shows a fking red line xdd
	if (app->physics->getDebug())
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
	app->render->camera.x = 0;

	groundEnemies.Clear();
	flyingEnemies.Clear();
	app->tex->UnLoad(img);
	app->tex->UnLoad(mouseTileTex);
	app->tex->UnLoad(originTex);
	app->tex->UnLoad(Checkpoint);
	img = nullptr;
	mouseTileTex = nullptr;
	originTex = nullptr;
	Checkpoint = nullptr;

	ListItem<Entity*>* item;
	item = itemsEnemies.start;

	while (item != NULL)
	{
		app->entityManager->DestroyEntity(item->data);
		RELEASE(item->data);
		item = item->next;
	}
	itemsEnemies.Clear();
	
	return true;
}

bool Scene2::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Event by %d ", control->id);

	switch (control->id)
	{
	case 1:
		app->guiManager->activateMenu();
		app->physics->Pause();
		LOG("Button 1 click");
		break;
	case 2:
		app->guiManager->settings = true;
		app->guiManager->menu = false;
		LOG("Button 2 click");
		break;
	case 3:
		returnToIntro = true;
		LOG("Button 3 click");
		break;
	case 4:
		exit = true;
		LOG("Button 4 click");
		break;
	case 5:
		LOG("Button 5 click");
		break;
	case 6:
		LOG("Button 6 click");
		break;
	case 7:
		LOG("Button 7 click");
		break;
	case 8:
		LOG("Button 8 click");
		/*app->render->Vsync = !app->render->Vsync;
		SDL_GL_SetSwapInterval(app->render->Vsync);*/
		/*if (app->render->Vsync == true) {

		}
		else {
			SDL_GL_SetSwapInterval(app->render->Vsync);
		}*/
		break;
	case 9:
		LOG("Button 9 click");
		break;
	case 10:
		LOG("Button fullscreen click");
		app->win->fullscreen = !app->win->fullscreen;
		if (app->win->fullscreen) {
			SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN);
		}
		else {
			SDL_SetWindowFullscreen(app->win->window, 0);
		}
		break;
	}

	return true;
}
