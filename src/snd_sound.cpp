// snd_sound.cpp
//

#include "quakedef.h"

#include "snd_local.h"

void idSound::PlayLocal( void ) {
	HRESULT hr;
	if ( FAILED(hr = pSourceVoice->Start( 0 ) ) ) {
		Sys_Error("idSound::PlayLocal: Failed to play sound\n");
	}
}