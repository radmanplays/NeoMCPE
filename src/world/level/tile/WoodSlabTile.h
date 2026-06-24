#ifndef NET_MINECRAFT_WORLD_LEVEL_TILE__WoodSlabTile_H__
#define NET_MINECRAFT_WORLD_LEVEL_TILE__WoodSlabTile_H__

#include "SlabTile.h"
#include "Tile.h"

class WoodSlabTile: public SlabTile
{
	typedef SlabTile super;
public:

	static const int OAK    = 0;
	
	WoodSlabTile(int id, bool fullSize);

	virtual ~WoodSlabTile();

	int getTexture(int face, int data);

	int getResource(int data, Random* random);

	static const int TYPE_MASK = 7;

protected:
	int getSpawnResourcesAuxValue(int data);
};

#endif /*NET_MINECRAFT_WORLD_LEVEL_TILE__WoodSlabTile_H__*/
