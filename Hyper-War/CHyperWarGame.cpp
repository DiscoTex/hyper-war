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

	SetHyperLevel(1);

	gameParams.debrisAmount = 6;
	gameParams.flakDebrisFactor = 6;
	gameParams.mouse1Index = 3;
	gameParams.mouse2Index = 2;
	gameParams.gameMode = MODE_SINGLE;
	gameParams.waveTime = 10000;

	waveNumber = 0;
	totalWaves = 0;
	blueCityCount = 4;
	greenCityCount = 4;
	blueWins = false;
	greenWins = false;

	for(int i=0; i<512; i++)
	{
		gameParams.randoms[i] = rand();
	}
	gameParams.randIndex = 0;
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

		hyperModeTimer = 0;

		//Set up initial rendering environment
		glClearColor (0.0f, 0.0f, 0.0f, 0.5f);						// Black Background
		glClearDepth (1.0f);										// Depth Buffer Setup
		glDepthFunc (GL_LEQUAL);									// The Type Of Depth Testing (Less Or Equal)
		glEnable (GL_DEPTH_TEST);									// Enable Depth Testing
		glShadeModel (GL_SMOOTH);									// Select Smooth Shading
		glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Set Perspective Calculations To Most Accurate

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Create font
		glGenTextures(1, &fontTex);
		font.Create("font.glf", fontTex);


		if(gameParams.gameMode == MODE_VS)
		{
			//** Create game objects
			//Create a green planet
			CPlanet *planet = new CPlanet(&gameParams);
			planet->SetSide(SIDE_GREEN);
			planet->SetColor(0.0f, 0.1f, 0.0f);
			planet->SetTranslation(-12, 0, 0);
			planet->SetScale(10, 10, 10);
			gameObjects.push_back(planet);

			//Create a blue planet
			planet = new CPlanet(&gameParams);
			planet->SetSide(SIDE_BLUE);
			planet->SetColor(0.0f, 0.0f, 0.1f);
			planet->SetTranslation(12, 0, 0);
			planet->SetScale(10, 10, 10);
			gameObjects.push_back(planet);

			//Create a gravity well for the green planet
			sGravityWell *gw = new sGravityWell();
			gw->mass = 100;
			gw->translation[0] = -12;
			gw->translation[1] = 0;
			gw->translation[2] = 0;
			gravityWells.push_back(gw);

			//Gravity well for blue
			gw = new sGravityWell();
			gw->mass = 100;
			gw->translation[0] = 12;
			gw->translation[1] = 0;
			gw->translation[2] = 0;
			gravityWells.push_back(gw);

			//Create missle bases
			CMissileBase *mb = new CMissileBase(&gameParams);
			mb->SetColor(0, .8f, 0);
			mb->SetSide(SIDE_GREEN);
			mb->SetScale(.1f, .1f, .1f);
			mb->SetRotation(0, 0, -84);
			mb->SetTranslation(10 * cos(6*DEG2RAD) - 12.01f, 10 * sin(6*DEG2RAD), -.001f);
			mb->SetLaunchKey('A');
			mb->SetCursorPointer(mousePos[0]);
			gameObjects.push_back(mb);

			mb = new CMissileBase(&gameParams);
			mb->SetColor(0, .8f, 0);
			mb->SetSide(SIDE_GREEN);
			mb->SetScale(.1f, .1f, .1f);
			mb->SetRotation(0, 0, -96);
			mb->SetTranslation(10 * cos(-6*DEG2RAD) - 12.01f, 10 * sin(-6*DEG2RAD), -.001f);
			mb->SetLaunchKey('D');
			mb->SetCursorPointer(mousePos[0]);
			gameObjects.push_back(mb);

			mb = new CMissileBase(&gameParams);
			mb->SetColor(0, 0, .8f);
			mb->SetSide(SIDE_BLUE);
			mb->SetScale(.1f, .1f, .1f);
			mb->SetRotation(0, 0, 96);
			mb->SetTranslation(10 * cos(186*DEG2RAD) + 12.01f, 10 * sin(186*DEG2RAD), -.001f);
			mb->SetLaunchKey('J');
			mb->SetCursorPointer(mousePos[1]);
			gameObjects.push_back(mb);

			mb = new CMissileBase(&gameParams);
			mb->SetColor(0, 0, .8f);
			mb->SetSide(SIDE_BLUE);
			mb->SetScale(.1f, .1f, .1f);
			mb->SetRotation(0, 0, 84);
			mb->SetTranslation(10 * cos(174*DEG2RAD) + 12.01f, 10 * sin(174*DEG2RAD), -.001f);
			mb->SetLaunchKey('L');
			mb->SetCursorPointer(mousePos[1]);
			gameObjects.push_back(mb);

			CCity *city = new CCity(&gameParams);
			city->SetColor(0, 0, .8f);
			city->SetSide(SIDE_BLUE);
			city->SetScale(.1f, .1f, .1f);
			city->SetRotation(0, 0, 82);
			city->SetTranslation(10 * cos(172*DEG2RAD) + 12.01f, 10 * sin(172*DEG2RAD), -.001f);
			gameObjects.push_back(city);

			city = new CCity(&gameParams);
			city->SetColor(0, 0, .8f);
			city->SetSide(SIDE_BLUE);
			city->SetScale(.1f, .1f, .1f);
			city->SetRotation(0, 0, 87);
			city->SetTranslation(10 * cos(177*DEG2RAD) + 12.01f, 10 * sin(177*DEG2RAD), -.001f);
			gameObjects.push_back(city);

			city = new CCity(&gameParams);
			city->SetColor(0, 0, .8f);
			city->SetSide(SIDE_BLUE);
			city->SetScale(.1f, .1f, .1f);
			city->SetRotation(0, 0, 93);
			city->SetTranslation(10 * cos(183*DEG2RAD) + 12.01f, 10 * sin(183*DEG2RAD), -.001f);
			gameObjects.push_back(city);

			city = new CCity(&gameParams);
			city->SetColor(0, 0, .8f);
			city->SetSide(SIDE_BLUE);
			city->SetScale(.1f, .1f, .1f);
			city->SetRotation(0, 0, 98);
			city->SetTranslation(10 * cos(188*DEG2RAD) + 12.01f, 10 * sin(188*DEG2RAD), -.001f);
			gameObjects.push_back(city);

			city = new CCity(&gameParams);
			city->SetColor(0, .8f, 0);
			city->SetSide(SIDE_GREEN);
			city->SetScale(.1f, .1f, .1f);
			city->SetRotation(0, 0, -98);
			city->SetTranslation(10 * cos(-8*DEG2RAD) - 12.01f, 10 * sin(-8*DEG2RAD), -.001f);
			gameObjects.push_back(city);

			city = new CCity(&gameParams);
			city->SetColor(0, .8f, 0);
			city->SetSide(SIDE_GREEN);
			city->SetScale(.1f, .1f, .1f);
			city->SetRotation(0, 0, -93);
			city->SetTranslation(10 * cos(-3*DEG2RAD) - 12.01f, 10 * sin(-3*DEG2RAD), -.001f);
			gameObjects.push_back(city);

			city = new CCity(&gameParams);
			city->SetColor(0, .8f, 0);
			city->SetSide(SIDE_GREEN);
			city->SetScale(.1f, .1f, .1f);
			city->SetRotation(0, 0, -87);
			city->SetTranslation(10 * cos(3*DEG2RAD) - 12.01f, 10 * sin(3*DEG2RAD), -.001f);
			gameObjects.push_back(city);

			city = new CCity(&gameParams);
			city->SetColor(0, .8f, 0);
			city->SetSide(SIDE_GREEN);
			city->SetScale(.1f, .1f, .1f);
			city->SetRotation(0, 0, -82);
			city->SetTranslation(10 * cos(8*DEG2RAD) - 12.01f, 10 * sin(8*DEG2RAD), -.001f);
			gameObjects.push_back(city);

			CFlakCannon *cannon = new CFlakCannon(&gameParams);
			cannon->SetColor(0, .8f, 0);
			cannon->SetSide(SIDE_GREEN);
			cannon->SetScale(.1f, .1f, .1f);
			cannon->SetRotation(0, 0, -90);
			cannon->SetTranslation(10 * cos(0*DEG2RAD) - 12.01f, 10 * sin(0*DEG2RAD), -.001f);
			cannon->SetCursorPointer(mousePos[0]);
			cannon->SetFireKey('S');
			gameObjects.push_back(cannon);

			cannon = new CFlakCannon(&gameParams);
			cannon->SetColor(0, 0, .8f);
			cannon->SetSide(SIDE_BLUE);
			cannon->SetScale(.1f, .1f, .1f);
			cannon->SetRotation(0, 0, 90);
			cannon->SetTranslation(10 * cos(180*DEG2RAD) + 12.01f, 10 * sin(180*DEG2RAD), -.001f);
			cannon->SetCursorPointer(mousePos[1]);
			cannon->SetFireKey('K');
			gameObjects.push_back(cannon);
		}
		else if(gameParams.gameMode == MODE_SINGLE)
		{
			audioRenderer.PlaySound(SOUND_SPMUSIC, 0, 0);

			//Create a green planet
			CPlanet *planet = new CPlanet(&gameParams);
			planet->SetColor(0.0f, 0.1f, 0.0f);
			planet->SetSide(SIDE_GREEN);
			planet->SetTranslation(-12, 0, 0);
			planet->SetScale(10, 10, 10);
			gameObjects.push_back(planet);

			//Create a gravity well for the green planet
			sGravityWell *gw = new sGravityWell();
			gw->mass = 30;
			gw->translation[0] = -12;
			gw->translation[1] = 0;
			gw->translation[2] = 0;
			gravityWells.push_back(gw);

			CMissileBase *mb = new CMissileBase(&gameParams);
			mb->SetColor(0, .8f, 0);
			mb->SetSide(SIDE_GREEN);
			mb->SetScale(.1f, .1f, .1f);
			mb->SetRotation(0, 0, -84);
			mb->SetTranslation(10 * cos(6*DEG2RAD) - 12.01f, 10 * sin(6*DEG2RAD), -.001f);
			mb->SetLaunchKey('A');
			mb->SetCursorPointer(mousePos[0]);
			gameObjects.push_back(mb);

			mb = new CMissileBase(&gameParams);
			mb->SetColor(0, .8f, 0);
			mb->SetSide(SIDE_GREEN);
			mb->SetScale(.1f, .1f, .1f);
			mb->SetRotation(0, 0, -96);
			mb->SetTranslation(10 * cos(-6*DEG2RAD) - 12.01f, 10 * sin(-6*DEG2RAD), -.001f);
			mb->SetLaunchKey('D');
			mb->SetCursorPointer(mousePos[0]);
			gameObjects.push_back(mb);

			CCity *city = new CCity(&gameParams);
			city->SetColor(0, .8f, 0);
			city->SetSide(SIDE_GREEN);
			city->SetScale(.1f, .1f, .1f);
			city->SetRotation(0, 0, -98);
			city->SetTranslation(10 * cos(-8*DEG2RAD) - 12.01f, 10 * sin(-8*DEG2RAD), -.001f);
			gameObjects.push_back(city);

			city = new CCity(&gameParams);
			city->SetColor(0, .8f, 0);
			city->SetSide(SIDE_GREEN);
			city->SetScale(.1f, .1f, .1f);
			city->SetRotation(0, 0, -93);
			city->SetTranslation(10 * cos(-3*DEG2RAD) - 12.01f, 10 * sin(-3*DEG2RAD), -.001f);
			gameObjects.push_back(city);

			city = new CCity(&gameParams);
			city->SetColor(0, .8f, 0);
			city->SetSide(SIDE_GREEN);
			city->SetScale(.1f, .1f, .1f);
			city->SetRotation(0, 0, -87);
			city->SetTranslation(10 * cos(3*DEG2RAD) - 12.01f, 10 * sin(3*DEG2RAD), -.001f);
			gameObjects.push_back(city);

			city = new CCity(&gameParams);
			city->SetColor(0, .8f, 0);
			city->SetSide(SIDE_GREEN);
			city->SetScale(.1f, .1f, .1f);
			city->SetRotation(0, 0, -82);
			city->SetTranslation(10 * cos(8*DEG2RAD) - 12.01f, 10 * sin(8*DEG2RAD), -.001f);
			gameObjects.push_back(city);

			CFlakCannon *cannon = new CFlakCannon(&gameParams);
			cannon->SetColor(0, .8f, 0);
			cannon->SetSide(SIDE_GREEN);
			cannon->SetScale(.1f, .1f, .1f);
			cannon->SetRotation(0, 0, -90);
			cannon->SetTranslation(10 * cos(0*DEG2RAD) - 12.01f, 10 * sin(0*DEG2RAD), -.001f);
			cannon->SetCursorPointer(mousePos[0]);
			cannon->SetFireKey('S');
			gameObjects.push_back(cannon);

			NextWave();
		}
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
	CProjectile *pj;
	float *position;
	float *nukeVector;
	float projVector[3];
	int TTL;

	if (g_keys->keyDown [VK_ESCAPE] == TRUE)					// Is ESC Being Pressed?
	{
		TerminateApplication (g_window);						// Terminate The Program
	}

	if (g_keys->keyDown [VK_F1] == TRUE)						// Is F1 Being Pressed?
	{
		ToggleFullscreen (g_window);							// Toggle Fullscreen Mode
	}

	if(gameParams.gameMode == MODE_VS)
	{
		mousePos[0][0] += rawMouse.get_x_delta(gameParams.mouse1Index) / 500.0f;
		if(mousePos[0][0] > 0)
			mousePos[0][0] = 0;
		else if(mousePos[0][0] < -1.8f)
			mousePos[0][0] = -1.8f;

		mousePos[0][1] -= rawMouse.get_y_delta(gameParams.mouse1Index) / 500.0f;
		if(mousePos[0][1] > 1.6f)
			mousePos[0][1] = 1.6f;
		else if(mousePos[0][1] < -1.6f)
			mousePos[0][1] = -1.6f;	

		mousePos[1][0] += rawMouse.get_x_delta(gameParams.mouse2Index) / 500.0f;
		if(mousePos[1][0] < 0)
			mousePos[1][0] = 0;
		else if(mousePos[1][0] > 1.8f)
			mousePos[1][0] = 1.8f;

		mousePos[1][1] -= rawMouse.get_y_delta(gameParams.mouse2Index) / 500.0f;
		if(mousePos[1][1] > 1.6f)
			mousePos[1][1] = 1.6f;
		else if(mousePos[1][1] < -1.6f)
			mousePos[1][1] = -1.6f;
	}
	else if(gameParams.gameMode == MODE_SINGLE)
	{
		mousePos[0][0] += rawMouse.get_x_delta(gameParams.mouse1Index) / 500.0f;
		if(mousePos[0][0] > 2.2f)
			mousePos[0][0] = 2.2f;
		else if(mousePos[0][0] < -1.8f)
			mousePos[0][0] = -1.8f;

		mousePos[0][1] -= rawMouse.get_y_delta(gameParams.mouse1Index) / 500.0f;
		if(mousePos[0][1] > 1.6f)
			mousePos[0][1] = 1.6f;
		else if(mousePos[0][1] < -1.6f)
			mousePos[0][1] = -1.6f;	
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
				float thrust = ((CMissileBase*)(gameObjects[i]))->Launch() / gameParams.chargeRateDivider;
				if(thrust > gameParams.maxThrust)
					thrust = gameParams.maxThrust;
				if(thrust < gameParams.minThrust)
					thrust = gameParams.minThrust;

				nuke = new CNuke(&gameParams);
				nuke->SetColor(gameObjects[i]->GetColor()[0], gameObjects[i]->GetColor()[1], gameObjects[i]->GetColor()[2]);
				nuke->SetSide(gameObjects[i]->GetSide());
				nuke->SetScale(gameObjects[i]->GetScale()[0], gameObjects[i]->GetScale()[1], gameObjects[i]->GetScale()[2]);
				position = ((CMissileBase*)(gameObjects[i]))->GetNukeTranslation();
				nuke->SetTranslation(position[0], position[1], position[2]);
				nukeVector = ((CMissileBase*)(gameObjects[i]))->GetNukeVector();
				nuke->SetMotionVector(nukeVector[0], nukeVector[1], nukeVector[2]);
				nuke->SetThrust(thrust);
				gameObjects.push_back(nuke);
			}
		}

		else if(gameObjects[i]->GetType() == TYPE_FLAKCANNON)
		{
			if(g_keys->keyDown[((CFlakCannon*)(gameObjects[i]))->GetFireKey()] && ((CFlakCannon*)(gameObjects[i]))->IsLoaded())
			{
				//Launch projectile
				//Set flak cannon to "just launched"
				((CFlakCannon*)(gameObjects[i]))->Fire();
				audioRenderer.PlaySound(SOUND_SHOOT, 0, 0);				

				//Spawn a projectile
				pj = new CProjectile(&gameParams);
				pj->SetColor(gameObjects[i]->GetColor()[0], gameObjects[i]->GetColor()[1], gameObjects[i]->GetColor()[2]);
				pj->SetScale(gameObjects[i]->GetScale()[0], gameObjects[i]->GetScale()[1], gameObjects[i]->GetScale()[2]);
				position = ((CFlakCannon*)(gameObjects[i]))->GetProjTranslation();
				pj->SetTranslation(position[0], position[1], -.0010f);
				((CFlakCannon*)(gameObjects[i]))->GetProjVector(&TTL, projVector);
				pj->SetMotionVector(projVector[0], projVector[1], projVector[2]);
				position = gameObjects[i]->GetTranslation();
				pj->SetOrigin(position[0], position[1],position[2]);
				pj->SetTTL(TTL);
				gameObjects.push_back(pj);
			}	
			else if(!g_keys->keyDown[((CFlakCannon*)(gameObjects[i]))->GetFireKey()] && !((CFlakCannon*)(gameObjects[i]))->IsLoaded())
			{
				//Allow reload			
				((CFlakCannon*)(gameObjects[i]))->AddTimeSinceFired(milliseconds);
			}
		}
		
		else if(gameObjects[i]->GetType() == TYPE_PROJECTILE)
		{
			((CProjectile*)(gameObjects[i]))->SubtractTTL(milliseconds);

			if(((CProjectile*)(gameObjects[i]))->GetTTL() < 0)
			{
				for(int k=0; k<gameParams.debrisAmount*gameParams.flakDebrisFactor; k++)
				{
					int heading = gameParams.randoms[gameParams.randIndex++%512]%360;
					float velocity = (float)((gameParams.randoms[gameParams.randIndex++%512]%100-50) / 250.0f);

					debris = new CDebris(&gameParams);
					debris->SetMotionVector(
						float(cos(heading*DEG2RAD) * velocity), 
						float(sin(heading*DEG2RAD) * velocity), 
						0);
					debris->SetTranslation(
						float(gameObjects[i]->GetTranslation()[0]),
						float(gameObjects[i]->GetTranslation()[1]),
						float(gameObjects[i]->GetTranslation()[2]));
					debris->SetScale(.01f, .01f, .01f);
					gameObjects.push_back(debris);
				}
				// Time to explode
				gameObjects.erase(gameObjects.begin() + i);
				continue;
			}		
		}

		else if(gameObjects[i]->GetType() == TYPE_DEBRIS)
		{
			if(((CDebris*)(gameObjects[i]))->GetTTL() < 0)
			{
				gameObjects.erase(gameObjects.begin() + i);
				continue;
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
					if(iType == TYPE_MOSHIP)
					{
						hyperModeTimer = 0;
						SetHyperLevel(GetHyperLevel() - 1);
						waveNumber--;
						for(int k=0; k<gameParams.debrisAmount*16; k++)
						{
							float  debrisAngle;
							float  debrisSize;							

							debrisAngle = gameParams.randoms[gameParams.randIndex++%512] % 180 + gameObjects[i]->GetRotation()[2];
							debrisSize = ((gameParams.randoms[gameParams.randIndex++%512]%100) / 100.0f) * .2f;

							debris = new CDebris(&gameParams);
							debris->SetMotionVector(
								float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								0);
							debris->SetTranslation(
								float(gameObjects[i]->GetTranslation()[0]),
								float(gameObjects[i]->GetTranslation()[1]),
								float(gameObjects[i]->GetTranslation()[2]));
							debris->SetScale(debrisSize, debrisSize, debrisSize);
							debris->SetTTL(15000 - gameParams.randoms[gameParams.randIndex++%512]%5000);
							gameObjects.push_back(debris);
						}
						audioRenderer.PlaySound(SOUND_EXPLOSION, 0, 0);
					}
					else if(iType == TYPE_CITY)
					{
						hyperModeTimer = 0;
						switch(gameObjects[i]->GetSide())
						{
						case SIDE_BLUE:
							blueCityCount--;
							break;
						case SIDE_GREEN:
							greenCityCount--;
							break;
						}
						for(int k=0; k<gameParams.debrisAmount*16; k++)
						{
							float  debrisAngle;
							float  debrisSize;							

							debrisAngle = gameParams.randoms[gameParams.randIndex++%512] % 180 + gameObjects[i]->GetRotation()[2];
							debrisSize = ((gameParams.randoms[gameParams.randIndex++%512]%100) / 100.0f) * .2f;

							debris = new CDebris(&gameParams);
							debris->SetMotionVector(
								float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								0);
							debris->SetTranslation(
								float(gameObjects[i]->GetTranslation()[0]),
								float(gameObjects[i]->GetTranslation()[1]),
								float(gameObjects[i]->GetTranslation()[2]));
							debris->SetScale(debrisSize, debrisSize, debrisSize);
							debris->SetTTL(15000 - gameParams.randoms[gameParams.randIndex++%512]%5000);
							gameObjects.push_back(debris);
						}
						audioRenderer.PlaySound(SOUND_EXPLOSION, 0, 0);
					}
					else if(iType != TYPE_DEBRIS)
					{
						audioRenderer.PlaySound(SOUND_MISSILE_EXPL, 0, 0);	
						for(int k=0; k<gameParams.debrisAmount; k++)
						{
							debris = new CDebris(&gameParams);
							debris->SetMotionVector(
								float(gameObjects[i]->GetMotionVector()[0] + (gameParams.randoms[gameParams.randIndex++%512]%100-50)/100.0), 
								float(gameObjects[i]->GetMotionVector()[1]+ (gameParams.randoms[gameParams.randIndex++%512]%100-50)/100.0), 
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
					if(objIndexType == TYPE_MOSHIP)
					{
						SetHyperLevel(GetHyperLevel() - 1);
						waveNumber--;
						hyperModeTimer = 0;
						for(int k=0; k<gameParams.debrisAmount*16; k++)
						{
							float  debrisAngle;
							float  debrisSize;							

							debrisAngle = gameParams.randoms[gameParams.randIndex++%512] % 180 + gameObjects[i]->GetRotation()[2];
							debrisSize = ((gameParams.randoms[gameParams.randIndex++%512]%100) / 100.0f) * .2f;

							debris = new CDebris(&gameParams);
							debris->SetMotionVector(
								float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								0);
							debris->SetTranslation(
								float(gameObjects[i]->GetTranslation()[0]),
								float(gameObjects[i]->GetTranslation()[1]),
								float(gameObjects[i]->GetTranslation()[2]));
							debris->SetScale(debrisSize, debrisSize, debrisSize);
							debris->SetTTL(15000 - gameParams.randoms[gameParams.randIndex++%512]%5000);
							gameObjects.push_back(debris);
						}
						audioRenderer.PlaySound(SOUND_EXPLOSION, 0, 0);
					}
					else if(objIndexType == TYPE_CITY)
					{
						switch(gameObjects[objIndex]->GetSide())
						{
						case SIDE_BLUE:
							blueCityCount--;
							break;
						case SIDE_GREEN:
							greenCityCount--;
							break;
						}
						hyperModeTimer = 0;
						for(int k=0; k<gameParams.debrisAmount*16; k++)
						{
							float  debrisAngle;
							float  debrisSize;							

							debrisAngle = gameParams.randoms[gameParams.randIndex++%512] % 180 + gameObjects[objIndex]->GetRotation()[2];
							debrisSize = ((gameParams.randoms[gameParams.randIndex++%512]%100) / 100.0f) * .2f;

							debris = new CDebris(&gameParams);
							debris->SetMotionVector(
								float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								0);
							debris->SetTranslation(
								float(gameObjects[objIndex]->GetTranslation()[0]),
								float(gameObjects[objIndex]->GetTranslation()[1]),
								float(gameObjects[objIndex]->GetTranslation()[2]));
							debris->SetScale(debrisSize, debrisSize, debrisSize);
							debris->SetTTL(15000 - gameParams.randoms[gameParams.randIndex++%512]%5000);
							gameObjects.push_back(debris);
						}
						audioRenderer.PlaySound(SOUND_EXPLOSION, 0, 0);
					}
					else if(objIndexType != TYPE_DEBRIS)
					{
						audioRenderer.PlaySound(SOUND_MISSILE_EXPL, 0, 0);	
						for(int k=0; k<gameParams.debrisAmount; k++)
						{
							debris = new CDebris(&gameParams);
							debris->SetMotionVector(
								float(gameObjects[objIndex]->GetMotionVector()[0] + (gameParams.randoms[gameParams.randIndex++%512]%100-50)/100.0), 
								float(gameObjects[objIndex]->GetMotionVector()[1]+ (gameParams.randoms[gameParams.randIndex++%512]%100-50)/100.0), 
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
					if(objIndexType == TYPE_MOSHIP)
					{
						SetHyperLevel(GetHyperLevel() - 1);
						waveNumber--;
						hyperModeTimer = 0;
						for(int k=0; k<gameParams.debrisAmount*16; k++)
						{
							float  debrisAngle;
							float  debrisSize;							

							debrisAngle = gameParams.randoms[gameParams.randIndex++%512] % 180 + gameObjects[i]->GetRotation()[2];
							debrisSize = ((gameParams.randoms[gameParams.randIndex++%512]%100) / 100.0f) * .2f;

							debris = new CDebris(&gameParams);
							debris->SetMotionVector(
								float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								0);
							debris->SetTranslation(
								float(gameObjects[i]->GetTranslation()[0]),
								float(gameObjects[i]->GetTranslation()[1]),
								float(gameObjects[i]->GetTranslation()[2]));
							debris->SetScale(debrisSize, debrisSize, debrisSize);
							debris->SetTTL(15000 - gameParams.randoms[gameParams.randIndex++%512]%5000);
							gameObjects.push_back(debris);
						}
						audioRenderer.PlaySound(SOUND_EXPLOSION, 0, 0);
					}
					else if(objIndexType == TYPE_CITY)
					{
						switch(gameObjects[objIndex]->GetSide())
						{
						case SIDE_BLUE:
							blueCityCount--;
							break;
						case SIDE_GREEN:
							greenCityCount--;
							break;
						}
						hyperModeTimer = 0;
						for(int k=0; k<gameParams.debrisAmount*16; k++)
						{
							float  debrisAngle;
							float  debrisSize;							

							debrisAngle = gameParams.randoms[gameParams.randIndex++%512] % 180 + gameObjects[objIndex]->GetRotation()[2];
							debrisSize = ((gameParams.randoms[gameParams.randIndex++%512]%100) / 100.0f) * .2f;

							debris = new CDebris(&gameParams);
							debris->SetMotionVector(
								float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								0);
							debris->SetTranslation(
								float(gameObjects[objIndex]->GetTranslation()[0]),
								float(gameObjects[objIndex]->GetTranslation()[1]),
								float(gameObjects[objIndex]->GetTranslation()[2]));
							debris->SetScale(debrisSize, debrisSize, debrisSize);
							debris->SetTTL(15000 - gameParams.randoms[gameParams.randIndex++%512]%5000);
							gameObjects.push_back(debris);
						}
						audioRenderer.PlaySound(SOUND_EXPLOSION, 0, 0);
					}
					else if(objIndexType != TYPE_DEBRIS)
					{
						audioRenderer.PlaySound(SOUND_MISSILE_EXPL, 0, 0);	
						for(int k=0; k<gameParams.debrisAmount; k++)
						{
							debris = new CDebris(&gameParams);
							debris->SetMotionVector(
								float(gameObjects[objIndex]->GetMotionVector()[0] + (gameParams.randoms[gameParams.randIndex++%512]%100-50)/100.0),
								float(gameObjects[objIndex]->GetMotionVector()[1] + (gameParams.randoms[gameParams.randIndex++%512]%100-50)/100.0),
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
					if(iType == TYPE_MOSHIP)
					{
						SetHyperLevel(GetHyperLevel() - 1);
						waveNumber--;
						hyperModeTimer = 0;
						for(int k=0; k<gameParams.debrisAmount*16; k++)
						{
							float  debrisAngle;
							float  debrisSize;							

							debrisAngle = gameParams.randoms[gameParams.randIndex++%512] % 180 + gameObjects[i]->GetRotation()[2];
							debrisSize = ((gameParams.randoms[gameParams.randIndex++%512]%100) / 100.0f) * .2f;

							debris = new CDebris(&gameParams);
							debris->SetMotionVector(
								float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								0);
							debris->SetTranslation(
								float(gameObjects[i]->GetTranslation()[0]),
								float(gameObjects[i]->GetTranslation()[1]),
								float(gameObjects[i]->GetTranslation()[2]));
							debris->SetScale(debrisSize, debrisSize, debrisSize);
							debris->SetTTL(15000 - gameParams.randoms[gameParams.randIndex++%512]%5000);
							gameObjects.push_back(debris);
						}
						audioRenderer.PlaySound(SOUND_EXPLOSION, 0, 0);
					}
					else if(iType == TYPE_CITY)
					{
						switch(gameObjects[i]->GetSide())
						{
						case SIDE_BLUE:
							blueCityCount--;
							break;
						case SIDE_GREEN:
							greenCityCount--;
							break;
						}
						hyperModeTimer = 0;
						for(int k=0; k<gameParams.debrisAmount*16; k++)
						{
							float  debrisAngle;
							float  debrisSize;							

							debrisAngle = gameParams.randoms[gameParams.randIndex++%512] % 180 + gameObjects[i]->GetRotation()[2];
							debrisSize = ((gameParams.randoms[gameParams.randIndex++%512]%100) / 100.0f) * .2f;

							debris = new CDebris(&gameParams);
							debris->SetMotionVector(
								float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
								0);
							debris->SetTranslation(
								float(gameObjects[i]->GetTranslation()[0]),
								float(gameObjects[i]->GetTranslation()[1]),
								float(gameObjects[i]->GetTranslation()[2]));
							debris->SetScale(debrisSize, debrisSize, debrisSize);
							debris->SetTTL(15000 - gameParams.randoms[gameParams.randIndex++%512]%5000);
							gameObjects.push_back(debris);
						}
						audioRenderer.PlaySound(SOUND_EXPLOSION, 0, 0);
					}
					else if(iType != TYPE_DEBRIS)
					{
						audioRenderer.PlaySound(SOUND_MISSILE_EXPL, 0, 0);	
						for(int k=0; k<gameParams.debrisAmount; k++)
						{
							debris = new CDebris(&gameParams);
							debris->SetMotionVector(
								float(gameObjects[i]->GetMotionVector()[0] + (gameParams.randoms[gameParams.randIndex++%512]%100-50)/100.0), 
								float(gameObjects[i]->GetMotionVector()[1] + (gameParams.randoms[gameParams.randIndex++%512]%100-50)/100.0), 
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

	hyperModeTimer += milliseconds;
	if(hyperModeTimer > gameParams.hyperModeDelay && gameParams.gameMode != MODE_SINGLE)
	{
		//increase hyper level
		SetHyperLevel(GetHyperLevel() + 1);
	}
	else if(gameParams.gameMode == MODE_SINGLE && hyperModeTimer > gameParams.waveTime)
	{
		NextWave();
		hyperModeTimer = 0;
	}

	//Update starfield position
	//Starfield position should be moved as a function of time
	globalEffects.SetStarFieldPosition(globalEffects.GetStarFieldPosition() + milliseconds);
	
	audioRenderer.RenderAudio(milliseconds, gameObjects);

	//check for game over
	if(blueCityCount < 1)
	{
		//Green wins
		gameObjects.clear();
		greenWins = true;

		//play game over sounds

	}
	else if(greenCityCount < 1)
	{
		//Blue wins
		gameObjects.clear();
		blueWins = true;

		//play game over sounds
	}
}

void CHyperWarGame::DrawCursors()
{
	glPushMatrix();

	glTranslatef(mousePos[0][0], mousePos[0][1], 0);
	glScalef(.05f, .05f, 1);

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

	if(gameParams.gameMode != MODE_SINGLE)
	{
		glPushMatrix();

		glTranslatef(mousePos[1][0], mousePos[1][1], 0);
		glScalef(.05f, .05f, 1);

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
}

void CHyperWarGame::Draw (void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glLoadIdentity ();											// Reset The Modelview Matrix
	
	//Set up the global rendering coordinate system
	glTranslatef (0.0f, 0.0f, -1.0f);
	glScalef(.25f, .25f, 1);

	if(!(blueWins || greenWins) )
	{
		//Draw all objects in the game at their current positions
		for(unsigned int i=0; i<gameObjects.size(); i++)
		{
			gameObjects[i]->Draw();
		}		

		DrawCursors();
	}
	else
	{
		//Draw GAME OVER screen
		//font.Begin();

		glPushMatrix();

		glScalef(.01, .01, .01);
		glEnable(GL_TEXTURE_2D);
		int width = font.GetCharWidthA('G') + font.GetCharWidthA('a') + font.GetCharWidthA('m') + font.GetCharWidthA('e') +
			font.GetCharWidthA('g') + font.GetCharWidthA('O') + font.GetCharWidthA('v') + font.GetCharWidthA('e') + font.GetCharWidthA('r');
		int height = font.GetCharHeight('G');

		glTranslatef(-width/2.0f, height/2.0f, 0);
		font.DrawString("Game", 0, 0);		
		width = font.GetCharWidthA('G') + font.GetCharWidthA('a') + font.GetCharWidthA('m') + font.GetCharWidthA('e') + font.GetCharWidthA('g');
		glTranslatef(width, 0, 0);
		font.DrawString("Over", 0, 0);		
		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
		
	}

	globalEffects.DrawStarfield();

	glFlush ();													// Flush The GL Rendering Pipeline
}

void CHyperWarGame::SetHyperLevel(int newLevel)
{
	hyperLevel = newLevel;
	hyperModeTimer = 0;

	if(hyperLevel < 1)
		hyperLevel = 1;

	switch(hyperLevel)
	{
	case 1:
		//Play sound indicating new hyper level
		audioRenderer.PlaySound(SOUND_INTRO, 0, 0);
		gameParams.chargeRateDivider = 5000.0f;			//min 500
		//gameParams.maxThrust = 3.0f;					//max 500
		//gameParams.minThrust = 0.3f;
		gameParams.maxThrust = 1.0f;					//max 500
		gameParams.minThrust = 1.0f;
		gameParams.maxGravityForce = 10.0f;
		gameParams.nukeGravityImmunityTime = 1000;		//min 200
		gameParams.nukeSpeedDivider = 16000.0f;			//min 1000
		gameParams.nukeReloadTime = 4000;				//min 150
		gameParams.flakReloadTime = 10;	
		gameParams.hyperModeDelay = 30000;
		break;
	case 2:
		//Play sound indicating new hyper level
		audioRenderer.PlaySound(SOUND_INTRO, 0, 0);
		gameParams.chargeRateDivider = 3000.0f;			//min 500
		//gameParams.maxThrust = 3.0f;					//max 500
		//gameParams.minThrust = 0.3f;
		gameParams.maxThrust = 1.0f;					//max 500
		gameParams.minThrust = 1.0f;
		gameParams.maxGravityForce = 10.0f;
		gameParams.nukeGravityImmunityTime = 1000;		//min 200
		gameParams.nukeSpeedDivider = 16000.0f;			//min 1000
		gameParams.nukeReloadTime = 2000;				//min 150
		gameParams.flakReloadTime = 10;	
		gameParams.hyperModeDelay = 25000;
		break;
	case 3:
		//Play sound indicating new hyper level
		audioRenderer.PlaySound(SOUND_INTRO, 0, 0);
		gameParams.chargeRateDivider = 2000.0f;			//min 500
		//gameParams.maxThrust = 3.0f;					//max 500
		//gameParams.minThrust = 0.3f;
		gameParams.maxThrust = 1.0f;					//max 500
		gameParams.minThrust = 1.0f;
		gameParams.maxGravityForce = 10.0f;
		gameParams.nukeGravityImmunityTime = 1000;		//min 200
		gameParams.nukeSpeedDivider = 8000.0f;			//min 1000
		gameParams.nukeReloadTime = 1000;				//min 150
		gameParams.flakReloadTime = 10;	
		gameParams.hyperModeDelay = 20000;
		break;
	case 4:
		//Play sound indicating new hyper level
		audioRenderer.PlaySound(SOUND_INTRO, 0, 0);
		gameParams.chargeRateDivider = 1000.0f;			//min 500
		//gameParams.maxThrust = 3.0f;					//max 500
		//gameParams.minThrust = 0.3f;
		gameParams.maxThrust = 1.0f;					//max 500
		gameParams.minThrust = 1.0f;
		gameParams.maxGravityForce = 10.0f;
		gameParams.nukeGravityImmunityTime = 1000;		//min 200
		gameParams.nukeSpeedDivider = 4000.0f;			//min 1000
		gameParams.nukeReloadTime = 500;				//min 150
		gameParams.flakReloadTime = 10;	
		gameParams.hyperModeDelay = 15000;
		break;
	case 5:
		//Play sound indicating new hyper level
		audioRenderer.PlaySound(SOUND_INTRO, 0, 0);
		gameParams.chargeRateDivider = 500.0f;			//min 500
		//gameParams.maxThrust = 3.0f;					//max 500
		//gameParams.minThrust = 0.3f;
		gameParams.maxThrust = 1.0f;					//max 500
		gameParams.minThrust = 1.0f;
		gameParams.maxGravityForce = 10.0f;
		gameParams.nukeGravityImmunityTime = 1000;		//min 200
		gameParams.nukeSpeedDivider = 2000.0f;			//min 1000
		gameParams.nukeReloadTime = 200;				//min 150
		gameParams.flakReloadTime = 10;	
		gameParams.hyperModeDelay = 60000;
		break;
	default:
		hyperLevel = 5;
		break;
	}
}

void CHyperWarGame::NextWave()
{
	CNuke *nuke;
	CMothership *moship;

	waveNumber++;
	totalWaves++;

	for(int i=0; i<waveNumber && i < 7; i++)
	{
		//Use layers for x location between 2.7 and 7.7

		nuke = new CNuke(&gameParams);
		nuke->SetColor(0, 0, .8f);
		nuke->SetScale(.1f, .1f, .1f);
		nuke->SetTranslation(2.75f + (i-1)%10 * .5f, (gameParams.randoms[gameParams.randIndex++%512]/(float)RAND_MAX) * -1.7f, 0);
		nuke->SetMotionVector(gameParams.randoms[gameParams.randIndex++%512]/(float)RAND_MAX * -waveNumber/8.0f, (gameParams.randoms[gameParams.randIndex++%512]/(float)RAND_MAX - .5f), 0);
		nuke->SetThrust(.025f * waveNumber);
		nuke->SetSide(SIDE_BLUE);
		gameObjects.push_back(nuke);

		nuke = new CNuke(&gameParams);
		nuke->SetColor(0, 0, .8f);
		nuke->SetScale(.1f, .1f, .1f);
		nuke->SetTranslation(2.75f + (i-1)%10 * .5f, (gameParams.randoms[gameParams.randIndex++%512]/(float)RAND_MAX) * 1.7f, 0);
		nuke->SetMotionVector(gameParams.randoms[gameParams.randIndex++%512]/(float)RAND_MAX * -waveNumber/8.0f, (gameParams.randoms[gameParams.randIndex++%512]/(float)RAND_MAX - .5f), 0);
		nuke->SetThrust(.025f * waveNumber);
		nuke->SetSide(SIDE_BLUE);
		gameObjects.push_back(nuke);
	}

	//Motherships	
	if(totalWaves % 4 == 3)
	{
		//create a mothership
		moship = new CMothership(&gameParams);
		moship->SetColor(0, 0, .8f);
		moship->SetSide(SIDE_BLUE);
		moship->SetScale(.07f, .07f, .07f);
		moship->SetRotation(0, 0, -90);
		moship->SetMotionVector(0, .2, 0);
		moship->SetTranslation(1, -1, 0);
		gameObjects.push_back(moship);
	}

	//Random gravity wells
	if(totalWaves % 6 == 0 && totalWaves > 18)
	{
		/*
		//Add a random gravity well
		sGravityWell *gw = new sGravityWell();
		gw->mass = gameParams.randoms[gameParams.randIndex++%512]/(float)RAND_MAX * 2;
		gw->translation[0] = (gameParams.randoms[gameParams.randIndex++%512]/(float)RAND_MAX - .5f) * 5.5f;
		gw->translation[1] = (gameParams.randoms[gameParams.randIndex++%512]/(float)RAND_MAX - .5f) * 3.5f;
		gw->translation[2] = 0;
		gravityWells.push_back(gw);
		*/
	}

	if(totalWaves % 4 == 0)
	{
		SetHyperLevel(GetHyperLevel() + 1);
	}
}