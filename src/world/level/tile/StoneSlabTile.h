#ifndef NET_MINECRAFT_WORLD_LEVEL_TILE__StoneSlabTile_H__
#define NET_MINECRAFT_WORLD_LEVEL_TILE__StoneSlabTile_H__

#include "SlabTile.h"

class StoneSlabTile: public SlabTile
{
	typedef SlabTile super;
public:
    static const int STONE_SLAB = 0;
    static const int SAND_SLAB  = 1;
    static const int WOOD_SLAB  = 2;
    static const int COBBLESTONE_SLAB = 3;
	static const int BRICK_SLAB = 4;
	static const int SMOOTHBRICK_SLAB = 5;
	static const int QUARTZ_SLAB = 6;

	static const int TYPE_MASK = 7;

    static const std::string SLAB_NAMES[];
	static const int SLAB_NAMES_COUNT;

    StoneSlabTile(int id, bool fullSize);

	virtual ~StoneSlabTile();

	int getTexture(int face, int data);
    int getTexture(int face);

	int getResource(int data, Random* random);

protected:
    int getSpawnResourcesAuxValue(int data);
};

#endif /*NET_MINECRAFT_WORLD_LEVEL_TILE__StoneSlabTile_H__*/
