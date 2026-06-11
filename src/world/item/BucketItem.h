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
    static const int WATER = 1;
    static const int LAVA = 2;
    static const int MILK = 3;

    BucketItem(int id);

    virtual int getIcon(int itemAuxValue) override;
    virtual std::string getDescriptionId(const ItemInstance* itemInstance) const override;
    virtual bool useOn(ItemInstance* instance, Player* player, Level* level, int x, int y, int z, int face, float clickX, float clickY, float clickZ) override;
    virtual bool useOn(ItemInstance* instance, Level* level, int x, int y, int z, int face) override {
        return false;
    }
    virtual ItemInstance* use(ItemInstance* instance, Level* level, Player* player) override;
};

#endif
