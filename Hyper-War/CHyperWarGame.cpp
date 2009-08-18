/****************************************************/
/*	CHyperWarGame.cpp: Main class for HyperWar game.*/
/*  Aaron E. Wegner									*/
/*	July 2009									    */
/****************************************************/


#include "CHyperWarGame.h"

CHyperWarGame::CHyperWarGame()
{
	initialized = false;

	rawMouse.init_raw_mouse(1,0,1);

	mousePos[0][0] = 0;
	mousePos[0][1] = 0;
	mousePos[1][0] = 0;
	mousePos[1][1] = 0;
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
	rawMouse.destroy_raw_mouse();
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

		/*
		CNuke *nuke;
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
		}*/

		//Create missle bases
		CMissileBase *mb = new CMissileBase;
		mb->SetColor(0, .8f, 0);
		mb->SetScale(.1f, .1f, .1f);
		mb->SetRotation(0, 0, -84);
		mb->SetTranslation(10 * cos(6*DEG2RAD) - 12.01f, 10 * sin(6*DEG2RAD), -.001f);
		mb->SetLaunchKey('A');
		mb->SetCursorPointer(mousePos[0]);
		gameObjects.push_back(mb);

		mb = new CMissileBase;
		mb->SetColor(0, .8f, 0);
		mb->SetScale(.1f, .1f, .1f);
		mb->SetRotation(0, 0, -96);
		mb->SetTranslation(10 * cos(-6*DEG2RAD) - 12.01f, 10 * sin(-6*DEG2RAD), -.001f);
		mb->SetLaunchKey('S');
		mb->SetCursorPointer(mousePos[0]);
		gameObjects.push_back(mb);

		mb = new CMissileBase;
		mb->SetColor(0, 0, .8f);
		mb->SetScale(.1f, .1f, .1f);
		mb->SetRotation(0, 0, 96);
		mb->SetTranslation(10 * cos(186*DEG2RAD) + 12.01f, 10 * sin(186*DEG2RAD), -.001f);
		mb->SetLaunchKey('J');
		mb->SetCursorPointer(mousePos[1]);
		gameObjects.push_back(mb);

		mb = new CMissileBase;
		mb->SetColor(0, 0, .8f);
		mb->SetScale(.1f, .1f, .1f);
		mb->SetRotation(0, 0, 84);
		mb->SetTranslation(10 * cos(174*DEG2RAD) + 12.01f, 10 * sin(174*DEG2RAD), -.001f);
		mb->SetLaunchKey('K');
		mb->SetCursorPointer(mousePos[1]);
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
		cannon->SetCursorPointer(mousePos[0]);
		gameObjects.push_back(cannon);

		cannon = new CFlakCannon();
		cannon->SetColor(0, 0, .8f);
		cannon->SetScale(.1f, .1f, .1f);
		cannon->SetRotation(0, 0, 90);
		cannon->SetTranslation(10 * cos(180*DEG2RAD) + 12.01f, 10 * sin(180*DEG2RAD), -.001f);
		cannon->SetCursorPointer(mousePos[1]);
		gameObjects.push_back(cannon);
	}


	initialized = true;

	return TRUE;												// Return TRUE (Initialization Successful)
}

void CHyperWarGame::Deinitialize (void)										// Any User DeInitialization Goes Here
{
}

void CHyperWarGame::ProcessRawInput(LPARAM lParam)
{
	rawMouse.process_raw_mouse((HRAWINPUT)lParam);
}

void CHyperWarGame::Update (DWORD milliseconds)								// Perform Motion Updates Here
{
	unsigned int objIndex = -1;
	unsigned int i=0;
	int iType, objIndexType;
	CDebris *debris;
	CNuke *nuke;
	float *position;
	float *nukeVector;

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
		//Process motion
		gameObjects[i]->ProcessMotion(milliseconds, g_keys);

		//Process gravity
		gameObjects[i]->ProcessGravity(milliseconds, gravityWells);

		if(gameObjects[i]->GetType() == TYPE_MISSILEBASE)
		{
			if(g_keys->keyDown[((CMissileBase*)(gameObjects[i]))->GetLaunchKey()] && ((CMissileBase*)(gameObjects[i]))->IsLoaded())
			{
				((CMissileBase*)(gameObjects[i]))->AddCharge(milliseconds);
			}
			else if(!g_keys->keyDown[((CMissileBase*)(gameObjects[i]))->GetLaunchKey()] && ((CMissileBase*)(gameObjects[i]))->LaunchReady())
			{
				//Launch missile
				float thrust = ((CMissileBase*)(gameObjects[i]))->Launch() / 5000.0f;
				if(thrust > 1)
					thrust = 1;
				else if(thrust < .1f)
					thrust = .1f;

				nuke = new CNuke();
				nuke->SetColor(gameObjects[i]->GetColor()[0], gameObjects[i]->GetColor()[1], gameObjects[i]->GetColor()[2]);
				nuke->SetScale(gameObjects[i]->GetScale()[0], gameObjects[i]->GetScale()[1], gameObjects[i]->GetScale()[2]);
				position = ((CMissileBase*)(gameObjects[i]))->GetNukeTranslation();
				nuke->SetTranslation(position[0], position[1], position[2]);
				nukeVector = ((CMissileBase*)(gameObjects[i]))->GetNukeVector();
				nuke->SetMotionVector(nukeVector[0], nukeVector[1], nukeVector[2]);
				nuke->SetThrust(thrust);
				gameObjects.push_back(nuke);
			}
		}

		//Check for collisions with all other objects in the game
		//returns -1 for no collision, otherwise returns the index of the collided object
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

	mousePos[0][0] += rawMouse.get_x_delta(1) / 500.0f;
	if(mousePos[0][0] > 0)
		mousePos[0][0] = 0;
	else if(mousePos[0][0] < -1.8f)
		mousePos[0][0] = -1.8f;

	mousePos[0][1] -= rawMouse.get_y_delta(1) / 500.0f;
	if(mousePos[0][1] > 1.6)
		mousePos[0][1] = 1.6;
	else if(mousePos[0][1] < -1.6)
		mousePos[0][1] = -1.6;

	mousePos[1][0] += rawMouse.get_x_delta(3) / 500.0f;
	if(mousePos[1][0] < 0)
		mousePos[1][0] = 0;
	else if(mousePos[1][0] > 1.8f)
		mousePos[1][0] = 1.8f;

	mousePos[1][1] -= rawMouse.get_y_delta(3) / 500.0f;
	if(mousePos[1][1] > 1.6)
		mousePos[1][1] = 1.6;
	else if(mousePos[1][1] < -1.6)
		mousePos[1][1] = -1.6;
}

void CHyperWarGame::DrawCursors()
{
	glPushMatrix();

	glTranslatef(mousePos[0][0], mousePos[0][1], 0);
	glScalef(.05, .05, 1);

	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
		glVertex3f(-1.0f, 0, 0);
		glVertex3f(-.2f, 0, 0);

		glVertex3f(.2f, 0, 0);
		glVertex3f(1, 0, 0);

		glVertex3f(0, -1, 0);
		glVertex3f(0, -.2f, 0);

		glVertex3f(0, .2f, 0);
		glVertex3f(0, 1, 0);
	glEnd();

	glPopMatrix();

	glPushMatrix();

	glTranslatef(mousePos[1][0], mousePos[1][1], 0);
	glScalef(.05, .05, 1);

	glColor3f(0, 0, 1);
	glBegin(GL_LINES);
		glVertex3f(-1.0f, 0, 0);
		glVertex3f(-.2f, 0, 0);

		glVertex3f(.2f, 0, 0);
		glVertex3f(1, 0, 0);

		glVertex3f(0, -1, 0);
		glVertex3f(0, -.2f, 0);

		glVertex3f(0, .2f, 0);
		glVertex3f(0, 1, 0);
	glEnd();

	glPopMatrix();
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

	DrawCursors();

	glFlush ();													// Flush The GL Rendering Pipeline
}