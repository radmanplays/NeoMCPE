#include "BucketItem.h"
#include "ItemInstance.h"
#include "Item.h"
#include "../level/tile/Tile.h"
#include "../level/tile/FireTile.h"
#include "../level/Level.h"
#include "../level/material/Material.h"
#include "../entity/player/Player.h"
#include "../Facing.h"

BucketItem::BucketItem(int id)
    : super(id)
{
    setStackedByData(true);
    setMaxDamage(0);
}

BucketItem::~BucketItem()
{
}

int BucketItem::getMaxStackSize(const ItemInstance* itemInstance)
{
    if (itemInstance->getAuxValue())
        return 1;
    return 16;
}

int BucketItem::getIcon(int auxValue)
{
    if (auxValue == Tile::water->id)
        return 11 + 4 * Item::ICON_COLUMNS;
    if (auxValue == Tile::lava->id)
        return 12 + 4 * Item::ICON_COLUMNS;
    if (auxValue == 1)
        return 13 + 4 * Item::ICON_COLUMNS;
    return 10 + 4 * Item::ICON_COLUMNS;
}

std::string BucketItem::getDescriptionId(const ItemInstance* itemInstance) const
{
    int auxValue = itemInstance->getAuxValue();
    if (auxValue == Tile::water->id)
        return "item.bucketWater";
    if (auxValue == Tile::lava->id)
        return "item.bucketLava";
    return "item.bucket";
}

bool BucketItem::isLiquidClipItem(int auxValue)
{
    return auxValue == 0;
}

bool BucketItem::isEmissive(int auxValue)
{
    return auxValue == Tile::lava->id;
}

bool BucketItem::emptyBucket(Level* level, int type, int x, int y, int z)
{
    if (type <= 0)
        return false;

    int existingTile = level->getTile(x, y, z);
    if (existingTile)
    {
        const Material* mat = level->getMaterial(x, y, z);
        if (mat->isSolid())
            return false;
    }

    level->setTileAndData(x, y, z, type, 0);

    if (type == Tile::water->id && (existingTile == Tile::fire->id || existingTile == Tile::lava->id || existingTile == Tile::calmLava->id))
    {
        float pitch = (level->random.nextFloat() - level->random.nextFloat()) * 0.8f + 2.6f;
        level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, "random.fizz", 0.5f, pitch);
    }

    return true;
}

bool BucketItem::useOn(ItemInstance* instance, Player* player, Level* level, int x, int y, int z, int face, float clickX, float clickY, float clickZ)
{
    int auxValue = instance->getAuxValue();

    if (player->inventory->getNumEmptySlots() > 0 || instance->count == 1)
    {
        if (!auxValue)
        {
            const Material* mat = level->getMaterial(x, y, z);
            if ((mat != Material::water && mat != Material::lava) || level->getData(x, y, z))
                return false;

            level->setTile(x, y, z, 0);

            if (player->abilities.instabuild)
                return true;

            ItemInstance filledBucket(Item::bucket, 1, (mat == Material::water) ? Tile::water->id : Tile::lava->id);

            instance->count--;
            if (!instance->count)
            {
                *instance = filledBucket;
                return true;
            }

            player->inventory->add(&filledBucket);
            return true;
        }

        if (auxValue <= 1)
            return false;

        if (face)
        {
            switch (face)
            {
                case Facing::UP:    y++; break;
                case Facing::NORTH: z--; break;
                case Facing::SOUTH: z++; break;
                case Facing::WEST:  x--; break;
                case Facing::EAST:  x++; break;
            }
        }
        else
        {
            y--;
        }

        if (!emptyBucket(level, auxValue, x, y, z))
            return false;

        ItemInstance emptyBucket(Item::bucket, 1, 0);

        instance->count--;
        if (!instance->count)
        {
            *instance = emptyBucket;
            return true;
        }

        player->inventory->add(&emptyBucket);
        return true;
    }

    if (auxValue)
    {
        if (auxValue <= 1)
            return false;

        if (face)
        {
            switch (face)
            {
                case Facing::UP:    y++; break;
                case Facing::NORTH: z--; break;
                case Facing::SOUTH: z++; break;
                case Facing::WEST:  x--; break;
                case Facing::EAST:  x++; break;
            }
        }
        else
        {
            y--;
        }

        if (!emptyBucket(level, auxValue, x, y, z))
            return false;

        ItemInstance emptyBucket(Item::bucket, 1, 0);

        instance->count--;
        if (!instance->count)
        {
            *instance = emptyBucket;
            return true;
        }

        player->inventory->add(&emptyBucket);
        return true;
    }

    if (player->abilities.instabuild)
    {
        const Material* mat = level->getMaterial(x, y, z);
        if ((mat != Material::water && mat != Material::lava) || level->getData(x, y, z))
            return false;

        level->setTile(x, y, z, 0);

        if (player->abilities.instabuild)
            return true;

        ItemInstance filledBucket(Item::bucket, 1, (mat == Material::water) ? Tile::water->id : Tile::lava->id);

        instance->count--;
        if (!instance->count)
        {
            *instance = filledBucket;
            return true;
        }

        player->inventory->add(&filledBucket);
        return true;
    }

    return false;
}
