#include "SlabTile.h"
#include "../material/Material.h"
#include "../Level.h"
#include "../../Facing.h"

SlabTile::SlabTile(int id, bool fullSize, const Material* material)
:	Tile(id, material)
{
	this->fullSize = fullSize;

	if (!fullSize) {
		setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f);
	}
	setLightBlock(255);
}

SlabTile::~SlabTile()
{
}

bool SlabTile::isBottomSlab(LevelSource* level, int x, int y, int z)
{
	return ((level->getData(x, y, z) & TOP_SLOT_BIT) == 0);
}

bool SlabTile::isSolidRender()
{
	return fullSize;
}

bool SlabTile::isCubeShaped()
{
	return fullSize;
}

void SlabTile::updateShape(LevelSource* level, int x, int y, int z)
{
	if (fullSize) {
		setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	} else {
		bool bottom = isBottomSlab(level, x, y, z);
		if (bottom) {
			setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f);
		} else {
			setShape(0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f);
		}
	}
}

void SlabTile::updateDefaultShape()
{
	if (fullSize) {
		setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	} else {
		setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f);
	}
}

int SlabTile::getPlacedOnFaceDataValue(Level* level, int x, int y, int z, int face, float clickX, float clickY, float clickZ, int itemValue)
{
	if (fullSize) return itemValue;

	if (face == Facing::DOWN || (face != Facing::UP && clickY > 0.5f)) {
		return itemValue | TOP_SLOT_BIT;
	}
	return itemValue;
}

int SlabTile::getResourceCount(Random* random)
{
	if (fullSize) return 2;
	return 1;
}

void SlabTile::addAABBs(Level* level, int x, int y, int z, const AABB* box, std::vector<AABB>& boxes)
{
	updateShape(level, x, y, z);
	super::addAABBs(level, x, y, z, box, boxes);
}

static bool isHalfSlab(int tileId)
{
	return tileId == Tile::stoneSlabHalf->id || tileId == Tile::woodSlabHalf->id;
}

bool SlabTile::shouldRenderFace(LevelSource* level, int x, int y, int z, int face)
{
	if (fullSize) return super::shouldRenderFace(level, x, y, z, face);

	if (face != Facing::UP && face != Facing::DOWN && !super::shouldRenderFace(level, x, y, z, face)) {
		return false;
	}

	int ox = x, oy = y, oz = z;
	ox += Facing::STEP_X[Facing::OPPOSITE_FACING[face]];
	oy += Facing::STEP_Y[Facing::OPPOSITE_FACING[face]];
	oz += Facing::STEP_Z[Facing::OPPOSITE_FACING[face]];

	bool isUpper = (level->getData(ox, oy, oz) & TOP_SLOT_BIT) != 0;
	if (isUpper) {
		if (face == Facing::DOWN) return true;
		if (face == Facing::UP && super::shouldRenderFace(level, x, y, z, face)) return true;
		return !(isHalfSlab(level->getTile(x, y, z)) && (level->getData(x, y, z) & TOP_SLOT_BIT) != 0);
	} else {
		if (face == Facing::UP) return true;
		if (face == Facing::DOWN && super::shouldRenderFace(level, x, y, z, face)) return true;
		return !(isHalfSlab(level->getTile(x, y, z)) && (level->getData(x, y, z) & TOP_SLOT_BIT) == 0);
	}
}
