/***************************************************************/
/*	CAudioRenderer.cpp: Class for rendering audio using OpenAL */
/*															   */
/*	July 2009												   */
/***************************************************************/

#include "CAudioRenderer.h"

CAudioRenderer::CAudioRenderer()
{
	timeSinceAudioRender = 999999;
}

CAudioRenderer::~CAudioRenderer()
{
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