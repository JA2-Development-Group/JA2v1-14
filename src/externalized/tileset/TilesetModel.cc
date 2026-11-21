#include "TilesetModel.h"

#include "Exceptions.h"
#include "TilesetTileIndexModel.h"
#include "World_Tileset_Enums.h"

#include <string_theory/format>

#include <stdexcept>


TilesetModel TilesetModel::deserialize(JsonValue const& json)
{
	JsonObject const obj = json.toObject();
	TilesetModel t;
	t.index     = static_cast<uint8_t>(obj.GetUInt("index"));
	t.name      = obj.GetString("name");
	t.ambientId = static_cast<uint8_t>(obj.GetUInt("ambientId"));

	JsonObject const files = obj["files"].toObject();
	for (ST::string const& typeName : files.keys())
	{
		TileTypeDefines type;
		try
		{
			type = TileTypeFromName(typeName);
		}
		catch (std::runtime_error const&)
		{
			throw DataError(ST::format("tilesets.json: tileset {} ({}): unknown tile type `{}`",
				t.index, t.name, typeName));
		}
		t.files[type] = files.GetString(typeName.c_str());
	}
	return t;
}


std::vector<TilesetModel> TilesetModel::deserializeAll(JsonValue const& json)
{
	std::vector<TilesetModel> tilesets;
	for (JsonValue const& element : json.toVec())
	{
		TilesetModel t = deserialize(element);
		if (t.index != tilesets.size())
		{
			throw DataError(ST::format("tilesets.json: entry {} ({}) has the index {}, the indices must be 0..n-1 in order",
				tilesets.size(), t.name, t.index));
		}
		tilesets.push_back(std::move(t));
	}
	if (tilesets.size() != VANILLA_NUM_TILESETS && tilesets.size() != JA25_NUM_TILESETS)
	{
		throw DataError(ST::format("tilesets.json: {} tilesets, there must be {} (vanilla) or {} (Unfinished Business)",
			tilesets.size(), int(VANILLA_NUM_TILESETS), int(JA25_NUM_TILESETS)));
	}
	return tilesets;
}


#ifdef WITH_UNITTESTS
#include "gtest/gtest.h"
#include "DefaultContentManagerUT.h"

// The exported data (docs/tilesets-json-and-item-tile-types-plan.md, stage 1)
TEST(TilesetModel, loadsTheExportedData)
{
	DefaultContentManager* const cm = DefaultContentManagerUT::createDefaultCMForTesting();
	ASSERT_TRUE(cm->loadGameData());

	auto const& tilesets = cm->getTilesets();
	ASSERT_EQ(tilesets.size(), size_t(VANILLA_NUM_TILESETS));

	// GENERIC 1 has a file for every tile type, the item pictures among them
	TilesetModel const& generic = tilesets[GENERIC_1];
	EXPECT_EQ(generic.name, "GENERIC 1");
	EXPECT_EQ(generic.files.size(), size_t(NUMBEROFTILETYPES));
	EXPECT_EQ(generic.files.at(FIRSTTEXTURE), "earth.sti");
	EXPECT_EQ(generic.files.at(GUNS),    "smguns.sti");
	EXPECT_EQ(generic.files.at(P1ITEMS), "smp1items.sti");
	EXPECT_EQ(generic.files.at(P2ITEMS), "smp2items.sti");
	EXPECT_EQ(generic.files.at(P3ITEMS), "smp3items.sti");

	// another tileset has only some files of its own
	TilesetModel const& caves = tilesets[CAVES_1];
	EXPECT_EQ(caves.name, "CAVES 1");
	EXPECT_EQ(caves.files.at(FIRSTTEXTURE), "cvflr4.sti");
	EXPECT_EQ(caves.files.count(GUNS), 0u);

	for (size_t i = 0; i < tilesets.size(); ++i) EXPECT_EQ(tilesets[i].index, i);

	delete cm;
}

static ST::string TilesetsJson(int count, ST::string const& firstFiles, int firstIndex = 0)
{
	ST::string json = "[";
	for (int i = 0; i < count; ++i)
	{
		if (i > 0) json += ",";
		json += "{\"index\": " + ST::string::from_int(i == 0 ? firstIndex : i) +
			", \"name\": \"T" + ST::string::from_int(i) +
			"\", \"ambientId\": 0, \"files\": " + (i == 0 ? firstFiles : ST::string("{}")) + "}";
	}
	return json + "]";
}

TEST(TilesetModel, checks)
{
	// a valid file
	auto const ok = TilesetModel::deserializeAll(JsonValue::deserialize(
		TilesetsJson(VANILLA_NUM_TILESETS, R"({"GUNS": "smguns.sti", "P3ITEMS": "smp3items.sti"})")));
	ASSERT_EQ(ok.size(), size_t(VANILLA_NUM_TILESETS));
	EXPECT_EQ(ok[0].files.at(GUNS), "smguns.sti");
	EXPECT_EQ(ok[0].files.at(P3ITEMS), "smp3items.sti");
	EXPECT_EQ(ok[1].files.size(), 0u);

	// an unknown tile type name
	EXPECT_THROW(TilesetModel::deserializeAll(JsonValue::deserialize(
		TilesetsJson(VANILLA_NUM_TILESETS, R"({"NOSUCHTYPE": "x.sti"})"))), DataError);
	// indices out of order
	EXPECT_THROW(TilesetModel::deserializeAll(JsonValue::deserialize(
		TilesetsJson(VANILLA_NUM_TILESETS, "{}", 5))), DataError);
	// neither 50 nor 70 tilesets
	EXPECT_THROW(TilesetModel::deserializeAll(JsonValue::deserialize(
		TilesetsJson(3, "{}"))), DataError);
	EXPECT_NO_THROW(TilesetModel::deserializeAll(JsonValue::deserialize(
		TilesetsJson(JA25_NUM_TILESETS, "{}"))));
}
#endif
