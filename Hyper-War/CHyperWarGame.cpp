/****************************************************/
/*	CHyperWarGame.cpp: Main class for HyperWar game.	*/
/*  Aaron E. Wegner									*/
/*	July 2009									    */
/****************************************************/

#include "CHyperWarGame.h"

CHyperWarGame::CHyperWarGame()
{
}

CHyperWarGame::~CHyperWarGame()
{
	for(unsigned int i=0; i<gameObjects.size(); i++)
	{
		delete gameObjects[i];
	}
	for(unsigned int i=0; i<gravityWells.size(); i++)
	{
		delete gravityWells[i];
	}
}

BOOL CHyperWarGame::Initialize (GL_Window* window, Keys* keys)					// Any GL Init Code & User Initialiazation Goes Here
{
	g_window	= window;
	g_keys		= keys;

	glClearColor (0.0f, 0.0f, 0.0f, 0.5f);						// Black Background
	glClearDepth (1.0f);										// Depth Buffer Setup
	glDepthFunc (GL_LEQUAL);									// The Type Of Depth Testing (Less Or Equal)
	glEnable (GL_DEPTH_TEST);									// Enable Depth Testing
	glShadeModel (GL_SMOOTH);									// Select Smooth Shading
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Set Perspective Calculations To Most Accurate

	//Create game objects
	CPlanet *planet = new CPlanet();
	planet->SetColor(0.0, 0.05, 0.0);
	planet->SetTranslation(-12, 0, 0);
	planet->SetScale(10, 10, 10);
	//gameObjects.push_back(planet);

	planet = new CPlanet();
	planet->SetColor(0.0, 0.0, 0.05);
	planet->SetTranslation(12, 0, 0);
	planet->SetScale(10, 10, 10);
	//gameObjects.push_back(planet);

	//Create gravity wells corresponding to planets
	sGravityWell *gw = new sGravityWell;
	gw->mass = 10;
	gw->translation[0] = -12;
	gw->translation[1] = 0;
	gw->translation[2] = 0;
	gravityWells.push_back(gw);

	gw = new sGravityWell;
	gw->mass = 10;
	gw->translation[0] = 12;
	gw->translation[1] = 0;
	gw->translation[2] = 0;
	gravityWells.push_back(gw);

	gw = new sGravityWell;
	gw->mass = 2;
	gw->translation[0] = -.1;
	gw->translation[1] = -.4;
	gw->translation[2] = 0;
	gravityWells.push_back(gw);

	CNuke *nuke;

	//Create some Nukes going left
	for(int i=1; i<2; i++)
	{
		nuke = new CNuke();
		nuke->SetColor(0.2, 0.0, 1.0);
		nuke->SetScale(.1, .1, .1);
		nuke->SetTranslation(1, -1.82 + i/8.0, -.00);
		nuke->SetMotionVector(0, -1, 0);
		gameObjects.push_back(nuke);
	}

	//Create some Nukes going up
	for(int i=1; i<2; i++)
	{
		nuke = new CNuke();
		nuke->SetColor(1.0, 0.0, 0.0);
		nuke->SetScale(.1, .1, .1);
		nuke->SetTranslation(-1.5 + i/8.0, -1.75, -.00);
		nuke->SetMotionVector(0, 1, 0);
		//gameObjects.push_back(nuke);
	}

	return TRUE;												// Return TRUE (Initialization Successful)
}

void CHyperWarGame::Deinitialize (void)										// Any User DeInitialization Goes Here
{
}

void CHyperWarGame::Update (DWORD milliseconds)								// Perform Motion Updates Here
{
	unsigned int objIndex = -1;
	unsigned int i=0;

	if (g_keys->keyDown [VK_ESCAPE] == TRUE)					// Is ESC Being Pressed?
	{
		TerminateApplication (g_window);						// Terminate The Program
	}

	if (g_keys->keyDown [VK_F1] == TRUE)						// Is F1 Being Pressed?
	{
		ToggleFullscreen (g_window);							// Toggle Fullscreen Mode
	}

	for(unsigned int i=0; i<gameObjects.size(); i++)
	{
		//Process gravity
		gameObjects[i]->ProcessGravity(milliseconds, gravityWells);
		//Process motion
		gameObjects[i]->ProcessMotion(milliseconds);
		//Check for collisions
		objIndex = 	gameObjects[i]->CheckCollision(gameObjects, milliseconds);
		if(objIndex != -1)
		{
			//Explode objects i and objIndex
			//If they return true, erase them from the vector, too
			
			if(i > objIndex)
			{
				if(gameObjects[i]->GetType() == TYPE_NUKE)
					gameObjects.erase(gameObjects.begin() + i);
				if(gameObjects[objIndex]->GetType() == TYPE_NUKE)
					gameObjects.erase(gameObjects.begin() + objIndex);
			}
			else
			{
				if(gameObjects[objIndex]->GetType() == TYPE_NUKE)
					gameObjects.erase(gameObjects.begin() + objIndex);
				if(gameObjects[i]->GetType() == TYPE_NUKE)
					gameObjects.erase(gameObjects.begin() + i);
			}
		}
	}
}

void CHyperWarGame::Draw (void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glLoadIdentity ();											// Reset The Modelview Matrix
	glTranslatef (0.0f, 0.0f, -1.0f);							// Translate 6 Units Into The Screen
	glScalef(.25, .25, 1);

	//glRotatef (angle, 0.0f, 0.0f, 1.0f);						// Rotate On The Y-Axis By angle
	for(unsigned int i=0; i<gameObjects.size(); i++)
	{
		gameObjects[i]->Draw();
	}

	glFlush ();													// Flush The GL Rendering Pipeline
}