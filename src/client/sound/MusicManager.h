#ifndef NET_MINECRAFT_CLIENT_SOUND__MusicManager_H__
#define NET_MINECRAFT_CLIENT_SOUND__MusicManager_H__

#include <string>
#include <vector>

class Options;

enum MusicType {
    MUSIC_TYPE_CALM,
    MUSIC_TYPE_HAL,
    MUSIC_TYPE_PIANO,
    MUSIC_TYPE_NUANCE,
    MUSIC_TYPE_COUNT
};

struct MusicTrack {
    std::string name;
    std::string filePath;
    MusicType type;
    MusicTrack() : type(MUSIC_TYPE_CALM) {}
};

class MusicManager {
public:
    MusicManager();
    ~MusicManager();

    void init(Options* options);
    void tick();
    void stop();
    bool isPlaying() const;
    void setVolume(float volume);

private:
    void loadTracks();
    void playTrack(int trackIndex);
    int pickNextTrack();

    Options* m_options;
    std::vector<MusicTrack> m_tracks;
    int m_musicDelay;
    int m_currentTrackIndex;
    bool m_isPlaying;
    float m_volume;

#if defined(ANDROID) && !defined(PRE_ANDROID23) && !defined(RPI)
    void* m_mediaPlayer;
    int m_currentSoundId;
    std::vector<int> m_soundIds;
    bool loadTracksAndroid();
    void playOnAndroid(int index);
    void stopOnAndroid();
    bool isPlayingOnAndroid();
#elif (defined(__APPLE__) || defined(PLATFORM_DESKTOP)) && !defined(NO_SOUND)
    unsigned int m_alSource;
    unsigned int m_alBuffers[2];
    bool m_alInitialized;
    bool initOpenAL();
    void shutdownOpenAL();
    bool loadAndQueueBuffer(const std::string& filePath);
    void unqueueBuffers();
    void playOpenAL();
    void stopOpenAL();
    bool isPlayingOpenAL();
    bool decodeOggToPcm(const std::string& filePath, std::vector<short>& pcmData, int& channels, int& sampleRate);
#endif
};

#endif
