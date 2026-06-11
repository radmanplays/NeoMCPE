#include "BucketItem.h"
#include "ItemInstance.h"
#include "Item.h"
#include "../level/tile/Tile.h"
#include "../level/Level.h"
#include "../entity/player/Player.h"
#include "../Facing.h"
#include "../phys/HitResult.h"
#include "../phys/AABB.h"

BucketItem::BucketItem(int id)
    : super(id)
{
    setStackedByData(true);
    setMaxDamage(0);
    setMaxStackSize(64);
}

int BucketItem::getIcon(int itemAuxValue)
{
    switch (itemAuxValue) {
        case WATER: return 11 * Item::ICON_COLUMNS + 4;
        case LAVA:  return 12 * Item::ICON_COLUMNS + 4;
        case MILK:  return 13 * Item::ICON_COLUMNS + 4;
        default:    return 10 * Item::ICON_COLUMNS + 4;
    }
}

std::string BucketItem::getDescriptionId(const ItemInstance* itemInstance) const
{
    switch (itemInstance->getAuxValue()) {
        case WATER: return "bucketWater";
        case LAVA:  return "bucketLava";
        case MILK:  return "milk";
        default:    return "bucket";
    }
}

bool BucketItem::useOn(ItemInstance* instance, Player* player, Level* level, int x, int y, int z, int face, float clickX, float clickY, float clickZ) {
    int aux = instance->getAuxValue();

    if (aux == EMPTY) {
        int tx = x;
        int ty = y;
        int tz = z;
        int tileId = level->getTile(tx, ty, tz);
        int data = level->getData(tx, ty, tz);

        if ((tileId == Tile::water->id || tileId == Tile::calmWater->id) && data == 0) {
            if (player != NULL && player->abilities.instabuild) {
                if (!level->isClientSide) level->setTile(tx, ty, tz, 0);
                if (!player->inventory->add(new ItemInstance(this, 1, WATER))) {
                    player->drop(new ItemInstance(this, 1, WATER), true);
                }
                return true;
            }
            if (!level->isClientSide) level->setTile(tx, ty, tz, 0);
            if (instance->count == 1) {
                instance->setAuxValue(WATER);
            } else {
                instance->count--;
                if (!player->inventory->add(new ItemInstance(this, 1, WATER))) {
                    player->drop(new ItemInstance(this, 1, WATER), true);
                }
            }
            return true;
        }

        if ((tileId == Tile::lava->id || tileId == Tile::calmLava->id) && data == 0) {
            if (player != NULL && player->abilities.instabuild) {
                if (!level->isClientSide) level->setTile(tx, ty, tz, 0);
                if (!player->inventory->add(new ItemInstance(this, 1, LAVA))) {
                    player->drop(new ItemInstance(this, 1, LAVA), true);
                }
                return true;
            }
            if (!level->isClientSide) level->setTile(tx, ty, tz, 0);
            if (instance->count == 1) {
                instance->setAuxValue(LAVA);
            } else {
                instance->count--;
                if (!player->inventory->add(new ItemInstance(this, 1, LAVA))) {
                    player->drop(new ItemInstance(this, 1, LAVA), true);
                }
            }
            return true;
        }

        switch (face) {
            case Facing::DOWN : y--; break;
            case Facing::UP   : y++; break;
            case Facing::NORTH: z--; break;
            case Facing::SOUTH: z++; break;
            case Facing::WEST : x--; break;
            case Facing::EAST : x++; break;
        }

        tileId = level->getTile(x, y, z);
        data = level->getData(x, y, z);

        if ((tileId == Tile::water->id || tileId == Tile::calmWater->id) && data == 0) {
            if (player != NULL && player->abilities.instabuild) {
                if (!level->isClientSide) level->setTile(x, y, z, 0);
                if (!player->inventory->add(new ItemInstance(this, 1, WATER))) {
                    player->drop(new ItemInstance(this, 1, WATER), true);
                }
                return true;
            }
            if (!level->isClientSide) level->setTile(x, y, z, 0);
            if (instance->count == 1) {
                instance->setAuxValue(WATER);
            } else {
                instance->count--;
                if (!player->inventory->add(new ItemInstance(this, 1, WATER))) {
                    player->drop(new ItemInstance(this, 1, WATER), true);
                }
            }
            return true;
        }

        if ((tileId == Tile::lava->id || tileId == Tile::calmLava->id) && data == 0) {
            if (player != NULL && player->abilities.instabuild) {
                if (!level->isClientSide) level->setTile(x, y, z, 0);
                if (!player->inventory->add(new ItemInstance(this, 1, LAVA))) {
                    player->drop(new ItemInstance(this, 1, LAVA), true);
                }
                return true;
            }
            if (!level->isClientSide) level->setTile(x, y, z, 0);
            if (instance->count == 1) {
                instance->setAuxValue(LAVA);
            } else {
                instance->count--;
                if (!player->inventory->add(new ItemInstance(this, 1, LAVA))) {
                    player->drop(new ItemInstance(this, 1, LAVA), true);
                }
            }
            return true;
        }

        return false;
    }

    if (aux == WATER || aux == LAVA) {
        int contains = (aux == WATER) ? Tile::water->id : Tile::lava->id;

        if (level->getTile(x, y, z) == Tile::topSnow->id) {
            face = 0;
        } else {
            switch (face) {
                case Facing::DOWN : y--; break;
                case Facing::UP   : y++; break;
                case Facing::NORTH: z--; break;
                case Facing::SOUTH: z++; break;
                case Facing::WEST : x--; break;
                case Facing::EAST : x++; break;
            }
        }

        if (instance->count == 0) return false;

        if (level->mayPlace(contains, x, y, z, false, (unsigned char)face)) {
            if (!level->isClientSide) level->setTileAndData(x, y, z, contains, 0);

            if (player != NULL && player->abilities.instabuild) {
                // do nothing because player is in creative
            } else {
                if (instance->count == 1) {
                    instance->setAuxValue(EMPTY);
                } else {
                    instance->count--;
                    if (!player->inventory->add(new ItemInstance(this, 1, EMPTY))) {
                        player->drop(new ItemInstance(this, 1, EMPTY), true);
                    }
                }
            }
            return true;
        }
        return false;
    }

    return false;
}

ItemInstance* BucketItem::use(ItemInstance* instance, Level* level, Player* player) {
    if (instance->getAuxValue() != MILK || player == NULL || instance == NULL) return instance;

    Vec3 from = player->getPos(1).add(0, player->getHeadHeight(), 0);
    Vec3 look = player->getViewVector(1);
    Vec3 to = from + look * 5.0f;

    Vec3 pv = look * 5.0f;
    AABB aabb = player->bb.expand(pv.x, pv.y, pv.z).grow(1.0f, 1.0f, 1.0f);
    EntityList& objects = level->getEntities(player, aabb);
    Entity* hovered = NULL;
    float nearest = 0;
    for (unsigned int i = 0; i < objects.size(); i++) {
        Entity* e = objects[i];
        if (!e->isPickable()) continue;

        float rr = e->getPickRadius();
        AABB eb = e->bb.grow(rr, rr, rr);
        HitResult p = eb.clip(from, to);

        if (eb.contains(from)) {
            if (nearest >= 0) {
                hovered = e;
                nearest = 0;
            }
        } else if (p.isHit()) {
            float dd = from.distanceTo(p.pos);
            if (dd < nearest || nearest == 0) {
                hovered = e;
                nearest = dd;
            }
        }
    }

    if (hovered != NULL) {
        float distToBlock = from.distanceTo(to);
        if (nearest < distToBlock) {
            if (instance->getAuxValue() == MILK && hovered->isEntityType(MobTypes::Cow)) {
                if (player->abilities.instabuild) {
                    if (!player->inventory->add(new ItemInstance(this, 1, MILK))) player->drop(new ItemInstance(this, 1, MILK), true);
                    return instance;
                }
                if (instance->count == 1) {
                    instance->setAuxValue(MILK);
                } else {
                    instance->count--;
                    if (!player->inventory->add(new ItemInstance(this, 1, MILK))) player->drop(new ItemInstance(this, 1, MILK), true);
                }
                return instance;
            }
        }
    }

    return instance;
}
