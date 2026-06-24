#include "WoodSlabTile.h"
#include "../../../util/Random.h"
#include "../material/Material.h"
#include "../Level.h"

WoodSlabTile::WoodSlabTile(int id, bool fullSize)
:	SlabTile(id, fullSize, Material::wood)
{
}

WoodSlabTile::~WoodSlabTile()
{
}

int WoodSlabTile::getTexture(int face, int data) {
	switch (data & TYPE_MASK) {
        default:
        case OAK:
            return 4;
    }
}

int WoodSlabTile::getResource(int data, Random* random) {
	return Tile::woodSlabHalf->id;
}

int WoodSlabTile::getSpawnResourcesAuxValue(int data) {
	return data & TYPE_MASK;
}
