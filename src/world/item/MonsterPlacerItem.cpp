#include "MonsterPlacerItem.h"
#include "../entity/Mob.h"
#include "../entity/MobFactory.h"
#include "../entity/EntityTypes.h"
#include "../entity/player/Player.h"
#include "../level/Level.h"
#include "../level/tile/Tile.h"
#include "../level/MobSpawner.h"
#include "../Facing.h"
#include "../../util/Mth.h"
#include "../../locale/I18n.h"

int MonsterPlacerItem::getIconForMobType( int mobTypeId )
{
    // nice looking switch for getting the icons incase we need to add more mobs in newer versions
	switch (mobTypeId) {
		case MobTypes::Pig:     return 226;
		case MobTypes::Sheep:   return 227;
		case MobTypes::Cow:     return 225;
		default:                return 224;
	}
}

int MonsterPlacerItem::getIcon( int itemAuxValue )
{
	return getIconForMobType(itemAuxValue);
}

MonsterPlacerItem::MonsterPlacerItem( int id )
:	super(id)
{
	setStackedByData(true);
	setMaxDamage(0);
}

std::string MonsterPlacerItem::getDisplayName(int auxValue) const
{
	switch (auxValue) {
		case MobTypes::Chicken: return I18n::get(getDescriptionId() + ".name") + " " + I18n::get("entity.Chicken.name");
		case MobTypes::Cow:     return I18n::get(getDescriptionId() + ".name") + " " + I18n::get("entity.Cow.name");
		case MobTypes::Pig:     return I18n::get(getDescriptionId() + ".name") + " " + I18n::get("entity.Pig.name");
		case MobTypes::Sheep:   return I18n::get(getDescriptionId() + ".name") + " " + I18n::get("entity.Sheep.name");
		default:                return I18n::get(getDescriptionId() + ".name");
	}
}

bool MonsterPlacerItem::useOn( ItemInstance* itemInstance, Player* player, Level* level, int x, int y, int z, int face, float clickX, float clickY, float clickZ )
{
	if (level->isClientSide) {
		return true;
	}

	x += Facing::STEP_X[face];
	y += Facing::STEP_Y[face];
	z += Facing::STEP_Z[face];

	float yOffset = 0.0f;
	if (face == Facing::UP && level->getTile(x, y, z) == Tile::fence->id) {
		yOffset = 0.5f;
	}

	int meta = itemInstance->getAuxValue();

	Mob* testMob = MobFactory::getStaticTestMob(meta, level);
	if (testMob && testMob->getCreatureBaseType() == MobTypes::BaseCreature) {
		Mob* mob = MobFactory::CreateMob(meta, level);
		if (mob) {
			mob->moveTo((float)x + 0.5f, (float)y + yOffset, (float)z + 0.5f, level->random.nextFloat() * 360.0f, 0.0f);
			MobSpawner::finalizeMobSettings(mob, level, 0.0f, 0.0f, 0.0f);
			mob->playAmbientSound();
			level->addEntity(mob);
			if (!player->abilities.instabuild) {
				itemInstance->count--;
			}
		}
	}

	return true;
}

