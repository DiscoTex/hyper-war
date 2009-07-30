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

const double DEG2RAD = 3.14159/180;
#define MAX_GRAVITY_FORCE 10

using namespace std;

enum{
	TYPE_GENERIC,
	TYPE_PLANET,
	TYPE_NUKE,
	TYPE_DEBRIS
};

struct sGravityWell
{
	double translation[3];
	double mass;
};

struct sCollisionSphere
{
	double radius;
	double translation[3];
	double globalPosition[3];

};


class CGameObject
{
public:
	CGameObject();
	~CGameObject();

	virtual void Draw();

	void SetScale(double x, double y, double z);
	void SetRotation(double x, double y, double z);
	void SetTranslation(double x, double y, double z);
	void SetColor(double r, double g, double b);
	void SetMotionVector(double x, double y, double z);
	void SetAngularVelocity(double x, double y, double z);

	double* GetScale(double& x, double& y, double& z);
	double* GetRotation(double &x, double &y,double &z);
	double* GetTranslation(double &x, double &y, double &z);
	double* GetColor(double &r, double &g, double &b);
	double* GetMotionVector(double &x, double &y, double &z);
	double* GetAngularVelocity(double &x, double &y, double &z);

	vector < sCollisionSphere * > GetCollisionSpheres() { return collisionSpheres;}
	double*	GetMotionVector() {return motionVector;}
	double* GetPosition() {return translation;}

	virtual void ProcessMotion(DWORD milliseconds);
	virtual void ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells);
	virtual int  CheckCollision(vector< CGameObject* > gObjects, DWORD milliseconds);
	virtual int	 GetType();

protected:
	double motionVector[3];
	double angularVelocity[3];

	double scale[3];
	double rotation[3];
	double translation[3];
	double color[3];

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

	void Draw();
private:
};

class CNuke : public CGameObject
{
public:
	CNuke();
	~CNuke() {}

	void ProcessMotion(DWORD milliseconds);
	int	 GetType();

	void Draw();
private:
};