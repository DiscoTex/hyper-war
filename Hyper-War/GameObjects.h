/*************************************************************/
/*	GameObjects.h: definitions of game objects, including	 */
/*  the base class, CGameObject and several derived classes	 */					
/*  Aaron E. Wegner											 */
/*	July 2009												 */
/*************************************************************/

#include <windows.h>											// Header File For Windows
#include <gl/glew.h>
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library
#include "glBase.h"												// Header File For NeHeGL
#include "joystick.h"

#pragma comment( lib, "opengl32.lib" )							// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )								// Search For GLu32.lib While Linking

const float DEG2RAD = 3.1415f/180;

//#define COLLISION_DEBUG 1
#define PC_CONTROLS 1
#define NUM_PRIMES 1117

using namespace std;

class CTheFlag;

enum{
	SIDE_NONE,
	SIDE_BLUE,
	SIDE_GREEN
};

enum{
	TYPE_GENERIC,
	TYPE_PLANET,
	TYPE_NUKE,
	TYPE_DEBRIS,
	TYPE_MISSILEBASE,
	TYPE_CITY,
	TYPE_FLAKCANNON,
	TYPE_PROJECTILE,
	TYPE_MOSHIP,
	TYPE_BEAM,
	TYPE_BLACKHOLE,
	TYPE_SHIP,
	TYPE_BULLET,
	TYPE_BARRIER,
	TYPE_FLAG
};

enum{
	MODE_VS,
	MODE_SINGLE,
	MODE_COOP,
	MODE_RACE,
	MODE_ATTRACT,
	MODE_GAMEOVER,
	MODE_MELEE,
	MODE_HIGHSCORE
};

enum{
	DEBRIS_TYPE_0,
	DEBRIS_TYPE_1,
	DEBRIS_TYPE_2,
	DEBRIS_TYPE_3
};

struct sGameParams
{
	//Speed info
	float chargeRateDivider;
	float maxThrust;
	float minThrust;
	float maxGravityForce;
	int nukeGravityImmunityTime;
	float nukeSpeedDivider;
	int nukeReloadTime;
	int flakReloadTime;
	int gameMode;
	int debrisAmount;
	int flakDebrisFactor;
	int hyperModeDelay;
	int	waveTime;
	int mouse1Index;
	int mouse2Index;
	int randoms[NUM_PRIMES];
	unsigned long int	randIndex;
	int	greenPoints;
	int	bluePoints;
	float flakVelocityDivider;
	int numGreenCities;
	int numBlueCities;
	int blueSuperFires;
	int numGreenLaunchers;
	int numBlueLaunchers;
	int greenSuperFires;
	int blueSuperAmmo;
	int	greenSuperAmmo;
	bool greenShip;
	bool blueShip;
	int  greenRespawnCountdown;
	int  blueRespawnCountdown;
	int greenShipLives{ 10 };
	int blueShipLives{ 10 };
	bool rotated;
};

struct sGravityWell
{
	float translation[3];
	float mass;
};

struct sCollisionSphere
{
	float radius;
	float translation[3];
	float globalPosition[3];
};


class CGameObject
{
public:
	CGameObject(sGameParams *newGameParams);
	~CGameObject();

	virtual void Draw();

	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetTranslation(float x, float y, float z);
	void SetColor(float r, float g, float b);
	void SetMotionVector(float x, float y, float z);
	void SetAngularVelocity(float x, float y, float z);
	void SetSide(int newSide) {mySide = newSide;}
	int  GetSide() {return mySide;}

	float* GetScale();
	float* GetRotation();
	float* GetTranslation();
	float* GetColor();
	virtual float* GetMotionVector();
	float* GetAngularVelocity();

	vector < sCollisionSphere * > GetCollisionSpheres() { return collisionSpheres;}
	float* GetPosition() {return translation;}

	virtual void ProcessMotion(DWORD milliseconds, Keys* keys);
	virtual void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells);
	virtual int  CheckCollision(vector< CGameObject* > gObjects, DWORD milliseconds, unsigned int checkAfterIndex);
	virtual int	 GetType();
	virtual bool CanDestroy(int destroyerType);

	bool deleteMe;
protected:
	float motionVector[3];
	float angularVelocity[3];
	float scale[3];
	float rotation[3];
	float translation[3];
	float color[3];
	int   mySide;
	sGameParams *gameParams;	

	vector < sCollisionSphere* > collisionSpheres;
};

class CPlanet : public CGameObject
{
public:
	CPlanet(sGameParams *newGameParams);
	~CPlanet() {}

	void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells);
	int  CheckCollision(vector< CGameObject* > gObjects) {return -1;}
	int	 GetType();
	bool CanDestroy(int destroyerType);

	void Draw();
private:
};

class CNuke : public CGameObject
{
public:
	CNuke(sGameParams *newGameParams);
	~CNuke() {}

	void ProcessMotion(DWORD milliseconds, Keys * keys);
	int	 GetType();
	void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells);

	void SetThrust(float newThrust) {thrust = newThrust;}
	float* GetMotionVector();

	void Draw();
private:
	float				thrust;
	unsigned short		animVal;
	float				flameColor[3];
	DWORD				lastLaunch;
	float				tempVec[3];
	int					gravityImmunity;
	int					TTL{ 15000 };
};

class CDebris : public CGameObject
{
public:
	CDebris(sGameParams *newGameParams);
	~CDebris();

	void ProcessMotion(DWORD milliseconds, Keys * keys);
	int GetType();
	bool CanDestroy(int destroyerType);
	int	 GetTTL() {return TTL;}
	void SetTTL(int newTTL) {TTL = newTTL;}

	void Draw();

private:
	int debType;
	int	TTL;
};

class CMissileBase : public CGameObject
{
public:
	CMissileBase(sGameParams *newGameParams);
	~CMissileBase();

	void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells);
	bool CanDestroy(int destroyerType);
	int	 GetType();
	void SetLaunchKey(char key) {launchKey = key;}
	char GetLaunchKey() {return launchKey;}
	bool IsLoaded() {return loaded && !destroyed;}
	int  Launch();
	bool LaunchReady();
	void AddCharge(DWORD milliseconds) {charge += milliseconds; launchReady = true;}
	float* GetNukeTranslation();
	float* GetNukeVector();
	void  SetCursorPointer(float* newPCursorPos) {pCursorPos = newPCursorPos;}
	void Draw();	
	bool GetFiredLast() { return firedLast; }
	void SetFiredLast(bool state) { firedLast = state; }

private:
	bool loaded;
	char launchKey;
	bool launchReady;
	int  charge;
	int	 timeToReload;
	float*	pCursorPos;
	float rotatedNuke[3];
	float nukeVector[3];
	bool destroyed;
	int	 timeToRebuild;
	int hitPoints;
	bool   firedLast;
};

class CCity : public CGameObject
{
public:
	CCity(sGameParams *newGameParams);
	~CCity();

	void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells);
	bool CanDestroy(int destroyerType);
	int	 GetType();

	void Draw();

private:

	int hitPoints;
};

class CFlakCannon : public CGameObject
{
public:
	CFlakCannon(sGameParams *newGameParams);
	~CFlakCannon();

	void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells);
	bool CanDestroy(int destroyerType);
	void SetCursorPointer(float* newPCursorPos) {pCursorPos = newPCursorPos;}
	void SetFireKey(char key) {fireKey = key;}
	char GetFireKey() {return fireKey;}
	void SetSingularityKey(char key) {singularityKey = key;}
	char GetSingularityKey() {return singularityKey;}
	void SetBeamKey(char key) {beamKey = key;}
	char GetBeamKey() {return beamKey;}
	int	 GetType();
	bool IsLoaded() 
	{
		return loaded && !destroyed;
	}
	void Fire();
	void GetProjVector(int* TTL, float* projVector);
	float GetProjAngle();
	float* GetProjTranslation();
	void AddTimeSinceFired(DWORD milliseconds);
	bool IsDisabled() {return destroyed;}
	bool HasSuperWeapon();
	void FireSuperWeapon();
	void SetCharge(int newCharge) {charge = newCharge;}
	int  GetCharge() {return charge;}
	void SetReady(bool newReady) {ready = newReady;}
	bool GetReady() {return ready;}

	void Draw();
private:
	float* pCursorPos;
	char   fireKey;
	char   singularityKey;
	char   beamKey;
	bool   loaded;
	int	   timeToReload;
	float  projTranslation[3];
	bool   destroyed;
	int    timeToRebuild;
	int    hitPoints;	
	int	   charge;
	bool   ready;
	//int    superWeaponsFired;
};

class CProjectile : public CGameObject
{
public:
	CProjectile(sGameParams *newGameParams);
	~CProjectile();

	void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells) {}
	int	 GetType() {return TYPE_PROJECTILE;}

	void SetOrigin(float x, float y, float z) {origin[0] = x; origin[1] = y; origin[2] = z;}
	void SubtractTTL(DWORD milliseconds) {timeToLive -= milliseconds;}
	int	 GetTTL() {return timeToLive;}
	void SetTTL(int newTTL) {timeToLive = newTTL;}

	void Draw();
private:
	float origin[3];
	int		timeToLive;
};

class CMothership;

class CBeam : public CGameObject
{
public:
	CBeam(sGameParams *newGameParams);
	~CBeam();

	void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells) {}
	int	 GetType() {return TYPE_BEAM;}
	bool CanDestroy(int destroyerType) {return false;}
	void AddCount(int milliseconds) {TTL -= milliseconds;}
	long int GetTTL() {return TTL;}
	void Kill() {TTL = -1;}
	void SetMyParent(CMothership* myNewParent) {myParent = myNewParent;}

	void Draw();

	CMothership* myParent;
private:
	long int animVal;
	long int TTL;	
};

class CMothership : public CGameObject
{
public:
	CMothership(sGameParams *newGameParams);
	~CMothership();

	void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells) {}
	int	 GetType() {return TYPE_MOSHIP;}
	bool CanDestroy(int destroyerType);
	void AddCount(int milliseconds) {countDown -= milliseconds;}
	bool IsFiring();
	void SetMyBeam(CBeam *myNewBeam) {myBeam = myNewBeam;}

	void Draw();

private:
	int countDown;
	int hitPoints;
	CBeam* myBeam;
};

class CBlackHole : public CGameObject
{
public:
	CBlackHole(sGameParams *newGameParams);
	~CBlackHole();

	void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells) {}
	int	 GetType() {return TYPE_BLACKHOLE;}
	bool CanDestroy(int destroyerType);
	void SetMyGravity(sGravityWell *newGravity) {myGravity = newGravity;}
	int  GetTTL() {return TTL;}
	void  SetTTL(int newTTL) {TTL = newTTL;}
	void KillGravity() {myGravity->mass = 0;}

	void ProcessMotion(DWORD milliseconds, Keys* keys);

	void Draw();

private:
	int TTL;
	int animVal;
	sGravityWell *myGravity;
};

class CShip : public CGameObject
{
public:
	CShip(sGameParams *newGameParams);
	~CShip();

	void ProcessMotion(DWORD milliseconds, Keys * keys);
	void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells);

	int	 GetType() {return TYPE_SHIP;}
	bool CanDestroy(int destroyerType);
	void SetJoyState( DIJOYSTATE2 *newJoystate ) { joystate = newJoystate; }
	bool IsFiring();

	bool IsFiringMissile();
	bool IsFiringSingularity();
	bool IsFiringBeam();

	void setLanded(bool newLanded) { landed = newLanded; }
	void setFlag(CTheFlag* theFlag) { ownedFlag = theFlag; }	
	bool hasFlag();
	void dropFlag();

	void Draw();

private:
	DIJOYSTATE2 *joystate;
	unsigned short		animVal;
	float				flameColor[3];
	int					timeSinceLastFire;
	int					hitPoints;
	bool				landed{ false };
	int					landingCounter{ 0 };
	CTheFlag*			ownedFlag{ nullptr };

};

class CBullet : public CGameObject
{
public:
	CBullet(sGameParams *newGameParams);
	~CBullet();

	int GetType() { return TYPE_BULLET; }
	bool CanDestroy(int destroyerType) { return true; }
	void ProcessMotion(DWORD milliseconds, Keys* keys);

	void Draw();

private:
	int	TTL;
};

class CBarrier : public CGameObject
{
public:
	CBarrier(sGameParams *newGameParams);
	~CBarrier();
	
	int	 GetType() { return TYPE_BARRIER; }
	bool CanDestroy(int destroyerType);
	void SetMyGravity(sGravityWell *newGravity) { myGravity = newGravity; }
	int  GetTTL() { return TTL; }
	void  SetTTL(int newTTL) { TTL = newTTL; }
	void KillGravity() { myGravity->mass = 0; }
	void SetScale(float x, float y, float z);

	void ProcessMotion(DWORD milliseconds, Keys* keys);

	void Draw();

private:
	int TTL;
	int animVal;
	sGravityWell *myGravity;
};

class CTheFlag : public CGameObject
{
public:
	CTheFlag(sGameParams *newGameParams);
	~CTheFlag(); 

	int	 GetType() { return TYPE_FLAG; }
	bool CanDestroy(int destroyerType);

	void ProcessMotion(DWORD milliseconds, Keys* keys);
	void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells);

	void setResting(bool resting) { atRest = resting; }

	void returnHome();

	void Draw();

private:
	int animVal{ 0 };
	bool atRest{ true };
	bool captured{ false };
};