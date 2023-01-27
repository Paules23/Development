#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Map.h"
#include "Physics.h"
#include "FadeToBlack.h"
#include "Map2.h"
#include "Scene2.h"
#include "GuiManager.h"
#include "SceneIntro.h"
#include "HUD.h"

#include "Defs.h"
#include "Log.h"


Scene::Scene(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	level1 = true;
	return true;
}

// Called before the first frame
bool Scene::Start()
{	
	
	LOG("Loading Scene");

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = app->LoadConfig2().child("scene").child("itemCoin"); itemNode; itemNode = itemNode.next_sibling("itemCoin"))
	{
		ItemCoin* item = (ItemCoin*)app->entityManager->CreateEntity(EntityType::ITEMCOIN);
		item->parameters = itemNode;
		item->Awake();
	}

	for (pugi::xml_node itemNode = app->LoadConfig2().child("scene").child("itemHeart"); itemNode; itemNode = itemNode.next_sibling("itemHeart"))
	{
		ItemHeart* item = (ItemHeart*)app->entityManager->CreateEntity(EntityType::ITEMHEART);
		item->parameters = itemNode;
		item->Awake();
	}

	for (pugi::xml_node itemNode = app->LoadConfig2().child("scene").child("itemGem"); itemNode; itemNode = itemNode.next_sibling("itemGem"))
	{
		ItemGem* item = (ItemGem*)app->entityManager->CreateEntity(EntityType::ITEMGEM);
		item->parameters = itemNode;
		item->Awake();
	}

	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = app->LoadConfig2().child("scene").child("player");
	app->entityManager->Enable();

	app->audio->PlayMusic("Assets/Audio/Music/song.ogg");
		
	// L03: DONE: Load map
	app->map->Load();

	// L04: DONE 7: Set the window title with map/tileset info
	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	app->win->SetTitle(title.GetString());
	app->render->camera.x = 0;

	stopcamera = true;
	level1 = true;
	exit = false;
	checkpoint = true;
	returnToIntro = false;
	CheckpointTaken = app->tex->Load("Assets/Textures/flag.png");
	Checkpoint = app->tex->Load("Assets/Textures/flag_red.png");
	checkpointFxId = app->audio->LoadFx("Assets/Audio/Fx/checkpoint.ogg");
	

	//menu buttons
	uint w, h;
	app->win->GetWindowSize(w, h);
	resume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Resume", { (int)w / 2 - 50,(int)h / 2 - 100,104,44 }, this);
	resume->parameters = app->LoadConfig2().child("scene").child("button");
	settings = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Settings", { (int)w / 2 - 50,(int)h / 2 -50,104,44 }, this);
	settings->parameters = app->LoadConfig2().child("scene").child("button");
	backToTitle = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "Back to title", { (int)w / 2 - 50,(int)h / 2,104,44 }, this);
	backToTitle->parameters = app->LoadConfig2().child("scene").child("button");
	Exit = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "Exit", { (int)w / 2 - 50,(int)h / 2 + 50,104,44 }, this);
	Exit->parameters = app->LoadConfig2().child("scene").child("button");
	//settings buttons

	musicVolume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 11, "Music volume", { (int)w / 2 - 50,(int)h / 2 - 100,104,44 }, this);
	musicVolume->parameters = app->LoadConfig2().child("scene").child("button");

	musicVolumeSlider = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 5, "", { (int)w / 2 + 50,(int)h / 2 - 90,30,27 }, this, { (int)w / 2 + 50,(int)h / 2 - 90,100,44 });
	musicVolumeSlider->parameters = app->LoadConfig2().child("scene").child("slider");

	fxVolume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 12, "Fx Volume", { (int)w / 2 - 50,(int)h / 2 - 50,104,44 }, this);
	fxVolume->parameters = app->LoadConfig2().child("scene").child("button");

	fxVolumeSlider = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 6, "", { (int)w / 2 + 50,(int)h / 2 - 40,30,27 }, this, { (int)w / 2 + 50,(int)h / 2 - 40,100,44 });
	fxVolumeSlider->parameters = app->LoadConfig2().child("scene").child("slider");

	fullscreen = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "Fullscreen", { (int)w / 2 - 50,(int)h / 2,104,44 }, this);
	fullscreen->parameters = app->LoadConfig2().child("scene").child("button");

	fullscreenCheck = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 10, "", { (int)w / 2 - 50 +104,(int)h / 2,104,36 }, this);
	fullscreenCheck->parameters = app->LoadConfig2().child("scene").child("checkbox");

	vsync = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, "Vsync", { (int)w / 2 - 50,(int)h / 2 + 50,104,44 }, this);
	vsync->parameters = app->LoadConfig2().child("scene").child("button");

	VsyncCheck = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 8, "", { (int)w / 2 - 50 +104,(int)h / 2 + 50,40,36 }, this);
	VsyncCheck->parameters = app->LoadConfig2().child("scene").child("checkbox");
	

	//activate gui
	app->guiManager->Enable();


	app->SaveGameRequest();

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//if 5 mins pass u lose
	if (app->hud->playtime > 300) {
		player->isdead = true;
		app->hud->playtime = 0;
	}

	if (returnToIntro) {
		app->hud->Disable();
		app->fade->FadeToBlack1((Module*)app->entityManager, (Module*)app->intro, 20);
		app->scene->Disable();
		app->map->Disable();
		app->guiManager->Disable();
		app->render->camera.x = 0;
		app->audio->PlayMusic("");
		app->physics->pause = false;
		returnToIntro = false;
		app->SaveGameRequest();
	}
	if (exit) {
		app->guiManager->menu = false;
		app->physics->Pause();
		return false;
	}

	if (player->isdead) {
		app->map->Disable();
		app->hud->Disable();
		app->fade->FadeToBlack1((Module*)app->entityManager, (Module*)app->scenedeath, 20);
		app->scene->Disable();
		app->map->Disable();
		app->guiManager->Disable();
		app->render->camera.x = 0;
		app->audio->PlayMusic("");
	}

	if (player->GetWinState() == true) {
		app->map->Disable();
		app->fade->FadeToBlack1((Module*)app->entityManager, (Module*)app->scene2, 20);
		app->map2->Enable();
		app->scene->Disable();
		app->guiManager->Disable();
		app->render->camera.x = 0;
		level1 = false;
		app->scene2->level2 = true;
	}

	// ALL DEBUG
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();
		

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		app->LoadGameRequest();
		if (player->position.x <= 953 && player->position.x >= 943) {
			app->render->camera.x = 0;
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
		app->render->camera.x += ceil(CAMERASPEED * dt);
		stopcamera = false;
	}
	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
		app->render->camera.x -= ceil(CAMERASPEED * dt);
		stopcamera = false;
	}
	if (app->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
		int xd = SDL_GL_SetSwapInterval(0);
		if (xd == -1) {
			app->physics->Pause();
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN) {
		int xd = SDL_GL_SetSwapInterval(1);
		if (xd == -1) {
			app->physics->Pause();
		}
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
	if (player->position.x >= 954 && player->position.x < 964) {
		app->render->camera.x = -954;

	}
	if (player->position.x <= 953 && player->position.x >= 943) {
		app->render->camera.x = 0;
	}

	if (player->position.x > 964 + app->render->camera.w) {
		if (player->position.x >= 2470 && stopcamera == true) {
			app->render->camera.x = -2170;
		}

		if (player->position.x >= 300 && player->position.x < 2470 && stopcamera == true) {
			app->render->camera.x = -player->position.x + 300;
		}
	}
	
	//godmode
	ListItem<PhysBody*>* colliderItem;
	colliderItem = app->map->mapColliders.start;
	while (colliderItem != NULL)
	{
		if (app->physics->getGodMode()) {
			colliderItem->data->body->SetActive(false);

		}
		else {
			colliderItem->data->body->SetActive(true);
		}
		colliderItem = colliderItem->next;
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
				if (control->data->id == i ) {
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
	if (level1) {
		app->map->Draw();
	}

	app->guiManager->Draw();
	

	//checkpoints
	if (checkpoint == true  ) {
		if (player->position.x >= 1013 && player->position.x < 1020) {
			app->SaveGameRequest();
			checkpoint = false;
			app->audio->PlayFx(checkpointFxId);
		}
		app->render->DrawTexture(Checkpoint, 1013, 480);
	}

	if (checkpoint == false) {
		app->render->DrawTexture(CheckpointTaken, 1013, 480);
	}
	
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
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

	return ret;
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
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
		app->render->Vsync = !app->render->Vsync;
		SDL_GL_SetSwapInterval(app->render->Vsync);
		if (app->render->Vsync == true) {
			
		}
		else {
			SDL_GL_SetSwapInterval(app->render->Vsync);
		}
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

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	app->tex->UnLoad(Checkpoint);
	app->tex->UnLoad(CheckpointTaken);

	return true;
}
