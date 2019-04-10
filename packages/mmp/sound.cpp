#include "sound.h"

#include "SDL.h"
#include "SDL_mixer.h"

#include <iostream>
#include "../libtext.h"
#include "../pmxvLogger.h"

using namespace std;

void initSound(string filename)
{
	if(filename=="") return;
	
	log("initSound initializing sound...");
	
	SDL_Init(SDL_INIT_AUDIO);
	
	Mix_Music *music=NULL;
	
	int audio_rate = 44100;
	Uint16 audio_format = AUDIO_S16SYS; //16-bit stereo
	int audio_channels = 2;
	int audio_buffers = 4096;
	
	
	Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3);
	
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)==-1)
	{
		err(libtext::format("Error initializing SDL_mixer: %s\n", Mix_GetError()));
		exit(EXIT_FAILURE);
	}
	

	//string filename="data/motion/Watashi no Jikan/jikan.ogg";
	music = Mix_LoadMUS(filename.c_str());
	
	if( music == NULL)
	{
		err("ERROR: playMusic failed to load music");
		cerr<<SDL_GetError()<<endl;
	}
	
	log("initSound done");

	Mix_PlayMusic(music, 0);
}
