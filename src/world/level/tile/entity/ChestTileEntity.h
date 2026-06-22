#ifndef NET_MINECRAFT_WORLD_LEVEL_TILE_ENTITY__ChestTileEntity_H__
#define NET_MINECRAFT_WORLD_LEVEL_TILE_ENTITY__ChestTileEntity_H__

//package net.minecraft.world.level->tile.entity;

#include "TileEntity.h"
#include "../../../inventory/FillingContainer.h"
#include "../../../item/ItemInstance.h"
#include <string>

class CompoundTag;
class Player;

/* import com.mojang.nbt.* */

class ChestTileEntity:	public TileEntity,
						public FillingContainer
{
	typedef TileEntity super;
public:
	ChestTileEntity();

    int getContainerSize() const;
	int getMaxStackSize() const;
	std::string getName() const;

    ItemInstance* getItem(int slot);
	void setItem(int slot, ItemInstance* item);
	bool add(ItemInstance* item);

	bool shouldSave();
    void load(CompoundTag* base);
    bool save(CompoundTag* base);

    bool stillValid(Player* player);

    void clearCache();
    void checkNeighbors();

    /*@Override*/
    void tick();

    void triggerEvent(int b0, int b1);

    void startOpen();
    void stopOpen();

    /*@Override*/
    void setRemoved();

    bool canPairWith(TileEntity* te);
    void pairWith(ChestTileEntity* other, bool isUnpaired);
    void unpair();
    bool canOpen();
    void openBy(Player* player);
    void _getCenter(float& x, float& y, float& z);
    void _unpair();
    bool _canOpenThis();
    void _resetAABB();
    void tryPairWithNeighbors();
    float getModelOffsetX();
    bool isPairValid() const;

	bool hasCheckedNeighbors;
	ChestTileEntity* n;
	ChestTileEntity* e;
	ChestTileEntity* w;
	ChestTileEntity* s;

	ChestTileEntity* pair;
	int pairX, pairZ;
	bool isUnpaired;
	bool pairSameXPos;

	float openness, oOpenness;
	int openCount;

	bool hasPendingPair;
	int pendingPairX, pendingPairZ;

	Player* openedBy;
	int openDelay;

private:
	static const int ItemsSize = 9*3;
	int tickInterval;
};

#endif /*NET_MINECRAFT_WORLD_LEVEL_TILE_ENTITY__ChestTileEntity_H__*/
