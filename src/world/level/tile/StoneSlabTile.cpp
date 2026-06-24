#include "StoneSlabTile.h"
#include "../../../util/Random.h"
#include "../material/Material.h"
#include "../Level.h"
#include "../../Facing.h"

const std::string StoneSlabTile::SLAB_NAMES[] = {
	"stone", "sand", "wood", "cobble", "brick", "smoothStoneBrick", "quartz"
};
const int StoneSlabTile::SLAB_NAMES_COUNT = sizeof(SLAB_NAMES) / sizeof(std::string);

StoneSlabTile::StoneSlabTile(int id, bool fullSize)
:	SlabTile(id, fullSize, Material::stone)
{
}

StoneSlabTile::~StoneSlabTile()
{
}

int StoneSlabTile::getTexture(int face, int data) {
	data = data & TYPE_MASK;
    if (data == STONE_SLAB) {
        if (face <= 1) return 6;
        return 5;
    } else if (data == SAND_SLAB) {
        if (face == Facing::DOWN) return 13 * 16;
        if (face == Facing::UP) return 11 * 16;
        return 12 * 16;
    } else if (data == WOOD_SLAB) {
        return 4;
    } else if (data == COBBLESTONE_SLAB) {
        return 16;
	} else if (data == BRICK_SLAB) {
		return Tile::redBrick->tex;
	} else if (data == SMOOTHBRICK_SLAB) {
		return Tile::stoneBrickSmooth->tex;
	} else if (data == QUARTZ_SLAB) {
		if (face == Facing::DOWN) return 3 + 13 * 16;
		if (face == Facing::UP) return 4 + 12 * 16;
		return Tile::quartzBlock->tex;
	}
    return 6;
}

int StoneSlabTile::getTexture(int face) {
    return getTexture(face, 0);
}

int StoneSlabTile::getResource(int data, Random* random) {
    if ((data & TYPE_MASK) == WOOD_SLAB) {
        return Tile::woodSlabHalf->id;
    }
    return Tile::stoneSlabHalf->id;
}

int StoneSlabTile::getSpawnResourcesAuxValue(int data) {
    if ((data & TYPE_MASK) == WOOD_SLAB) {
        return 0;
    }
    return data & TYPE_MASK;
}
