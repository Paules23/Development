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
	level1 = true;
	

	return true;
}

// Called before the first frame
bool Scene::Start()
{	
	
	LOG("Loading Scene");

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

	//menu buttons
	uint w, h;
	app->win->GetWindowSize(w, h);
	resume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Resume", { (int)w / 2 - 50,(int)h / 2 - 60,100,20 }, this);
	settings = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Settings", { (int)w / 2 - 50,(int)h / 2 -30,100,20 }, this);
	backToTitle = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "Back to title", { (int)w / 2 - 50,(int)h / 2,100,20 }, this);
	Exit = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "Exit", { (int)w / 2 - 50,(int)h / 2 + 30,100,20 }, this);

	//settings buttons
	musicVolume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "Music volume", { (int)w / 2 - 50,(int)h / 2 - 60,100,20 }, this);
	fxVolume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "Fx Volume", { (int)w / 2 - 50,(int)h / 2 - 30,100,20 }, this);
	fullscreenmode = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "Fullscreen mode", { (int)w / 2 - 50,(int)h / 2,100,20 }, this);
	Vsync = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, "Vsync", { (int)w / 2 - 50,(int)h / 2 + 30,100,20 }, this);

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

	if (exit) {
		app->guiManager->menu = false;
		app->physics->Pause();
		return false;
	}

	if (player->GetWinState() == true) {
		app->map->Disable();
		app->fade->FadeToBlack1((Module*)app->entityManager, (Module*)app->scene2, 20);
		app->map2->Enable();
		app->scene->Disable();
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

	//debug keys for change of levels
	/*if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		app->map->Disable();
		app->fade->FadeToBlack1((Module*)app->entityManager, (Module*)app->scene2, 20);
		app->map2->Enable();
		app->scene->Disable();
		app->render->camera.x = 0;
		level1 = false;
		app->scene2->level2 = true;
	}*/
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
			for (int i = 5; i < 9; ++i) {
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
			for (int i = 5; i < 9; ++i) {
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
		app->fade->FadeToBlack1(this, (Module*)app->intro, 20);
		app->entityManager->Disable();
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
		break;
	case 9:
		LOG("Button 9 click");
		break;
	}

	return true;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	app->tex->UnLoad(img);

	return true;
}
