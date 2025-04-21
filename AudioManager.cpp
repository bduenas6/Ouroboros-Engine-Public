#include "AudioManager.h"

void AudioManager::initialize()
{
	AudioHelper::Mix_OpenAudio(48000, AUDIO_F32SYS, 1, 2048);
	AudioHelper::Mix_AllocateChannels(50);
}

int AudioManager::playSound(int channel, std::string filename, bool does_loop)
{
	//std::cout << "attempting to play " << filename << std::endl;
	Mix_Chunk* chunk;
	if (sound_map.find(filename) != sound_map.end()) {
		//std::cout << "found in map" << std::endl;
		chunk = sound_map[filename];
	}
	else {
		//std::cout << "making new chunk" << std::endl;
		std::string sound_location;
		if (std::filesystem::exists("resources/audio/" + filename + ".wav"))
			sound_location = "resources/audio/" + filename + ".wav";
		else if (std::filesystem::exists("resources/audio/" + filename + ".ogg"))
			sound_location = "resources/audio/" + filename + ".ogg";
		else if (filename == "")
			return -1;
		else {
			"error: failed to play audio clip " + filename;
			exit(0);
		}
		chunk = AudioHelper::Mix_LoadWAV(sound_location.c_str());
		sound_map.insert(std::pair<std::string, Mix_Chunk*>(filename, chunk));
	}
	if (does_loop) return AudioHelper::Mix_PlayChannel(channel, chunk, -1);
	return AudioHelper::Mix_PlayChannel(channel, chunk, 0);
}

int AudioManager::stopChannel(int channel) {
	return AudioHelper::Mix_HaltChannel(channel);
}

int AudioManager::setVolume(int channel, int volume)
{
	return AudioHelper::Mix_Volume(channel, volume);
}

//TODO: Test suite 5
/*
Current plan:
make unordered map to cache sounds, with string keys and sound chunk values? (value data type is up in the air)
encapsulate everything into callable functions so sounds can be called from elsewhere
make things static so that it can be called from scene? might not be necessary as step_sfx is the only one that might be trouble to reach from engine
*/