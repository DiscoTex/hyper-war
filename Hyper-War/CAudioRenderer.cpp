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
	Buffers[SOUND_STORY] = alutCreateBufferFromFile ("story.wav");
	Buffers[SOUND_GAMEOVER] = alutCreateBufferFromFile ("gameover.wav");
	Buffers[SOUND_LEVEL1] = alutCreateBufferFromFile ("level1.wav");
	Buffers[SOUND_LEVEL2] = alutCreateBufferFromFile ("level2.wav");
	Buffers[SOUND_LEVEL3] = alutCreateBufferFromFile ("level3.wav");
	Buffers[SOUND_LEVEL4] = alutCreateBufferFromFile ("level4.wav");
	Buffers[SOUND_LEVEL5] = alutCreateBufferFromFile ("level5.wav");
	Buffers[SOUND_ZAP] = alutCreateBufferFromFile ("zap.wav");
	Buffers[SOUND_BOOM] = alutCreateBufferFromFile ("boom.wav");
	Buffers[SOUND_UFOBLAST] = alutCreateBufferFromFile ("ufoblast.wav");


	// Bind buffers into audio sources.
	alGenSources(NUM_SOURCES, Sources);

	if(alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	alSourcei (Sources[SOUND_EXPLOSION], AL_BUFFER,   Buffers[SOUND_EXPLOSION]   );
	alSourcef (Sources[SOUND_EXPLOSION], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_EXPLOSION], AL_GAIN,     0.75f              );
	alSourcefv(Sources[SOUND_EXPLOSION], AL_POSITION, SourcesPos[SOUND_EXPLOSION]);
	alSourcefv(Sources[SOUND_EXPLOSION], AL_VELOCITY, SourcesVel[SOUND_EXPLOSION]);
	alSourcei (Sources[SOUND_EXPLOSION], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_SHOOT], AL_BUFFER,   Buffers[SOUND_SHOOT]   );
	alSourcef (Sources[SOUND_SHOOT], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_SHOOT], AL_GAIN,     .33f              );
	alSourcefv(Sources[SOUND_SHOOT], AL_POSITION, SourcesPos[SOUND_SHOOT]);
	alSourcefv(Sources[SOUND_SHOOT], AL_VELOCITY, SourcesVel[SOUND_SHOOT]);
	alSourcei (Sources[SOUND_SHOOT], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_INTRO], AL_BUFFER,   Buffers[SOUND_INTRO]   );
	alSourcef (Sources[SOUND_INTRO], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_INTRO], AL_GAIN,     .15f              );
	alSourcefv(Sources[SOUND_INTRO], AL_POSITION, SourcesPos[SOUND_INTRO]);
	alSourcefv(Sources[SOUND_INTRO], AL_VELOCITY, SourcesVel[SOUND_INTRO]);
	alSourcei (Sources[SOUND_INTRO], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_MISSILE_EXPL], AL_BUFFER,   Buffers[SOUND_MISSILE_EXPL]   );
	alSourcef (Sources[SOUND_MISSILE_EXPL], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_MISSILE_EXPL], AL_GAIN,     1.0f              );
	alSourcefv(Sources[SOUND_MISSILE_EXPL], AL_POSITION, SourcesPos[SOUND_MISSILE_EXPL]);
	alSourcefv(Sources[SOUND_MISSILE_EXPL], AL_VELOCITY, SourcesVel[SOUND_MISSILE_EXPL]);
	alSourcei (Sources[SOUND_MISSILE_EXPL], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_SPMUSIC], AL_BUFFER,   Buffers[SOUND_SPMUSIC]   );
	alSourcef (Sources[SOUND_SPMUSIC], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_SPMUSIC], AL_GAIN,     .8f              );
	alSourcefv(Sources[SOUND_SPMUSIC], AL_POSITION, SourcesPos[SOUND_SPMUSIC]);
	alSourcefv(Sources[SOUND_SPMUSIC], AL_VELOCITY, SourcesVel[SOUND_SPMUSIC]);
	alSourcei (Sources[SOUND_SPMUSIC], AL_LOOPING,  AL_TRUE           );

	alSourcei (Sources[SOUND_STORY], AL_BUFFER,   Buffers[SOUND_STORY]   );
	alSourcef (Sources[SOUND_STORY], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_STORY], AL_GAIN,     1.0f              );
	alSourcefv(Sources[SOUND_STORY], AL_POSITION, SourcesPos[SOUND_STORY]);
	alSourcefv(Sources[SOUND_STORY], AL_VELOCITY, SourcesVel[SOUND_STORY]);
	alSourcei (Sources[SOUND_STORY], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_GAMEOVER], AL_BUFFER,   Buffers[SOUND_GAMEOVER]   );
	alSourcef (Sources[SOUND_GAMEOVER], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_GAMEOVER], AL_GAIN,     1.0f              );
	alSourcefv(Sources[SOUND_GAMEOVER], AL_POSITION, SourcesPos[SOUND_GAMEOVER]);
	alSourcefv(Sources[SOUND_GAMEOVER], AL_VELOCITY, SourcesVel[SOUND_GAMEOVER]);
	alSourcei (Sources[SOUND_GAMEOVER], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_LEVEL1], AL_BUFFER,   Buffers[SOUND_LEVEL1]   );
	alSourcef (Sources[SOUND_LEVEL1], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_LEVEL1], AL_GAIN,     1.0f              );
	alSourcefv(Sources[SOUND_LEVEL1], AL_POSITION, SourcesPos[SOUND_LEVEL1]);
	alSourcefv(Sources[SOUND_LEVEL1], AL_VELOCITY, SourcesVel[SOUND_LEVEL1]);
	alSourcei (Sources[SOUND_LEVEL1], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_LEVEL2], AL_BUFFER,   Buffers[SOUND_LEVEL2]   );
	alSourcef (Sources[SOUND_LEVEL2], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_LEVEL2], AL_GAIN,     1.0f              );
	alSourcefv(Sources[SOUND_LEVEL2], AL_POSITION, SourcesPos[SOUND_LEVEL2]);
	alSourcefv(Sources[SOUND_LEVEL2], AL_VELOCITY, SourcesVel[SOUND_LEVEL2]);
	alSourcei (Sources[SOUND_LEVEL2], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_LEVEL3], AL_BUFFER,   Buffers[SOUND_LEVEL3]   );
	alSourcef (Sources[SOUND_LEVEL3], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_LEVEL3], AL_GAIN,     1.0f              );
	alSourcefv(Sources[SOUND_LEVEL3], AL_POSITION, SourcesPos[SOUND_LEVEL3]);
	alSourcefv(Sources[SOUND_LEVEL3], AL_VELOCITY, SourcesVel[SOUND_LEVEL3]);
	alSourcei (Sources[SOUND_LEVEL3], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_LEVEL4], AL_BUFFER,   Buffers[SOUND_LEVEL4]   );
	alSourcef (Sources[SOUND_LEVEL4], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_LEVEL4], AL_GAIN,     1.0f              );
	alSourcefv(Sources[SOUND_LEVEL4], AL_POSITION, SourcesPos[SOUND_LEVEL4]);
	alSourcefv(Sources[SOUND_LEVEL4], AL_VELOCITY, SourcesVel[SOUND_LEVEL4]);
	alSourcei (Sources[SOUND_LEVEL4], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_LEVEL5], AL_BUFFER,   Buffers[SOUND_LEVEL5]   );
	alSourcef (Sources[SOUND_LEVEL5], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_LEVEL5], AL_GAIN,     1.0f              );
	alSourcefv(Sources[SOUND_LEVEL5], AL_POSITION, SourcesPos[SOUND_LEVEL5]);
	alSourcefv(Sources[SOUND_LEVEL5], AL_VELOCITY, SourcesVel[SOUND_LEVEL5]);
	alSourcei (Sources[SOUND_LEVEL5], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_ZAP], AL_BUFFER,   Buffers[SOUND_ZAP]   );
	alSourcef (Sources[SOUND_ZAP], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_ZAP], AL_GAIN,     .5f              );
	alSourcefv(Sources[SOUND_ZAP], AL_POSITION, SourcesPos[SOUND_ZAP]);
	alSourcefv(Sources[SOUND_ZAP], AL_VELOCITY, SourcesVel[SOUND_ZAP]);
	alSourcei (Sources[SOUND_ZAP], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_BOOM], AL_BUFFER,   Buffers[SOUND_BOOM]   );
	alSourcef (Sources[SOUND_BOOM], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_BOOM], AL_GAIN,     .65f              );
	alSourcefv(Sources[SOUND_BOOM], AL_POSITION, SourcesPos[SOUND_BOOM]);
	alSourcefv(Sources[SOUND_BOOM], AL_VELOCITY, SourcesVel[SOUND_BOOM]);
	alSourcei (Sources[SOUND_BOOM], AL_LOOPING,  AL_FALSE           );

	alSourcei (Sources[SOUND_UFOBLAST], AL_BUFFER,   Buffers[SOUND_UFOBLAST]   );
	alSourcef (Sources[SOUND_UFOBLAST], AL_PITCH,    1.0f              );
	alSourcef (Sources[SOUND_UFOBLAST], AL_GAIN,     .65f              );
	alSourcefv(Sources[SOUND_UFOBLAST], AL_POSITION, SourcesPos[SOUND_UFOBLAST]);
	alSourcefv(Sources[SOUND_UFOBLAST], AL_VELOCITY, SourcesVel[SOUND_UFOBLAST]);
	alSourcei (Sources[SOUND_UFOBLAST], AL_LOOPING,  AL_FALSE           );

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

void CAudioRenderer::StopAll()
{
	for(int i=0; i<NUM_SOURCES; i++)
		alSourceStop(Sources[i]);
}

void CAudioRenderer::PlaySound(TYPE_SOUND soundIndex, float xpos, float ypos, float pitch)
{
	ALfloat position[3];

	position[0] = xpos/10.0;
	position[1] = ypos/10.0;
	position[2] = 0;

	alSourcefv(Sources[soundIndex], AL_POSITION, position);
	alSourcef (Sources[soundIndex], AL_PITCH, pitch);
	alSourcePlay(Sources[soundIndex]);
}