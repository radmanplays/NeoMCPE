#ifndef NET_MINECRAFT_CLIENT_MODEL__ChestModel_H__
#define NET_MINECRAFT_CLIENT_MODEL__ChestModel_H__

//package net.minecraft.client.model;

#include "Model.h"
#include "geom/ModelPart.h"

class ChestModel: public Model
{
public:
    ChestModel(bool isLarge = false)
    :   lid(0, 0),
        lock(0, 0),
        bottom(0, 19)
    {
        int texW, boxW;
        float lidOffsetX;

        if (isLarge) {
            texW = 128;
            boxW = 30;
            lidOffsetX = -7.0f;
        } else {
            texW = 64;
            boxW = 14;
            lidOffsetX = 1.0f;
        }

        lid.setModel(this);
        lid.setTexSize(texW, 64);
        lid.addBox(0.0f, -5.0f, -14.0f, boxW, 5, 14, 0.0f);
        lid.x = lidOffsetX;
        lid.y = 7.0f;
        lid.z = 15.0f;

        lock.setModel(this);
        lock.setTexSize(texW, 64);
        lock.addBox(-1.0f, -2.0f, -15.0f, 2, 4, 1, 0.0f);
        lock.x = 8.0f;
        lock.y = 7.0f;
        lock.z = 15.0f;

        bottom.setModel(this);
        bottom.setTexSize(texW, 64);
        bottom.addBox(0.0f, 0.0f, 0.0f, boxW, 10, 14, 0.0f);
        bottom.x = lidOffsetX;
        bottom.z = 1.0f;
        bottom.y = 6.0f;
    }

    void render() {
        lock.xRot = lid.xRot;

        lid.render(1 / 16.0f);
        lock.render(1 / 16.0f);
        bottom.render(1 / 16.0f);
    }

    ModelPart lid;
    ModelPart bottom;
    ModelPart lock;
};

#endif /*NET_MINECRAFT_CLIENT_MODEL__ChestModel_H__*/
