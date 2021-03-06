/****************************************************/
/*	CHyperWarGame.h: Main class for HyperWar game.	*/
/*  Aaron E. Wegner									*/
/*	July 2009									    */
/****************************************************/

#include <windows.h>											// Header File For Windows
#include <iostream>
#include <gl/glew.h>
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library

#include "glBase.h"												// Header File For NeHeGL
#include "CAudioRenderer.h"
#include "CEffects.h"
#include "RawMouse.h"
#include "joystick.h"
//#include "glfont2.h"
#include "CHighScoreList.h"

using namespace std;
using namespace glfont;

#pragma comment( lib, "opengl32.lib" )							// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )								// Search For GLu32.lib While Linkings

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
	void ProcessRawInput(LPARAM lParam);				// Process new raw input structure
	int  GetNumRawMice() {return rawMouse.raw_mouse_count();}
	bool MouseButtonPresed(int mousenum, int buttonnum) {return rawMouse.mouse_button_pressed(mousenum, buttonnum);}
	void TryCollide(unsigned int collider, unsigned int collidee);
	void CollideDestroyObjects(unsigned int obj1, unsigned int obj2);
	void Update (DWORD milliseconds);					// Perform Motion Updates Here
	void DrawCursors();
	void OuterDraw();
	void Draw();										// Draw the current frame
	void DrawHUD();
	void DrawAttract();
	void DrawHighScoreEntry();

	void SetHyperLevel(int newLevel);
	int  GetHyperLevel() {return hyperLevel;}
	void NextWave();
	void RunAttractMode();
	void RunHighScoreEntry();

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
	//Joysticks
	Joystick					*joysticks[2];
	//Current state of joysticks
	DIJOYSTATE2					joystates[2];
	//High score list	
	CHighScoreList				*hsList;
	//Game speed parameters
	sGameParams					gameParams;
	//hypermode timer
	int							hyperModeTimer;
	//hyper level
	int							hyperLevel;
	//wave number for single player
	int							waveNumber;
	//total number of waves
	int							totalWaves;
	bool						greenWins;
	bool						blueWins;
	bool						attractMode;
	GLFont						*titleFont;
	GLuint						titleFontTex;
	GLFont						*storyFont;
	GLuint						storyFontTex;
	GLFont						*scoreFont;
	GLuint						scoreFontTex;
	int							pointMultiplier;
	bool						playingStory;
	bool						playingIntro;
	bool						exploded;
	bool						nukesLaunched;
	bool						nukesLaunched2;
	bool						nukesLaunched3;
	bool						nukesLaunched4;
	char						highScoreName[64];
	int							nameIndex;
	double						currentLetter;
	bool						listTime;
	bool						firstAttract;
	bool						mouseMoved{ false };

	//Shaders
	GLenum						vertGuassPrg;
	GLenum						horizGuassPrg;
	GLenum						vertGuassFragShader;
	GLenum						horizGuassFragShader;
	GLenum						vertGuassVertShader;
	GLenum						horizGuassVertShader;
	GLenum						rt_h_attrib;
	GLenum						rt_w_attrib;

	GLuint						renderTex;
	GLuint						fbo;

	GLuint						rbo;

};

/* Util */
char* ReadShaderCode(const char*);
int CheckGLError();