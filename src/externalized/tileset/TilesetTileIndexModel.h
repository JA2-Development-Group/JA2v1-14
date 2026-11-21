#pragma once

#include "TileDat.h"
#include "Json.h"

#include <string_theory/string>

// A tile type and its name in the JSON data ("GUNS", "P1ITEMS" ...).
// TileTypeFromName throws std::runtime_error for an unknown name; TileTypeName
// gives "UNKNOWNTILETYPE" for a number out of range.
TileTypeDefines TileTypeFromName(ST::string const& name);
ST::string TileTypeName(TileTypeDefines type);

class TilesetTileIndexModel {
	public:
		constexpr TilesetTileIndexModel(TileTypeDefines type, uint16_t subIndexArg) noexcept
			: tileType{ type }, subIndex{ subIndexArg }
		{
		}

		TileTypeDefines tileType;
		uint16_t subIndex;

		static TilesetTileIndexModel deserialize(const JsonValue &json);
		JsonValue serialize() const;
};
