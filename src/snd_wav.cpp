// snd_wav.cpp
//

#include "quakedef.h"

#include "snd_local.h"

//
// From http://msdn.microsoft.com/en-us/library/windows/desktop/aa365541(v=vs.85).aspx
//

#if defined(_XBOX)  //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#else
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

int filePakBase = 0;

//
// S_Wav_FindChunk
//
HRESULT S_Wav_FindChunk(FILE *hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
    HRESULT hr = S_OK;

	
	fseek( hFile, filePakBase, SEEK_SET );
    
    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;
	 DWORD dwRead = 1;

	const DWORD fourCCRef = fourccRIFF;

    while (dwRead > 0)
    {
       

		dwRead = fread( &dwChunkType, sizeof(DWORD), 1, hFile );
		if(dwRead <= 0) {
			return HRESULT_FROM_WIN32( GetLastError() );
		}

		dwRead = fread( &dwChunkDataSize, sizeof(DWORD), 1, hFile );
		if(dwRead <= 0) {
			return HRESULT_FROM_WIN32( GetLastError() );
		}

        switch (dwChunkType)
        {
        case fourCCRef:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
			dwRead = fread(&dwFileType, sizeof(DWORD), 1, hFile );
            
            break;

        default:
			fseek(hFile, dwChunkDataSize, SEEK_CUR );
        }

        dwOffset += sizeof(DWORD) * 2;
        
        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;
        
        if (bytesRead >= dwRIFFDataSize) return S_FALSE;

    }

    return S_OK;
    
}

HRESULT ReadChunkData(FILE *hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;

	fseek( hFile, filePakBase + bufferoffset, SEEK_SET );

    DWORD dwRead;
	dwRead = fread( buffer, buffersize, 1, hFile ); 
    return S_OK;
}

bool S_LoadWavFromFS( const char *path, XAUDIO2_BUFFER &buffer, WAVEFORMATEX &wf ) {
	FILE *hFile;
	WAVEFORMATEXTENSIBLE wfx = {0};
	char sndpath[522];

	sprintf(sndpath, "sound/%s", path );

	int len = COM_FOpenFile( (char *)sndpath, &hFile );

	if(len <= 0 || !hFile) {
		Con_Printf( "Failed to load WAV %s\n", sndpath );
		return false;
	}

	filePakBase = ftell( hFile );

	DWORD dwChunkSize;
	DWORD dwChunkPosition;
	//check the file type, should be fourccWAVE or 'XWMA'
	S_Wav_FindChunk(hFile,fourccRIFF,dwChunkSize, dwChunkPosition );
	DWORD filetype;
	ReadChunkData(hFile,&filetype,sizeof(DWORD),dwChunkPosition);
	int typeCompare = fourccWAVE;
	if (filetype != typeCompare) {
		Sys_Error("Invalid wav file\n");
		return false;
	}

	S_Wav_FindChunk(hFile,fourccFMT, dwChunkSize, dwChunkPosition );
	ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition );

	//fill out the audio data buffer with the contents of the fourccDATA chunk
	S_Wav_FindChunk(hFile,fourccDATA,dwChunkSize, dwChunkPosition );
	BYTE * pDataBuffer = new BYTE[dwChunkSize];
	ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

	buffer.AudioBytes = dwChunkSize;  //buffer containing audio data
	buffer.pAudioData = pDataBuffer;  //size of the audio buffer in bytes
	buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

	wf = wfx.Format;

	return true;
}