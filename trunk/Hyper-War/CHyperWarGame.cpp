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
		planet->SetColor(0.0f, 0.1f, 0.0f);
		planet->SetTranslation(-12, 0, 0);
		planet->SetScale(10, 10, 10);
		gameObjects.push_back(planet);

		//Create a blue planet
		planet = new CPlanet();
		planet->SetColor(0.0f, 0.0f, 0.1f);
		planet->SetTranslation(12, 0, 0);
		planet->SetScale(10, 10, 10);
		gameObjects.push_back(planet);

		//Create a gravity well for the green planet
		sGravityWell *gw = new sGravityWell;
		gw->mass = 50;
		gw->translation[0] = -12;
		gw->translation[1] = 0;
		gw->translation[2] = 0;
		gravityWells.push_back(gw);

		//Gravity well for blue
		gw = new sGravityWell;
		gw->mass = 50;
		gw->translation[0] = 12;
		gw->translation[1] = 0;
		gw->translation[2] = 0;
		gravityWells.push_back(gw);

		gw = new sGravityWell;
		gw->mass = .02f;
		gw->translation[0] = -.1f;
		gw->translation[1] = -.4f;
		gw->translation[2] = 0;
		//gravityWells.push_back(gw);

		//CNuke *nuke;

		/*
		//Create some blue Nukes
		for(int i=1; i<4; i++)
		{
			nuke = new CNuke();
			nuke->SetColor(0.0, 0.0, 1.0);
			nuke->SetScale(.1f, .1f, .1f);
			nuke->SetTranslation(1.85f, i/1.3f - 2.5f, -.00f);
			nuke->SetMotionVector(-0.0001, .0000, 0);
			//nuke->SetMotionVector(.001, 0, 0);
			gameObjects.push_back(nuke);
		}

		//Create some green Nukes
		for(int i=1; i<12; i++)
		{
			nuke = new CNuke();
			nuke->SetColor(0.0, 1.0, 0.0);
			nuke->SetScale(.1f, .1f, .1f);
			nuke->SetTranslation(-1.35f, i/5.0f -1.5f, -.00f);
			nuke->SetMotionVector(0.0001, -0.00005, 0);
			//nuke->SetMotionVector(0, 0, 0);
			gameObjects.push_back(nuke);
		}
		*/

		//Create missle bases
		CMissileBase *mb = new CMissileBase;
		mb->SetColor(0, .8f, 0);
		mb->SetScale(.1f, .1f, .1f);
		mb->SetRotation(0, 0, -84);
		mb->SetTranslation(10 * cos(6*DEG2RAD) - 12.01f, 10 * sin(6*DEG2RAD), -.001f);
		gameObjects.push_back(mb);

		mb = new CMissileBase;
		mb->SetColor(0, .8f, 0);
		mb->SetScale(.1f, .1f, .1f);
		mb->SetRotation(0, 0, -96);
		mb->SetTranslation(10 * cos(-6*DEG2RAD) - 12.01f, 10 * sin(-6*DEG2RAD), -.001f);
		gameObjects.push_back(mb);

		mb = new CMissileBase;
		mb->SetColor(0, 0, .8f);
		mb->SetScale(.1f, .1f, .1f);
		mb->SetRotation(0, 0, 96);
		mb->SetTranslation(10 * cos(186*DEG2RAD) + 12.01f, 10 * sin(186*DEG2RAD), -.001f);
		gameObjects.push_back(mb);

		mb = new CMissileBase;
		mb->SetColor(0, 0, .8f);
		mb->SetScale(.1f, .1f, .1f);
		mb->SetRotation(0, 0, 84);
		mb->SetTranslation(10 * cos(174*DEG2RAD) + 12.01f, 10 * sin(174*DEG2RAD), -.001f);
		gameObjects.push_back(mb);

		CCity *city = new CCity();
		city->SetColor(0, 0, .8f);
		city->SetScale(.1f, .1f, .1f);
		city->SetRotation(0, 0, 82);
		city->SetTranslation(10 * cos(172*DEG2RAD) + 12.01f, 10 * sin(172*DEG2RAD), -.001f);
		gameObjects.push_back(city);

		city = new CCity();
		city->SetColor(0, 0, .8f);
		city->SetScale(.1f, .1f, .1f);
		city->SetRotation(0, 0, 87);
		city->SetTranslation(10 * cos(177*DEG2RAD) + 12.01f, 10 * sin(177*DEG2RAD), -.001f);
		gameObjects.push_back(city);

		city = new CCity();
		city->SetColor(0, 0, .8f);
		city->SetScale(.1f, .1f, .1f);
		city->SetRotation(0, 0, 93);
		city->SetTranslation(10 * cos(183*DEG2RAD) + 12.01f, 10 * sin(183*DEG2RAD), -.001f);
		gameObjects.push_back(city);

		city = new CCity();
		city->SetColor(0, 0, .8f);
		city->SetScale(.1f, .1f, .1f);
		city->SetRotation(0, 0, 98);
		city->SetTranslation(10 * cos(188*DEG2RAD) + 12.01f, 10 * sin(188*DEG2RAD), -.001f);
		gameObjects.push_back(city);

		city = new CCity();
		city->SetColor(0, .8f, 0);
		city->SetScale(.1f, .1f, .1f);
		city->SetRotation(0, 0, -98);
		city->SetTranslation(10 * cos(-8*DEG2RAD) - 12.01f, 10 * sin(-8*DEG2RAD), -.001f);
		gameObjects.push_back(city);

		city = new CCity();
		city->SetColor(0, .8f, 0);
		city->SetScale(.1f, .1f, .1f);
		city->SetRotation(0, 0, -93);
		city->SetTranslation(10 * cos(-3*DEG2RAD) - 12.01f, 10 * sin(-3*DEG2RAD), -.001f);
		gameObjects.push_back(city);

		city = new CCity();
		city->SetColor(0, .8f, 0);
		city->SetScale(.1f, .1f, .1f);
		city->SetRotation(0, 0, -87);
		city->SetTranslation(10 * cos(3*DEG2RAD) - 12.01f, 10 * sin(3*DEG2RAD), -.001f);
		gameObjects.push_back(city);

		city = new CCity();
		city->SetColor(0, .8f, 0);
		city->SetScale(.1f, .1f, .1f);
		city->SetRotation(0, 0, -82);
		city->SetTranslation(10 * cos(8*DEG2RAD) - 12.01f, 10 * sin(8*DEG2RAD), -.001f);
		gameObjects.push_back(city);

		CFlakCannon *cannon = new CFlakCannon();
		cannon->SetColor(0, .8f, 0);
		cannon->SetScale(.1f, .1f, .1f);
		cannon->SetRotation(0, 0, -90);
		cannon->SetTranslation(10 * cos(0*DEG2RAD) - 12.01f, 10 * sin(0*DEG2RAD), -.001f);
		gameObjects.push_back(cannon);

		cannon = new CFlakCannon();
		cannon->SetColor(0, 0, .8f);
		cannon->SetScale(.1f, .1f, .1f);
		cannon->SetRotation(0, 0, 90);
		cannon->SetTranslation(10 * cos(180*DEG2RAD) + 12.01f, 10 * sin(180*DEG2RAD), -.001f);
		gameObjects.push_back(cannon);

		lastLaunch = 0;
		keydownTime = 0;
	}

	initialized = true;

	return TRUE;												// Return TRUE (Initialization Successful)
}

void CHyperWarGame::Deinitialize (void)										// Any User DeInitialization Goes Here
{
}

void CHyperWarGame::ProcessRawInput(/*some parameters*/)
{
}

void CHyperWarGame::ProcessKeyInput(DWORD wParam, bool state)
{

}

void CHyperWarGame::Update (DWORD milliseconds)								// Perform Motion Updates Here
{
	unsigned int objIndex = -1;
	unsigned int i=0;
	int iType, objIndexType;
	CDebris *debris;
	CNuke *nuke;

	if (g_keys->keyDown [VK_ESCAPE] == TRUE)					// Is ESC Being Pressed?
	{
		TerminateApplication (g_window);						// Terminate The Program
	}

	if (g_keys->keyDown [VK_F1] == TRUE)						// Is F1 Being Pressed?
	{
		ToggleFullscreen (g_window);							// Toggle Fullscreen Mode
	}

	lastLaunch += milliseconds;
	if (g_keys->keyDown ['A'] == TRUE)						// Is F1 Being Pressed?
	{
		keydownTime += milliseconds;
	}
	else if(lastLaunch > 100 && keydownTime > 100)
	{
		nuke = new CNuke();
		nuke->SetColor(0.0, 0.0, 1.0);
		nuke->SetScale(.1f, .1f, .1f);
		nuke->SetTranslation(1.85f, .5f, -.00f);
		nuke->SetMotionVector(-0.1, .0000, 0);

		float thrust = keydownTime / 5000.0;
		if(thrust > 1)
			thrust = 1;
		else if(thrust < .1)
			thrust = .1;

		nuke->SetThrust(thrust);
		gameObjects.push_back(nuke);

		lastLaunch = 0;
		keydownTime = 0;
	}

	//For every object in the game
	for(unsigned int i=0; i<gameObjects.size(); i++)
	{
		//Process motion
		gameObjects[i]->ProcessMotion(milliseconds, g_keys);
		//Process gravity
		gameObjects[i]->ProcessGravity(milliseconds, gravityWells);
		//Check for collisions with all other objects in the game
		//returns -1 for no collision, otherwise returns the index of the collided object
		iType = gameObjects[i]->GetType();
		objIndex = 	gameObjects[i]->CheckCollision(gameObjects, milliseconds, i);
		//If a collision occurred, handle it
		if(objIndex != -1)
		{
			//Store types
			iType = gameObjects[i]->GetType();
			objIndexType = gameObjects[objIndex]->GetType();
			
			//Explode objects i and objIndex if they are the appropriate type of object
			//You must delete them last first to maintain integrity of the indexes
			if(i > objIndex)
			{
				if(gameObjects[i]->CanDestroy(objIndexType))
				{
					if(iType != TYPE_DEBRIS)
					{
						for(int k=0; k<DEBRIS_AMOUNT; k++)
						{
							debris = new CDebris();
							debris->SetMotionVector(
								float(gameObjects[i]->GetMotionVector()[0] + (rand()%100-50)/100.0), 
								float(gameObjects[i]->GetMotionVector()[1]+ (rand()%100-50)/100.0), 
								0);
							debris->SetTranslation(
								float(gameObjects[i]->GetTranslation()[0]),
								float(gameObjects[i]->GetTranslation()[1]),
								float(gameObjects[i]->GetTranslation()[2]));
							debris->SetScale(.1f, .1f, .1f);
							gameObjects.push_back(debris);
						}
					}
					gameObjects.erase(gameObjects.begin() + i);
				}
				if(gameObjects[objIndex]->CanDestroy(iType))
				{
					if(objIndexType != TYPE_DEBRIS)
					{
						for(int k=0; k<DEBRIS_AMOUNT; k++)
						{
							debris = new CDebris();
							debris->SetMotionVector(
								float(gameObjects[objIndex]->GetMotionVector()[0] + (rand()%100-50)/100.0), 
								float(gameObjects[objIndex]->GetMotionVector()[1]+ (rand()%100-50)/100.0), 
								0);
							debris->SetTranslation(
								float(gameObjects[objIndex]->GetTranslation()[0]),
								float(gameObjects[objIndex]->GetTranslation()[1]),
								float(gameObjects[objIndex]->GetTranslation()[2]));
							debris->SetScale(.1f, .1f, .1f);
							gameObjects.push_back(debris);
						}
					}
					gameObjects.erase(gameObjects.begin() + objIndex);
				}
			}
			else
			{
				if(gameObjects[objIndex]->CanDestroy(iType))
				{
					if(objIndexType != TYPE_DEBRIS)
					{
						for(int k=0; k<DEBRIS_AMOUNT; k++)
						{
							debris = new CDebris();
							debris->SetMotionVector(
								float(gameObjects[objIndex]->GetMotionVector()[0] + (rand()%100-50)/100.0),
								float(gameObjects[objIndex]->GetMotionVector()[1] + (rand()%100-50)/100.0),
								0);
							debris->SetTranslation(
								float(gameObjects[objIndex]->GetTranslation()[0]),
								float(gameObjects[objIndex]->GetTranslation()[1]),
								float(gameObjects[objIndex]->GetTranslation()[2]));
							debris->SetScale(.1f, .1f, .1f);
							gameObjects.push_back(debris);
						}
					}
					gameObjects.erase(gameObjects.begin() + objIndex);
				}
				if(gameObjects[i]->CanDestroy(objIndexType))
				{
					if(iType != TYPE_DEBRIS)
					{
						for(int k=0; k<DEBRIS_AMOUNT; k++)
						{
							debris = new CDebris();
							debris->SetMotionVector(
								float(gameObjects[i]->GetMotionVector()[0] + (rand()%100-50)/100.0), 
								float(gameObjects[i]->GetMotionVector()[1] + (rand()%100-50)/100.0), 
								0);
							debris->SetTranslation(
								float(gameObjects[i]->GetTranslation()[0]),
								float(gameObjects[i]->GetTranslation()[1]),
								float(gameObjects[i]->GetTranslation()[2]));
							debris->SetScale(.1f, .1f, .1f);
							gameObjects.push_back(debris);
						}
					}
					gameObjects.erase(gameObjects.begin() + i);
				}
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