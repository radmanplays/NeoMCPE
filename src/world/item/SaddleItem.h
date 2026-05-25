#ifndef NET_MINECRAFT_WORLD_ITEM__SaddleItem_H__
#define NET_MINECRAFT_WORLD_ITEM__SaddleItem_H__

#include "Item.h"

class SaddleItem: public Item
{
    typedef Item super;
public:
    SaddleItem(int id)
	:	super(id)
	{
        maxStackSize = 1;
    }

    ItemInstance* use(ItemInstance* instance, Level* level, Player* player) {
        //currently serves no purpose and is unobtainable(according to the minecraft wiki)
        return instance;
    }
};

#endif /*NET_MINECRAFT_WORLD_ITEM__SaddleItem_H__*/
