// snd_local.h
//

// snd_local.h
//

#include <windows.h>
#include <x3daudio.h>
#include <Xaudio2.h>

#define MAX_LOADED_SOUNDS			5000

class idSound {
public:
	XAUDIO2_BUFFER		buffer;
	IXAudio2SourceVoice* pSourceVoice;
	void				PlayLocal( void );
};

struct idSoundSystem {
	static IXAudio2* pXAudio2;
	static IXAudio2MasteringVoice* pMasterVoice;
	static IXAudio2SourceVoice* pSourceVoice;
	static idSound	 sounds[MAX_LOADED_SOUNDS];
	static sfx_t     soundPool[MAX_LOADED_SOUNDS];
	static int	numLoadedSounds;
};

bool S_LoadWavFromFS( const char *path, XAUDIO2_BUFFER &buffer, WAVEFORMATEX &wx );