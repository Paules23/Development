#ifndef __MAP2_H__
#define __MAP2_H__

#include "Module.h"
#include "List.h"
#include "Point.h"
#include "Map.h"

#include "PugiXml\src\pugixml.hpp"

class Map2 : public Module
{
public:

	Map2(bool startEnabled);

	// Destructor
	virtual ~Map2();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load();

	// L05: DONE 8: Create a method that translates x,y coordinates from map positions to world positions
	iPoint MapToWorld(int x, int y) const;

	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

private:

	bool LoadMap(pugi::xml_node mapFile);

	// L04: DONE 4: Create and call a private function to load a tileset
	bool LoadTileSet(pugi::xml_node mapFile);

	// L05
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadAllLayers(pugi::xml_node mapNode);

	// L06: DONE 2
	TileSet* GetTilesetFromTileId(int gid) const;

	// L06: DONE 6: Load a group of properties 
	bool LoadProperties(pugi::xml_node& node, Properties& properties);

	void LoadCollisionsFromTileId();



public:

	// L04: DONE 1: Declare a variable data of the struct MapData
	MapData mapData;
	List<PhysBody*> mapColliders;
	List<PhysBody*> enemies;

private:
	SString map2FileName;
	SString map2Folder;

	bool mapLoaded;
};

#endif // __MAP2_H__
