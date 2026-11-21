#!/usr/bin/env python3
"""Export the original BinaryData/ja2set.dat to tilesets.json.

docs/tilesets-json-and-item-tile-types-plan.md, stage 1 (T4): a one-time export,
the game reads only tilesets.json.

Layout of ja2set.dat:
  1 byte   number of tilesets (50 vanilla, 70 Unfinished Business)
  4 bytes  number of file slots per tileset (little endian)
  per tileset:
    32 bytes  name
     1 byte   ambient sound id
    32 bytes  file name, once per slot; the slot is the tile type number

Slot numbers become tile type names by the order of namesArray in
src/externalized/tileset/TilesetTileIndexModel.cc. Empty slots are left out
(the game then uses the file of tileset 0).

Usage:
  python tools/export_tilesets.py <ja2set.dat> <tilesets.json>
"""

import json
import os
import re
import struct
import sys

NAME_LENGTH = 32
FILE_NAME_LENGTH = 32

HEADER = """\
// Tilesets (docs/tilesets-json-and-item-tile-types-plan.md), exported from the
// original BinaryData/ja2set.dat by tools/export_tilesets.py.
//  index      the tileset number (its files are in Tilesets/<index>/), 0..n-1 in order
//  name       shown by the map editor
//  ambientId  the sector ambience sound set
//  files      tile type -> picture file of this tileset; a tile type left out uses
//             the file of tileset 0 (GENERIC 1)
"""


def tile_type_names(repo_root):
    path = os.path.join(repo_root, 'src', 'externalized', 'tileset', 'TilesetTileIndexModel.cc')
    text = open(path, encoding='utf-8').read()
    start = text.index('namesArray')
    body = text[text.index('{', start) + 1:text.index('};', start)]
    body = re.sub(r'//[^\n]*', '', body)
    return re.findall(r'"([^"]+)"', body)


def read_string(data, pos, length):
    return data[pos:pos + length].split(b'\0', 1)[0].decode('utf-8')


def read_tilesets(path, names):
    data = open(path, 'rb').read()
    count = data[0]
    slots = struct.unpack_from('<I', data, 1)[0]
    if slots > len(names):
        sys.exit('%s has %d slots, the code knows %d tile types' % (path, slots, len(names)))
    pos = 5
    tilesets = []
    for index in range(count):
        name = read_string(data, pos, NAME_LENGTH)
        pos += NAME_LENGTH
        ambient = data[pos]
        pos += 1
        files = {}
        for slot in range(slots):
            file_name = read_string(data, pos, FILE_NAME_LENGTH)
            pos += FILE_NAME_LENGTH
            if file_name:
                files[names[slot]] = file_name
        tilesets.append({'index': index, 'name': name, 'ambientId': ambient, 'files': files})
    if pos != len(data):
        sys.exit('%s: read %d of %d bytes' % (path, pos, len(data)))
    return tilesets


def write_json(path, tilesets):
    lines = [HEADER.rstrip('\n'), '[']
    for i, t in enumerate(tilesets):
        lines.append('    {')
        lines.append('        "index": %d,' % t['index'])
        lines.append('        "name": %s,' % json.dumps(t['name']))
        lines.append('        "ambientId": %d,' % t['ambientId'])
        if not t['files']:
            lines.append('        "files": {}')
        else:
            lines.append('        "files": {')
            items = list(t['files'].items())
            for k, (tile_type, file_name) in enumerate(items):
                comma = ',' if k < len(items) - 1 else ''
                lines.append('            %s: %s%s' % (json.dumps(tile_type), json.dumps(file_name), comma))
            lines.append('        }')
        lines.append('    }' + (',' if i < len(tilesets) - 1 else ''))
    lines.append(']')
    with open(path, 'w', encoding='utf-8', newline='\n') as f:
        f.write('\n'.join(lines) + '\n')


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    names = tile_type_names(repo_root)
    tilesets = read_tilesets(sys.argv[1], names)
    write_json(sys.argv[2], tilesets)
    print('%d tilesets written to %s' % (len(tilesets), sys.argv[2]))


if __name__ == '__main__':
    main()
