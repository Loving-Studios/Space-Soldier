
#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Log.h"
#include "Physics.h"
#include <unordered_map>
#include <string>

#include <math.h>

Map::Map() : Module(), mapLoaded(false)
{
    name = "map";
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake()
{
    name = "map";
    LOG("Loading Map Parser");

    return true;
}

bool Map::Start() {

    return true;
}

bool Map::Update(float dt)
{
    bool ret = true;

    if (mapLoaded) {

        // L07 TODO 5: Prepare the loop to draw all tiles in a layer + DrawTexture()
        // iterate all tiles in a layer
        for (const auto& mapLayer : mapData.layers) {
            //Check if the property Draw exist get the value, if it's true draw the lawyer
            if (mapLayer->properties.GetProperty("Draw") != NULL && mapLayer->properties.GetProperty("Draw")->value == true) {

                Vector2D camPos = Vector2D(Engine::GetInstance().render->camera.x * -1, Engine::GetInstance().render->camera.y * -1);
                if (camPos.getX() < 0) camPos.setX(0);
                if (camPos.getY() < 0) camPos.setY(0);
                Vector2D camPosTile = WorldToMap(camPos.getX(), camPos.getY());

                Vector2D camSize = Vector2D(Engine::GetInstance().render->camera.w, Engine::GetInstance().render->camera.h);
                Vector2D camSizeTile = WorldToMap(camSize.getX(), camSize.getY());

                Vector2D limits = Vector2D(camPosTile.getX() + camSizeTile.getX(), camPosTile.getY() + camSizeTile.getY());
                if (limits.getX() > mapData.width) limits.setX(mapData.width);
                if (limits.getY() > mapData.height) limits.setY(mapData.height);

                for (int i = camPosTile.getX(); i < limits.getX(); i++) {
                    for (int j = camPosTile.getY(); j < limits.getY(); j++) {

                        // L07 TODO 9: Complete the draw function

                        //Get the gid from tile
                        int gid = mapLayer->Get(i, j);
                        //Check if the gid is different from 0 - some tiles are empty
                        if (gid != 0) {
                            //L09: TODO 3: Obtain the tile set using GetTilesetFromTileId
                            TileSet* tileSet = GetTilesetFromTileId(gid);
                            if (tileSet != nullptr) {
                                //Get the Rect from the tileSetTexture;
                                SDL_Rect tileRect = tileSet->GetRect(gid);
                                //Get the screen coordinates from the tile coordinates
                                Vector2D mapCoord = MapToWorld(i, j);
                                //Draw the texture
                                Engine::GetInstance().render->DrawTexture(tileSet->texture, mapCoord.getX(), mapCoord.getY(), &tileRect);
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}

// L09: TODO 2: Implement function to the Tileset based on a tile id
TileSet* Map::GetTilesetFromTileId(int gid) const
{
	TileSet* set = nullptr;

    for (const auto& tileset : mapData.tilesets) {
    	if (gid >= tileset->firstGid && gid < (tileset->firstGid + tileset->tileCount)) {
			set = tileset;
			break;
		}
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // L06: TODO 2: Make sure you clean up any memory allocated from tilesets/map
    for (const auto& tileset : mapData.tilesets) {
        delete tileset;
    }
    mapData.tilesets.clear();

    // L07 TODO 2: clean up all layer data
    for (const auto& layer : mapData.layers)
    {
        delete layer;
    }
    mapData.layers.clear();

    return true;
}

// Load new map
bool Map::Load(std::string path, std::string fileName)
{
    bool ret = false;

    // Assigns the name of the map file and the path
    mapFileName = fileName;
    mapPath = path;
    std::string mapPathName = mapPath + mapFileName;

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapPathName.c_str());

    if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
		ret = false;
    }
    else {

        // L06: TODO 3: Implement LoadMap to load the map properties
        // retrieve the paremeters of the <map> node and store the into the mapData struct
        mapData.width = mapFileXML.child("map").attribute("width").as_int();
        mapData.height = mapFileXML.child("map").attribute("height").as_int();
        mapData.tileWidth = mapFileXML.child("map").attribute("tilewidth").as_int();
        mapData.tileHeight = mapFileXML.child("map").attribute("tileheight").as_int();

        // L06: TODO 4: Implement the LoadTileSet function to load the tileset properties
       
        //Iterate the Tileset
        for(pugi::xml_node tilesetNode = mapFileXML.child("map").child("tileset"); tilesetNode!=NULL; tilesetNode = tilesetNode.next_sibling("tileset"))
		{
            //Load Tileset attributes
			TileSet* tileSet = new TileSet();
            tileSet->firstGid = tilesetNode.attribute("firstgid").as_int();
            tileSet->name = tilesetNode.attribute("name").as_string();
            tileSet->tileWidth = tilesetNode.attribute("tilewidth").as_int();
            tileSet->tileHeight = tilesetNode.attribute("tileheight").as_int();
            tileSet->spacing = tilesetNode.attribute("spacing").as_int();
            tileSet->margin = tilesetNode.attribute("margin").as_int();
            tileSet->tileCount = tilesetNode.attribute("tilecount").as_int();
            tileSet->columns = tilesetNode.attribute("columns").as_int();

			//Load the tileset image
			std::string imgName = tilesetNode.child("image").attribute("source").as_string();
            tileSet->texture = Engine::GetInstance().textures->Load((mapPath+imgName).c_str());

			mapData.tilesets.push_back(tileSet);
		}

        // L07: TODO 3: Iterate all layers in the TMX and load each of them
        for (pugi::xml_node layerNode = mapFileXML.child("map").child("layer"); layerNode != NULL; layerNode = layerNode.next_sibling("layer")) {

            // L07: TODO 4: Implement the load of a single layer 
            //Load the attributes and saved in a new MapLayer
            MapLayer* mapLayer = new MapLayer();
            mapLayer->id = layerNode.attribute("id").as_int();
            mapLayer->name = layerNode.attribute("name").as_string();
            mapLayer->width = layerNode.attribute("width").as_int();
            mapLayer->height = layerNode.attribute("height").as_int();

            //L09: TODO 6 Call Load Layer Properties
            LoadProperties(layerNode, mapLayer->properties);

            //Iterate over all the tiles and assign the values in the data array
            for (pugi::xml_node tileNode = layerNode.child("data").child("tile"); tileNode != NULL; tileNode = tileNode.next_sibling("tile")) {
                mapLayer->tiles.push_back(tileNode.attribute("gid").as_int());
            }

            //add the layer to the map
            mapData.layers.push_back(mapLayer);
        }
        // Define un mapa para convertir nombres de capas en enteros
        std::unordered_map<std::string, int> layerNameToId = {
            {"Checkpoint", 1},
            {"Jump", 2},
            {"SeAcabo", 3},
            {"lados", 4},
            {"CarcelVolador", 5},
            {"Win", 6},
            {"Tp", 7 }
        };

        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;

        for (pugi::xml_node layerNode = mapFileXML.child("map").child("objectgroup");
            layerNode != NULL;
            layerNode = layerNode.next_sibling("objectgroup")) {

            std::string layerName = layerNode.attribute("name").as_string();

            for (pugi::xml_node tileNode = layerNode.child("object");
                tileNode != NULL;
                tileNode = tileNode.next_sibling("object")) {

                // Asigna los valores correctos desde el XML
                x = tileNode.attribute("x").as_float();
                y = tileNode.attribute("y").as_float();
                width = tileNode.attribute("width").as_float();
                height = tileNode.attribute("height").as_float();

                PhysBody* rect = nullptr;
                auto it = layerNameToId.find(layerName);
                int layerId = (it != layerNameToId.end()) ? it->second : 0; // Valor por defecto: 0 para plataformas

                switch (layerId) {
                case 1: // Checkpoint
                    rect = Engine::GetInstance().physics.get()->CreateRectangleSensor(x + width / 2, y + height / 2, width, height, STATIC);
                    rect->ctype = ColliderType::CHECKPOINT;
                    break;

                case 2: // Jump
                    rect = Engine::GetInstance().physics.get()->CreateRectangleSensor(x + width / 2, y + height / 2, width, height, STATIC);
                    rect->ctype = ColliderType::JUMP;
                    break;

                case 3: // SeAcabo
                    rect = Engine::GetInstance().physics.get()->CreateRectangleSensor(x + width / 2, y + height / 2, width, height, STATIC);
                    rect->ctype = ColliderType::FIN;
                    break;
                case 4: // SeAcabo
                    rect = Engine::GetInstance().physics.get()->CreateRectangleSensor(x + width / 2, y + height / 2, width, height, STATIC);
                    rect->ctype = ColliderType::LADOS;
                    break;
                case 5: // Carcel
                    rect = Engine::GetInstance().physics.get()->CreateRectangleSensor(x + width / 2, y + height / 2, width, height, STATIC);
                    rect->ctype = ColliderType::CARCEL;
                    break;
                case 6: // WIN CONDITION
                    rect = Engine::GetInstance().physics.get()->CreateRectangleSensor(x + width / 2, y + height / 2, width, height, STATIC);
                    rect->ctype = ColliderType::WIN;
                    break;
                case 7: // TP TO LEVEL 2
                    rect = Engine::GetInstance().physics.get()->CreateRectangleSensor(x + width / 2, y + height / 2, width, height, STATIC);
                    rect->ctype = ColliderType::TEPE;
                    break;
                default: // Plataformas
                    rect = Engine::GetInstance().physics.get()->CreateRectangle(x + width / 2, y + height / 2, width, height, STATIC);
                    rect->ctype = ColliderType::PLATFORM;
                    break;
                }
            }
        }
        ret = true;

        if (mapFileXML) mapFileXML.reset();

    }

    mapLoaded = ret;
    return ret;
}

// L07: TODO 8: Create a method that translates x,y coordinates from map positions to world positions
Vector2D Map::MapToWorld(int x, int y) const
{
    Vector2D ret;

    ret.setX(x * mapData.tileWidth);
    ret.setY(y * mapData.tileHeight);

    return ret;
}

Vector2D Map::WorldToMap(int x, int y) {

    Vector2D ret(0, 0);

    ret.setX(x / mapData.tileWidth);
    ret.setY(y / mapData.tileHeight);

    return ret;
}


// L09: TODO 6: Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.propertyList.push_back(p);
    }

    return ret;
}

// L09: TODO 7: Implement a method to get the value of a custom property
Properties::Property* Properties::GetProperty(const char* name)
{
    for (const auto& property : propertyList) {
        if (property->name == name) {
			return property;
		}
    }

    return nullptr;
}

MapLayer* Map::GetNavigationLayer() {
    for (const auto& layer : mapData.layers) {
        if (layer->properties.GetProperty("Navigation") != NULL &&
            layer->properties.GetProperty("Navigation")->value) {
            return layer;
        }
    }

    return nullptr;
}
