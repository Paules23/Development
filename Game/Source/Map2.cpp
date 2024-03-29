#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map2.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"

#include <math.h>
#include "SDL_image/include/SDL_image.h"

Map2::Map2(bool startEnabled) : Module(startEnabled), mapLoaded(false)
{
    name.Create("map2");
}

// Destructor
Map2::~Map2()
{}

// Called before render is available
bool Map2::Awake(pugi::xml_node& config)
{
    LOG("Loading Map2 Parser");
    bool ret = true;

    map2FileName = config.child("mapfile").attribute("path").as_string();
    map2Folder = config.child("mapfolder").attribute("path").as_string();


    return ret;
}

bool Map2::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
    bool ret = false;
    ListItem<MapLayer*>* item;
    item = mapData.maplayers.start;

    for (item = mapData.maplayers.start; item != NULL; item = item->next)
    {
        MapLayer* layer = item->data;

        if (layer->properties.GetProperty("Navigation") == NULL) {
            continue;
        }
           

        uchar* map = new uchar[layer->width * layer->height];
        memset(map, 1, layer->width * layer->height);

        for (int y = 0; y < mapData.height; ++y)
        {
            for (int x = 0; x < mapData.width; ++x)
            {
                int i = (y * layer->width) + x;

                int tileId = layer->Get(x, y);
                TileSet* tileset = (tileId > 0) ? GetTilesetFromTileId(tileId) : NULL;

                if (tileset != NULL)
                {
                    map[i] = (tileId - tileset->firstgid) > 0 ? 0 : 1;
                }
            }
        }

        *buffer = map;
        width = mapData.width;
        height = mapData.height;
        ret = true;

        break;
    }

    return ret;
}


void Map2::Draw()
{
    if (mapLoaded == false)
        return;

    /*
    // L04: DONE 6: Iterate all tilesets and draw all their
    // images in 0,0 (you should have only one tileset for now)

    ListItem<TileSet*>* tileset;
    tileset = mapData.tilesets.start;

    while (tileset != NULL) {
        app->render->DrawTexture(tileset->data->texture,0,0);
        tileset = tileset->next;
    }
    */

    // L05: DONE 5: Prepare the loop to draw all tiles in a layer + DrawTexture()

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    while (mapLayerItem != NULL) {

        //L06: DONE 7: use GetProperty method to ask each layer if your �Draw� property is true.
        if (mapLayerItem->data->properties.GetProperty("Draw") != NULL && mapLayerItem->data->properties.GetProperty("Draw")->value) {

            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                    // L05: DONE 9: Complete the draw function
                    int gid = mapLayerItem->data->Get(x, y);

                    //L06: DONE 3: Obtain the tile set using GetTilesetFromTileId
                    TileSet* tileset = GetTilesetFromTileId(gid);

                    SDL_Rect r = tileset->GetTileRect(gid);
                    iPoint pos = MapToWorld(x, y);

                    app->render->DrawTexture(tileset->texture,
                        pos.x,
                        pos.y,
                        &r);
                }
            }
        }
        mapLayerItem = mapLayerItem->next;

    }
}

// L05: DONE 8: Create a method that translates x,y coordinates from map positions to world positions
iPoint Map2::MapToWorld(int x, int y) const
{
    iPoint ret;

    ret.x = x * mapData.tileWidth;
    ret.y = y * mapData.tileHeight;

    return ret;
}

// L06: DONE 2: Pick the right Tileset based on a tile id
TileSet* Map2::GetTilesetFromTileId(int gid) const
{
    ListItem<TileSet*>* item = mapData.tilesets.start;
    TileSet* set = NULL;

    while (item)
    {
        set = item->data;
        if (gid < (item->data->firstgid + item->data->tilecount))
        {
            break;
        }
        item = item->next;
    }

    return set;
}

// Called before quitting
bool Map2::CleanUp()
{
    LOG("Unloading map");

    // L04: DONE 2: Make sure you clean up any memory allocated from tilesets/map
    ListItem<TileSet*>* item;
    item = mapData.tilesets.start;

    while (item != NULL)
    {
        RELEASE(item->data);
        item = item->next;
    }
    mapData.tilesets.Clear();

    // L05: DONE 2: clean up all layer data
    // Remove all layers
    ListItem<MapLayer*>* layerItem;
    layerItem = mapData.maplayers.start;

    while (layerItem != NULL)
    {
        RELEASE(layerItem->data);
        layerItem = layerItem->next;
    }
    mapData.maplayers.Clear();
    //clean colliders

    ListItem<PhysBody*>* collisions;
    collisions = mapColliders.start;

    while (collisions != NULL)
    {
        collisions->data->body->GetWorld()->DestroyBody(collisions->data->body);
        RELEASE(collisions->data);
        collisions = collisions->next;
    }
    mapColliders.Clear();

    ListItem<PhysBody*>* enemyItem;
    enemyItem = enemies.start;

    while (enemyItem != NULL)
    {
        enemyItem->data->body->GetWorld()->DestroyBody(enemyItem->data->body);
        RELEASE(enemyItem->data); 
        enemyItem = enemyItem->next;
    }
    enemies.Clear();

    ListItem<PhysBody*>* items;
    items = Items.start;

    while (items != NULL)
    {
        items->data->body->GetWorld()->DestroyBody(items->data->body);
        RELEASE(items->data);
        items = items->next;
    }
    Items.Clear();

    return true;
}

// Load new map
bool Map2::Load()
{
    bool ret = true;

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(map2FileName.GetString());

    if (result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", map2FileName, result.description());
        ret = false;
    }

    if (ret == true)
    {
        ret = LoadMap(mapFileXML);
    }

    if (ret == true)
    {
        ret = LoadTileSet(mapFileXML);
    }

    // L05: DONE 4: Iterate all layers and load each of them
    if (ret == true)
    {
        ret = LoadAllLayers(mapFileXML.child("map"));
    }

    // L07 DONE 3: Create colliders
    // Later you can create a function here to load and create the colliders from the map
    if (ret == true) {
        LoadCollisionsFromTileId();
    }
    


    if (ret == true)
    {
        // L04: DONE 5: LOG all the data loaded iterate all tilesets and LOG everything

        LOG("Successfully parsed map XML file :%s", map2FileName.GetString());
        LOG("width : %d height : %d", mapData.width, mapData.height);
        LOG("tile_width : %d tile_height : %d", mapData.tileWidth, mapData.tileHeight);

        LOG("Tilesets----");

        ListItem<TileSet*>* tileset;
        tileset = mapData.tilesets.start;

        while (tileset != NULL) {
            LOG("name : %s firstgid : %d", tileset->data->name.GetString(), tileset->data->firstgid);
            LOG("tile width : %d tile height : %d", tileset->data->tileWidth, tileset->data->tileHeight);
            LOG("spacing : %d margin : %d", tileset->data->spacing, tileset->data->margin);
            tileset = tileset->next;
        }

        // L05: DONE 4: LOG the info for each loaded layer
        ListItem<MapLayer*>* mapLayer;
        mapLayer = mapData.maplayers.start;

        while (mapLayer != NULL) {
            LOG("id : %d name : %s", mapLayer->data->id, mapLayer->data->name.GetString());
            LOG("Layer width : %d Layer height : %d", mapLayer->data->width, mapLayer->data->height);
            mapLayer = mapLayer->next;
        }
    }

    if (mapFileXML) mapFileXML.reset();

    mapLoaded = ret;

    return ret;
}

// L04: DONE 3: Implement LoadMap to load the map properties
bool Map2::LoadMap(pugi::xml_node mapFile)
{
    bool ret = true;
    pugi::xml_node map = mapFile.child("map");

    if (map == NULL)
    {
        LOG("Error parsing map xml file: Cannot find 'map' tag.");
        ret = false;
    }
    else
    {
        //Load map general properties
        mapData.height = map.attribute("height").as_int();
        mapData.width = map.attribute("width").as_int();
        mapData.tileHeight = map.attribute("tileheight").as_int();
        mapData.tileWidth = map.attribute("tilewidth").as_int();
    }

    return ret;
}

// L04: DONE 4: Implement the LoadTileSet function to load the tileset properties
bool Map2::LoadTileSet(pugi::xml_node mapFile) {

    bool ret = true;

    pugi::xml_node tileset;
    for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
    {
        TileSet* set = new TileSet();

        // L04: DONE 4: Load Tileset attributes
        set->name = tileset.attribute("name").as_string();
        set->firstgid = tileset.attribute("firstgid").as_int();
        set->margin = tileset.attribute("margin").as_int();
        set->spacing = tileset.attribute("spacing").as_int();
        set->tileWidth = tileset.attribute("tilewidth").as_int();
        set->tileHeight = tileset.attribute("tileheight").as_int();
        set->columns = tileset.attribute("columns").as_int();
        set->tilecount = tileset.attribute("tilecount").as_int();

        // L04: DONE 4: Load Tileset image
        SString tmp("%s%s", map2Folder.GetString(), tileset.child("image").attribute("source").as_string());
        set->texture = app->tex->Load(tmp.GetString());

        mapData.tilesets.Add(set);
    }

    return ret;
}

// L05: DONE 3: Implement a function that loads a single layer layer
bool Map2::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
    bool ret = true;

    //Load the attributes
    layer->id = node.attribute("id").as_int();
    layer->name = node.attribute("name").as_string();
    layer->width = node.attribute("width").as_int();
    layer->height = node.attribute("height").as_int();

    //L06: DONE 6 Call Load Propoerties
    LoadProperties(node, layer->properties);

    //Reserve the memory for the data 
    layer->data = new uint[layer->width * layer->height];
    memset(layer->data, 0, layer->width * layer->height);

    //Iterate over all the tiles and assign the values
    pugi::xml_node tile;
    int i = 0;
    for (tile = node.child("data").child("tile"); tile && ret; tile = tile.next_sibling("tile"))
    {
        layer->data[i] = tile.attribute("gid").as_int();
        i++;
    }

    return ret;
}

// L05: DONE 4: Iterate all layers and load each of them
bool Map2::LoadAllLayers(pugi::xml_node mapNode) {
    bool ret = true;

    for (pugi::xml_node layerNode = mapNode.child("layer"); layerNode && ret; layerNode = layerNode.next_sibling("layer"))
    {
        //Load the layer
        MapLayer* mapLayer = new MapLayer();
        ret = LoadLayer(layerNode, mapLayer);

        //add the layer to the map
        mapData.maplayers.Add(mapLayer);
    }

    return ret;
}

// L06: DONE 6: Load a group of properties from a node and fill a list with it
bool Map2::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.list.Add(p);
    }

    return ret;
}

void Map2::LoadCollisionsFromTileId() {

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    ListItem<TileSet*>* mapTiles;

    while (mapLayerItem != NULL) {

        for (int x = 0; x < mapLayerItem->data->width; x++)
        {
            for (int y = 0; y < mapLayerItem->data->height; y++)
            {
                int gid = mapLayerItem->data->Get(x, y);
                TileSet* tileset = GetTilesetFromTileId(gid);
                SDL_Rect r = tileset->GetTileRect(gid);
                iPoint pos = MapToWorld(x, y);
                if (mapLayerItem->data->properties.GetProperty("Collision") != NULL && mapLayerItem->data->properties.GetProperty("Collision")->value == true) {
                    if (gid != NULL) {
                        PhysBody* c1 = app->physics->CreateRectangle(pos.x + 16, pos.y + 16, 32, 32, STATIC);
                        mapColliders.Add(c1);
                        c1->ctype = ColliderType::PLATFORM;
                    }
                }
                if (mapLayerItem->data->properties.GetProperty("Death") != NULL && mapLayerItem->data->properties.GetProperty("Death")->value == true) {
                    if (gid != NULL) {
                        PhysBody* c1 = app->physics->CreateRectangleSensor(pos.x + 16, pos.y + 16, 32, 32, STATIC);
                        mapColliders.Add(c1);
                        c1->ctype = ColliderType::DEATH;
                    }
                }
                if (mapLayerItem->data->properties.GetProperty("Win") != NULL && mapLayerItem->data->properties.GetProperty("Win")->value == true) {
                    if (gid != NULL) {
                        PhysBody* c1 = app->physics->CreateRectangleSensor(pos.x + 16, pos.y + 16, 32, 32, STATIC);
                        mapColliders.Add(c1);
                        c1->ctype = ColliderType::WIN;
                    }
                }
                if (mapLayerItem->data->properties.GetProperty("Platform") != NULL && mapLayerItem->data->properties.GetProperty("Platform")->value == true) {
                    if (gid != NULL) {
                        PhysBody* c1 = app->physics->CreateRectangle(pos.x + 16, pos.y + 4, 32, 8, STATIC);
                        mapColliders.Add(c1);
                        c1->ctype = ColliderType::PLATFORM;
                    }
                }
                if (mapLayerItem->data->properties.GetProperty("Jump") != NULL && mapLayerItem->data->properties.GetProperty("Jump")->value == true) {
                    if (gid != NULL) {
                        PhysBody* c1 = app->physics->CreateRectangleSensor(pos.x + 16, pos.y, 32, 1, STATIC);
                        mapColliders.Add(c1);
                        c1->ctype = ColliderType::GROUND;
                    }
                }
                if (mapLayerItem->data->properties.GetProperty("ChangeDir") != NULL && mapLayerItem->data->properties.GetProperty("ChangeDir")->value == true) {
                    if (gid != NULL) {
                        PhysBody* c1 = app->physics->CreateRectangleSensor(pos.x + 16, pos.y + 16, 34, 34, STATIC);
                        mapColliders.Add(c1);
                        c1->ctype = ColliderType::CHANGE_DIR;
                    }
                }
                if (mapLayerItem->data->properties.GetProperty("EnemyJump") != NULL && mapLayerItem->data->properties.GetProperty("EnemyJump")->value == true) {
                    if (gid != NULL) {
                        PhysBody* c1 = app->physics->CreateRectangleSensor(pos.x + 16, pos.y + 16, 20, 20, STATIC);
                        mapColliders.Add(c1);
                        c1->ctype = ColliderType::ENEMY_JUMP;
                    }
                }
            }
        }
        mapLayerItem = mapLayerItem->next;
    }
}

