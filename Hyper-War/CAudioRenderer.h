/*************************************************************/
/*	CAudioRenderer.h: Class for rendering audio using OpenAL */
/*															 */
/*	July 2009												 */
/*************************************************************/

#include <windows.h>
#include "glBase.h"
#include "GameObjects.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

using namespace std;

// Maximum data buffers we will need.
#define NUM_BUFFERS 19

// Maximum emissions we will need.
#define NUM_SOURCES 19

#define AUDIO_RENDER_FREQ_MS  67  //67 == 15 times per seconds

typedef enum TYPE_SOUND
{
	SOUND_EXPLOSION,
	SOUND_SHOOT,
	SOUND_INTRO,
	SOUND_MISSILE_EXPL,
	SOUND_SPMUSIC,
	SOUND_STORY,
	SOUND_GAMEOVER,
	SOUND_LEVEL1,
	SOUND_LEVEL2,
	SOUND_LEVEL3,
	SOUND_LEVEL4,
	SOUND_LEVEL5,
	SOUND_ZAP,
	SOUND_BOOM,
	SOUND_UFOBLAST,
	SOUND_MEGABLAST,
	SOUND_MISSILE,
	SOUND_CHARGEUP

};

class CAudioRenderer
{
public:
	CAudioRenderer();
	~CAudioRenderer();

	ALboolean LoadALData();
	ALboolean SetListenerValues();

	//Render/update OpenAL audio rendering
	void RenderAudio(DWORD milliseconds, vector< CGameObject* > gObjects);
	void PlaySound(TYPE_SOUND soundIndex, float xpos, float ypos, float pitch=1);

	void StopAll();
private:

	//Audio rendering data
	DWORD		timeSinceAudioRender;

	// Buffers hold sound data.
	ALuint Buffers[NUM_BUFFERS];

	// Sources are points of emitting sound.
	ALuint Sources[NUM_SOURCES];

	// Position of the source sounds.
	ALfloat SourcesPos[NUM_SOURCES][3];

	// Velocity of the source sounds.
	ALfloat SourcesVel[NUM_SOURCES][3];

	// Position of the listener.
	ALfloat ListenerPos[3];

	// Velocity of the listener.
	ALfloat ListenerVel[3];

	// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
	ALfloat ListenerOri[6];
};