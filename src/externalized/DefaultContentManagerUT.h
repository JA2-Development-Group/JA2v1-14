#pragma once

#include "DefaultContentManager.h"
#ifdef WITH_UNITTESTS
#include "gtest/gtest.h"
extern ContentManager * GCM;
#endif

struct EngineOptions;

class DefaultContentManagerUT : public DefaultContentManager
{
	DefaultContentManagerUT(RustPointer<EngineOptions> engineOptions);

public:
	/** Create DefaultContentManager for usage in unit testing. */
	static DefaultContentManagerUT* createDefaultCMForTesting();

	bool loadGameData() override;

protected:
	/** The test data has no game pictures: the ground picture files of the new
	    item tile types count as there (CheckItemTileFiles has its own tests). */
	bool itemTileFileExists(uint8_t, const ST::string&) const override { return true; }

public:
	/** Sets the "image_png_override" engine option. */
	void setImagePNGOverride(bool value);

	/** Registers an item as a data file would (index block and duplicate checks). */
	void registerItemForTesting(const ItemModel* item, ItemDataFile file) { registerItem(item, file, JsonObject().toValue()); }

#ifdef WITH_UNITTESTS

// Helper class to ease writing unit test that use the
// UT content manager. To use this class, first define
// an alias for this class:
//
// using MyTest = DefaultContentManagerUT::BaseTest;
//
// Then use
//
// TEST_F(MyTest, Testname) {}
//
// for the actual test.

class BaseTest : public testing::Test
{
protected:
	ContentManager * oldGCM;
	DefaultContentManagerUT * cm;

	BaseTest() :
		oldGCM{ GCM },
		cm{ DefaultContentManagerUT::createDefaultCMForTesting() }
	{
		GCM = cm;
		cm->loadGameData();
	}

	~BaseTest()
	{
		GCM = oldGCM;
		delete cm;
	}
};

#endif
};
