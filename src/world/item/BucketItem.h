#ifndef NET_MINECRAFT_WORLD_ITEM__BucketItem_H__
#define NET_MINECRAFT_WORLD_ITEM__BucketItem_H__

#include "Item.h"
#include <string>

class Player;
class Level;
class ItemInstance;

class BucketItem: public Item {
    typedef Item super;
public:
    static const int EMPTY = 0;
    static const int WATER = 8;
    static const int LAVA = 10;
    static const int MILK = 1;

    BucketItem(int id);
    virtual ~BucketItem();

    virtual int getMaxStackSize(const ItemInstance* itemInstance);
    virtual int getIcon(int auxValue) override;
    virtual std::string getDescriptionId(const ItemInstance* itemInstance) const override;
    virtual bool useOn(ItemInstance* instance, Player* player, Level* level, int x, int y, int z, int face, float clickX, float clickY, float clickZ) override;
    virtual bool isLiquidClipItem(int auxValue);
    virtual bool isEmissive(int auxValue);

private:
    bool emptyBucket(Level* level, int type, int x, int y, int z);
};

#endif
