#include "CakeTile.h"
#include "../material/Material.h"
#include "../Level.h"
#include "../../entity/player/Player.h"
#include "../../Facing.h"
#include "../../item/Item.h"

CakeTile::CakeTile(int id) : super(id, TEX, Material::cake) {
    setTicking(true);
}

int CakeTile::getTexture(int face, int data) {
    if (face == Facing::UP) return tex;
    if (face == Facing::DOWN) return tex + 3;
    if (data > 0 && face == Facing::WEST) return tex + 2;
    return tex + 1;
}

int CakeTile::getTexture(int face) {
    return getTexture(face, 0);
}

bool CakeTile::use(Level* level, int x, int y, int z, Player* player) {
    if (level->isClientSide) return true;
    eatCakeSlice(level, x, y, z, player);
    return true;
}

void CakeTile::attack(Level* level, int x, int y, int z, Player* player) {
    eatCakeSlice(level, x, y, z, player);
}

void CakeTile::eatCakeSlice(Level* level, int x, int y, int z, Player* player) {
    if (player->health < player->getMaxHealth()) {
        player->heal(3);
        int meta = level->getData(x, y, z);
        meta++;
        if (meta >= 6) {
            level->setTile(x, y, z, 0);
        } else {
            level->setData(x, y, z, meta);
        }
    }
}

void CakeTile::neighborChanged(Level* level, int x, int y, int z, int type) {
    if (!canSurvive(level, x, y, z)) {
        level->setTile(x, y, z, 0);
    }
}

int CakeTile::getResource(int data, Random* random) {
    return 0;
}

int CakeTile::getResourceCount(Random* random) {
    return 0;
}

void CakeTile::updateDefaultShape() {
    float f = 1.0f / 16.0f;
    float h = 0.5f;
    setShape(f, 0.0f, f, 1.0f - f, h, 1.0f - f);
}

void CakeTile::updateShape(LevelSource* level, int x, int y, int z) {
    int meta = level->getData(x, y, z);
    float f = 1.0f / 16.0f;
    float left = (1 + meta * 2) / 16.0f;
    float h = 0.5f;
    setShape(left, 0.0f, f, 1.0f - f, h, 1.0f - f);
}

int CakeTile::getRenderLayer() {
    return Tile::RENDERLAYER_ALPHATEST;
}

bool CakeTile::isCubeShaped() {
    return false;
}

bool CakeTile::isSolidRender() {
    return false;
}

bool CakeTile::canSurvive(Level* level, int x, int y, int z) {
    return true;
}
