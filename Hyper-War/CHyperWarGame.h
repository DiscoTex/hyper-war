/****************************************************/
/*	CHyperWarGame.h: Main class for HyperWar game.	*/
/*  Aaron E. Wegner									*/
/*	July 2009									    */
/****************************************************/

#include <windows.h>											// Header File For Windows
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library
#include <gl\glaux.h>											// Header File For The GLaux Library
#include "glBase.h"												// Header File For NeHeGL
#include "GameObjects.h"

using namespace std;

#pragma comment( lib, "opengl32.lib" )							// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )								// Search For GLu32.lib While Linking
#pragma comment( lib, "glaux.lib" )								// Search For GLaux.lib While Linking

#ifndef CDS_FULLSCREEN											// CDS_FULLSCREEN Is Not Defined By Some
#define CDS_FULLSCREEN 4										// Compilers. By Defining It This Way,
#endif	

class CHyperWarGame
{
public:
	CHyperWarGame();
	~CHyperWarGame();

	BOOL Initialize(GL_Window* window, Keys* keys);		// Any GL Init Code & User Initialiazation Goes Here
	void Deinitialize();								// Any User DeInitialization Goes Here
	void Update (DWORD milliseconds);					// Perform Motion Updates Here
	void Draw();										// Draw the current frame

private:
	GL_Window*	g_window;
	Keys*		g_keys;

	vector< CGameObject* > gameObjects;
	vector< sGravityWell* > gravityWells;
};