#include "MusicManager.h"
#include "../Options.h"
#include "../../util/Mth.h"
#include "../../platform/log.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if (defined(__APPLE__) || defined(PLATFORM_DESKTOP)) && !defined(NO_SOUND)
#if defined(__APPLE__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#define STB_VORBIS_NO_STDIO
#include "stb/stb_vorbis.c"
#elif defined(ANDROID) && !defined(PRE_ANDROID23) && !defined(RPI)
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#define STB_VORBIS_NO_STDIO
#include "stb/stb_vorbis.c"
#endif

static const int MUSIC_DELAY_MIN = 600;
static const int MUSIC_DELAY_MAX = 1800;

MusicManager::MusicManager()
    : m_options(NULL)
    , m_musicDelay(0)
    , m_currentTrackIndex(-1)
    , m_isPlaying(false)
    , m_volume(1.0f)
#if defined(ANDROID) && !defined(PRE_ANDROID23) && !defined(RPI)
    , m_audioPlayer(NULL)
    , m_bufferQueueItf(NULL)
    , m_volumeItf(NULL)
    , m_playItf(NULL)
    , m_currentSampleOffset(0)
    , m_totalSamples(0)
    , m_channels(0)
    , m_sampleRate(0)
    , m_androidPlayerCreated(false)
#elif (defined(__APPLE__) || defined(PLATFORM_DESKTOP)) && !defined(NO_SOUND)
    , m_alSource(0)
    , m_alInitialized(false)
#endif
{
#if (defined(__APPLE__) || defined(PLATFORM_DESKTOP)) && !defined(NO_SOUND)
    m_alBuffers[0] = 0;
    m_alBuffers[1] = 0;
#endif
}

MusicManager::~MusicManager()
{
    stop();
#if (defined(__APPLE__) || defined(PLATFORM_DESKTOP)) && !defined(NO_SOUND)
    shutdownOpenAL();
#endif
}

void MusicManager::init(Options* options, SoundSystem* soundSystem, Minecraft* minecraft)
{
    m_options = options;
    m_soundSystem = soundSystem;
    m_minecraft = minecraft;
    m_musicDelay = Mth::random(MUSIC_DELAY_MAX - MUSIC_DELAY_MIN) + MUSIC_DELAY_MIN;
    m_currentTrackIndex = -1;
    m_isPlaying = false;
    m_volume = 1.0f;
    loadTracks();
#if (defined(__APPLE__) || defined(PLATFORM_DESKTOP)) && !defined(NO_SOUND)
    initOpenAL();
#endif
}

void MusicManager::loadTracks()
{
    m_tracks.clear();
    MusicTrack t;
    t.name = "calm1"; t.filePath = "data/sound/music/calm1.ogg"; t.type = MUSIC_TYPE_CALM; m_tracks.push_back(t);
    t.name = "calm2"; t.filePath = "data/sound/music/calm2.ogg"; t.type = MUSIC_TYPE_CALM; m_tracks.push_back(t);
    t.name = "calm3"; t.filePath = "data/sound/music/calm3.ogg"; t.type = MUSIC_TYPE_CALM; m_tracks.push_back(t);
    t.name = "piano1"; t.filePath = "data/sound/newmusic/piano1.ogg"; t.type = MUSIC_TYPE_PIANO; m_tracks.push_back(t);
    t.name = "piano2"; t.filePath = "data/sound/newmusic/piano2.ogg"; t.type = MUSIC_TYPE_PIANO; m_tracks.push_back(t);
    t.name = "piano3"; t.filePath = "data/sound/newmusic/piano3.ogg"; t.type = MUSIC_TYPE_PIANO; m_tracks.push_back(t);
    t.name = "hal1"; t.filePath = "data/sound/newmusic/hal1.ogg"; t.type = MUSIC_TYPE_HAL; m_tracks.push_back(t);
    t.name = "hal2"; t.filePath = "data/sound/newmusic/hal2.ogg"; t.type = MUSIC_TYPE_HAL; m_tracks.push_back(t);
    t.name = "hal3"; t.filePath = "data/sound/newmusic/hal3.ogg"; t.type = MUSIC_TYPE_HAL; m_tracks.push_back(t);
    t.name = "hal4"; t.filePath = "data/sound/newmusic/hal4.ogg"; t.type = MUSIC_TYPE_HAL; m_tracks.push_back(t);
    t.name = "nuance1"; t.filePath = "data/sound/newmusic/nuance1.ogg"; t.type = MUSIC_TYPE_NUANCE; m_tracks.push_back(t);
    t.name = "nuance2"; t.filePath = "data/sound/newmusic/nuance2.ogg"; t.type = MUSIC_TYPE_NUANCE; m_tracks.push_back(t);
}

void MusicManager::tick()
{
    if (!m_options) return;
    float musicVol = m_options->getProgressValue(OPTIONS_MUSIC_VOLUME);
    if (musicVol <= 0.0f) {
        if (m_isPlaying) stop();
        return;
    }
    setVolume(musicVol);
    if (m_isPlaying) {
#if defined(ANDROID) && !defined(PRE_ANDROID23) && !defined(RPI)
        if (!isPlayingOnAndroid()) {
            m_isPlaying = false;
            m_musicDelay = Mth::random(MUSIC_DELAY_MAX - MUSIC_DELAY_MIN) + MUSIC_DELAY_MIN;
        }
#elif (defined(__APPLE__) || defined(PLATFORM_DESKTOP)) && !defined(NO_SOUND)
        if (!isPlayingOpenAL()) {
            m_isPlaying = false;
            m_musicDelay = Mth::random(MUSIC_DELAY_MAX - MUSIC_DELAY_MIN) + MUSIC_DELAY_MIN;
        }
#else
        m_isPlaying = false;
        m_musicDelay = Mth::random(MUSIC_DELAY_MAX - MUSIC_DELAY_MIN) + MUSIC_DELAY_MIN;
#endif
        return;
    }
    if (--m_musicDelay <= 0) {
        int trackIndex = pickNextTrack();
        if (trackIndex >= 0 && trackIndex < (int)m_tracks.size()) {
            playTrack(trackIndex);
        }
        m_musicDelay = Mth::random(MUSIC_DELAY_MAX - MUSIC_DELAY_MIN) + MUSIC_DELAY_MIN;
    }
}

int MusicManager::pickNextTrack()
{
    if (m_tracks.empty()) return -1;
    return Mth::random((int)m_tracks.size());
}

void MusicManager::playTrack(int trackIndex)
{
    if (trackIndex < 0 || trackIndex >= (int)m_tracks.size()) return;
    m_currentTrackIndex = trackIndex;
    MusicTrack& track = m_tracks[trackIndex];
#if defined(ANDROID) && !defined(PRE_ANDROID23) && !defined(RPI)
    stopOnAndroid();
    playOnAndroid(trackIndex);
    m_isPlaying = true;
#elif (defined(__APPLE__) || defined(PLATFORM_DESKTOP)) && !defined(NO_SOUND)
    stopOpenAL();
    if (loadAndQueueBuffer(track.filePath)) {
        playOpenAL();
        m_isPlaying = true;
    }
#else
    m_isPlaying = true;
#endif
}

void MusicManager::stop()
{
#if defined(ANDROID) && !defined(PRE_ANDROID23) && !defined(RPI)
    stopOnAndroid();
#elif (defined(__APPLE__) || defined(PLATFORM_DESKTOP)) && !defined(NO_SOUND)
    stopOpenAL();
#endif
    m_isPlaying = false;
}

bool MusicManager::isPlaying() const
{
    return m_isPlaying;
}

void MusicManager::setVolume(float volume)
{
    m_volume = volume;
#if (defined(__APPLE__) || defined(PLATFORM_DESKTOP)) && !defined(NO_SOUND)
    if (m_alInitialized && m_alSource != 0) {
        alSourcef(m_alSource, AL_GAIN, m_volume);
    }
#elif defined(ANDROID) && !defined(PRE_ANDROID23) && !defined(RPI)
    if (m_volumeItf) {
        SLmillibel maxVol = 0;
        (*(SLVolumeItf)m_volumeItf)->GetMaxVolumeLevel((SLVolumeItf)m_volumeItf, &maxVol);
        (*(SLVolumeItf)m_volumeItf)->SetVolumeLevel((SLVolumeItf)m_volumeItf, maxVol - (SLmillibel)((1.0f - m_volume) * 2000.0f));
    }
#endif
}

#if defined(ANDROID) && !defined(PRE_ANDROID23) && !defined(RPI)

bool MusicManager::loadTracksAndroid() { return true; }

void MusicManager::bufferCallback(void* context, void* player) {
    MusicManager* self = (MusicManager*)context;
    self->fillNextBuffer();
}

void MusicManager::fillNextBuffer() {
    if (!m_bufferQueueItf || m_currentSampleOffset >= m_totalSamples) return;
    int samplesPerBuffer = m_sampleRate * 2;
    int remaining = m_totalSamples - m_currentSampleOffset;
    int toCopy = (remaining < samplesPerBuffer) ? remaining : samplesPerBuffer;
    int bytesToCopy = toCopy * m_channels * sizeof(short);
    SLAndroidSimpleBufferQueueItf queue = (SLAndroidSimpleBufferQueueItf)m_bufferQueueItf;
    SLresult res = (*queue)->Enqueue(queue, &m_pcmData[m_currentSampleOffset * m_channels], bytesToCopy);
    if (res == SL_RESULT_SUCCESS) m_currentSampleOffset += toCopy;
}

void MusicManager::playOnAndroid(int index) {
    if (index < 0 || index >= (int)m_tracks.size()) return;
    MusicTrack& track = m_tracks[index];
    stopOnAndroid();

    BinaryBlob blob;
    if (m_minecraft && m_minecraft->platform()) {
        blob = m_minecraft->platform()->readAssetFile(track.filePath);
    }
    if (!blob.data || blob.size <= 0) {
        LOGE("Music: cannot load asset %s (size=%d)\n", track.filePath.c_str(), blob.size);
        return;
    }

    int numChannels = 0, sampleRateOut = 0;
    short* decoded = NULL;
    int numSamples = stb_vorbis_decode_memory(blob.data, blob.size, &numChannels, &sampleRateOut, &decoded);
    if (numSamples <= 0 || !decoded) { LOGE("Music: decode failed %s\n", track.filePath.c_str()); return; }

    m_channels = numChannels;
    m_sampleRate = sampleRateOut;
    m_totalSamples = numSamples;
    m_currentSampleOffset = 0;
    m_pcmData.resize((size_t)numSamples * numChannels);
    memcpy(&m_pcmData[0], decoded, (size_t)numSamples * numChannels * sizeof(short));
    free(decoded);
    LOGI("Music: decoded %s (%d samples, %d Hz, %d ch)\n", track.filePath.c_str(), numSamples, sampleRateOut, numChannels);

    SLDataLocator_AndroidSimpleBufferQueue locBufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM formatPcm = {
        SL_DATAFORMAT_PCM, (SLuint32)m_channels, (SLuint32)(m_sampleRate * 1000),
        SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
        m_channels == 1 ? SL_SPEAKER_FRONT_CENTER : (SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT),
        SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource audioSrc = {&locBufq, &formatPcm};

    SoundSystemSL* soundSys = (SoundSystemSL*)m_soundSystem;
    if (!soundSys) { LOGE("Music: no sound system\n"); return; }
    SLObjectItf outputMix = soundSys->getOutputMix();
    SLEngineItf eng = soundSys->getEngine();
    if (!eng || !outputMix) { LOGE("Music: no engine/outputmix\n"); return; }

    SLDataLocator_OutputMix locOutmix = {SL_DATALOCATOR_OUTPUTMIX, outputMix};
    SLDataSink audioSnk = {&locOutmix, NULL};
    SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
    SLboolean req[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    SLresult res = (*eng)->CreateAudioPlayer(eng, (SLObjectItf*)&m_audioPlayer, &audioSrc, &audioSnk, 2, ids, req);
    if (res != SL_RESULT_SUCCESS) { LOGE("Music: CreateAudioPlayer failed %d\n", res); return; }
    (*(SLObjectItf)m_audioPlayer)->Realize((SLObjectItf)m_audioPlayer, SL_BOOLEAN_FALSE);
    (*(SLObjectItf)m_audioPlayer)->GetInterface((SLObjectItf)m_audioPlayer, SL_IID_PLAY, &m_playItf);
    (*(SLObjectItf)m_audioPlayer)->GetInterface((SLObjectItf)m_audioPlayer, SL_IID_BUFFERQUEUE, &m_bufferQueueItf);
    (*(SLObjectItf)m_audioPlayer)->GetInterface((SLObjectItf)m_audioPlayer, SL_IID_VOLUME, &m_volumeItf);

    fillNextBuffer();
    fillNextBuffer();
    (*(SLAndroidSimpleBufferQueueItf)m_bufferQueueItf)->RegisterCallback((SLAndroidSimpleBufferQueueItf)m_bufferQueueItf, &MusicManager::bufferCallback, this);

    SLmillibel maxVol = 0;
    (*(SLVolumeItf)m_volumeItf)->GetMaxVolumeLevel((SLVolumeItf)m_volumeItf, &maxVol);
    (*(SLVolumeItf)m_volumeItf)->SetVolumeLevel((SLVolumeItf)m_volumeItf, maxVol - (SLmillibel)((1.0f - m_volume) * 2000.0f));
    (*(SLPlayItf)m_playItf)->SetPlayState((SLPlayItf)m_playItf, SL_PLAYSTATE_PLAYING);
    m_androidPlayerCreated = true;
}

void MusicManager::stopOnAndroid() {
    if (m_audioPlayer) { (*(SLObjectItf)m_audioPlayer)->Destroy((SLObjectItf)m_audioPlayer); m_audioPlayer = NULL; m_bufferQueueItf = NULL; m_volumeItf = NULL; m_playItf = NULL; m_androidPlayerCreated = false; }
    m_pcmData.clear(); m_currentSampleOffset = 0; m_totalSamples = 0;
}

bool MusicManager::isPlayingOnAndroid() {
    if (!m_androidPlayerCreated || !m_playItf) return false;
    SLuint32 state = 0;
    (*(SLPlayItf)m_playItf)->GetPlayState((SLPlayItf)m_playItf, &state);
    return state == SL_PLAYSTATE_PLAYING && m_currentSampleOffset < m_totalSamples;
}

#elif (defined(__APPLE__) || defined(PLATFORM_DESKTOP)) && !defined(NO_SOUND)

bool MusicManager::initOpenAL()
{
    ALCcontext* context = alcGetCurrentContext();
    if (!context) return false;
    alGenSources(1, &m_alSource);
    alGenBuffers(2, m_alBuffers);
    if (m_alSource == 0) return false;
    alSourcei(m_alSource, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(m_alSource, AL_POSITION, 0.0f, 0.0f, 0.0f);
    m_alInitialized = true;
    return true;
}

void MusicManager::shutdownOpenAL()
{
    if (!m_alInitialized) return;
    if (!alcGetCurrentContext()) { m_alInitialized = false; return; }
    stopOpenAL();
    if (m_alSource != 0) { alDeleteSources(1, &m_alSource); m_alSource = 0; }
    if (m_alBuffers[0] != 0) { alDeleteBuffers(2, m_alBuffers); m_alBuffers[0] = 0; m_alBuffers[1] = 0; }
    m_alInitialized = false;
}

bool MusicManager::decodeOggToPcm(const std::string& filePath, std::vector<short>& pcmData, int& channels, int& sampleRate)
{
    FILE* f = fopen(filePath.c_str(), "rb");
    if (!f) {
        std::string altPath = "data/" + filePath;
        f = fopen(altPath.c_str(), "rb");
        if (!f) { LOGE("Music: cannot open %s\n", filePath.c_str()); return false; }
    }
    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* fileData = new unsigned char[fileSize];
    fread(fileData, 1, fileSize, f);
    fclose(f);

    int numChannels = 0;
    int sampleRateOut = 0;
    short* decoded = NULL;
    int numSamples = stb_vorbis_decode_memory(fileData, (int)fileSize, &numChannels, &sampleRateOut, &decoded);
    delete[] fileData;

    if (numSamples <= 0 || !decoded) {
        LOGE("Music: stb_vorbis_decode failed for %s (samples=%d)\n", filePath.c_str(), numSamples);
        return false;
    }

    channels = numChannels;
    sampleRate = sampleRateOut;
    pcmData.resize((size_t)numSamples * numChannels);
    memcpy(&pcmData[0], decoded, (size_t)numSamples * numChannels * sizeof(short));
    free(decoded);

    LOGI("Music: decoded %s (%d samples, %d Hz, %d ch)\n", filePath.c_str(), numSamples, sampleRate, channels);
    return true;
}

bool MusicManager::loadAndQueueBuffer(const std::string& filePath)
{
    if (!m_alInitialized || m_alSource == 0) return false;
    std::vector<short> pcmData;
    int channels, sampleRate;
    if (!decodeOggToPcm(filePath, pcmData, channels, sampleRate)) return false;
    if (pcmData.empty()) return false;
    ALenum format = (channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, &pcmData[0], (ALsizei)(pcmData.size() * sizeof(short)), sampleRate);
    alSourceQueueBuffers(m_alSource, 1, &buffer);
    return true;
}

void MusicManager::unqueueBuffers()
{
    ALint queued;
    alGetSourcei(m_alSource, AL_BUFFERS_QUEUED, &queued);
    while (queued > 0) {
        ALuint buf;
        alSourceUnqueueBuffers(m_alSource, 1, &buf);
        alDeleteBuffers(1, &buf);
        queued--;
    }
}

void MusicManager::playOpenAL()
{
    if (!m_alInitialized || m_alSource == 0) return;
    alSourcef(m_alSource, AL_GAIN, m_volume);
    alSourcePlay(m_alSource);
}

void MusicManager::stopOpenAL()
{
    if (!m_alInitialized || m_alSource == 0) return;
    ALint state;
    alGetSourcei(m_alSource, AL_SOURCE_STATE, &state);
    if (state == AL_PLAYING) alSourceStop(m_alSource);
    unqueueBuffers();
}

bool MusicManager::isPlayingOpenAL()
{
    if (!m_alInitialized || m_alSource == 0) return false;
    ALint state;
    alGetSourcei(m_alSource, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

#endif
