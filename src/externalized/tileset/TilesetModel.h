#pragma once

#include "Json.h"
#include "TileDat.h"

#include <string_theory/string>

#include <functional>
#include <map>
#include <vector>

struct ItemModel;

// One tileset of tilesets.json (docs/tilesets-json-and-item-tile-types-plan.md):
// its name, ambience and the picture file of each tile type it has its own file
// for. A tile type left out uses the file of tileset 0 (GetAdjustedTilesetResource).
struct TilesetModel
{
	uint8_t    index;
	ST::string name;
	uint8_t    ambientId;
	std::map<TileTypeDefines, ST::string> files;

	// One entry; an unknown tile type name is a DataError.
	static TilesetModel deserialize(JsonValue const& json);

	// The whole file: 50 (vanilla) or 70 (Unfinished Business) tilesets with
	// the indices 0..n-1 in order, else a DataError.
	static std::vector<TilesetModel> deserializeAll(JsonValue const& json);
};

// docs/tilesets-json-and-item-tile-types-plan.md, stage 2 (T7): every item whose
// ground picture uses a new item tile type (IsNewItemTileType) needs that type's
// file in the default tileset (0, or 50 for Unfinished Business); fileExists(tileset,
// file) looks for it. Else a DataError naming the item and the file. The older item
// tile types are not checked.
void CheckItemTileFiles(std::vector<const ItemModel*> const& items,
	std::vector<TilesetModel> const& tilesets,
	std::function<bool(uint8_t tileset, ST::string const& file)> const& fileExists);
