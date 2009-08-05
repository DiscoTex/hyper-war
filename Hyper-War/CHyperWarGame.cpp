/****************************************************/
/*	CHyperWarGame.cpp: Main class for HyperWar game.*/
/*  Aaron E. Wegner									*/
/*	July 2009									    */
/****************************************************/


#include "CHyperWarGame.h"

CHyperWarGame::CHyperWarGame()
{
	initialized = false;
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
	if(!initialized)
	{
		//Store keys array and window info
		g_window	= window;
		g_keys		= keys;

		//Set up initial rendering environment
		glClearColor (0.0f, 0.0f, 0.0f, 0.5f);						// Black Background
		glClearDepth (1.0f);										// Depth Buffer Setup
		glDepthFunc (GL_LEQUAL);									// The Type Of Depth Testing (Less Or Equal)
		glEnable (GL_DEPTH_TEST);									// Enable Depth Testing
		glShadeModel (GL_SMOOTH);									// Select Smooth Shading
		glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Set Perspective Calculations To Most Accurate

		//** Create game objects

		//Create a green planet
		CPlanet *planet = new CPlanet();
		planet->SetColor(0.0f, 0.05f, 0.0f);
		planet->SetTranslation(-12, 0, 0);
		planet->SetScale(10, 10, 10);
		gameObjects.push_back(planet);

		//Create a blue planet
		planet = new CPlanet();
		planet->SetColor(0.0f, 0.0f, 0.05f);
		planet->SetTranslation(12, 0, 0);
		planet->SetScale(10, 10, 10);
		gameObjects.push_back(planet);

		//Create a gravity well for the green planet
		sGravityWell *gw = new sGravityWell;
		gw->mass = 10;
		gw->translation[0] = -12;
		gw->translation[1] = 0;
		gw->translation[2] = 0;
		gravityWells.push_back(gw);

		//Gravity well for blue
		gw = new sGravityWell;
		gw->mass = 10;
		gw->translation[0] = 12;
		gw->translation[1] = 0;
		gw->translation[2] = 0;
		gravityWells.push_back(gw);

		gw = new sGravityWell;
		gw->mass = 2;
		gw->translation[0] = -.1f;
		gw->translation[1] = -.4f;
		gw->translation[2] = 0;
		gravityWells.push_back(gw);

		CNuke *nuke;

		nuke = new CNuke();
		nuke->SetColor(0.0, 1.0, 0.0);
		nuke->SetScale(.4f, .4f, .4f);
		nuke->SetTranslation(1,0,0);
		nuke->SetMotionVector(0, 0, 0);
		gameObjects.push_back(nuke);
		
		//Create some blue Nukes
		for(int i=1; i<10; i++)
		{
			nuke = new CNuke();
			nuke->SetColor(0.0, 0.0, 1.0);
			nuke->SetScale(.1f, .1f, .1f);
			nuke->SetTranslation(i/8.0f, 1.5f, -.00f);
			nuke->SetMotionVector(0, 0, 0);
			gameObjects.push_back(nuke);
		}

		//Create some red Nukes
		for(int i=1; i<10; i++)
		{
			nuke = new CNuke();
			nuke->SetColor(1.0, 0.0, 0.0);
			nuke->SetScale(.1f, .1f, .1f);
			nuke->SetTranslation(i/8.0f, -1.5f, -.00f);
			nuke->SetMotionVector(0, 0, 0);
			gameObjects.push_back(nuke);
		}
	}

	initialized = true;

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

	//For every object in the game
	for(unsigned int i=0; i<gameObjects.size(); i++)
	{
		//Process gravity
		gameObjects[i]->ProcessGravity(milliseconds, gravityWells);
		//Process motion
		gameObjects[i]->ProcessMotion(milliseconds);
		//Check for collisions with all other objects in the game
		//returns -1 for no collision, otherwise returns the index of the collided object
		objIndex = 	gameObjects[i]->CheckCollision(gameObjects, milliseconds);
		//If a collision occurred, handle it
		if(objIndex != -1)
		{
			//Explode objects i and objIndex if they are the appropriate type of object
			//You must delete them last first to maintain integrity of the indexes
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

	//Update starfield position
	//Starfield position should be moved as a function of time
	//starFieldPosition = (starFieldPosition + milliseconds) % (num possible positions);

	audioRenderer.RenderAudio(milliseconds, gameObjects);
}

void CHyperWarGame::Draw (void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glLoadIdentity ();											// Reset The Modelview Matrix
	
	//Set up the global rendering coordinate system
	glTranslatef (0.0f, 0.0f, -1.0f);
	glScalef(.25, .25, 1);

	//Draw all objects in the game at their current positions
	for(unsigned int i=0; i<gameObjects.size(); i++)
	{
		gameObjects[i]->Draw();
	}

	globalEffects.DrawStarfield();

	glFlush ();													// Flush The GL Rendering Pipeline
}