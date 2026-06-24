#ifndef NET_MINECRAFT_WORLD_LEVEL_TILE__SlabTile_H__
#define NET_MINECRAFT_WORLD_LEVEL_TILE__SlabTile_H__

#include "Tile.h"

class Level;
class LevelSource;

class SlabTile: public Tile
{
	typedef Tile super;
public:
	SlabTile(int id, bool fullSize, const Material* material);

	virtual ~SlabTile();

	virtual bool isSolidRender();
	virtual bool isCubeShaped();

	virtual void updateShape(LevelSource* level, int x, int y, int z);
	virtual void updateDefaultShape();

	virtual void addAABBs(Level* level, int x, int y, int z, const AABB* box, std::vector<AABB>& boxes);

	virtual int getPlacedOnFaceDataValue(Level* level, int x, int y, int z, int face, float clickX, float clickY, float clickZ, int itemValue);

	virtual int getResourceCount(Random* random);

	virtual bool shouldRenderFace(LevelSource* level, int x, int y, int z, int face);

protected:
	bool fullSize;

	bool isBottomSlab(LevelSource* level, int x, int y, int z);

public:
	static const int TOP_SLOT_BIT = 8;
};

#endif /*NET_MINECRAFT_WORLD_LEVEL_TILE__SlabTile_H__*/
