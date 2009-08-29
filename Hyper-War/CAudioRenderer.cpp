/***************************************************************/
/*	CAudioRenderer.cpp: Class for rendering audio using OpenAL */
/*															   */
/*	July 2009												   */
/***************************************************************/

#include "CAudioRenderer.h"

CAudioRenderer::CAudioRenderer()
{
	timeSinceAudioRender = 999999;

	// Initialize OpenAL and clear the error bit.
	alutInit(NULL, 0);
	alGetError();

	// Position of the listener.
	memset(ListenerPos, 0, sizeof(float)*3);

	// Velocity of the listener.
	memset(ListenerVel, 0, sizeof(float)*3);

	// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
	memset(ListenerOri, 0, sizeof(float)*6);
	ListenerOri[0] = 0.0;
	ListenerOri[1] = 0.0;
	ListenerOri[2] = -1.0;
	ListenerOri[3] = 0.0;
	ListenerOri[4] = 1.0;
	ListenerOri[5] = 0.0;

	LoadALData();
	SetListenerValues();
}

CAudioRenderer::~CAudioRenderer()
{
	alDeleteBuffers(NUM_BUFFERS, Buffers);
	alDeleteSources(NUM_SOURCES, Sources);
	alutExit();
}

ALboolean CAudioRenderer::LoadALData()
{
	// Variables to load into.
	ALenum format;
	ALsizei size;
	ALvoid* data;
	ALsizei freq;
	ALboolean loop;

	// Load wav data into buffers.
	alGenBuffers(NUM_BUFFERS, Buffers);

	if(alGetError() != AL_NO_ERROR)
		return AL_FALSE;
/*

	alutLoadWAVFile("wavdata/Battle.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[BATTLE], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);
*/
    // load battle sounds
	Buffers[SOUND_EXPLOSION] = alutCreateBufferFromFile ("explosion5.wav");
	Buffers[SOUND_SHOOT] = alutCreateBufferFromFile ("shoot.wav");
	Buffers[SOUND_INTRO] = alutCreateBufferFromFile ("intro.wav");	
	Buffers[SOUND_MISSILE_EXPL] = alutCreateBufferFromFile ("missile_expl.wav");
	Buffers[SOUND_SPMUSIC] = alutCreateBufferFromFile ("spmusic.wav");

	// Bind buffers into audio sources.
	alGenSources(NUM_SOURCES, Sources);

	if(alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	alSourcei (Sources[SOUND_EXPLOSION], AL_BUFFER,   Buffers[SOUND_EXPLOSION]   );
	alSourcef (Sources[SOUND_EXPLOSION], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_EXPLOSION], AL_GAIN,     0.8f              );
	alSourcefv(Sources[SOUND_EXPLOSION], AL_POSITION, SourcesPos[SOUND_EXPLOSION]);
	alSourcefv(Sources[SOUND_EXPLOSION], AL_VELOCITY, SourcesVel[SOUND_EXPLOSION]);
	alSourcei (Sources[SOUND_EXPLOSION], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_SHOOT], AL_BUFFER,   Buffers[SOUND_SHOOT]   );
	alSourcef (Sources[SOUND_SHOOT], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_SHOOT], AL_GAIN,     0.8f              );
	alSourcefv(Sources[SOUND_SHOOT], AL_POSITION, SourcesPos[SOUND_SHOOT]);
	alSourcefv(Sources[SOUND_SHOOT], AL_VELOCITY, SourcesVel[SOUND_SHOOT]);
	alSourcei (Sources[SOUND_SHOOT], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_INTRO], AL_BUFFER,   Buffers[SOUND_INTRO]   );
	alSourcef (Sources[SOUND_INTRO], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_INTRO], AL_GAIN,     0.8f              );
	alSourcefv(Sources[SOUND_INTRO], AL_POSITION, SourcesPos[SOUND_INTRO]);
	alSourcefv(Sources[SOUND_INTRO], AL_VELOCITY, SourcesVel[SOUND_INTRO]);
	alSourcei (Sources[SOUND_INTRO], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_MISSILE_EXPL], AL_BUFFER,   Buffers[SOUND_MISSILE_EXPL]   );
	alSourcef (Sources[SOUND_MISSILE_EXPL], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_MISSILE_EXPL], AL_GAIN,     0.8f              );
	alSourcefv(Sources[SOUND_MISSILE_EXPL], AL_POSITION, SourcesPos[SOUND_MISSILE_EXPL]);
	alSourcefv(Sources[SOUND_MISSILE_EXPL], AL_VELOCITY, SourcesVel[SOUND_MISSILE_EXPL]);
	alSourcei (Sources[SOUND_MISSILE_EXPL], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_SPMUSIC], AL_BUFFER,   Buffers[SOUND_SPMUSIC]   );
	alSourcef (Sources[SOUND_SPMUSIC], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_SPMUSIC], AL_GAIN,     1.0f              );
	alSourcefv(Sources[SOUND_SPMUSIC], AL_POSITION, SourcesPos[SOUND_SPMUSIC]);
	alSourcefv(Sources[SOUND_SPMUSIC], AL_VELOCITY, SourcesVel[SOUND_SPMUSIC]);
	alSourcei (Sources[SOUND_SPMUSIC], AL_LOOPING,  AL_TRUE           );

	// Do another error check and return.

	if(alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	//OpenAL distance model
	alDistanceModel(AL_NONE);

	if(alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	return AL_TRUE;

}

ALboolean CAudioRenderer::SetListenerValues()
{
	//Set listener values
	alListenerfv(AL_POSITION,    ListenerPos);
	alListenerfv(AL_VELOCITY,    ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
	if(alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	return AL_TRUE;
}

void CAudioRenderer::RenderAudio(DWORD milliseconds, vector< CGameObject* > gObjects)
{
	//Only update audio rendering every so often.
	timeSinceAudioRender += milliseconds;
	if(timeSinceAudioRender > AUDIO_RENDER_FREQ_MS)
	{
		timeSinceAudioRender = 0;

		//Do all OpenAL calls here
		//Loop through entire gObjects vector and render sounds for each object
	}
}