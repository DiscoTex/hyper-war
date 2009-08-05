/*************************************************************/
/*	CAudioRenderer.h: Class for rendering audio using OpenAL */
/*															 */
/*	July 2009												 */
/*************************************************************/

#include <windows.h>
#include "glBase.h"
#include "GameObjects.h"

using namespace std;

#define AUDIO_RENDER_FREQ_MS  67  //67 == 15 times per seconds

class CAudioRenderer
{
public:
	CAudioRenderer();
	~CAudioRenderer();

	//Render/update OpenAL audio rendering
	void RenderAudio(DWORD milliseconds, vector< CGameObject* > gObjects);
private:

	//Audio rendering data
	DWORD		timeSinceAudioRender;
};