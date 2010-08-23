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

	gameParams.gameMode = MODE_ATTRACT;

	for(int i=0; i<1024; i++)
	{
		gameParams.randoms[i] = rand();
	}
	gameParams.randIndex = 0;

	memset(highScoreName, 0, 64);
	currentLetter = 65;
	nameIndex = 0;
	listTime = false;

	gameParams.blueRespawnCountdown = -1;
	gameParams.greenRespawnCountdown = -1;

	joysticks[0] = new Joystick(0);
	joysticks[0]->open();
	joysticks[1] = new Joystick(1);
	joysticks[1]->open();
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

	glDeleteTextures(1, &scoreFontTex);
	glDeleteTextures(1, &titleFontTex);
	glDeleteTextures(1, &storyFontTex);

	hsList->SaveScores();
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

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		//Create fonts
		glGenTextures(1, &scoreFontTex);
		glGenTextures(1, &titleFontTex);
		glGenTextures(1, &storyFontTex);
		
		scoreFont = new GLFont();
		titleFont = new GLFont();
		storyFont = new GLFont();
		
		titleFont->Create("titleFont.glf", titleFontTex);
		storyFont->Create("storyFont.glf", storyFontTex);
		scoreFont->Create("scoreFont.glf", scoreFontTex);

		hsList = new CHighScoreList(titleFont);
		hsList->ReadScores();

		gameObjects.reserve(1000);
		initialized = true;
	}

	gameParams.chargeRateDivider = 5000.0f;			//min 500
	gameParams.maxThrust = 1.0f;					//max 500
	gameParams.minThrust = 1.0f;
	gameParams.maxGravityForce = 10.0f;
	gameParams.nukeGravityImmunityTime = 1000;		//min 200
	gameParams.nukeSpeedDivider = 16000.0f;			//min 1000
	gameParams.nukeReloadTime = 4000;				//min 150
	gameParams.flakVelocityDivider = 150.0;
	gameParams.flakReloadTime = 10;	
	gameParams.hyperModeDelay = 30000;
	gameParams.greenSuperFires = 0;
	gameParams.blueSuperFires = 0;
	gameParams.blueSuperAmmo = 0;
	gameParams.greenSuperAmmo = 0;
	gameParams.blueShip = false;
	gameParams.greenShip = false;

	gameObjects.clear();
	gravityWells.clear();
	hyperModeTimer = 0;
	waveNumber = 0;
	totalWaves = 0;
	gameParams.numBlueCities = 4;
	gameParams.numGreenCities = 4;
	blueWins = false;
	greenWins = false;
	playingStory = false;
	playingIntro = false;
	exploded = false;
	nukesLaunched = false;
	nukesLaunched2 = false;
	nukesLaunched3 = false;
	nukesLaunched4 = false;
	gameParams.waveTime = 5000;
	gameParams.greenPoints = 0;
	gameParams.bluePoints = 0;
	gameParams.debrisAmount = 5;
	gameParams.flakDebrisFactor = 6;
#ifdef PC_CONTROLS
	gameParams.mouse1Index = 0;
	gameParams.mouse2Index = 1;
#else
	gameParams.mouse1Index = 2;
	gameParams.mouse2Index = 1;
#endif
	pointMultiplier = 1;
	audioRenderer.StopAll();

	if(gameParams.gameMode == MODE_VS)
	{
		audioRenderer.PlaySound(SOUND_SPMUSIC, 0, 0);

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
		mb->SetLaunchKey(6);
		mb->SetCursorPointer(mousePos[0]);
		gameObjects.push_back(mb);

		mb = new CMissileBase(&gameParams);
		mb->SetColor(0, .8f, 0);
		mb->SetSide(SIDE_GREEN);
		mb->SetScale(.1f, .1f, .1f);
		mb->SetRotation(0, 0, -96);
		mb->SetTranslation(10 * cos(-6*DEG2RAD) - 12.01f, 10 * sin(-6*DEG2RAD), -.001f);
		mb->SetLaunchKey(7);
		mb->SetCursorPointer(mousePos[0]);
		gameObjects.push_back(mb);

		mb = new CMissileBase(&gameParams);
		mb->SetColor(0, 0, .8f);
		mb->SetSide(SIDE_BLUE);
		mb->SetScale(.1f, .1f, .1f);
		mb->SetRotation(0, 0, 96);
		mb->SetTranslation(10 * cos(186*DEG2RAD) + 12.01f, 10 * sin(186*DEG2RAD), -.001f);
		mb->SetLaunchKey(3);
		mb->SetCursorPointer(mousePos[1]);
		gameObjects.push_back(mb);

		mb = new CMissileBase(&gameParams);
		mb->SetColor(0, 0, .8f);
		mb->SetSide(SIDE_BLUE);
		mb->SetScale(.1f, .1f, .1f);
		mb->SetRotation(0, 0, 84);
		mb->SetTranslation(10 * cos(174*DEG2RAD) + 12.01f, 10 * sin(174*DEG2RAD), -.001f);
		mb->SetLaunchKey(4);
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
		cannon->SetFireKey(8);
		cannon->SetSingularityKey('Z');
		cannon->SetBeamKey(8);
		gameObjects.push_back(cannon);

		cannon = new CFlakCannon(&gameParams);
		cannon->SetColor(0, 0, .8f);
		cannon->SetSide(SIDE_BLUE);
		cannon->SetScale(.1f, .1f, .1f);
		cannon->SetRotation(0, 0, 90);
		cannon->SetTranslation(10 * cos(180*DEG2RAD) + 12.01f, 10 * sin(180*DEG2RAD), -.001f);
		cannon->SetCursorPointer(mousePos[1]);
		cannon->SetFireKey(5);
		cannon->SetSingularityKey('N');
		cannon->SetBeamKey(5);
		gameObjects.push_back(cannon);

		SetHyperLevel(1);
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
#ifdef PC_CONTROLS
		mb->SetLaunchKey('A');
#else 
		mb->SetLaunchKey(6);
#endif		
		mb->SetCursorPointer(mousePos[0]);
		gameObjects.push_back(mb);

		mb = new CMissileBase(&gameParams);
		mb->SetColor(0, .8f, 0);
		mb->SetSide(SIDE_GREEN);
		mb->SetScale(.1f, .1f, .1f);
		mb->SetRotation(0, 0, -96);
		mb->SetTranslation(10 * cos(-6*DEG2RAD) - 12.01f, 10 * sin(-6*DEG2RAD), -.001f);
		
#ifdef PC_CONTROLS
		mb->SetLaunchKey('D');
#else
		mb->SetLaunchKey(7);
#endif
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
#ifdef PC_CONTROLS
		cannon->SetFireKey('S');
		cannon->SetSingularityKey('X');
		cannon->SetBeamKey('W');
#else
		cannon->SetFireKey(8);
		cannon->SetSingularityKey('X');
		cannon->SetBeamKey(8);
#endif
		gameObjects.push_back(cannon);		

		SetHyperLevel(1);
		NextWave();
	}	

	else if(gameParams.gameMode == MODE_ATTRACT)
	{
		playingStory = false;
		playingIntro = false;
		exploded = false;
		nukesLaunched = false;
		nukesLaunched2 = false;
		nukesLaunched3 = false;
		nukesLaunched4 = false;
		audioRenderer.PlaySound(SOUND_INTRO, 0, 0);
	}

	return TRUE;												// Return TRUE (Initialization Successful)
}

void CHyperWarGame::Deinitialize (void)										// Any User DeInitialization Goes Here
{
}

void CHyperWarGame::ProcessRawInput(LPARAM lParam)
{
	rawMouse.process_raw_mouse((HRAWINPUT)lParam);
}

void CHyperWarGame::TryCollide(unsigned int collider, unsigned int collidee)
{
	int colliderType, collideeType;
	CDebris *debris;

	//Store types
	colliderType = gameObjects[collider]->GetType();
	collideeType = gameObjects[collidee]->GetType();
	
	if(gameObjects[collider]->CanDestroy(collideeType))
	{
		switch(colliderType)
		{
		case TYPE_MOSHIP:		
			hyperModeTimer = 0;
			SetHyperLevel(GetHyperLevel() - 1);
			for(int k=0; k<gameParams.debrisAmount*16; k++)
			{
				float  debrisAngle;
				float  debrisSize;							

				debrisAngle = gameParams.randoms[gameParams.randIndex++%1024] % 180 + gameObjects[collider]->GetRotation()[2];
				debrisSize = ((gameParams.randoms[gameParams.randIndex++%1024]%100) / 100.0f) * .2f;

				debris = new CDebris(&gameParams);
				debris->SetMotionVector(
					float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
					float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
					0);
				debris->SetTranslation(
					float(gameObjects[collider]->GetTranslation()[0]),
					float(gameObjects[collider]->GetTranslation()[1]),
					float(gameObjects[collider]->GetTranslation()[2]));
				debris->SetScale(debrisSize, debrisSize, debrisSize);
				debris->SetTTL(5000 - gameParams.randoms[gameParams.randIndex++%1024]%5000);
				gameObjects.push_back(debris);
			}
			audioRenderer.PlaySound(SOUND_UFOBLAST, 
				gameObjects[collider]->GetTranslation()[0],
				gameObjects[collider]->GetTranslation()[1],
				gameParams.randoms[gameParams.randIndex++%1024]%100 / 300.0 + .66f);

			switch(gameObjects[collider]->GetSide())
			{
			case SIDE_BLUE:
				gameParams.greenPoints += 500 * pointMultiplier;
				break;
			case SIDE_GREEN:
				gameParams.bluePoints += 500 * pointMultiplier;
			}
			break;
		case TYPE_CITY:		

			hyperModeTimer = 0;
			switch(gameObjects[collider]->GetSide())
			{
			case SIDE_BLUE:
				gameParams.numBlueCities--;
				gameParams.greenPoints += 500 * pointMultiplier;
				break;
			case SIDE_GREEN:
				gameParams.numGreenCities--;
				gameParams.bluePoints += 500 * pointMultiplier;
				break;
			}
			for(int k=0; k<gameParams.debrisAmount*16; k++)
			{
				float  debrisAngle;
				float  debrisSize;							

				debrisAngle = gameParams.randoms[gameParams.randIndex++%1024] % 180 + gameObjects[collider]->GetRotation()[2];
				debrisSize = ((gameParams.randoms[gameParams.randIndex++%1024]%100) / 100.0f) * .2f;

				debris = new CDebris(&gameParams);
				debris->SetMotionVector(
					float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
					float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
					0);
				debris->SetTranslation(
					float(gameObjects[collider]->GetTranslation()[0]),
					float(gameObjects[collider]->GetTranslation()[1]),
					float(gameObjects[collider]->GetTranslation()[2]));
				debris->SetScale(debrisSize, debrisSize, debrisSize);
				debris->SetTTL(15000 - gameParams.randoms[gameParams.randIndex++%1024]%5000);
				gameObjects.push_back(debris);
			}
			audioRenderer.PlaySound(SOUND_EXPLOSION, 
				gameObjects[collider]->GetTranslation()[0],
				gameObjects[collider]->GetTranslation()[1],
				gameParams.randoms[gameParams.randIndex++%1024]%100 / 200.0 + .5f);
			break;

		case TYPE_FLAKCANNON:
		case TYPE_MISSILEBASE:
			hyperModeTimer = 0;
			switch(gameObjects[collider]->GetSide())
			{
			case SIDE_BLUE:
				gameParams.greenPoints += 500 * pointMultiplier;				
				break;
			case SIDE_GREEN:
				gameParams.bluePoints += 500 * pointMultiplier;
				break;
			}

			for(int k=0; k<gameParams.debrisAmount*16; k++)
			{
				float  debrisAngle;
				float  debrisSize;							

				debrisAngle = gameParams.randoms[gameParams.randIndex++%1024] % 180 + gameObjects[collider]->GetRotation()[2];
				debrisSize = ((gameParams.randoms[gameParams.randIndex++%1024]%100) / 100.0f) * .2f;

				debris = new CDebris(&gameParams);
				debris->SetMotionVector(
					float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
					float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
					0);
				debris->SetTranslation(
					float(gameObjects[collider]->GetTranslation()[0]),
					float(gameObjects[collider]->GetTranslation()[1]),
					float(gameObjects[collider]->GetTranslation()[2]));
				debris->SetScale(debrisSize, debrisSize, debrisSize);
				debris->SetTTL(15000 - gameParams.randoms[gameParams.randIndex++%1024]%5000);
				gameObjects.push_back(debris);
			}
			audioRenderer.PlaySound(SOUND_EXPLOSION, 
				gameObjects[collider]->GetTranslation()[0],
				gameObjects[collider]->GetTranslation()[1],
				gameParams.randoms[gameParams.randIndex++%1024]%100 / 200.0 + .5f);
			break;
		case TYPE_SHIP:
			hyperModeTimer = 0;
			switch(gameObjects[collider]->GetSide())
			{
			case SIDE_BLUE:
				gameParams.greenPoints += 500 * pointMultiplier;
				gameParams.blueShip = false;
				gameParams.blueRespawnCountdown = 15000;
				break;
			case SIDE_GREEN:
				gameParams.bluePoints += 500 * pointMultiplier;
				gameParams.greenShip = false;
				gameParams.greenRespawnCountdown = 15000;
				break;
			}
			for(int k=0; k<gameParams.debrisAmount*16; k++)
			{
				float  debrisAngle;
				float  debrisSize;							

				debrisAngle = gameParams.randoms[gameParams.randIndex++%1024] % 360 + gameObjects[collider]->GetRotation()[2];
				debrisSize = ((gameParams.randoms[gameParams.randIndex++%1024]%100) / 100.0f) * .2f;

				debris = new CDebris(&gameParams);
				debris->SetMotionVector(
					float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )) + gameObjects[collider]->GetMotionVector()[0] ,
					float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )) + gameObjects[collider]->GetMotionVector()[1],
					0);
				debris->SetTranslation(
					float(gameObjects[collider]->GetTranslation()[0]),
					float(gameObjects[collider]->GetTranslation()[1]),
					float(gameObjects[collider]->GetTranslation()[2]));
				debris->SetScale(debrisSize, debrisSize, debrisSize);
				debris->SetTTL(5000 - gameParams.randoms[gameParams.randIndex++%1024]%5000);
				gameObjects.push_back(debris);
			}


			audioRenderer.PlaySound(SOUND_EXPLOSION, 
				gameObjects[collider]->GetTranslation()[0],
				gameObjects[collider]->GetTranslation()[1],
				gameParams.randoms[gameParams.randIndex++%1024]%100 / 200.0 + .5f);
			break;
		case TYPE_DEBRIS:
			break;
		default:
			audioRenderer.PlaySound(SOUND_MISSILE_EXPL, gameObjects[collider]->GetTranslation()[0],
				gameObjects[collider]->GetTranslation()[1],
				gameParams.randoms[gameParams.randIndex++%1024]%100 / 300.0 + .66f);	
			for(int k=0; k<gameParams.debrisAmount; k++)
			{
				debris = new CDebris(&gameParams);
				debris->SetMotionVector(
					float(gameObjects[collider]->GetMotionVector()[0] + (gameParams.randoms[gameParams.randIndex++%1024]%100-50)/100.0), 
					float(gameObjects[collider]->GetMotionVector()[1]+ (gameParams.randoms[gameParams.randIndex++%1024]%100-50)/100.0), 
					0);
				debris->SetTranslation(
					float(gameObjects[collider]->GetTranslation()[0]),
					float(gameObjects[collider]->GetTranslation()[1]),
					float(gameObjects[collider]->GetTranslation()[2]));
				debris->SetScale(.1f, .1f, .1f);
				gameObjects.push_back(debris);
			}

			if(collideeType == TYPE_DEBRIS)
			{
				switch(gameObjects[collider]->GetSide())
				{
				case SIDE_BLUE:
					gameParams.greenPoints += 200 * pointMultiplier;
					break;
				case SIDE_GREEN:
					gameParams.bluePoints += 200 * pointMultiplier;
					break;
				}
			}
			break;
		}

		//gameObjects.erase(gameObjects.begin() + i);
		switch(gameObjects[collider]->GetSide())
		{
		case SIDE_BLUE:
			gameParams.greenPoints += 200 * pointMultiplier;
			break;
		case SIDE_GREEN:
			gameParams.bluePoints += 200 * pointMultiplier;
			break;
		}
	}
}

void CHyperWarGame::CollideDestroyObjects(unsigned int obj1, unsigned int obj2)
{
	int obj1Type = gameObjects[obj1]->GetType();
	int obj2Type = gameObjects[obj2]->GetType();

	if(obj1 < obj2)
	{
		//Erase obj2 first
		if(gameObjects[obj2]->CanDestroy(obj1Type) && obj2Type != TYPE_FLAKCANNON && obj2Type != TYPE_MISSILEBASE)
			gameObjects.erase(gameObjects.begin() + obj2);
		if(gameObjects[obj1]->CanDestroy(obj2Type) && obj1Type != TYPE_FLAKCANNON && obj1Type != TYPE_MISSILEBASE)
			gameObjects.erase(gameObjects.begin() + obj1);
	}
	else
	{
		//Erase obj1 first
		if(gameObjects[obj1]->CanDestroy(obj2Type) && obj1Type != TYPE_FLAKCANNON && obj1Type != TYPE_MISSILEBASE)
			gameObjects.erase(gameObjects.begin() + obj1);
		if(gameObjects[obj2]->CanDestroy(obj1Type) && obj2Type != TYPE_FLAKCANNON && obj2Type != TYPE_MISSILEBASE)
			gameObjects.erase(gameObjects.begin() + obj2);
	}
}

void CHyperWarGame::Update (DWORD milliseconds)								// Perform Motion Updates Here
{
	unsigned int objIndex = -1;
	unsigned int i=0;
	CDebris *debris;
	CNuke *nuke;
	CProjectile *pj;
	CBeam *beam;
	CBlackHole *bh;
	CBullet *bullet;
	float *position;
	float *nukeVector;
	float projVector[3];
	int TTL;

	joysticks[0]->poll(&joystates[0]);
	joysticks[1]->poll(&joystates[1]);

	if(gameParams.blueRespawnCountdown >= 0)
		gameParams.blueRespawnCountdown -= milliseconds;
	if(gameParams.greenRespawnCountdown >= 0)
		gameParams.greenRespawnCountdown -= milliseconds;

	if(joystates[0].rgbButtons[5] & 0x80)
	{		
		if(!gameParams.greenShip && gameParams.greenRespawnCountdown < 0)
		{
			//Spawn green ship
			CShip *ship = new CShip(&gameParams);
			ship->SetColor(0, .8f, 0);
			ship->SetSide(SIDE_GREEN);
			ship->SetScale(.05f, .05f, .05f);
			ship->SetTranslation(-1, 0, 0);
			ship->SetJoyState(&joystates[0]);
			ship->SetRotation(0, 0, -90);
			gameParams.greenShip = true;

			gameObjects.push_back(ship);
		}
	}
	if(joystates[1].rgbButtons[5] & 0x80)
	{
		if(!gameParams.blueShip && gameParams.blueRespawnCountdown < 0)
		{
			//Spawn blue ship
			CShip *ship = new CShip(&gameParams);
			ship->SetColor(0, 0, 0.8f);
			ship->SetSide(SIDE_BLUE);
			ship->SetScale(.05f, .05f, .05f);
			ship->SetTranslation(1, 0, 0);
			ship->SetJoyState(&joystates[1]);
			ship->SetRotation(0, 0, 90);
			gameParams.blueShip = true;
			gameObjects.push_back(ship);
		}
	}

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
#ifdef PC_CONTROLS
		mousePos[0][0] -= rawMouse.get_x_delta(gameParams.mouse1Index) / 500.0f;
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
#else

		mousePos[0][0] -= rawMouse.get_y_delta(gameParams.mouse1Index) / 500.0f;
		if(mousePos[0][0] > 0)
			mousePos[0][0] = 0;
		else if(mousePos[0][0] < -1.8f)
			mousePos[0][0] = -1.8f;

		mousePos[0][1] -= rawMouse.get_x_delta(gameParams.mouse1Index) / 500.0f;
		if(mousePos[0][1] > 1.6f)
			mousePos[0][1] = 1.6f;
		else if(mousePos[0][1] < -1.6f)
			mousePos[0][1] = -1.6f;	

		mousePos[1][0] += rawMouse.get_y_delta(gameParams.mouse2Index) / 500.0f;
		if(mousePos[1][0] < 0)
			mousePos[1][0] = 0;
		else if(mousePos[1][0] > 1.8f)
			mousePos[1][0] = 1.8f;

		mousePos[1][1] += rawMouse.get_x_delta(gameParams.mouse2Index) / 500.0f;
		if(mousePos[1][1] > 1.6f)
			mousePos[1][1] = 1.6f;
		else if(mousePos[1][1] < -1.6f)
			mousePos[1][1] = -1.6f;

#endif
	}
	else if(gameParams.gameMode == MODE_SINGLE)
	{

#ifdef PC_CONTROLS
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

#else
		mousePos[0][0] -= rawMouse.get_y_delta(gameParams.mouse1Index) / 500.0f;
		if(mousePos[0][0] > 2.2f)
			mousePos[0][0] = 2.2f;
		else if(mousePos[0][0] < -1.8f)
			mousePos[0][0] = -1.8f;

		mousePos[0][1] -= rawMouse.get_x_delta(gameParams.mouse1Index) / 500.0f;
		if(mousePos[0][1] > 1.6f)
			mousePos[0][1] = 1.6f;
		else if(mousePos[0][1] < -1.6f)
			mousePos[0][1] = -1.6f;	

#endif
	}


	//For every object in the game
	for(unsigned int i=0; i<gameObjects.size(); i++)
	{
		//Process motion
		gameObjects[i]->ProcessMotion(milliseconds, g_keys);

		//Process gravity
		gameObjects[i]->ProcessGravity(milliseconds, gravityWells);

		if(gameObjects[i]->deleteMe)
		{
			gameObjects.erase(gameObjects.begin() + i);
			continue;
		}

		if(gameObjects[i]->GetType() == TYPE_SHIP)
		{
			if(((CShip*)(gameObjects[i]))->IsFiring())
			{
				bullet = new CBullet(&gameParams);
				
				bullet->SetScale(.04, .04, .04);
				
				projVector[0] = cos((((CShip*)(gameObjects[i]))->GetRotation()[2] + 90) * DEG2RAD);
				projVector[1] = sin((((CShip*)(gameObjects[i]))->GetRotation()[2] + 90) * DEG2RAD);

				bullet->SetRotation(((CShip*)(gameObjects[i]))->GetRotation()[0],
					((CShip*)(gameObjects[i]))->GetRotation()[1],
					((CShip*)(gameObjects[i]))->GetRotation()[2]);
				
				bullet->SetTranslation(((CShip*)(gameObjects[i]))->GetTranslation()[0] + projVector[0]/16,
					((CShip*)(gameObjects[i]))->GetTranslation()[1] + projVector[1]/16,
					((CShip*)(gameObjects[i]))->GetTranslation()[2]);
				bullet->SetMotionVector(
					((CShip*)(gameObjects[i]))->GetMotionVector()[0] + projVector[0] * 4,
					((CShip*)(gameObjects[i]))->GetMotionVector()[1] + projVector[1] * 4,
					((CShip*)(gameObjects[i]))->GetMotionVector()[2] );		

				audioRenderer.PlaySound(SOUND_SHOOT, 
					((CShip*)(gameObjects[i]))->GetTranslation()[1], 
					((CShip*)(gameObjects[i]))->GetTranslation()[2],
					gameParams.randoms[gameParams.randIndex++%1024]%100 / 300.0 + .66f);

				gameObjects.push_back(bullet);				
			}
		}
		else if(gameObjects[i]->GetType() == TYPE_MISSILEBASE)
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

				audioRenderer.PlaySound(SOUND_MISSILE, 
					position[0], 
					position[1],
					gameParams.randoms[gameParams.randIndex++%1024]%100 / 300.0 + .66f);

				switch(gameObjects[i]->GetSide())
				{
				case SIDE_GREEN:
					gameParams.greenPoints += 10 * pointMultiplier;
					break;
				case SIDE_BLUE:
					gameParams.bluePoints += 10 * pointMultiplier;
					break;
				}
			}
		}

		else if(gameObjects[i]->GetType() == TYPE_FLAKCANNON)
		{
			if(g_keys->keyDown[((CFlakCannon*)(gameObjects[i]))->GetSingularityKey()] 
			&& ((CFlakCannon*)(gameObjects[i]))->HasSuperWeapon()
			&& ((CFlakCannon*)(gameObjects[i]))->IsLoaded())
			{
				//Fire singularity
				((CFlakCannon*)(gameObjects[i]))->FireSuperWeapon();
				position = ((CFlakCannon*)(gameObjects[i]))->GetProjTranslation();
				((CFlakCannon*)(gameObjects[i]))->GetProjVector(&TTL, projVector);

				audioRenderer.PlaySound(SOUND_MEGABLAST, 
					position[0], 
					position[1],
					gameParams.randoms[gameParams.randIndex++%1024]%100 / 300.0 + .66f);

				//Create a gravity well for the black hoole
				sGravityWell *gw = new sGravityWell();
				gw->mass = gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX * 40;
				gw->translation[0] = position[0];
				gw->translation[1] = position[1];
				gw->translation[2] = position[2];

				//Create a black hole
				bh = new CBlackHole(&gameParams);
				bh->SetColor(gameObjects[i]->GetColor()[0], gameObjects[i]->GetColor()[1], gameObjects[i]->GetColor()[2]);
				bh->SetScale(gameObjects[i]->GetScale()[0], gameObjects[i]->GetScale()[1], gameObjects[i]->GetScale()[2]);
				bh->SetTranslation(position[0] + projVector[0]/50.0f, position[1] + projVector[1] / 50.0f, -.0010f);
				((CFlakCannon*)(gameObjects[i]))->GetProjVector(&TTL, projVector);
				bh->SetMotionVector(projVector[0]/15.0f, projVector[1]/15.0f, projVector[2]/15.0f);
				position = gameObjects[i]->GetTranslation();
				bh->SetMyGravity(gw);				
				
				gravityWells.push_back(gw);
				gameObjects.push_back(bh);
			}
			else if(g_keys->keyDown[((CFlakCannon*)(gameObjects[i]))->GetBeamKey()] 
			&& ((CFlakCannon*)(gameObjects[i]))->HasSuperWeapon()
			&& ((CFlakCannon*)(gameObjects[i]))->IsLoaded())
			{
				//Fire beam
				((CFlakCannon*)(gameObjects[i]))->FireSuperWeapon();
				position = ((CFlakCannon*)(gameObjects[i]))->GetProjTranslation();
				float angle = ((CFlakCannon*)(gameObjects[i]))->GetProjAngle();
				((CFlakCannon*)(gameObjects[i]))->GetProjVector(&TTL, projVector);

				beam = new CBeam(&gameParams);
				if(gameObjects[i]->GetSide() == SIDE_GREEN)
					beam->SetRotation(gameObjects[i]->GetRotation()[0],
						gameObjects[i]->GetRotation()[1], angle + 90);
				else
					beam->SetRotation(gameObjects[i]->GetRotation()[0],
						gameObjects[i]->GetRotation()[1], angle - 90);

				beam->SetTranslation(
					float(gameObjects[i]->GetTranslation()[0] + projVector[0] / 100.0f),
					float(gameObjects[i]->GetTranslation()[1] + projVector[1] / 100.0f),
					float(gameObjects[i]->GetTranslation()[2]));
				beam->SetScale(gameObjects[i]->GetScale()[0],
					gameObjects[i]->GetScale()[1],
					gameObjects[i]->GetScale()[2]);
				beam->SetSide(gameObjects[i]->GetSide());

				gameObjects.push_back(beam);
				audioRenderer.PlaySound(SOUND_ZAP, 
					gameObjects[i]->GetTranslation()[0], 
					gameObjects[i]->GetTranslation()[1]);
			}
			else if(g_keys->keyDown[((CFlakCannon*)(gameObjects[i]))->GetFireKey()] && ((CFlakCannon*)(gameObjects[i]))->IsLoaded())
			{
				//Launch projectile
				//Set flak cannon to "just launched"
				((CFlakCannon*)(gameObjects[i]))->Fire();
				position = ((CFlakCannon*)(gameObjects[i]))->GetProjTranslation();

				audioRenderer.PlaySound(SOUND_SHOOT, 
					position[0], 
					position[1],
					gameParams.randoms[gameParams.randIndex++%1024]%100 / 300.0 + .66f);

				//Spawn a projectile
				pj = new CProjectile(&gameParams);
				pj->SetColor(gameObjects[i]->GetColor()[0], gameObjects[i]->GetColor()[1], gameObjects[i]->GetColor()[2]);
				pj->SetScale(gameObjects[i]->GetScale()[0], gameObjects[i]->GetScale()[1], gameObjects[i]->GetScale()[2]);
				
				pj->SetTranslation(position[0], position[1], -.0010f);
				((CFlakCannon*)(gameObjects[i]))->GetProjVector(&TTL, projVector);
				pj->SetMotionVector(projVector[0], projVector[1], projVector[2]);
				position = gameObjects[i]->GetTranslation();
				pj->SetOrigin(position[0], position[1],position[2]);
				pj->SetTTL(TTL);
				gameObjects.push_back(pj);
			}	
			else if(!g_keys->keyDown[((CFlakCannon*)(gameObjects[i]))->GetFireKey()] 
			&& !g_keys->keyDown[((CFlakCannon*)(gameObjects[i]))->GetBeamKey()] 
			&& !g_keys->keyDown[((CFlakCannon*)(gameObjects[i]))->GetSingularityKey()] 
			&& !((CFlakCannon*)(gameObjects[i]))->IsLoaded())
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
					int heading = gameParams.randoms[gameParams.randIndex++%1024]%360;
					float velocity = (float)((gameParams.randoms[gameParams.randIndex++%1024]%100-50) / gameParams.flakVelocityDivider);

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
				audioRenderer.PlaySound(SOUND_BOOM, 
					gameObjects[i]->GetTranslation()[0],
					gameObjects[i]->GetTranslation()[1],
					gameParams.randoms[gameParams.randIndex++%1024]%100 / 200.0 + .5f);
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
		else if(gameObjects[i]->GetType() == TYPE_MOSHIP)
		{
			((CMothership*)(gameObjects[i]))->AddCount(milliseconds);
			if(((CMothership*)(gameObjects[i]))->IsFiring())
			{
				beam = new CBeam(&gameParams);
				beam->SetRotation(gameObjects[i]->GetRotation()[0],
					gameObjects[i]->GetRotation()[1],
				gameObjects[i]->GetRotation()[2]);
				beam->SetMotionVector(gameObjects[i]->GetMotionVector()[0],
					gameObjects[i]->GetMotionVector()[1],
					gameObjects[i]->GetMotionVector()[2]);
				beam->SetTranslation(
					float(gameObjects[i]->GetTranslation()[0] - .075f),
					float(gameObjects[i]->GetTranslation()[1]),
					float(gameObjects[i]->GetTranslation()[2]));
				beam->SetScale(gameObjects[i]->GetScale()[0],
					gameObjects[i]->GetScale()[1],
					gameObjects[i]->GetScale()[2]);
				((CMothership*)(gameObjects[i]))->SetMyBeam(beam);
				beam->SetMyParent(((CMothership*)(gameObjects[i])));
				beam->SetSide(gameObjects[i]->GetSide());
				gameObjects.push_back(beam);
				audioRenderer.PlaySound(SOUND_ZAP, 
					gameObjects[i]->GetTranslation()[0], 
					gameObjects[i]->GetTranslation()[1]);
			}
		}
		else if(gameObjects[i]->GetType() == TYPE_BEAM)
		{
			((CBeam*)(gameObjects[i]))->AddCount(milliseconds);
			if(((CBeam*)(gameObjects[i]))->GetTTL() < 0)
			{
				if(((CBeam*)(gameObjects[i]))->myParent != NULL)
					((CBeam*)(gameObjects[i]))->myParent->SetMyBeam(NULL);
				gameObjects.erase(gameObjects.begin() + i);
				continue;
			}
		}
		else if(gameObjects[i]->GetType() == TYPE_BLACKHOLE)
		{
			if(((CBlackHole*)(gameObjects[i]))->GetTTL() < 0)
			{
				((CBlackHole*)(gameObjects[i]))->KillGravity();
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
			TryCollide(objIndex, i);
			TryCollide(i, objIndex);
			CollideDestroyObjects(objIndex, i);
		}
	}

	hyperModeTimer += milliseconds;

	if(hyperModeTimer > gameParams.hyperModeDelay && gameParams.gameMode == MODE_VS)
	{
		//increase hyper level
		SetHyperLevel(GetHyperLevel() + 1);
	}
	else if(gameParams.gameMode == MODE_SINGLE && hyperModeTimer > gameParams.waveTime)
	{
		NextWave();
		hyperModeTimer = 0;
	}
	else if(gameParams.gameMode == MODE_ATTRACT)
	{
		RunAttractMode();
	}
	else if(gameParams.gameMode == MODE_HIGHSCORE)
	{
		RunHighScoreEntry();
	}

	//Update starfield position
	//Starfield position should be moved as a function of time
	globalEffects.SetStarFieldPosition(globalEffects.GetStarFieldPosition() + milliseconds);
	
	audioRenderer.RenderAudio(milliseconds, gameObjects);

	//Check to see if it is time to add super weapon ammo
	if(gameParams.gameMode == MODE_VS && gameParams.bluePoints > (gameParams.blueSuperAmmo + 5) * 5000)
	{
		//gameParams.blueSuperAmmo += 5;
		gameParams.blueSuperAmmo += 4;
		audioRenderer.PlaySound(SOUND_CHARGEUP, 0, 0);
	}

	if(gameParams.greenPoints > (gameParams.greenSuperAmmo + 5) * 5000)
	{
		gameParams.greenSuperAmmo += 5;
		audioRenderer.PlaySound(SOUND_CHARGEUP, 0, 0);
	}

	//check for game over
	if(gameParams.numBlueCities < 1 && gameParams.gameMode != MODE_GAMEOVER && gameParams.gameMode != MODE_HIGHSCORE)
	{
		//Green wins
		gameObjects.clear();
		greenWins = true;

		gameParams.gameMode = MODE_GAMEOVER;
		hyperModeTimer = 0;

		//play game over sounds
		audioRenderer.StopAll();
		audioRenderer.PlaySound(SOUND_GAMEOVER, 0, 0);
	}
	else if(gameParams.numGreenCities < 1 && gameParams.gameMode != MODE_GAMEOVER && gameParams.gameMode != MODE_HIGHSCORE)
	{
		//Blue wins
		gameObjects.clear();
		blueWins = true;		

		if(hsList->CheckScore(gameParams.greenPoints) && gameParams.gameMode == MODE_SINGLE)
		{
			memset(highScoreName, 0, 64);
			nameIndex = 0;
			currentLetter = 65;
			gameParams.gameMode = MODE_HIGHSCORE;
			listTime = false;
			hyperModeTimer = 0;

			audioRenderer.StopAll();
			audioRenderer.PlaySound(SOUND_EXPLOSION, 0, 0);
			//Play high score music?
		}
		else
		{
			gameParams.gameMode = MODE_GAMEOVER;
			hyperModeTimer = 0;

			//play game over sounds
			audioRenderer.StopAll();
			audioRenderer.PlaySound(SOUND_EXPLOSION, 0, 0);
			audioRenderer.PlaySound(SOUND_GAMEOVER, 0, 0);
		}
	}
}

void CHyperWarGame::RunHighScoreEntry()
{
	//sprintf_s(highScoreName, 64, "High score dude");
	char charLetter;

	if(!listTime)
	{
		currentLetter = (currentLetter + rawMouse.get_x_delta(0) / 32.0);

		if(currentLetter < ' ')
			currentLetter = ' ';
		else if(currentLetter > '~')
			currentLetter = '~';

		charLetter = (char)currentLetter % 128;

		//Disallow some unfriendly characters
		if(charLetter == '\\')
			charLetter++;
		if(charLetter == '\'')
			charLetter++;
		if(charLetter == '\"')
			charLetter++;

		if(charLetter == 127)
		{
			//Let's call this the 'enter' character
		}

		highScoreName[nameIndex] = charLetter;

		if(g_keys->keyDown[8] && hyperModeTimer > 150 && nameIndex < 63)
		{
			hyperModeTimer = 0;
			nameIndex++;
		}
		else if(g_keys->keyDown[6] && hyperModeTimer > 150 && nameIndex > 0)
		{
			hyperModeTimer = 0;
			highScoreName[nameIndex] = '\0';
			nameIndex--;
		}
		else if(g_keys->keyDown[7] && hyperModeTimer > 150 && nameIndex > 0)
		{
			hsList->AddScore(highScoreName, gameParams.greenPoints);
			listTime = true;
			hyperModeTimer = 0;
		}
	}
	else
	{
		if(hyperModeTimer > 7000)
		{
			gameParams.gameMode = MODE_GAMEOVER;
			hyperModeTimer = 0;

			//play game over sounds
			audioRenderer.StopAll();
			audioRenderer.PlaySound(SOUND_GAMEOVER, 0, 0);
		}
	}
}

void CHyperWarGame::RunAttractMode()
{
	float trans[3];

	if(hyperModeTimer > 4000 && !playingStory)
	{
		audioRenderer.PlaySound(SOUND_STORY, 0, 0);
		playingStory = true;
	}

	if(hyperModeTimer > 39000 && !exploded)
	{
		exploded = true;
		for(int k=0; k<gameParams.debrisAmount*64; k++)
		{
			float  debrisAngle;
			float  debrisSize;							

			debrisAngle = (float)(gameParams.randoms[gameParams.randIndex++%1024] % 360);
			debrisSize = ((gameParams.randoms[gameParams.randIndex++%1024]%100) / 100.0f) * .2f;

			CDebris *debris = new CDebris(&gameParams);
			debris->SetMotionVector(
				float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
				float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
				0);
			debris->SetTranslation(
				float(0),
				float(0),
				float(-.01f));
			debris->SetScale(debrisSize, debrisSize, debrisSize);
			debris->SetTTL(10000 - gameParams.randoms[gameParams.randIndex++%1024]%5000);
			gameObjects.push_back(debris);
		}
		for(int k=0; k<gameParams.debrisAmount*64; k++)
		{
			float  debrisAngle;
			float  debrisSize;							

			debrisAngle = (float)(gameParams.randoms[gameParams.randIndex++%1024] % 360);
			debrisSize = ((gameParams.randoms[gameParams.randIndex++%1024]%100) / 100.0f) * .2f;

			CDebris *debris = new CDebris(&gameParams);
			debris->SetMotionVector(
				float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
				float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
				0);
			debris->SetTranslation(
				float(.1f),
				float(0),
				float(-.01f));
			debris->SetScale(debrisSize, debrisSize, debrisSize);
			debris->SetTTL(8000 - gameParams.randoms[gameParams.randIndex++%1024]%5000);
			gameObjects.push_back(debris);
		}
		for(int k=0; k<gameParams.debrisAmount*64; k++)
		{
			float  debrisAngle;
			float  debrisSize;							

			debrisAngle = (float)(gameParams.randoms[gameParams.randIndex++%1024] % 360);
			debrisSize = ((gameParams.randoms[gameParams.randIndex++%1024]%100) / 100.0f) * .2f;

			CDebris *debris = new CDebris(&gameParams);
			debris->SetMotionVector(
				float(cos(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
				float(sin(debrisAngle * DEG2RAD) * (.07f / debrisSize )),
				0);
			debris->SetTranslation(
				float(-.1f),
				float(0),
				float(-.01f));
			debris->SetScale(debrisSize, debrisSize, debrisSize);
			debris->SetTTL(10000 - gameParams.randoms[gameParams.randIndex++%1024]%5000);
			gameObjects.push_back(debris);
		}
	}

	if(hyperModeTimer > 44000 && !nukesLaunched)
	{
		nukesLaunched = true;

		//Add some random gravity wells
		for(int i=0; i<8; i++)
		{			
			sGravityWell *gw = new sGravityWell();
			gw->mass = gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX * 2;
			gw->translation[0] = (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f) * 4.5f;
			gw->translation[1] = (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f) * 2.5f;
			gw->translation[2] = 0;
			gravityWells.push_back(gw);
		}
	}
	
	if(hyperModeTimer > 47000 && !nukesLaunched2)
	{
		nukesLaunched2 = true;
		gameParams.nukeGravityImmunityTime = 0;		
		gameParams.nukeSpeedDivider = 8000;

		trans[0] = (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f) * 25.0f;
		trans[1] = (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f) * 25.0f;
		trans[2] = 0;

		for(int i = 0; i<30; i++)
		{
			CNuke *nuke = new CNuke(&gameParams);
			nuke->SetScale(.1f, .1f, .1f);
			nuke->SetTranslation(i%4 - 2.0f, i/10.0f - 1.5f, 0);
			nuke->SetMotionVector(trans[0], trans[1], trans[2]);
			if(i<8)
				nuke->SetColor(0, 1, 0);
			else
				nuke->SetColor(0, 0, 1);
			gameObjects.push_back(nuke);
		}
	}

	if(hyperModeTimer > 52000 && !nukesLaunched3)
	{
		nukesLaunched3 = true;

		trans[0] = (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f) * 25.0f;
		trans[1] = (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f) * 25.0f;
		trans[2] = 0;
		for(int i = 0; i<30; i++)
		{
			CNuke *nuke = new CNuke(&gameParams);
			nuke->SetScale(.1f, .1f, .1f);
			nuke->SetTranslation(i%4 - 2.0f, i/10.0f - 1.5f, 0);
			trans[0] = (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f) * 25.0f;
			trans[1] = (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f) * 25.0f;
			trans[2] = 0;
			nuke->SetMotionVector(trans[0], trans[1], trans[2]);
			if(i<10)
				nuke->SetColor(0, 1, 0);
			else
				nuke->SetColor(0, 0, 1);
			gameObjects.push_back(nuke);
		}
	}

	if(hyperModeTimer > 57000 && !nukesLaunched4)
	{
		nukesLaunched4 = true;

		trans[0] = (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f) * 25.0f;
		trans[1] = (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f) * 25.0f;
		trans[2] = 0;
		for(int i = 0; i<30; i++)
		{
			CNuke *nuke = new CNuke(&gameParams);
			nuke->SetScale(.1f, .1f, .1f);
			nuke->SetTranslation(i%4 - 2.0f, i/10.0f - 1.5f, 0);
			trans[0] = (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f) * 25.0f;
			trans[1] = (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f) * 25.0f;
			trans[2] = 0;
			nuke->SetMotionVector(trans[0], trans[1], trans[2]);
			if(i<15)
				nuke->SetColor(0, 1, 0);
			else
				nuke->SetColor(0, 0, 1);
			gameObjects.push_back(nuke);
		}
	}

	if(hyperModeTimer > 72000)
	{
		Initialize(g_window, g_keys);
	}

	if(g_keys->keyDown['1'])
	{
		gameParams.gameMode = MODE_SINGLE;
		Initialize(g_window, g_keys);
	}

	if(g_keys->keyDown['2'])
	{
		gameParams.gameMode = MODE_VS;
		Initialize(g_window, g_keys);
	}
}

void CHyperWarGame::DrawCursors()
{
	glPushMatrix();

	glTranslatef(mousePos[0][0], mousePos[0][1], 0);
	glScalef(.05f, .05f, 1);
	float randVal1 = .5f + gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX / 2.0f;
	float randVal2 = .5f + gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX / 2.0f;
	float randVal3 = .5f + gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX / 2.0f;

	if(gameParams.greenSuperAmmo > gameParams.greenSuperFires)
	{
		//Draw super cursor
		glColor3f(randVal1, randVal2, randVal3);
	}		
	else
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

		if(gameParams.blueSuperAmmo > gameParams.blueSuperFires)
		{
			//Draw super cursor
			glColor3f(randVal1, randVal2, randVal3);
		}
		else
			glColor3f(0, 1, 1);

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
	char tempStr[64];
	int width, height;

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glLoadIdentity ();											// Reset The Modelview Matrix
	
	//Set up the global rendering coordinate system
	glTranslatef (0.0f, 0.0f, -1.0f);
	glScalef(.25f, .25f, 1);

	globalEffects.DrawStarfield();

	switch(gameParams.gameMode)
	{
	case MODE_SINGLE:
	case MODE_VS:
	case MODE_COOP:
		//Draw all objects in the game at their current positions
		for(unsigned int i=0; i<gameObjects.size(); i++)
		{
			gameObjects[i]->Draw();
		}		
		DrawCursors();		
		DrawHUD();
		break;

	case MODE_ATTRACT:		
		for(unsigned int i=0; i<gameObjects.size(); i++)
		{
			gameObjects[i]->Draw();
		}	
		DrawAttract();
		break;
	case MODE_HIGHSCORE:
		DrawHighScoreEntry();
		break;
	case MODE_GAMEOVER:
		//Draw GAME OVER screen
		glPushMatrix();		
		glEnable(GL_TEXTURE_2D);
		glRotatef(-90, 0, 0, 1);

		width = 0;
		height = 0;
		sprintf_s(tempStr, 64, "GAME OVER");
		for(unsigned int i=0; i<strnlen(tempStr, 64); i++)
		{
			width += titleFont->GetCharWidthA(tempStr[i]);
		}
		height = titleFont->GetCharHeight('D');
		glScalef(.005f, .005f, .005f);
		glTranslatef(-width/2.0f, 0, 0);	
		titleFont->Begin();
		titleFont->DrawString(tempStr, 0, 0);
		glPopMatrix();

		glPushMatrix();
		glRotatef(-90, 0, 0, 1);
		width = 0;
		height = 0;
		if(greenWins)
			sprintf_s(tempStr, 64, "You Win.");
		else if(blueWins)
			sprintf_s(tempStr, 64, "You Lose.");
		else
			sprintf_s(tempStr, 64, "");
		for(unsigned int i=0; i<strnlen(tempStr, 64); i++)
		{
			width += titleFont->GetCharWidthA(tempStr[i]);
		}
		glScalef(.0025f, .0025f, .0025f);
		glTranslatef(-width/2.0f, -200, 0);	
		titleFont->Begin();
		titleFont->DrawString(tempStr, 0, 0);
		glPopMatrix();

		glPushMatrix();		
		glEnable(GL_TEXTURE_2D);
		glRotatef(90, 0, 0, 1);

		width = 0;
		height = 0;
		sprintf_s(tempStr, 64, "GAME OVER");
		for(unsigned int i=0; i<strnlen(tempStr, 64); i++)
		{
			width += titleFont->GetCharWidthA(tempStr[i]);
		}
		height = titleFont->GetCharHeight('D');
		glScalef(.005f, .005f, .005f);
		glTranslatef(-width/2.0f, 0, 0);	
		titleFont->Begin();
		titleFont->DrawString(tempStr, 0, 0);			
		glPopMatrix();

		glPushMatrix();
		glRotatef(90, 0, 0, 1);
		width = 0;
		height = 0;
		if(greenWins)
			sprintf_s(tempStr, 64, "You Lose.");
		else if(blueWins)
			sprintf_s(tempStr, 64, "You Win.");
		else
			sprintf_s(tempStr, 64, "");
		for(unsigned int i=0; i<strnlen(tempStr, 64); i++)
		{
			width += titleFont->GetCharWidthA(tempStr[i]);
		}
		height = titleFont->GetCharHeight('D');
		glScalef(.0025f, .0025f, .0025f);
		glTranslatef(-width/2.0f, -200, 0);	
		titleFont->Begin();
		titleFont->DrawString(tempStr, 0, 0);
		glPopMatrix();

		DrawHUD();

		if(hyperModeTimer > 7000)
		{
			gameParams.gameMode = MODE_ATTRACT;
			this->Initialize(g_window, g_keys);
		}
		break;
	}	

	glFlush ();													// Flush The GL Rendering Pipeline
}

void CHyperWarGame::DrawHUD()
{
	char pointsString[16];
	int width = 0;
	int height = 0;
	
	glPushMatrix();

	glColor3f(1, 1, 1);
	glTranslatef(-2.1f, 0, 0);
	glRotatef(-90, 0, 0, 1);

	glScalef(.001f, .001f, .001f);
	glEnable(GL_TEXTURE_2D);

	sprintf_s(pointsString, 16, "%d", gameParams.greenPoints);

	for(unsigned int i=0; i<strnlen(pointsString, 16); i++)
	{
		width += scoreFont->GetCharWidthA(pointsString[i]);
	}

	height = scoreFont->GetCharHeight('8');
	
	glTranslatef(-width/2.0f, height/2.0f, 0);
	scoreFont->Begin();
	glColor3f(1, 1, 1);
	scoreFont->DrawString(pointsString, 0, 0);

	glPopMatrix();

	if(gameParams.gameMode != MODE_SINGLE)
	{
		width = 0;
		height = 0;

		glPushMatrix();


		glTranslatef(2.1f, 0, 0);
		glRotatef(90, 0, 0, 1);

		glScalef(.001f, .001f, .001f);
		glEnable(GL_TEXTURE_2D);

		sprintf_s(pointsString, 16, "%d", gameParams.bluePoints);

		for(unsigned int i=0; i<strnlen(pointsString, 16); i++)
		{
			width += scoreFont->GetCharWidthA(pointsString[i]);
		}

		height = scoreFont->GetCharHeight('8');
		
		glTranslatef(-width/2.0f, height/2.0f, 0);
		scoreFont->Begin();
		scoreFont->DrawString(pointsString, 0, 0);

		glPopMatrix();
	}

	glDisable(GL_TEXTURE_2D);
}

void CHyperWarGame::DrawAttract()
{

	char storyLine[64];
	int width = 0;
	int height = 0;
	
	glEnable(GL_TEXTURE_2D);
	storyFont->Begin();
	
	glPushMatrix();	
	glColor3f(1, .8f, .8f);

	if(hyperModeTimer < 39000)
	{
		glTranslatef(0, hyperModeTimer / 11000.0f - 1.0f, 0);
		if(hyperModeTimer > 4000)
		{
			glPushMatrix();			
			glScalef(.001f, .001f, .001f);
			sprintf_s(storyLine, 64, "Deep in the Perseus Arm of the Milky Way, two");
			for(unsigned int i=0; i<strnlen(storyLine, 64); i++)
			{
				width += storyFont->GetCharWidthA(storyLine[i]);
			}
			height = storyFont->GetCharHeight('D');
			glTranslatef(-width/2.0f, height/2.0f, 0);
			storyFont->DrawString(storyLine, 0, 0);
			glPopMatrix();
		}

		if(hyperModeTimer > 7000)
		{
			width = 0;
			height = 0;

			glPushMatrix();
			glScalef(.001f, .001f, .001f);
			sprintf_s(storyLine, 64, "opposing civilizations have been locked in a");
			for(unsigned int i=0; i<strnlen(storyLine, 64); i++)
			{
				width += storyFont->GetCharWidthA(storyLine[i]);
			}
			height = storyFont->GetCharHeight('D');
			glTranslatef(-width/2.0f, height/2.0f - 2*height, 0);
			storyFont->DrawString(storyLine, 0, 0);
			glPopMatrix();
		}

		if(hyperModeTimer > 10000)
		{
			width = 0;
			height = 0;

			glPushMatrix();
			glScalef(.001f, .001f, .001f);
			sprintf_s(storyLine, 64, "bitter war of attrition for 9,000 years.  The blue");
			for(unsigned int i=0; i<strnlen(storyLine, 64); i++)
			{
				width += storyFont->GetCharWidthA(storyLine[i]);
			}
			height = storyFont->GetCharHeight('D');
			glTranslatef(-width/2.0f, height/2.0f - 4*height, 0);
			storyFont->DrawString(storyLine, 0, 0);
			glPopMatrix();
		}

		if(hyperModeTimer > 14000)
		{
			width = 0;
			height = 0;

			glPushMatrix();
			glScalef(.001f, .001f, .001f);
			sprintf_s(storyLine, 64, "ocean planet of Tantusiouswui and the green");
			for(unsigned int i=0; i<strnlen(storyLine, 64); i++)
			{
				width += storyFont->GetCharWidthA(storyLine[i]);
			}
			height = storyFont->GetCharHeight('D');
			glTranslatef(-width/2.0f, height/2.0f - 6*height, 0);
			storyFont->DrawString(storyLine, 0, 0);
			glPopMatrix();
		}

		if(hyperModeTimer > 17000)
		{
			width = 0;
			height = 0;

			glPushMatrix();
			glScalef(.001f, .001f, .001f);
			sprintf_s(storyLine, 64, "forest planet of Varelyykesbri each believe that");
			for(unsigned int i=0; i<strnlen(storyLine, 64); i++)
			{
				width += storyFont->GetCharWidthA(storyLine[i]);
			}
			height = storyFont->GetCharHeight('D');
			glTranslatef(-width/2.0f, height/2.0f - 8*height, 0);
			storyFont->DrawString(storyLine, 0, 0);
			glPopMatrix();
		}

		if(hyperModeTimer > 20000)
		{
			width = 0;
			height = 0;

			glPushMatrix();
			glScalef(.001f, .001f, .001f);
			sprintf_s(storyLine, 64, "the other is responsible for starting this horrific conflict.");
			for(unsigned int i=0; i<strnlen(storyLine, 64); i++)
			{
				width += storyFont->GetCharWidthA(storyLine[i]);
			}
			height = storyFont->GetCharHeight('D');
			glTranslatef(-width/2.0f, height/2.0f - 10*height, 0);
			storyFont->DrawString(storyLine, 0, 0);
			glPopMatrix();
		}

		if(hyperModeTimer > 24500)
		{
			width = 0;
			height = 0;

			glPushMatrix();
			glScalef(.001f, .001f, .001f);
			sprintf_s(storyLine, 64, "Now, with the recent creation of the first");
			for(unsigned int i=0; i<strnlen(storyLine, 64); i++)
			{
				width += storyFont->GetCharWidthA(storyLine[i]);
			}
			height = storyFont->GetCharHeight('D');
			glTranslatef(-width/2.0f, height/2.0f - 14*height, 0);
			storyFont->DrawString(storyLine, 0, 0);
			glPopMatrix();
		}

		if(hyperModeTimer > 27500)
		{
			width = 0;
			height = 0;

			glPushMatrix();
			glScalef(.001f, .001f, .001f);
			sprintf_s(storyLine, 64, "Interplanetary Nuclear Missile, YOU must be");
			for(unsigned int i=0; i<strnlen(storyLine, 64); i++)
			{
				width += storyFont->GetCharWidthA(storyLine[i]);
			}
			height = storyFont->GetCharHeight('D');
			glTranslatef(-width/2.0f, height/2.0f - 16*height, 0);
			storyFont->DrawString(storyLine, 0, 0);
			glPopMatrix();
		}

		if(hyperModeTimer > 30500)
		{
			width = 0;
			height = 0;

			glPushMatrix();
			glScalef(.001f, .001f, .001f);
			sprintf_s(storyLine, 64, "the one to end it! Use your nukes to wipe your");
			for(unsigned int i=0; i<strnlen(storyLine, 64); i++)
			{
				width += storyFont->GetCharWidthA(storyLine[i]);
			}
			height = storyFont->GetCharHeight('D');
			glTranslatef(-width/2.0f, height/2.0f - 18*height, 0);
			storyFont->DrawString(storyLine, 0, 0);
			glPopMatrix();
		}

		if(hyperModeTimer > 33500)
		{
			width = 0;
			height = 0;

			glPushMatrix();
			glScalef(.001f, .001f, .001f);
			sprintf_s(storyLine, 64, "opponent's homeworld clean of their filthy");
			for(unsigned int i=0; i<strnlen(storyLine, 64); i++)
			{
				width += storyFont->GetCharWidthA(storyLine[i]);
			}
			height = storyFont->GetCharHeight('D');
			glTranslatef(-width/2.0f, height/2.0f - 20*height, 0);
			storyFont->DrawString(storyLine, 0, 0);
			glPopMatrix();
		}

		if(hyperModeTimer > 36500)
		{
			width = 0;
			height = 0;

			glPushMatrix();
			glScalef(.001f, .001f, .001f);
			sprintf_s(storyLine, 64, "cities.  So begins....");
			for(unsigned int i=0; i<strnlen(storyLine, 64); i++)
			{
				width += storyFont->GetCharWidthA(storyLine[i]);
			}
			height = storyFont->GetCharHeight('D');
			glTranslatef(-width/2.0f, height/2.0f - 22*height, 0);			
			storyFont->DrawString(storyLine, 0, 0);
			glPopMatrix();
		}
	}
	else if(hyperModeTimer < 65000)
	{
		width = 0;
		height = 0;

		glPushMatrix();		
		glScalef(.006f, .006f, .006f);
		glColor3f(1, 1, 1);
		sprintf_s(storyLine, 64, "HYPERWAR");
		for(unsigned int i=0; i<strnlen(storyLine, 64); i++)
		{
			width += titleFont->GetCharWidthA(storyLine[i]);
		}
		height = titleFont->GetCharHeight('D');
		if(hyperModeTimer > 44000 && hyperModeTimer  <= 48000)
			glTranslatef(-width/2.0f, height/2.0f, -(hyperModeTimer - 44000)/50.0f);
		else if(hyperModeTimer > 48000)
			glTranslatef(-width/2.0f, height/2.0f, -4000/50.0f);
		else
			glTranslatef(-width/2.0f, height/2.0f, 0);
		glEnable(GL_BLEND);
		titleFont->Begin();
		titleFont->DrawString(storyLine, 0, 0);
		glPopMatrix();
	}
	else
	{
		hsList->Draw(false);
	}

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void CHyperWarGame::DrawHighScoreEntry()
{
	int width, height;
	char tempStr[64];
	unsigned int i=0;

	if(!listTime)
	{
		glPushMatrix();		
		glEnable(GL_TEXTURE_2D);

		glRotatef(-90, 0, 0, 1);

		width = 0;
		height = 0;
		sprintf_s(tempStr, 64, "CONGRATUALTIONS.");
		for(unsigned int i=0; i<strnlen(tempStr, 64); i++)
		{
			width += titleFont->GetCharWidthA(tempStr[i]);
		}
		height = titleFont->GetCharHeight('D');
		glScalef(.003f, .003f, .003f);
		glTranslatef(-width/2.0f, 700, 0);	
		titleFont->Begin();
		titleFont->DrawString(tempStr, 0, 0);
		glPopMatrix();

		glPushMatrix();
		glRotatef(-90, 0, 0, 1);
		width = 0;
		height = 0;
		sprintf_s(tempStr, 64, "You are a Hero of Intergalactic Peace.");
		for(unsigned int i=0; i<strnlen(tempStr, 64); i++)
		{
			width += titleFont->GetCharWidthA(tempStr[i]);
		}
		glScalef(.0018f, .0018f, .0018f);
		glTranslatef(-width/2.0f, 950, 0);	
		titleFont->Begin();
		titleFont->DrawString(tempStr, 0, 0);
		glPopMatrix();

		glPushMatrix();
		glRotatef(-90, 0, 0, 1);
		width = 0;
		height = 0;
		sprintf_s(tempStr, 64, "Mark this historic triumph:");
		for(unsigned int i=0; i<strnlen(tempStr, 64); i++)
		{
			width += titleFont->GetCharWidthA(tempStr[i]);
		}
		glScalef(.0018f, .0018f, .0018f);
		glTranslatef(-width/2.0f, 850, 0);	
		titleFont->Begin();
		titleFont->DrawString(tempStr, 0, 0);
		glPopMatrix();

		//Draw the name string
		glPushMatrix();
		glRotatef(-90, 0, 0, 1);
		width = 0;
		height = 0;

		for(unsigned int i=0; i<strnlen(highScoreName, 64); i++)
		{
			width += titleFont->GetCharWidthA(highScoreName[i]);
		}
		glScalef(.0018f, .0018f, .0018f);
		glTranslatef(-width/2.0f, 250, 0);	
		glColor3f(1.0, 0.0, 0.0);
		titleFont->Begin();
		titleFont->DrawString(highScoreName, 0, 0);
		glTranslatef((float)width - titleFont->GetCharWidthA((char)currentLetter % 128), 0, 0);
		titleFont->DrawString("_", 0, 0);
		glPopMatrix();

		glPushMatrix();
		glRotatef(-90, 0, 0, 1);
		width = 0;
		height = 0;
		sprintf_s(tempStr, 64, "Missile 1 = BACKSPACE      Cannon = NEXT      Missile 2 = DONE");
		for(unsigned int i=0; i<strnlen(tempStr, 64); i++)
		{
			width += titleFont->GetCharWidthA(tempStr[i]);
		}
		glScalef(.001f, .001f, .001f);
		glTranslatef(-width/2.0f, -1600, 0);	
		titleFont->Begin();
		glColor3f(.8f, .8f, 1.0);
		titleFont->DrawString(tempStr, 0, 0);
		glPopMatrix();
	}
	else
	{
		hsList->Draw(true);
	}

	glDisable(GL_TEXTURE_2D);
}

void CHyperWarGame::SetHyperLevel(int newLevel)
{
	hyperLevel = newLevel;
	hyperModeTimer = 0;

	if(hyperLevel < 1)
		hyperLevel = 1;

	if(gameParams.gameMode == MODE_VS)
		pointMultiplier = hyperLevel;

	//switch(hyperLevel)
	switch(0)
	{
	case 0:
		//Play sound indicating new hyper level
		
		gameParams.chargeRateDivider = 500.0f;			//min 500
		gameParams.maxThrust = 1.0f;					//max 500
		gameParams.minThrust = 1.0f;
		//gameParams.maxThrust = 1.0f;					//max 500
		//gameParams.minThrust = 1.0f;
		gameParams.maxGravityForce = 10.0f;
		gameParams.nukeGravityImmunityTime = 1000;		//min 200
		gameParams.flakReloadTime = 10;	
		gameParams.hyperModeDelay = 60000;
		if(gameParams.gameMode == MODE_SINGLE)
		{
			gameParams.nukeSpeedDivider = 4000.0f;			//min 1000
			gameParams.nukeReloadTime = 200;				//min 150
		}
		else
		{
			gameParams.nukeSpeedDivider = 4000.0f;			//min 1000
			gameParams.nukeReloadTime = 200;				//min 150
			//audioRenderer.PlaySound(SOUND_LEVEL5, 0, 0);
		}
		break;
	case 1:
		//Play sound indicating new hyper level
		
		gameParams.chargeRateDivider = 5000.0f;			//min 500
		gameParams.maxThrust = 3.0f;					//max 500
		gameParams.minThrust = 0.3f;
		//gameParams.maxThrust = 1.0f;					//max 500
		//gameParams.minThrust = 1.0f;
		gameParams.maxGravityForce = 10.0f;
		gameParams.nukeGravityImmunityTime = 1000;		//min 200
		if(gameParams.gameMode == MODE_SINGLE)
		{
			gameParams.nukeSpeedDivider = 4000.0f;			//min 1000
			gameParams.nukeReloadTime = 200;				//min 150
		}
		else
		{
			gameParams.nukeSpeedDivider = 16000.0f;			//min 1000
			gameParams.nukeReloadTime = 4000;				//min 150
			//audioRenderer.PlaySound(SOUND_LEVEL1, 0, 0);
		}
		gameParams.flakReloadTime = 10;	
		gameParams.hyperModeDelay = 30000;
		break;
	case 2:
		//Play sound indicating new hyper level
		
		gameParams.chargeRateDivider = 3000.0f;			//min 500
		gameParams.maxThrust = 3.0f;					//max 500
		gameParams.minThrust = 0.3f;
		//gameParams.maxThrust = 1.0f;					//max 500
		//gameParams.minThrust = 1.0f;
		gameParams.maxGravityForce = 10.0f;
		gameParams.nukeGravityImmunityTime = 1000;		//min 200
		if(gameParams.gameMode == MODE_SINGLE)
		{
			gameParams.nukeSpeedDivider = 4000.0f;			//min 1000
			gameParams.nukeReloadTime = 200;				//min 150
		}
		else
		{
			gameParams.nukeSpeedDivider = 13000.0f;			//min 1000
			gameParams.nukeReloadTime = 2000;				//min 150
			//audioRenderer.PlaySound(SOUND_LEVEL2, 0, 0);
		}
		gameParams.flakReloadTime = 10;	
		gameParams.hyperModeDelay = 25000;
		break;
	case 3:
		//Play sound indicating new hyper level
		
		gameParams.chargeRateDivider = 2000.0f;			//min 500
		gameParams.maxThrust = 3.0f;					//max 500
		gameParams.minThrust = 0.3f;
		//gameParams.maxThrust = 1.0f;					//max 500
		//gameParams.minThrust = 1.0f;
		gameParams.maxGravityForce = 10.0f;
		gameParams.nukeGravityImmunityTime = 1000;		//min 200
		if(gameParams.gameMode == MODE_SINGLE)
		{
			gameParams.nukeSpeedDivider = 4000.0f;			//min 1000
			gameParams.nukeReloadTime = 200;				//min 150
		}
		else
		{
			gameParams.nukeSpeedDivider = 10000.0f;			//min 1000
			gameParams.nukeReloadTime = 1000;				//min 150
			//audioRenderer.PlaySound(SOUND_LEVEL3, 0, 0);
		}
		gameParams.flakReloadTime = 10;	
		gameParams.hyperModeDelay = 20000;
		break;
	case 4:
		//Play sound indicating new hyper level
		
		gameParams.chargeRateDivider = 1000.0f;			//min 500
		gameParams.maxThrust = 3.0f;					//max 500
		gameParams.minThrust = 0.3f;
		//gameParams.maxThrust = 1.0f;					//max 500
		//gameParams.minThrust = 1.0f;
		gameParams.maxGravityForce = 10.0f;
		gameParams.nukeGravityImmunityTime = 1000;		//min 200
		if(gameParams.gameMode == MODE_SINGLE)
		{
			gameParams.nukeSpeedDivider = 4000.0f;			//min 1000
			gameParams.nukeReloadTime = 200;				//min 150
		}
		else
		{
			gameParams.nukeSpeedDivider = 7000.0f;			//min 1000
			gameParams.nukeReloadTime = 500;				//min 150
			//audioRenderer.PlaySound(SOUND_LEVEL4, 0, 0);
		}
		gameParams.flakReloadTime = 10;	
		gameParams.hyperModeDelay = 15000;
		break;
	case 5:
		//Play sound indicating new hyper level
		
		gameParams.chargeRateDivider = 500.0f;			//min 500
		gameParams.maxThrust = 3.0f;					//max 500
		gameParams.minThrust = 0.3f;
		//gameParams.maxThrust = 1.0f;					//max 500
		//gameParams.minThrust = 1.0f;
		gameParams.maxGravityForce = 10.0f;
		gameParams.nukeGravityImmunityTime = 1000;		//min 200
		gameParams.flakReloadTime = 10;	
		gameParams.hyperModeDelay = 60000;
		if(gameParams.gameMode == MODE_SINGLE)
		{
			gameParams.nukeSpeedDivider = 4000.0f;			//min 1000
			gameParams.nukeReloadTime = 200;				//min 150
		}
		else
		{
			gameParams.nukeSpeedDivider = 4000.0f;			//min 1000
			gameParams.nukeReloadTime = 200;				//min 150
			//audioRenderer.PlaySound(SOUND_LEVEL5, 0, 0);
		}
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

	//pointMultiplier = (int)(totalWaves/4.0f + 1);

	if(totalWaves % 10 == 0)
	{
		SetHyperLevel(5);

		for(int i=0; i<totalWaves/5; i++)
		{
			//create a mothership
			moship = new CMothership(&gameParams);
			moship->SetColor(0, 0, .8f);
			moship->SetSide(SIDE_BLUE);
			moship->SetScale(.07f, .07f, .07f);
			moship->SetRotation(0, 0, -90);
			moship->SetMotionVector(0, .5f * gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX, 0);
			moship->SetTranslation(gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX + .5f, -1, 0);
			gameObjects.push_back(moship);
		}
	}
	else
	{
		for(int i=0; i<waveNumber/10 + 1 && i < 7; i++)
		{
			//Use layers for x location between 2.7 and 7.7

			nuke = new CNuke(&gameParams);
			nuke->SetColor(0, 0, .8f);
			nuke->SetScale(.1f, .1f, .1f);
			nuke->SetTranslation(2.75f + (i-1)%10 * .5f, (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX) * -1.7f, 0);
			nuke->SetMotionVector(gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX * -waveNumber/64.0f, (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f), 0);
			nuke->SetThrust(.025f * waveNumber);
			nuke->SetSide(SIDE_BLUE);
			gameObjects.push_back(nuke);

			nuke = new CNuke(&gameParams);
			nuke->SetColor(0, 0, .8f);
			nuke->SetScale(.1f, .1f, .1f);
			nuke->SetTranslation(2.75f + (i-1)%10 * .5f, (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX) * 1.7f, 0);
			nuke->SetMotionVector(gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX * -waveNumber/64.0f, (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f), 0);
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
			moship->SetMotionVector(0, .5f * gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX, 0);
			moship->SetTranslation(gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX + .5f, -1, 0);
			gameObjects.push_back(moship);
		}

		//Random gravity wells
		if(totalWaves % 5 == 0 && totalWaves > 15)
		{
			//Add a random gravity well
			sGravityWell *gw = new sGravityWell();
			gw->mass = gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX * 5;
			gw->translation[0] = (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f) * 2.5f;
			gw->translation[1] = (gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f) * 1.5f;
			gw->translation[2] = 0;
			gravityWells.push_back(gw);
			
			CBlackHole *hole = new CBlackHole(&gameParams);
			hole->SetColor(0, 0, .8f);
			hole->SetScale(.1f, .1f, .1f);
			hole->SetTranslation(gw->translation[0], gw->translation[1], gw->translation[2]);
			hole->SetMotionVector(gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX / 10.0f,
				gameParams.randoms[gameParams.randIndex++%1024]/(float)RAND_MAX - .5f,
				0);
			hole->SetSide(SIDE_BLUE);
			hole->SetMyGravity(gw);

			gameObjects.push_back(hole);
		}

		if(totalWaves % 4 == 0 && gameParams.gameMode != MODE_SINGLE)
		{
			SetHyperLevel(GetHyperLevel() + 1);
		}
	}
}