/*************************************************************/
/*	GameObjects.h: definitions of game objects, including	 */
/*  the base class, CGameObject and several derived classes	 */					
/*  Aaron E. Wegner											 */
/*	July 2009												 */
/*************************************************************/

#include <windows.h>											// Header File For Windows
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library
#include "glBase.h"												// Header File For NeHeGL

#pragma comment( lib, "opengl32.lib" )							// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )								// Search For GLu32.lib While Linking

const float DEG2RAD = 3.1415f/180;
#define MAX_GRAVITY_FORCE 10

//#define COLLISION_DEBUG 1

using namespace std;

enum{
	TYPE_GENERIC,
	TYPE_PLANET,
	TYPE_NUKE,
	TYPE_DEBRIS,
	TYPE_MISSILEBASE,
	TYPE_CITY,
	TYPE_FLAKCANNON
};

enum{
	DEBRIS_TYPE_0,
	DEBRIS_TYPE_1,
	DEBRIS_TYPE_2,
	DEBRIS_TYPE_3
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
	CGameObject();
	~CGameObject();

	virtual void Draw();

	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetTranslation(float x, float y, float z);
	void SetColor(float r, float g, float b);
	void SetMotionVector(float x, float y, float z);
	void SetAngularVelocity(float x, float y, float z);

	float* GetScale();
	float* GetRotation();
	float* GetTranslation();
	float* GetColor();
	float* GetMotionVector();
	float* GetAngularVelocity();

	vector < sCollisionSphere * > GetCollisionSpheres() { return collisionSpheres;}
	float* GetPosition() {return translation;}

	virtual void ProcessMotion(DWORD milliseconds, bool keys[]);
	virtual void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells);
	virtual int  CheckCollision(vector< CGameObject* > gObjects, DWORD milliseconds, unsigned int checkAfterIndex);
	virtual int	 GetType();
	virtual bool CanDestroy(int destroyerType);

protected:
	float motionVector[3];
	float angularVelocity[3];

	float scale[3];
	float rotation[3];
	float translation[3];
	float color[3];

	vector < sCollisionSphere* > collisionSpheres;
};

class CPlanet : public CGameObject
{
public:
	CPlanet();
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
	CNuke();
	~CNuke() {}

	void ProcessMotion(DWORD milliseconds, bool keys[]);
	int	 GetType();

	void Draw();
private:
	float thrust;
	float flameColor[3];
};

class CDebris : public CGameObject
{
public:
	CDebris();
	~CDebris();

	void ProcessMotion(DWORD milliseconds, bool keys[]);
	int GetType();
	bool CanDestroy(int destroyerType);

	void Draw();

private:
	int debType;
};

class CMissileBase : public CGameObject
{
public:
	CMissileBase();
	~CMissileBase();

	void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells);
	bool CanDestroy(int destroyerType);
	int	 GetType();

	void Draw();

private:
	bool loaded;

};

class CCity : public CGameObject
{
public:
	CCity();
	~CCity();

	void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells);
	bool CanDestroy(int destroyerType);
	int	 GetType();

	void Draw();

private:


};

class CFlakCannon : public CGameObject
{
public:
	CFlakCannon();
	~CFlakCannon();

	void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells);
	bool CanDestroy(int destroyerType);
	int	 GetType();

	void Draw();
};