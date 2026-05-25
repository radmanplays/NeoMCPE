#include "MonsterPlacerItem.h"
#include "../entity/Mob.h"
#include "../entity/MobFactory.h"
#include "../entity/EntityTypes.h"
#include "../entity/player/Player.h"
#include "../level/Level.h"
#include "../level/MobSpawner.h"
#include "../../util/Mth.h"

static const int FacingOffsetX[] = { 0, 0, 0, 0, -1, 1 };
static const int FacingOffsetY[] = { -1, 1, 0, 0, 0, 0 };
static const int FacingOffsetZ[] = { 0, 0, -1, 1, 0, 0 };

MonsterPlacerItem::MonsterPlacerItem( int id )
:	super(id)
{
    setMaxStackSize(1);
	setStackedByData(true);
	setMaxDamage(0);
}

int MonsterPlacerItem::getIconForMobType( int mobTypeId )
{
    // nice looking switch for getting the icons incase we need to add more mobs in newer versions
	switch (mobTypeId) {
		case MobTypes::Chicken: return 224;
		case MobTypes::Cow:     return 225;
		case MobTypes::Pig:     return 226;
		case MobTypes::Sheep:   return 227;
		default:                return 224;
	}
}

int MonsterPlacerItem::getIcon( int itemAuxValue )
{
	return getIconForMobType(itemAuxValue);
}

bool MonsterPlacerItem::useOn( ItemInstance* itemInstance, Player* player, Level* level, int x, int y, int z, int face, float clickX, float clickY, float clickZ )
{
	if (level->isClientSide) {
		return true;
	}

	if (face >= 0 && face < 6) {
		x += FacingOffsetX[face];
		y += FacingOffsetY[face];
		z += FacingOffsetZ[face];
	}

	int mobTypeId = itemInstance->getAuxValue();

	Mob* mob = MobFactory::CreateMob(mobTypeId, level);
	if (mob) {
		float xx = (float)x + 0.5f;
		float yy = (float)y;
		float zz = (float)z + 0.5f;

		float randomRotation = level->random.nextFloat() * 360.0f;
        //i tried to make it more accurate to mcpe alpha 0.7.0
		if (MobSpawner::addMob(level, mob, xx, yy, zz, randomRotation, 0.0f, true)) {
			if (!player->abilities.instabuild) {
				itemInstance->count--;
			}
			return true;
		} else {
			delete mob;
		}
	}

	return false;
}