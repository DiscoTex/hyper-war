/****************************************************/
/*	CHyperWarGame.h: Main class for HyperWar game.	*/
/*  Aaron E. Wegner									*/
/*	July 2009									    */
/****************************************************/

#include <windows.h>											// Header File For Windows
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library

#include "glBase.h"												// Header File For NeHeGL
#include "CAudioRenderer.h"
#include "CEffects.h"
#include "RawMouse.h"

using namespace std;

#pragma comment( lib, "opengl32.lib" )							// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )								// Search For GLu32.lib While Linkings

#ifndef CDS_FULLSCREEN											// CDS_FULLSCREEN Is Not Defined By Some
#define CDS_FULLSCREEN 4										// Compilers. By Defining It This Way,
#endif	

#define DEBRIS_AMOUNT 6

class CHyperWarGame
{
public:
	CHyperWarGame();
	~CHyperWarGame();

	BOOL Initialize(GL_Window* window, Keys* keys);		// Any GL Init Code & User Initialiazation Goes Here
	void Deinitialize();								// Any User DeInitialization Goes Here
	void ProcessRawInput(LPARAM lParam);				// Process new raw input structure
	void Update (DWORD milliseconds);					// Perform Motion Updates Here
	void DrawCursors();
	void Draw();										// Draw the current frame

private:
	GL_Window*	g_window;
	Keys*		g_keys;
	bool		initialized;

	float		mousePos[2][2];
	
	//All of the CGameObjects in the game, e.g. rockets, planets, debris
	vector< CGameObject* >		gameObjects;
	//All of the gravity wells, which effect trajectory of CGameObjects
	vector< sGravityWell* >		gravityWells;
	//Renderer for global effects such as the starfield background
	CEffects					globalEffects;
	//Audio Renderer
	CAudioRenderer				audioRenderer;
	//Raw Input Reader
	CRawMouse					rawMouse;
};