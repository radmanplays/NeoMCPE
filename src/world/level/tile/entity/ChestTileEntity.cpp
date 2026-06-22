#include "ChestTileEntity.h"
#include "../ChestTile.h"
#include "../../Level.h"
#include "../../material/Material.h"
#include "../../../entity/player/Player.h"
#include "../../../../nbt/NbtIo.h"

ChestTileEntity::ChestTileEntity()
:	super(TileEntityType::Chest),
	FillingContainer(ItemsSize, 0, ContainerType::CONTAINER, false),
	tickInterval(0),
	openCount(0),
	openness(0), oOpenness(0),
	hasCheckedNeighbors(false),
	n(NULL), s(NULL), w(NULL), e(NULL),
	pair(NULL),
	isUnpaired(true),
	pairSameXPos(false),
	hasPendingPair(false),
	pendingPairX(0), pendingPairZ(0),
	openedBy(NULL),
	openDelay(0)
{
	rendererId = TR_CHEST_RENDERER;
}

int ChestTileEntity::getContainerSize() const
{
	if (isPairValid()) return ItemsSize * 2;
	return ItemsSize;
}

ItemInstance* ChestTileEntity::getItem( int slot )
{
	if (isPairValid() && !isUnpaired) {
		return pair->getItem(slot);
	}

	if (slot < ItemsSize) {
		return items[slot];
	}

	if (isPairValid()) {
		return pair->items[slot - ItemsSize];
	}

	return items[slot];
}

void ChestTileEntity::setItem( int slot, ItemInstance* item )
{
	if (isPairValid() && !isUnpaired) {
		pair->setItem(slot, item);
		return;
	}

	if (slot < ItemsSize) {
		if (items[slot]) {
			*items[slot] = (item ? *item : ItemInstance());
		} else {
			items[slot] = (item ? new ItemInstance(*item) : new ItemInstance());
		}
	} else if (isPairValid()) {
		int pairSlot = slot - ItemsSize;
		if (pair->items[pairSlot]) {
			*pair->items[pairSlot] = (item ? *item : ItemInstance());
		} else {
			pair->items[pairSlot] = (item ? new ItemInstance(*item) : new ItemInstance());
		}
	}
	setChanged();
}

std::string ChestTileEntity::getName() const
{
	if (isPairValid()) return "container.chestDouble";
	return "container.chest";
}

bool ChestTileEntity::add(ItemInstance* item)
{
	if (!pair) {
		return FillingContainer::add(item);
	}

	if (FillingContainer::add(item)) {
		return true;
	}

	if (item && !item->isNull() && item->count > 0) {
		return pair->FillingContainer::add(item);
	}

	return false;
}

bool ChestTileEntity::shouldSave() {
	return true;
}

void ChestTileEntity::load( CompoundTag* base )
{
	super::load(base);

	hasPendingPair = base->contains("pairx");
	if (hasPendingPair) {
		pendingPairX = base->getInt("pairx");
		pendingPairZ = base->getInt("pairz");
	}

	if (!base->contains("Items"))
		return;

	ListTag* inventoryList = base->getList("Items");
	for (int i = 0; i < inventoryList->size(); i++) {
		Tag* ttag = inventoryList->get(i);
		if (ttag->getId() != Tag::TAG_Compound)
			continue;

		CompoundTag* tag = (CompoundTag*) ttag;
		int slot = tag->getByte("Slot") & 0xff;
		if (slot >= 0 && slot < ItemsSize) {
			if (!items[slot]) items[slot] = new ItemInstance(); //@chestodo
			items[slot]->load(tag);
		}
	}
}

bool ChestTileEntity::save( CompoundTag* base )
{
	if (!super::save(base))
		return false;

	if (pair && isUnpaired) {
		base->putInt("pairx", pair->x);
		base->putInt("pairz", pair->z);
	}

	ListTag* listTag = new ListTag();

	for (int i = 0; i < ItemsSize; i++) {
		if (items[i] && !items[i]->isNull()) {
			CompoundTag* tag = new CompoundTag();
			tag->putByte("Slot", (char) i);
			items[i]->save(tag);
			listTag->add(tag);
		}
	}
	base->put("Items", listTag);
	return true;
}

int ChestTileEntity::getMaxStackSize() const
{
	return Container::LARGE_MAX_STACK_SIZE;
}

bool ChestTileEntity::stillValid( Player* player )
{
	if (level->getTileEntity(x, y, z) != this) return false;
	if (player->distanceToSqr(x + 0.5f, y + 0.5f, z + 0.5f) > 8 * 8) return false;
	return true;
}

void ChestTileEntity::clearCache()
{
	super::clearCache();
	hasCheckedNeighbors = false;
}

void ChestTileEntity::checkNeighbors()
{
	if (hasCheckedNeighbors)
		return;

	hasCheckedNeighbors = true;
	n = NULL;
	e = NULL;
	w = NULL;
	s = NULL;

	if (level->getTile(x - 1, y, z) == Tile::chest->id) {
		w = (ChestTileEntity*) level->getTileEntity(x - 1, y, z);
	}
	if (level->getTile(x + 1, y, z) == Tile::chest->id) {
		e = (ChestTileEntity*) level->getTileEntity(x + 1, y, z);
	}
	if (level->getTile(x, y, z - 1) == Tile::chest->id) {
		n = (ChestTileEntity*) level->getTileEntity(x, y, z - 1);
	}
	if (level->getTile(x, y, z + 1) == Tile::chest->id) {
		s = (ChestTileEntity*) level->getTileEntity(x, y, z + 1);
	}

	if (n != NULL) n->clearCache();
	if (s != NULL) s->clearCache();
	if (e != NULL) e->clearCache();
	if (w != NULL) w->clearCache();
}

bool ChestTileEntity::_canOpenThis()
{
	Tile* above = Tile::tiles[level->getTile(x, y + 1, z)];
	if (above && above->material->isSolidBlocking()) {
		return !level->isSolidBlockingTile(x, y + 1, z);
	}
	return true;
}

void ChestTileEntity::_getCenter(float& cx, float& cy, float& cz)
{
	cy = (float) y;
	if (pair) {
		cx = (float)(x + pair->x) * 0.5f;
		cz = (float)(z + pair->z) * 0.5f;
	} else {
		cx = (float) x;
		cz = (float) z;
	}
}

bool ChestTileEntity::canPairWith(TileEntity* te)
{
	if (!te) return false;
	if (!te->isType(TileEntityType::Chest)) return false;
	if (te->y != y) return false;

	ChestTileEntity* other = (ChestTileEntity*) te;
	if (other->pair) return false;

	int d = getData();
	if (d != te->getData()) return false;

	int checkVal;
	if (te->x == x) {
		checkVal = d - 2;
	} else {
		checkVal = d - 4;
	}
	return (unsigned int)checkVal > 1;
}

void ChestTileEntity::pairWith(ChestTileEntity* other, bool isUnpairedFlag)
{
	pair = other;
	pairX = other->x;
	pairZ = other->z;
	isUnpaired = isUnpairedFlag;
	pairSameXPos = (other->x == x);
}

void ChestTileEntity::_unpair()
{
	pair = NULL;
	pairX = 0;
	pairZ = 0;
	isUnpaired = true;
}

bool ChestTileEntity::isPairValid() const
{
	if (!pair) return false;
	if (!level) return false;
	Level* lvl = const_cast<Level*>(level);
	TileEntity* te = lvl->getTileEntity(pairX, y, pairZ);
	return (te == pair);
}

void ChestTileEntity::unpair()
{ 
	if (pair) {
		pair->_unpair();
		_unpair();
	}
}

bool ChestTileEntity::canOpen()
{
	if (pair) {
		if (!_canOpenThis()) return false;
		return pair->_canOpenThis();
	}
	return _canOpenThis();
}

void ChestTileEntity::openBy(Player* player)
{
	ChestTileEntity* primary = this;
	while (!primary->isUnpaired) {
		primary = primary->pair;
	}
	if (!primary->openedBy) {
		primary->openDelay = 6;
		primary->openedBy = player;
		primary->startOpen();
	}
}

void ChestTileEntity::tryPairWithNeighbors()
{
	if (pair) return;

	int dx[] = {-1, 1, 0, 0};
	int dz[] = {0, 0, -1, 1};
	for (int i = 0; i < 4; i++) {
		TileEntity* te = level->getTileEntity(x + dx[i], y, z + dz[i]);
		if (canPairWith(te)) {
			pairWith((ChestTileEntity*)te, true);
			((ChestTileEntity*)te)->pairWith(this, false);
			return;
		}
	}
}

void ChestTileEntity::_resetAABB()
{
}

float ChestTileEntity::getModelOffsetX()
{
	if (!isUnpaired) return 0.0f;

	ChestTileEntity* p = pair;
	int posZ, posX;
	if (pairSameXPos) {
		posZ = p->z;
		posX = z;
	} else {
		posZ = p->x;
		posX = x;
	}
	if (posZ >= posX) return 0.5f;
	else return -0.5f;
}

void ChestTileEntity::tick()
{
	super::tick();
	checkNeighbors();

	if (hasPendingPair) {
		TileEntity* te = level->getTileEntity(pendingPairX, y, pendingPairZ);
		if (te && canPairWith(te)) {
			pairWith((ChestTileEntity*)te, true);
			((ChestTileEntity*)te)->pairWith(this, false);
		}
		hasPendingPair = false;
	}

	tryPairWithNeighbors();

	if (++tickInterval >= 4 * SharedConstants::TicksPerSecond) {
		level->tileEvent(x, y, z, ChestTile::EVENT_SET_OPEN_COUNT, openCount);
		tickInterval = 0;
	}

	if (openedBy) {
		openDelay--;
		if (!openDelay) {
			openedBy->openContainer(this);
			openedBy = NULL;
		}
	}

	oOpenness = openness;

	ChestTileEntity* soundSource = this;
	if (!isUnpaired && pair) {
		soundSource = pair;
	}

	float speed = 0.10f;
	if (openCount > 0 && openness == 0) {
		float cx, cy, cz;
		soundSource->_getCenter(cx, cy, cz);
		level->playSound(cx, (float)y + 0.5f, cz, "random.chestopen", 0.5f, level->random.nextFloat() * 0.1f + 0.9f);
	}
	if ((openCount == 0 && openness > 0) || (openCount > 0 && openness < 1)) {
		float oldOpen = openness;
		if (openCount > 0) openness += speed;
		else openness -= speed;
		if (openness > 1) {
			openness = 1;
		}
		float lim = 0.5f;
		if (openness < lim && oldOpen >= lim) {
			float cx, cy, cz;
			soundSource->_getCenter(cx, cy, cz);
			level->playSound(cx, (float)y + 0.5f, cz, "random.chestclosed", 0.5f, level->random.nextFloat() * 0.1f + 0.9f);
		}
		if (openness < 0) {
			openness = 0;
		}
	}
}

void ChestTileEntity::triggerEvent( int b0, int b1 )
{
	if (b0 == ChestTile::EVENT_SET_OPEN_COUNT) {
		openCount = b1;
	}
}

void ChestTileEntity::startOpen()
{
	ChestTileEntity* primary = this;
	if (!isUnpaired && pair) {
		primary = pair;
	}
	primary->openCount++;
	if (level && !level->isClientSide) {
		primary->level->tileEvent(primary->x, primary->y, primary->z, ChestTile::EVENT_SET_OPEN_COUNT, primary->openCount);
	}
}

void ChestTileEntity::stopOpen()
{
	ChestTileEntity* primary = this;
	if (!isUnpaired && pair) {
		primary = pair;
	}
	if (primary->isUnpaired) {
		primary->openCount--;
		if (level && !level->isClientSide) {
			primary->level->tileEvent(primary->x, primary->y, primary->z, ChestTile::EVENT_SET_OPEN_COUNT, primary->openCount);
		}
	}
}

void ChestTileEntity::setRemoved()
{
	clearCache();
	super::setRemoved();
}
