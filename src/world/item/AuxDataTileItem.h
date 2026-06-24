#ifndef NET_MINECRAFT_WORLD_ITEM__AuxDataTileItem_H__
#define NET_MINECRAFT_WORLD_ITEM__AuxDataTileItem_H__

//package net.minecraft.world.item;

#include "TileItem.h"
#include "../level/tile/Tile.h"
#include "../level/tile/SlabTile.h"
#include "../level/tile/WoodSlabTile.h"
#include "../Facing.h"

class AuxDataTileItem: public TileItem
{
    typedef TileItem super;
public:
    AuxDataTileItem(int id, Tile* parentTile)
    :   super(id),
        parentTile(parentTile)
    {
        setMaxDamage(0);
        setStackedByData(true);
    }

    /*@Override*/
    int getIcon(int itemAuxValue) {
        return parentTile->getTexture(2, itemAuxValue);
    }

    /*@Override*/
    int getLevelDataForAuxValue(int auxValue) {
        return auxValue;
    }

    //for wooden slabs only cuz idk how else to fix slab merging into a "full block"
    /*@Override*/
    bool useOn(ItemInstance* instance, Player* player, Level* level, int x, int y, int z, int face, float clickX, float clickY, float clickZ) {
        if (instance->count == 0) return false;
        if (parentTile && parentTile->id == Tile::woodSlabHalf->id && !parentTile->isCubeShaped()) {
            int currentTile = level->getTile(x, y, z);
            int currentData = level->getData(x, y, z);
            int slabType = currentData & WoodSlabTile::TYPE_MASK;
            bool isUpper = (currentData & SlabTile::TOP_SLOT_BIT) != 0;

            if (((face == Facing::UP && !isUpper) || (face == Facing::DOWN && isUpper)) && currentTile == Tile::woodSlabHalf->id && slabType == instance->getAuxValue()) {
                bool unobstructed = level->isUnobstructed(*Tile::woodSlab->getAABB(level, x, y, z));
                if (unobstructed && level->setTileAndData(x, y, z, Tile::woodSlab->id, slabType)) {
                    level->playSound(x + 0.5f, y + 0.5f, z + 0.5f, Tile::woodSlab->soundType->getStepSound(), (Tile::woodSlab->soundType->getVolume() + 1) / 2, Tile::woodSlab->soundType->getPitch() * 0.8f);
                    instance->count--;
                }
                return true;
            }
        }

        return super::useOn(instance, player, level, x, y, z, face, clickX, clickY, clickZ);
    }

private:
    Tile* parentTile;
};

#endif /*NET_MINECRAFT_WORLD_ITEM__AuxDataTileItem_H__*/
