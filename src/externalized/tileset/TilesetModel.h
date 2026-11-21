#pragma once

#include "Json.h"
#include "TileDat.h"

#include <string_theory/string>

#include <map>
#include <vector>

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
