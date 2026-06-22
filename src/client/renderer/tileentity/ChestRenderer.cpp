#include "ChestRenderer.h"
#include "TileEntityRenderer.h"
#include "../gles.h"
#include "../../../world/level/tile/entity/ChestTileEntity.h"
#include "../../../world/level/tile/ChestTile.h"
#include "../../../util/Mth.h"

void ChestRenderer::render( TileEntity* entity, float x, float y, float z, float a )
{
	ChestTileEntity* chest = (ChestTileEntity*) entity;

	if (!chest->isUnpaired) return;

	int data = 0;
	bool noLevel = false;

	if (chest->level) {
		Tile* tile = chest->getTile();
		data = chest->getData();

		if (tile != NULL && data == 0) {
			((ChestTile*)tile)->recalcLockDir(chest->level, chest->x, chest->y, chest->z);
			data = chest->getData();
		}

		chest->checkNeighbors();
	} else {
		noLevel = true;
		data = 0;
	}

	bool hasValidPair = chest->isPairValid();

	ChestModel* model;
	if (hasValidPair) {
		model = &largeChestModel;
		bindTexture("item/chest/double_normal.png");
	} else {
		model = &chestModel;
		bindTexture("item/chest/normal.png");
	}

	glPushMatrix2();
	glTranslatef2(x, y + 1, z + 1);
	glScalef2(1, -1, -1);

	glTranslatef2(0.5f, 0.5f, 0.5f);

	if (!noLevel) {
		float rot = 0;
		if (data == 2) rot = 180;
		if (data == 3) rot = 0;
		if (data == 4) rot = 90;
		if (data == 5) rot = -90;

		glRotatef2(rot, 0, 1, 0);
		glTranslatef2(-0.5f, -0.5f, -0.5f);
	} else {
		glTranslatef2(-1.0f, 0.0f, 0.0f);
	}

	if (hasValidPair) {
		float offsetDir;
		if (data == 2 || data == 5) {
			offsetDir = -1.0f;
		} else {
			offsetDir = 1.0f;
		}
		float modelOffsetX = chest->getModelOffsetX();
		glTranslatef2(modelOffsetX * offsetDir, 0.0f, 0.0f);
	}

	float open = chest->oOpenness + (chest->openness - chest->oOpenness) * a;
	if (chest->n != NULL) {
		float open2 = chest->n->oOpenness + (chest->n->openness - chest->n->oOpenness) * a;
		if (open2 > open) open = open2;
	}
	if (chest->w != NULL) {
		float open2 = chest->w->oOpenness + (chest->w->openness - chest->w->oOpenness) * a;
		if (open2 > open) open = open2;
	}

	open = 1.0f - open;
	model->lid.xRot = -((1.0f - (open * open * open)) * 3.1416f * 0.5f);
	model->render();
	glPopMatrix2();
}
