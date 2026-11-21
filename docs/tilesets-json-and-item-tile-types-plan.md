# Tilesets in JSON and new item tile types - plan

Status: plan written 2026-11-21, decisions T1-T8 decided 2026-11-21 (all A, as
recommended), T9 added and decided 2026-11-21 (A). Implemented on the Weapons branch.
Tilesets in JSON and new item tile types COMPLETE (2026-11-21).

| Stage | Commit | State |
|---|---|---|
| 1 - tilesets from JSON | 4f434ede9 | game-verified |
| 2 - new item tile types | a0d9eeca7 | game-verified |
| 3 - subIndex from 0 for the new types (T9) | the commit after a0d9eeca7 | game-verified (USP9 ... DESERTEAGLE_50AE on NEW_GUNS) |

New items should get picture files of their own, sorted by kind:

| Kind | Inventory picture (medium) | Ground picture (small) | `tileGraphic.type` |
|---|---|---|---|
| new guns | `Interface/MD_NEW_GUNS.STI` | `Tilesets/0/SM_NEW_GUNS.STI` | `NEW_GUNS` |
| new items | `Interface/MD_NEW_ITEMS.STI` | `Tilesets/0/SM_NEW_ITEMS.STI` | `NEW_ITEMS` |
| more items | `Interface/MDP4ITEMS.STI` ... `MDP6ITEMS.STI` | `Tilesets/0/SMP4ITEMS.STI` ... `SMP6ITEMS.STI` | `P4ITEMS` ... `P6ITEMS` |

Example item data:

```
"inventoryGraphics": {
    "small": { "path": "interface/mdp4items.sti", "subImageIndex": 0 },
    "big":   { "path": "bigitems/p4item0.sti" }
},
"tileGraphic": { "type": "P4ITEMS", "subIndex": 0 }
```

For the new types both numbers count from 0, so the same number names the same
picture in the `MD` and the `SM` file (T9). The original types keep
`tileGraphic.subIndex` counted from 1 (`GUNS` 1 = the first picture of
`SMGUNS.STI` = `subImageIndex` 0 of `MDGUNS.STI`).

## 1. Current state (code facts)

### 1.1 Inventory pictures (`Interface/MD*.STI`): no code change needed

- At start `LoadInterfaceItemsGraphics` (`Interface_Items.cc`) loads every file
  named by any item's `inventoryGraphics.small.path`
  (`GCM->getAllSmallInventoryGraphicPaths()`); no file name is fixed in the code.
- So `interface/md_new_guns.sti`, `interface/mdp4items.sti` ... work today: put the
  file in `Data/Interface` and name it in the item data. `subImageIndex` counts
  from 0, up to 65,535 (docs/item-graphic-frame-limits-plan.md).
- A missing file or picture is logged and the "graphic not found" picture is shown
  (`GetSmallInventoryGraphicForItem`).

### 1.2 Ground pictures (`Tilesets/0/SM*.STI`): a fixed list of tile types

- An item lying on the ground is drawn from the tile type in
  `tileGraphic.type` (`TilesetTileIndexModel`): today `GUNS`, `P1ITEMS`, `P2ITEMS`,
  `P3ITEMS` (`TileDat.h`, enum `TileTypeDefines`, 151 types in all).
  `"type": "P4ITEMS"` stops the load with "Unknown TileType".
- Each type has a fixed number of pictures (`gNumTilesPerType`, `TileDat.cc`:
  `GUNS` 2000, `P1ITEMS` 2000, `P2ITEMS` 1000, `P3ITEMS` 1000) and a range of tile
  indices (`TileDefines`: `GUN1 ... GUN_LAST` ...). The tile database has 8,855
  tiles; a unit test keeps it below 32,767 (`TileDat, itemTileCounts`).
- Other per-type tables: the type names for JSON (`namesArray`,
  `TilesetTileIndexModel.cc`) and the schema
  (`types/tileset-tile-index.schema.yaml`), the readable names
  (`gTileSurfaceName`), the logical height (`gTileTypeLogicalHeight`), and the
  object layer flag for item tiles (`SetSpecificDatabaseValues`:
  `GUNS <= type <= P2ITEMS`, `type == P3ITEMS`). The tables are sized
  `NUMBEROFTILETYPES`; an entry left out of an initialiser silently becomes 0.
- Maps store a tile as its type number (one byte) and sub index
  (`WorldDef.cc`, `LoadWorld`); types from `FIRSTPOINTERS` on are skipped. Item
  tiles are never stored in maps: items are stored as items and their tiles are
  rebuilt from the item data when a sector loads.

### 1.3 The tile file names come from `ja2set.dat`

- `InitEngineTilesets` (`WorldDat.cc`) reads `BinaryData/ja2set.dat` from the
  original game data. Layout (checked on the user's vanilla copy,
  `Data/UNSLF/BinaryDataJA2/JA2SET.DAT`, 243,255 bytes, read to the last byte):
  - 1 byte: number of tilesets (50 vanilla, 70 Unfinished Business);
  - 4 bytes: number of file slots per tileset (151);
  - per tileset: name (32 bytes, e.g. "GENERIC 1", "CAVES 1"), ambient sound id
    (1 byte), then 151 file names of 32 bytes, slot = tile type number
    (slot 123 `smguns.sti`, 124 `smp1items.sti`, 125 `smp2items.sti`,
    127 `smp3items.sti`).
- The game stops when the slot count differs from `NUMBEROFTILETYPES`
  ("Number of tilesets slots in code does not match data file"). So a new tile
  type cannot be added without changing the binary file.
- Only "GENERIC 1" (tileset 0, `Tilesets/0`) fills all 151 slots; the other
  tilesets fill a few (CAVES 1: 21). An empty slot uses the GENERIC 1 file
  (`GetAdjustedTilesetResource`, `WorldDef.cc`). All item ground pictures are in
  `Tilesets/0`.
- Users of the data: `LoadTileSurfaces` (tile files of a sector), the overhead
  map (`Overhead_Map.cc`, the same names with a `t/` prefix; item types have no
  `t/` file, `t/grass.sti` stands in), the sector ambience
  (`gTilesets[].ubAmbientID`), the map editor (tileset names), the explosion code
  (`fount1.sti`).
- Nothing in the project reads `ja2set.dat` as JSON yet; `Ja2Set.dat.xml` is only
  used by the launcher to detect a 1.13 installation.

## 2. The proposal

- Stage 1: a one-time export of `ja2set.dat` to `assets/externalized/tilesets.json`;
  the game reads the JSON instead of the binary file. Files are named by tile
  type name, so the slot count no longer has to match anything:

```
// tilesets.json
[
  {
    "index": 0,
    "name": "GENERIC 1",
    "ambientId": 0,
    "files": {
      "FIRSTTEXTURE": "earth.sti",
      ...
      "GUNS": "smguns.sti",
      "P1ITEMS": "smp1items.sti",
      "P2ITEMS": "smp2items.sti",
      "P3ITEMS": "smp3items.sti",
      ...
    }
  },
  { "index": 1, "name": "CAVES 1", "ambientId": 0, "files": { "FIRSTTEXTURE": "cvflr4.sti", ... } },
  ...
]
```

- Stage 2: five new tile types at the end of the type list, their files added to
  GENERIC 1 in the JSON:

```
      "NEW_GUNS": "sm_new_guns.sti",
      "NEW_ITEMS": "sm_new_items.sti",
      "P4ITEMS": "smp4items.sti",
      "P5ITEMS": "smp5items.sti",
      "P6ITEMS": "smp6items.sti"
```

## 3. Decisions

- **T1 - where the tileset data lives.** (A, recommended) `tilesets.json` in
  `assets/externalized` (repo and `E:\Release1\externalized`), with a schema, read
  like `weapons.json`. (B) keep `ja2set.dat` and write a tool that edits the
  binary file.
- **T2 - JSON layout.** (A, recommended) one entry per tileset with `index`,
  `name`, `ambientId` and `files` keyed by tile type name, empty slots left out
  (readable, independent of the order of the types). (B) `files` as an array of
  151 names in slot order, as in the binary file.
- **T3 - `ja2set.dat` after stage 1.** (A, recommended) no longer read; the JSON is
  the only source. Unfinished Business data (70 tilesets) would need its own JSON
  made with the same export. (B) the JSON when present, else the binary file
  (two code paths; the binary file can never hold the new types).
- **T4 - the export.** (A, recommended) a Python script in `tools/` that reads a
  `ja2set.dat` and writes the JSON; run once on the user's vanilla file, the
  JSON committed. (B) the game writes the JSON on a command line switch.
- **T5 - names and place of the new types.** (A, recommended) `NEW_GUNS`,
  `NEW_ITEMS`, `P4ITEMS`, `P5ITEMS`, `P6ITEMS`, added at the very end of
  `TileTypeDefines` (after `WIREFRAMES`) and their tile ranges after
  `WIREFRAMES15`: every existing type number and tile index stays as it is, so
  maps and `LASTUIELEM` (`WIREFRAMES - 1`) are untouched. (B) next to `P3ITEMS`
  (shifts every later type number, which maps store).
- **T6 - pictures per new type.** (A, recommended) `NEW_GUNS` 2000 and `NEW_ITEMS`
  2000 (like `GUNS` / `P1ITEMS`), `P4ITEMS`-`P6ITEMS` 1000 each (like `P2ITEMS` /
  `P3ITEMS`): +7,000 tiles, 15,855 in all, below the 32,767 limit. (B) 1000 each
  (+5,000).
- **T7 - a new type whose file is missing.** (A, recommended) allowed while no item
  uses the type: the type gets the `P1ITEMS` file as a stand-in, never shown, and a
  log line; an item whose `tileGraphic.type` has no file stops the game at start
  with an error naming the item and the file. So the five files can be made one
  by one. (B) all five files required from the start. (C) no check, the item shows
  the stand-in picture.
- **T8 - names of the files.** (A, recommended) as listed by the user, lower case
  in the JSON: `sm_new_guns.sti`, `sm_new_items.sti`, `smp4items.sti`,
  `smp5items.sti`, `smp6items.sti` (file names are not case sensitive). (B) other
  names.
- **T9 - counting of `tileGraphic.subIndex` for the new types** (added after stage
  2). (A, recommended, decided) from 0, like `inventoryGraphics.*.subImageIndex`:
  `ItemTileSubIndex` (`TileDat.h`) adds 1 for `IsNewItemTileType` before
  `GetTileGraphicForItem` takes the tile; the original types keep counting from 1,
  no data changes (no item used the new types yet). (B) from 1 for every type, as
  in stage 2.

## 4. Stages (after T1-T9)

### Stage 1 - tilesets from JSON

Done: `tools/export_tilesets.py` (run on the vanilla `JA2SET.DAT` from
`Binarydata.slf`; the JSON matches the binary file slot by slot, 50 tilesets, every
ambient id 0); `assets/externalized/tilesets.json`; schema `tilesets.schema.yaml`;
`TilesetModel` (`src/externalized/tileset/`) with the checks;
`ContentManager::getTilesets()` replaces `getTilesetDBResName()`;
`InitEngineTilesets` fills `gTilesets` from it; tests `TilesetModel.*`,
`WorldDatTest.tilesetsFromJson`.

- `tools/` script: `ja2set.dat` -> `tilesets.json` (T2 layout, slot number ->
  type name by the order of `namesArray`). Run on the vanilla file; the JSON goes
  to the repo and `E:\Release1\externalized`.
- Schema `tilesets.schema.yaml`: index, name (up to 32 characters), ambientId
  (0-255), files (keys = tile type names, values = file names).
- `InitEngineTilesets` reads the JSON (`readJsonDataFileWithSchema`): number of
  entries 50 or 70 (as now), indices 0..n-1 in order, unknown type name or
  duplicate index -> data error naming the entry. The `.dat` reader is removed
  (T3). `GetAdjustedTilesetResource`, the overhead map, ambience and the editor
  keep working on `gTilesets` as before.
- Unit tests: 50 tilesets; GENERIC 1 has every type and `GUNS` = `smguns.sti`;
  CAVES 1 `FIRSTTEXTURE` = `cvflr4.sti` and an empty slot falls back to GENERIC 1;
  an unknown type name is a data error.
- Game test: sectors of several tilesets (town, desert, caves, the airstrip) look
  as before, items on the ground, the overhead map, sector ambience; the editor's
  tileset list.

### Stage 2 - new item tile types

Done: `NEW_GUNS`, `NEW_ITEMS`, `P4ITEMS`, `P5ITEMS`, `P6ITEMS` after `WIREFRAMES`,
tiles `NEWGUN1` ... `P6ITEM_LAST` after `WIREFRAMES15` (15,855 tiles); counts,
readable names, `namesArray`, schema enum; `IsItemTileType` (object layer flag) and
`IsNewItemTileType` (`TileDat.h`); GENERIC 1 files in `tilesets.json`;
`CheckItemTileFiles` at game start and the `P1ITEMS` stand-in in
`LoadTileSurfaces` (T7); tests `TileDat.newItemTileTypes`,
`TilesetModel.itemTileFiles`.

- `TileTypeDefines`: the five types at the end (T5); `TileDefines`: their ranges
  after `WIREFRAMES15` (T6 counts as constants next to `GUN_TILES` ...).
- Tables: `gNumTilesPerType`, `gTileSurfaceName`, `gTileTypeLogicalHeight`
  (same values as the existing item types), `namesArray`, the schema enum; the
  object layer flag in `SetSpecificDatabaseValues` for all item types.
- `tilesets.json` GENERIC 1: the five files (T8). Missing files per T7.
- Unit tests: counts, start indices and total (`TileDat, itemTileCounts`),
  `deserializeTileTypeDefine("P4ITEMS")`, every table has an entry for every
  type, the T7 check.
- Game test: one test item per new type with pictures in `MD*.STI` and `SM*.STI`:
  inventory picture, item on the ground, picked up and dropped again, sector
  inventory, a saved game (new game, project rule).

### Stage 3 - subIndex from 0 for the new types (T9)

- `ItemTileSubIndex(type, subIndex)` (`TileDat.h`): `subIndex + 1` for the new item
  tile types, `subIndex` for the others; `GetTileGraphicForItem`
  (`Interface_Items.cc`) uses it, the only place that turns an item's
  `tileGraphic` into a tile (ground, drop, throw, cursor). The warning for a number
  beyond the file names the counting.
- Schema `types/tileset-tile-index.schema.yaml`: `subIndex` description.
- Unit test `InterfaceItems.itemTileSubIndex`.
- Game test: an item with `"type": "NEW_GUNS", "subIndex": 0` shows the first
  picture of `SM_NEW_GUNS.STI` on the ground, the same picture as
  `"subImageIndex": 0` of `MD_NEW_GUNS.STI`; an original item (`GUNS`) unchanged.
