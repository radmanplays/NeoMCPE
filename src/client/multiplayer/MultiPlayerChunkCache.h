#ifndef NET_MINECRAFT_CLIENT_MULTIPLAYER__MultiPlayerChunkCache_H__
#define NET_MINECRAFT_CLIENT_MULTIPLAYER__MultiPlayerChunkCache_H__

#include "../../world/level/chunk/ChunkSource.h"
#include "../../world/level/chunk/LevelChunk.h"
#include "../../world/level/Level.h"
#include "../../world/level/LevelConstants.h"

#include <map>

class MultiPlayerChunkCache : public ChunkSource
{
public:
	MultiPlayerChunkCache(Level* level)
	:	level(level)
	{
	}

	~MultiPlayerChunkCache() {
		for (ChunkMap::iterator it = chunks.begin(); it != chunks.end(); ++it) {
			delete it->second;
		}
	}

	bool hasChunk(int x, int z) {
		return chunks.find(ChunkPos(x, z)) != chunks.end();
	}

	LevelChunk* getChunk(int x, int z) {
		ChunkMap::iterator it = chunks.find(ChunkPos(x, z));
		if (it != chunks.end()) {
			return it->second;
		}
		return NULL;
	}

	LevelChunk* create(int x, int z) {
		ChunkPos pos(x, z);
		ChunkMap::iterator it = chunks.find(pos);
		if (it != chunks.end()) {
			return it->second;
		}
		LevelChunk* chunk = new LevelChunk(level, x, z);
		chunks[pos] = chunk;
		return chunk;
	}

	void postProcess(ChunkSource* parent, int x, int z) {
	}

	bool tick() {
		return false;
	}

	bool shouldSave() {
		return false;
	}

	void saveAll(bool onlyUnsaved) {
	}

	Biome::MobList getMobsAt(const MobCategory& mobCategory, int x, int y, int z) {
		return Biome::MobList();
	}

	std::string gatherStats() {
		return "";
	}

private:
	struct ChunkPos {
		int x, z;
		ChunkPos(int x, int z) : x(x), z(z) {}
		bool operator<(const ChunkPos& other) const {
			if (x != other.x) return x < other.x;
			return z < other.z;
		}
	};

	typedef std::map<ChunkPos, LevelChunk*> ChunkMap;

	Level* level;
	ChunkMap chunks;
};

#endif /* NET_MINECRAFT_CLIENT_MULTIPLAYER__MultiPlayerChunkCache_H__ */
