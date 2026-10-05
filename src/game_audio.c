#include "game_audio.h"

#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    AMBIENT_SIREN,
    AMBIENT_FRIGHTENED,
    AMBIENT_EYES,
    AMBIENT_COUNT,
    AMBIENT_NONE
} AmbientType;

typedef struct {
    Sound first;
    Music loop;
} AmbientSound;

static struct {
    AmbientSound ambient[AMBIENT_COUNT];
    Sound ghostEaten;
    AmbientType current;
    bool loopStarted;
} audio;

static Sound loadSoundChecked(const char *path)
{
    Sound sound = LoadSound(path);

    if (!IsSoundValid(sound)) {
        fprintf(stderr, "failed to load sound: %s\n", path);
        exit(EXIT_FAILURE);
    }

    return sound;
}

static Music loadMusicChecked(const char *path)
{
    Music music = LoadMusicStream(path);

    if (!IsMusicValid(music)) {
        fprintf(stderr, "failed to load music: %s\n", path);
        exit(EXIT_FAILURE);
    }

    music.looping = true;
    return music;
}

static void stopAmbient(void)
{
    if (audio.current != AMBIENT_NONE) {
        StopSound(audio.ambient[audio.current].first);
        StopMusicStream(audio.ambient[audio.current].loop);
    }

    audio.current = AMBIENT_NONE;
    audio.loopStarted = false;
}

void initGameAudio(void)
{
    audio.current = AMBIENT_NONE;
    audio.loopStarted = false;

    audio.ambient[AMBIENT_SIREN].first =
        loadSoundChecked("resources/audio/pacman-arcade-siren-0-first-loop.wav");
    audio.ambient[AMBIENT_SIREN].loop =
        loadMusicChecked("resources/audio/pacman-arcade-siren-0-loop.wav");

    audio.ambient[AMBIENT_FRIGHTENED].first =
        loadSoundChecked("resources/audio/pacman-arcade-frightened-first-loop.wav");
    audio.ambient[AMBIENT_FRIGHTENED].loop =
        loadMusicChecked("resources/audio/pacman-arcade-frightened-loop.wav");

    audio.ambient[AMBIENT_EYES].first =
        loadSoundChecked("resources/audio/pacman-arcade-eyes-first-loop.wav");
    audio.ambient[AMBIENT_EYES].loop =
        loadMusicChecked("resources/audio/pacman-arcade-eyes-loop.wav");

    audio.ghostEaten =
        loadSoundChecked("resources/audio/pacman-arcade-eat-ghost.wav");
}

void stopGameAudio(void)
{
    stopAmbient();
    StopSound(audio.ghostEaten);
}

void updateGameAudio(bool frightened, bool eyesReturning)
{
    AmbientType wanted = eyesReturning
        ? AMBIENT_EYES
        : frightened ? AMBIENT_FRIGHTENED : AMBIENT_SIREN;

    if (wanted != audio.current) {
        stopAmbient();
        audio.current = wanted;
        PlaySound(audio.ambient[wanted].first);
    }

    AmbientSound *track = &audio.ambient[audio.current];

    if (!audio.loopStarted && !IsSoundPlaying(track->first)) {
        PlayMusicStream(track->loop);
        audio.loopStarted = true;
    }

    if (audio.loopStarted) {
        UpdateMusicStream(track->loop);
    }
}

void playGhostEatenSound(void)
{
    PlaySound(audio.ghostEaten);
}

void endGameAudio(void)
{
    stopGameAudio();

    for (int i = 0; i < AMBIENT_COUNT; i++) {
        UnloadSound(audio.ambient[i].first);
        UnloadMusicStream(audio.ambient[i].loop);
    }

    UnloadSound(audio.ghostEaten);
}
