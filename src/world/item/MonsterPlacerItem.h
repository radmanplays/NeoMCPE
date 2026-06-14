#ifndef NET_MINECRAFT_WORLD_ITEM__MonsterPlacerItem_H__
#define NET_MINECRAFT_WORLD_ITEM__MonsterPlacerItem_H__

#include "Item.h"
#include <string>

class ItemInstance;
class Level;
class Player;
class Mob;

class MonsterPlacerItem : public Item
{
    typedef Item super;
public:
    MonsterPlacerItem(int id);

    virtual int getIcon(int itemAuxValue) override;
    virtual std::string getDisplayName(int auxValue) const override;

    virtual bool useOn(ItemInstance* itemInstance, Player* player, Level* level, int x, int y, int z, int face, float clickX, float clickY, float clickZ) override;

private:
    static int getIconForMobType(int mobTypeId);
};

#endif /*NET_MINECRAFT_WORLD_ITEM__MonsterPlacerItem_H__*/