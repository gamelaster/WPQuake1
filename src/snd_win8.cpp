/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// snd_win8.cpp

#include "quakedef.h"

#include "snd_local.h"

cvar_t bgmvolume = {"bgmvolume", "1", true};
cvar_t volume = {"volume", "0.7", true};
bool scr_skipupdate = false;
bool DDActive = false;

IXAudio2* idSoundSystem::pXAudio2;
IXAudio2MasteringVoice* idSoundSystem::pMasterVoice;
idSound	 idSoundSystem::sounds[MAX_LOADED_SOUNDS];

sfx_t idSoundSystem::soundPool[MAX_LOADED_SOUNDS];
int idSoundSystem::numLoadedSounds = 0;



void S_BlockSound (void) {

}
void S_Init (void)
{
	HRESULT hr;
	Con_Printf("------- Sound Init ----------\n");

	Con_Printf("Init XAudio...\n");
	
	if ( FAILED(hr = XAudio2Create( &idSoundSystem::pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR ) ) ) {
		Sys_Error("Failed to init xaudio...\n");
	}
	
	if ( FAILED(hr = idSoundSystem::pXAudio2->CreateMasteringVoice( &idSoundSystem::pMasterVoice ) ) ) {
		Sys_Error("Failed to init master voice for xaudio\n");
	}
}

void S_AmbientOff (void)
{
}

void S_AmbientOn (void)
{
}

void S_Shutdown (void)
{
}

void S_TouchSound (char *sample)
{
}

void S_ClearBuffer (void)
{
}

void S_StaticSound (sfx_t *sfx, vec3_t origin, float vol, float attenuation)
{
	((idSound *)sfx->cache.data)->PlayLocal();
}

void S_StartSound (int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol,  float attenuation)
{
	((idSound *)sfx->cache.data)->PlayLocal();
}

void S_StopSound (int entnum, int entchannel)
{
}

sfx_t *S_PrecacheSound (char *sample)
{
	sfx_t *sfx;
	idSound *sound;
	WAVEFORMATEX wfx;
	HRESULT hr;

	// Check to see if the sound is already loaded.
	for(int i = 0; i < idSoundSystem::numLoadedSounds; i++) {
		if(strstr(idSoundSystem::soundPool[i].name, sample )) {
			return &idSoundSystem::soundPool[i];
		}
	}

	sfx = &idSoundSystem::soundPool[idSoundSystem::numLoadedSounds];
	strcpy(sfx->name, sample);
	
	sound = &idSoundSystem::sounds[idSoundSystem::numLoadedSounds];
	if(!S_LoadWavFromFS( sample, sound->buffer, wfx )) {
		return NULL;
	}

	if( FAILED(hr = idSoundSystem::pXAudio2->CreateSourceVoice( &sound->pSourceVoice, (WAVEFORMATEX*)&wfx ) ) ) {
		Sys_Error("S_PrecacheSound: Failed to create source voice for %s\n", sample );
		return NULL;
	}

	if( FAILED(hr = sound->pSourceVoice->SubmitSourceBuffer( &sound->buffer ) ) ) {
		Sys_Error("S_PrecacheSound: Failed to submit source buffer for %s\n", sample );
		return NULL;
	}


	sfx->cache.data = (void *)sound;

	idSoundSystem::numLoadedSounds++;
	return sfx;
}

void S_ClearPrecache (void)
{
}

void S_Update (vec3_t origin, vec3_t v_forward, vec3_t v_right, vec3_t v_up)
{	

}

void S_StopAllSounds (qboolean clear)
{
}

void S_BeginPrecaching (void)
{
}

void S_EndPrecaching (void)
{
}

void S_ExtraUpdate (void)
{
}

void S_LocalSound (char *s)
{
	sfx_t *sfx = S_PrecacheSound( s );
	if(!sfx) {
		Con_Printf("S_LocalSound: Failed to find sound %s\n", s );
		return;
	}

	((idSound *)sfx->cache.data)->PlayLocal();
}

