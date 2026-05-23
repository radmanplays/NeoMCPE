#ifndef NET_MINECRAFT_WORLD_LEVEL_TILE__CakeTile_H__
#define NET_MINECRAFT_WORLD_LEVEL_TILE__CakeTile_H__

#include "Tile.h"

class CakeTile : public Tile {
	typedef Tile super;
public:
    CakeTile(int id);

    int getTexture(int face, int data) override;
    int getTexture(int face) override;

    bool use(Level* level, int x, int y, int z, Player* player) override;
    void attack(Level* level, int x, int y, int z, Player* player) override;
    void neighborChanged(Level* level, int x, int y, int z, int type) override;

    int getResource(int data, Random* random) override;
    int getResourceCount(Random* random) override;

    void updateDefaultShape() override;
    void updateShape(LevelSource* level, int x, int y, int z) override;

    int getRenderLayer() override;
    bool isCubeShaped() override;
    bool isSolidRender() override;

    bool canSurvive(Level* level, int x, int y, int z) override;
private:
    void eatCakeSlice(Level* level, int x, int y, int z, Player* player);
    static const int TEX = 121;
};

#endif /*NET_MINECRAFT_WORLD_LEVEL_TILE__CakeTile_H__*/
