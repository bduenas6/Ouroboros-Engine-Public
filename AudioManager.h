#pragma once
#include <string>
#include "Helper.h"
#include "AudioHelper.h"
#include "SDL2_mixer/SDL_mixer.h"
#include <unordered_map>

class AudioManager
{
public:
	static void initialize();
	static int playSound(int channel, std::string filename, bool does_loop);
	static int stopChannel(int channel);
	static int setVolume(int channel, int volume);
private:
	static inline std::unordered_map < std::string, Mix_Chunk*> sound_map;
};

