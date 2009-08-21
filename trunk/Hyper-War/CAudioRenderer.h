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

#define AUDIO_RENDER_FREQ_MS  67  //67 == 15 times per seconds

typedef enum TYPE_SOUND
{
	SOUND_EXPLOSION,
	SOUND_MISSILE,
	SOUND_LAUNCH
};

class CAudioRenderer
{
public:
	CAudioRenderer();
	~CAudioRenderer();

	//Render/update OpenAL audio rendering
	void RenderAudio(DWORD milliseconds, vector< CGameObject* > gObjects);
	void PlaySound(TYPE_SOUND soundIndex, float xpos, float ypos);
private:

	//Audio rendering data
	DWORD		timeSinceAudioRender;
};