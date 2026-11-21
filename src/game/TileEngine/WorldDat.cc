#include "WorldDat.h"
#include "TileDef.h"
#include "Tile_Surface.h"

#include "ContentManager.h"
#include "GameInstance.h"
#include "TilesetModel.h"

#include <string_theory/format>


// THIS FILE CONTAINS DEFINITIONS FOR TILESET FILES

UINT8 gubNumTilesets;
TILESET gTilesets[MAX_NUM_TILESETS];


static void SetTilesetFourTerrainValues(void);
static void SetTilesetThreeTerrainValues(void);
static void SetTilesetTwoTerrainValues(void);


// The tilesets come from tilesets.json (docs/tilesets-json-and-item-tile-types-plan.md),
// checked when the game data is loaded: 50 (vanilla) or 70 (Unfinished Business)
// tilesets, indices 0..n-1, known tile type names.
void InitEngineTilesets(void)
{
	auto const& tilesets = GCM->getTilesets();
	gubNumTilesets = static_cast<UINT8>(tilesets.size());
	if (gubNumTilesets == JA25_NUM_TILESETS) SLOGI("Loading tilesets for Unfinished Business");
	else                                     SLOGD("Loading vanilla tilesets");

	for (TilesetModel const& t : tilesets)
	{
		TILESET& ts = gTilesets[t.index];
		ts.zName       = t.name;
		ts.ubAmbientID = t.ambientId;
		for (ST::string& file : ts.zTileSurfaceFilenames) file.clear();
		for (auto const& [type, file] : t.files) ts.zTileSurfaceFilenames[type] = file;
		ts.MovementCostFnc = SetTilesetOneTerrainValues;
	}

	// Set non-default callbacks
	gTilesets[CAVES_1      ].MovementCostFnc = SetTilesetTwoTerrainValues;
	gTilesets[AIRSTRIP     ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[DEAD_AIRSTRIP].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TEMP_14      ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TEMP_18      ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TEMP_19      ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TEMP_26      ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TEMP_27      ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TEMP_28      ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TEMP_29      ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TROPICAL_1   ].MovementCostFnc = SetTilesetFourTerrainValues;
	gTilesets[TEMP_20      ].MovementCostFnc = SetTilesetFourTerrainValues;
}


void SetTilesetOneTerrainValues( )
{
		// FIRST TEXUTRES
		gTileSurfaceArray[ FIRSTTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ SECONDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ THIRDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FOURTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FIFTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SIXTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SEVENTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ REGWATERTEXTURE ]->ubTerrainID = LOW_WATER;
		gTileSurfaceArray[ DEEPWATERTEXTURE ]->ubTerrainID = DEEP_WATER;

		// NOW ROADS
		gTileSurfaceArray[ FIRSTROAD ]->ubTerrainID = DIRT_ROAD;
		gTileSurfaceArray[ ROADPIECES ]->ubTerrainID = DIRT_ROAD;

		// NOW FLOORS
		gTileSurfaceArray[ FIRSTFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ SECONDFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ THIRDFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ FOURTHFLOOR ]->ubTerrainID = FLAT_FLOOR;

		// NOW ANY TERRAIN MODIFYING DEBRIS

}


static void SetTilesetTwoTerrainValues(void)
{
		// FIRST TEXUTRES
		gTileSurfaceArray[ FIRSTTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ SECONDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ THIRDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FOURTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FIFTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SIXTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SEVENTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ REGWATERTEXTURE ]->ubTerrainID = LOW_WATER;
		gTileSurfaceArray[ DEEPWATERTEXTURE ]->ubTerrainID = DEEP_WATER;

		// NOW ROADS
		gTileSurfaceArray[ FIRSTROAD ]->ubTerrainID = DIRT_ROAD;
		gTileSurfaceArray[ ROADPIECES ]->ubTerrainID = DIRT_ROAD;


		// NOW FLOORS
		gTileSurfaceArray[ FIRSTFLOOR ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ SECONDFLOOR ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ THIRDFLOOR ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FOURTHFLOOR ]->ubTerrainID = FLAT_GROUND;

}


static void SetTilesetThreeTerrainValues(void)
{
		// DIFFERENCE FROM #1 IS THAT ROADS ARE PAVED

		// FIRST TEXUTRES
		gTileSurfaceArray[ FIRSTTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ SECONDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ THIRDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FOURTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FIFTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SIXTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SEVENTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ REGWATERTEXTURE ]->ubTerrainID = LOW_WATER;
		gTileSurfaceArray[ DEEPWATERTEXTURE ]->ubTerrainID = DEEP_WATER;

		// NOW ROADS
		gTileSurfaceArray[ FIRSTROAD ]->ubTerrainID = PAVED_ROAD;
		gTileSurfaceArray[ ROADPIECES ]->ubTerrainID = PAVED_ROAD;


		// NOW FLOORS
		gTileSurfaceArray[ FIRSTFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ SECONDFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ THIRDFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ FOURTHFLOOR ]->ubTerrainID = FLAT_FLOOR;

		// NOW ANY TERRAIN MODIFYING DEBRIS

}


static void SetTilesetFourTerrainValues(void)
{
		// DIFFERENCE FROM #1 IS THAT FLOOR2 IS NOT FLAT_FLOOR BUT FLAT_GROUND

		// FIRST TEXUTRES
		gTileSurfaceArray[ FIRSTTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ SECONDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ THIRDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FOURTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FIFTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SIXTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SEVENTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ REGWATERTEXTURE ]->ubTerrainID = LOW_WATER;
		gTileSurfaceArray[ DEEPWATERTEXTURE ]->ubTerrainID = DEEP_WATER;

		// NOW ROADS
		gTileSurfaceArray[ FIRSTROAD ]->ubTerrainID = DIRT_ROAD;
		gTileSurfaceArray[ ROADPIECES ]->ubTerrainID = DIRT_ROAD;


		// NOW FLOORS
		gTileSurfaceArray[ FIRSTFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ SECONDFLOOR ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ THIRDFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ FOURTHFLOOR ]->ubTerrainID = FLAT_FLOOR;

		// NOW ANY TERRAIN MODIFYING DEBRIS

}


#ifdef WITH_UNITTESTS
#include "gtest/gtest.h"
#include "DefaultContentManagerUT.h"
#include "WorldDef.h"

// docs/tilesets-json-and-item-tile-types-plan.md, stage 1: the tilesets from
// tilesets.json; a tile type a tileset has no file for uses the file of tileset 0
class WorldDatTest : public DefaultContentManagerUT::BaseTest {};

TEST_F(WorldDatTest, tilesetsFromJson)
{
	InitEngineTilesets();
	EXPECT_EQ(gubNumTilesets, VANILLA_NUM_TILESETS);
	EXPECT_EQ(gTilesets[GENERIC_1].zName, "GENERIC 1");
	EXPECT_EQ(gTilesets[GENERIC_1].zTileSurfaceFilenames[GUNS], "smguns.sti");
	EXPECT_EQ(gTilesets[CAVES_1].zName, "CAVES 1");
	EXPECT_EQ(gTilesets[CAVES_1].zTileSurfaceFilenames[FIRSTTEXTURE], "cvflr4.sti");
	EXPECT_TRUE(gTilesets[CAVES_1].zTileSurfaceFilenames[GUNS].empty());
	EXPECT_NE(gTilesets[CAVES_1].MovementCostFnc, gTilesets[GENERIC_1].MovementCostFnc); // its own terrain costs

	auto own = GetAdjustedTilesetResource(CAVES_1, FIRSTTEXTURE);
	EXPECT_EQ(own.tilesetID, CAVES_1);
	EXPECT_EQ(own.resourceFileName, GCM->getTilesetResourceName(CAVES_1, "cvflr4.sti"));

	auto fallback = GetAdjustedTilesetResource(CAVES_1, GUNS);
	EXPECT_EQ(fallback.tilesetID, GENERIC_1);
	EXPECT_EQ(fallback.resourceFileName, GCM->getTilesetResourceName(GENERIC_1, "smguns.sti"));
}
#endif
