#ifndef NET_MINECRAFT_CLIENT_SOUND__MusicManager_H__
#define NET_MINECRAFT_CLIENT_SOUND__MusicManager_H__

#include <string>
#include <vector>

class Options;
class SoundSystem;

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

    void init(Options* options, SoundSystem* soundSystem);
    void tick();
    void stop();
    bool isPlaying() const;
    void setVolume(float volume);

private:
    void loadTracks();
    void playTrack(int trackIndex);
    int pickNextTrack();

    Options* m_options;
    SoundSystem* m_soundSystem;
    std::vector<MusicTrack> m_tracks;
    int m_musicDelay;
    int m_currentTrackIndex;
    bool m_isPlaying;
    float m_volume;

#if defined(ANDROID) && !defined(PRE_ANDROID23) && !defined(RPI)
    void* m_audioPlayer;
    void* m_bufferQueueItf;
    void* m_volumeItf;
    void* m_playItf;
    std::vector<short> m_pcmData;
    int m_currentSampleOffset;
    int m_totalSamples;
    int m_channels;
    int m_sampleRate;
    bool m_androidPlayerCreated;
    bool loadTracksAndroid();
    void playOnAndroid(int index);
    void stopOnAndroid();
    bool isPlayingOnAndroid();
    void fillNextBuffer();
    static void bufferCallback(void* context, void* player);
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
