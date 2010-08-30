/*************************************************************/
/*	GameObjects.cpp: definitions of game objects, including	 */
/*  the base class, CGameObject and several derived classes	 */					
/*  Aaron E. Wegner											 */
/*	July 2009												 */
/*************************************************************/

#include "GameObjects.h"

//Generic Game Object
CGameObject::CGameObject(sGameParams *newGameParams)
{
	mySide = SIDE_NONE;

	for(int i=0; i<3; i++)
	{
		scale[i] = 1;
		rotation[i] = 0;
		translation[i] = 0;
		motionVector[i] = 0;
		angularVelocity[i] = 0;
	}	

	color[0] = 0;
	color[1] = 0;
	color[2] = 1;

	gameParams = newGameParams;	
	deleteMe = false;
}

CGameObject::~CGameObject()
{
	for(unsigned int i=0; i<collisionSpheres.size(); i++)
	{
		delete collisionSpheres[i];
	}
}

void CGameObject::Draw()
{
	glPushMatrix();

	glTranslatef(translation[0], translation[1], translation[2]);
	glRotatef(rotation[0], 1, 0, 0);
	glRotatef(rotation[1], 0, 1, 0);
	glRotatef(rotation[2], 0, 0, 1);
	glScalef(scale[0], scale[1], scale[2]);
	
	glBegin (GL_QUADS);								
		glColor3f (color[0], color[1], color[2]);	glVertex3f( -1.0f, 1.0f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f(1.0f, 1.0f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( 1.0f,-1.0f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( -1.0f,-1.0f, 0.0f);
	glEnd ();	

	glBegin(GL_LINE_LOOP);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( -1.0f, 1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f(1.0f, 1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 1.0f,-1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( -1.0f,-1.0f, 0.0f);
	glEnd();

	glPopMatrix();
}

void CGameObject::SetScale(float x, float y, float z)
{
	scale[0] = x;
	scale[1] = y;
	scale[2] = z;
}

void CGameObject::SetRotation(float x, float y, float z)
{
	rotation[0] = x;
	rotation[1] = y;
	rotation[2] = z;
}

void CGameObject::SetTranslation(float x, float y, float z)
{
	translation[0] = x;
	translation[1] = y;
	translation[2] = z;
}

void CGameObject::SetColor(float r, float g, float b)
{
	color[0] = r;
	color[1] = g;
	color[2] = b;
}

void CGameObject::SetMotionVector(float x, float y, float z)
{
	motionVector[0] = x;
	motionVector[1] = y;
	motionVector[2] = z;
}

void CGameObject::SetAngularVelocity(float x, float y, float z)
{
	angularVelocity[0] = x;
	angularVelocity[1] = y;
	angularVelocity[2] = z;
}

float* CGameObject::GetScale()
{
	return scale;
}

float* CGameObject::GetRotation()
{
	return rotation;
}

float* CGameObject::GetTranslation()
{
	return translation;
}

float* CGameObject::GetColor()
{
	return color;
}

float* CGameObject::GetMotionVector()
{
	return motionVector;
}

float* CGameObject::GetAngularVelocity()
{
	return angularVelocity;
}

void CGameObject::ProcessMotion(DWORD milliseconds, Keys* keys)
{
	//Add motion based on motion vector and elapsed time
	translation[0] += milliseconds * motionVector[0] / 1000;
	translation[1] += milliseconds * motionVector[1] / 1000;
	translation[2] += milliseconds * motionVector[2] / 1000;

	//Rotate based on angular velocity and elapse time
	rotation[0] += milliseconds * angularVelocity[0] / 1000;
	rotation[1] += milliseconds * angularVelocity[1] / 1000;
	rotation[2] += milliseconds * angularVelocity[2] / 1000;

	//Update collision sphere data
	//Update collision sphere locations
	for(unsigned int i = 0; i < collisionSpheres.size(); i++)
	{
		//First, rotate the sphere
		collisionSpheres[i]->globalPosition[0] = cos(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[0] + sin(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[1];
		collisionSpheres[i]->globalPosition[1] = -sin(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[0] + cos(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[1];
		collisionSpheres[i]->globalPosition[2] = 0;

		//Scale by the object's scale
		collisionSpheres[i]->globalPosition[0] *= scale[0];
		collisionSpheres[i]->globalPosition[1] *= scale[1];
		collisionSpheres[i]->globalPosition[2] *= scale[2];

		//Translate by the object's translation
		collisionSpheres[i]->globalPosition[0] += translation[0];
		collisionSpheres[i]->globalPosition[1] += translation[1];
		collisionSpheres[i]->globalPosition[2] += translation[2];
	}

	if(GetType() !=  TYPE_PLANET)
	{
		if(translation[1] > 1.70f)
			translation[1] = -1.70f;
		else if(translation[1] < -1.70f)
			translation[1] = 1.70f;

		if(translation[0] > 2.75f || translation[0] < -2.75f)
			deleteMe = true;
		//	translation[0] = -2.75f;
		//else if(translation[0] < -2.75f)
		//	translation[0] = 2.75f;
	}

}

void CGameObject::ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells)
{
	float vectorToGravity[3];
	float forceVector[3];
	float distance;

	for(unsigned int i=0; i<gWells.size(); i++)
	{
		//Determine vector from this object to gravity well
		vectorToGravity[0] = gWells[i]->translation[0] - translation[0];
		vectorToGravity[1] = gWells[i]->translation[1] - translation[1];
		vectorToGravity[2] = gWells[i]->translation[2] - translation[2];
		
		//Find distance to gravity well
		distance = sqrt(pow(vectorToGravity[0], 2) + pow(vectorToGravity[1], 2) + pow(vectorToGravity[2], 2));
		
		//Make vectorToGravity a unit vector
		vectorToGravity[0] /= distance;
		vectorToGravity[1] /= distance;
		vectorToGravity[2] /= distance;
		
		//Compute force vector due to gravity
		forceVector[0] = vectorToGravity[0] * (gWells[i]->mass / pow(distance, 2));
		forceVector[1] = vectorToGravity[1] * (gWells[i]->mass / pow(distance, 2));
		forceVector[2] = vectorToGravity[2] * (gWells[i]->mass / pow(distance, 2));

		if(forceVector[0] > gameParams->maxGravityForce)
			forceVector[0] = gameParams->maxGravityForce;
		if(forceVector[1] > gameParams->maxGravityForce)
			forceVector[1] = gameParams->maxGravityForce;
		if(forceVector[2] > gameParams->maxGravityForce)
			forceVector[2] = gameParams->maxGravityForce;

		if(forceVector[0] < -gameParams->maxGravityForce)
			forceVector[0] = -gameParams->maxGravityForce;
		if(forceVector[1] < -gameParams->maxGravityForce)
			forceVector[1] = -gameParams->maxGravityForce;
		if(forceVector[2] < -gameParams->maxGravityForce)
			forceVector[2] = -gameParams->maxGravityForce;

		//Add force vector to this object's motionVector
		motionVector[0] += forceVector[0] * milliseconds / 1000;
		motionVector[1] += forceVector[1] * milliseconds / 1000;
		//motionVector[2] += forceVector[2] * milliseconds / 1000;
	}
}

//Return the index of collided object
int CGameObject::CheckCollision(vector< CGameObject* > gObjects, DWORD milliseconds, unsigned int checkAfterIndex)
{
	//float relVelVector[3];
	float relPosVector[3];
	int thisType, otherType;


	//Check for intersection with all other objects in the game
	for(unsigned int i=checkAfterIndex; i<gObjects.size(); i++)
	{
		thisType = GetType();
		otherType = gObjects[i]->GetType();

		//Decide here whether or not two types of objects can collide at all.
		//Remember we can only detect one collision per object at a time
		switch(thisType)
		{
		case TYPE_DEBRIS:
			if(otherType == TYPE_DEBRIS || otherType == TYPE_MISSILEBASE || otherType == TYPE_CITY || otherType == TYPE_FLAKCANNON 
				|| otherType == TYPE_MOSHIP || otherType == TYPE_BEAM || otherType == TYPE_PROJECTILE || otherType == TYPE_SHIP || TYPE_BULLET)
				continue;
			break;
		case TYPE_PLANET:
			if(otherType == TYPE_PLANET || otherType == TYPE_MISSILEBASE || otherType ==  TYPE_CITY || otherType == TYPE_FLAKCANNON 
				|| otherType == TYPE_MOSHIP || otherType == TYPE_BEAM || otherType == TYPE_PROJECTILE)
				continue;
			break;
		case TYPE_MISSILEBASE:
			if(otherType == TYPE_PLANET || otherType ==  TYPE_DEBRIS || otherType == TYPE_MOSHIP || otherType == TYPE_PROJECTILE || otherType == TYPE_SHIP)
				continue;
		case TYPE_CITY:
			if(otherType == TYPE_PLANET || otherType ==  TYPE_DEBRIS || otherType == TYPE_MOSHIP || otherType == TYPE_PROJECTILE || otherType == TYPE_SHIP)
				continue;
			break;
		case TYPE_FLAKCANNON:
			if(otherType == TYPE_PLANET || otherType ==  TYPE_DEBRIS || otherType == TYPE_MOSHIP || otherType == TYPE_PROJECTILE || otherType == TYPE_SHIP)
				continue;
			break;
		case TYPE_MOSHIP:
			if(gObjects[i]->GetSide() == mySide || otherType == TYPE_PLANET || otherType == TYPE_MISSILEBASE 
				|| otherType ==  TYPE_CITY || otherType == TYPE_FLAKCANNON || otherType == TYPE_MOSHIP || otherType == TYPE_PROJECTILE || otherType == TYPE_SHIP)
				continue;
			break;
		case TYPE_BEAM:
			if(otherType == TYPE_PLANET || otherType ==  TYPE_DEBRIS || otherType == TYPE_PROJECTILE)
				continue;
		case TYPE_SHIP:
			if(otherType == TYPE_DEBRIS || otherType == TYPE_MISSILEBASE || otherType == TYPE_CITY || otherType == TYPE_FLAKCANNON || otherType == TYPE_MOSHIP)
				continue;
		case TYPE_BULLET:
			if(otherType == TYPE_DEBRIS)
				continue;
			break;
		}

		if(gObjects[i] == this)
			continue;

		if(otherType == TYPE_MOSHIP && mySide == SIDE_BLUE)
			continue;
		
		for(unsigned int k=0; k<collisionSpheres.size(); k++)
		{
			for(unsigned int j=0; j<gObjects[i]->GetCollisionSpheres().size(); j++)
			{
				// Relative velocity
				//relVelVector[0] = (gObjects[i]->GetMotionVector()[0] - motionVector[0]);
				//relVelVector[1] = (gObjects[i]->GetMotionVector()[1] - motionVector[1]);
				//relVelVector[2] = (gObjects[i]->GetMotionVector()[2] - motionVector[2]);

				//Motion vector is in units per second
				//Assume we are getting at least 60 fps
				//relVelVector[0] /= 60;
				//relVelVector[1] /= 60;
				//relVelVector[2] /= 60;
				
				// Relative position
				float *gPos, *cPos;
				gPos = gObjects[i]->GetCollisionSpheres()[j]->globalPosition;
				cPos = collisionSpheres[k]->globalPosition;

				relPosVector[0] = gPos[0] - cPos[0];
				relPosVector[1] = gPos[1] - cPos[1];
				relPosVector[2] = gPos[2] - cPos[2];

				/*
				relPosVector[0] = gObjects[i]->GetCollisionSpheres()[j]->globalPosition[0] - collisionSpheres[k]->globalPosition[0];
				relPosVector[1] = gObjects[i]->GetCollisionSpheres()[j]->globalPosition[1] - collisionSpheres[k]->globalPosition[1];
				relPosVector[2] = gObjects[i]->GetCollisionSpheres()[j]->globalPosition[2] - collisionSpheres[k]->globalPosition[2];
				*/

				//Minimal distance to avoid collision
				float r = collisionSpheres[k]->radius * scale[0] + gObjects[i]->GetCollisionSpheres()[j]->radius * gObjects[i]->GetScale()[0];

				//(Distance between the objects)^2 - (Minimal distance)^2
				//float pp = relPosVector[0] * relPosVector[0] + relPosVector[1] * relPosVector[1] + relPosVector[2] * relPosVector[2] - r*r;
				float pp = relPosVector[0] * relPosVector[0] + relPosVector[1] * relPosVector[1] - r*r;

				//If (pp < 0), the collision spheres are intersecting right now
				if ( pp < 0 )
					return i;

				/*
				//Now check to see if the spheres will intersect within 1/60th of a second
				//pv = relPosVector *dot* relVelVector
				float pv = relPosVector[0] * relVelVector[0] + relPosVector[1] * relVelVector[1] + relPosVector[2] * relVelVector[2];
				//Check if the spheres are moving away from each other
				if ( pv < 0 ) 
				{
					//vv = (relative velocity vector)^2
					float vv = relVelVector[0] * relVelVector[0] + relVelVector[1] * relVelVector[1] + relVelVector[2] * relVelVector[2];

					//(3)Check if the spheres can intersect within 1 frame
					if ( (pv + vv) <= 0 && (vv + 2 * pv + pp) >= 0 )
					{
						return -1;
					}
					else
					{
						//Discriminant/4
						float D    = pv * pv - pp * vv; 
						if ( D > 0 )
							return -1;

						// tmin = -dP*dV/dV*2 
						//the time when the distance between the spheres is minimal
						float tmin = -pv/vv;

						//Discriminant/(4*dV^2) = -(dp^2-r^2+dP*dV*tmin)
						if( pp + pv * tmin > 0 )
							return i;
					}
				}*/
			}
		}
	}

	return -1;
}

int CGameObject::GetType()
{
	return TYPE_GENERIC;
}

bool CGameObject::CanDestroy(int destroyerType)
{
	return true;
}


//CPlanet

CPlanet::CPlanet(sGameParams *newGameParams) : CGameObject(newGameParams)
{
	//Create collision spheres
	sCollisionSphere* cSphere = new sCollisionSphere;

	cSphere->translation[0] = 0;
	cSphere->translation[1] = 0;
	cSphere->translation[2] = 0;
	cSphere->radius = 1;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;

	collisionSpheres.push_back(cSphere);
}

void CPlanet::Draw()
{
	glPushMatrix();
	
	glTranslatef(translation[0], translation[1], translation[2]);
	glRotatef(rotation[0], 1, 0, 0);
	glRotatef(rotation[1], 0, 1, 0);
	glRotatef(rotation[2], 0, 0, 1);
	glScalef(scale[0], scale[1], scale[2]);
	glColor3f(color[0], color[1], color[2]);

	float radius = 1;
	glBegin(GL_TRIANGLES);
		for (int i=0; i<361; i+=4)
		{
			float degInRad = i*DEG2RAD;
			glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius, 0);

			degInRad = (i+4)*DEG2RAD;
			glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius, 0);

			glVertex3f(0, 0, 0);
		}
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
		for (int i=0; i<361; i+=4)
		{
			float degInRad = (i+4)*DEG2RAD;
			glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius, 0);
		}
	glEnd();

	glPopMatrix();
}

void CPlanet::ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells)
{
	//Do nothing.  Planets are immune to gravity in this universe
}

int CPlanet::GetType()
{
	return TYPE_PLANET;
}

bool CPlanet::CanDestroy(int destroyerType)
{
	return false;
}


// CNuke

CNuke::CNuke(sGameParams *newGameParams) : CGameObject(newGameParams)
{
	//Create collision spheres
	sCollisionSphere* cSphere = new sCollisionSphere;
	cSphere->translation[0] = .25;
	cSphere->translation[1] = 0;
	cSphere->translation[2] = 0;
	cSphere->radius = .2f;
	cSphere->globalPosition[0] = 10;
	cSphere->globalPosition[1] = 10;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	cSphere = new sCollisionSphere;
	cSphere->translation[0] = -.15f;
	cSphere->translation[1] = 0.0f;
	cSphere->translation[2] = 0.0f;
	cSphere->radius = .25;
	cSphere->globalPosition[0] = 10;
	cSphere->globalPosition[1] = 10;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	thrust = .3f;

	//Cycle colors
	flameColor[0] = 1.0f;
	flameColor[1] = (float)(gameParams->randoms[gameParams->randIndex++%512] / RAND_MAX);
	flameColor[2] = 0;

	animVal = 0;
	lastLaunch = 0;
	gravityImmunity = gameParams->nukeGravityImmunityTime;
}

void CNuke::Draw()
{
	float offset = 0;
	float radius;

	glPushMatrix();
	
	glTranslatef(translation[0], translation[1], translation[2]);
	glRotatef(rotation[0], 1, 0, 0);
	glRotatef(rotation[1], 0, 1, 0);
	glRotatef(rotation[2] - 90, 0, 0, 1);
	glScalef(scale[0], scale[1], scale[2]);

	glColor3f(color[0], color[1], color[2]);  

	//Draw one half of Nuke body
	radius = 1;
	offset = -cos(-30.0f*DEG2RAD);

	glBegin(GL_TRIANGLES);
	for (int i=-20; i<30; i+=10)
	{
		float degInRad = i*DEG2RAD;
		glVertex3f(cos(degInRad)*radius + offset, sin(degInRad)*radius, -.001f);

		degInRad = (i+10)*DEG2RAD;
		glVertex3f(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001f);

		glVertex3f(0, 0, -.001f);
	}
	glEnd();

	//Draw the other half of Nuke body
	offset = -cos(150.0f*DEG2RAD);
	glBegin(GL_TRIANGLES);
	for (int i=150; i<200; i+=10)
	{
		float degInRad = i*DEG2RAD;
		glVertex3f(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001f);

		degInRad = (i+10)*DEG2RAD;
		glVertex3f(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001f);

		glVertex3f(0, 0, -.001f);
	}
	glEnd();

	//Fill in bottom triangle
	glBegin(GL_TRIANGLES);
		glVertex3f(0,0,0);
		//glVertex3f(cos(200 * DEG2RAD)*radius - cos(150.0f*DEG2RAD), sin(200 * DEG2RAD)*radius, -.001f);
		//glVertex3f(cos(-20 * DEG2RAD)*radius - cos(-30.0f*DEG2RAD), sin(-20 * DEG2RAD)*radius, -.001f);
		glVertex3f(-0.073740899562835693f, -0.34192359447479248f, -.001f);
		glVertex3f(0.073662996292114258f, -0.34201046824455261f, -.001f);
	glEnd();

	//Make one fin
	offset = sin(200 * DEG2RAD);
	glBegin(GL_TRIANGLES);
	radius = .28f;
	for (int i=120; i<180; i+=15)
	{
		float degInRad = i*DEG2RAD;
		glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.01f);

		degInRad = (i+8)*DEG2RAD;
		glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.01f);

		glVertex3f(0, offset/2, -.01f);
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	//Make another fin
	for (int i=120; i<180; i+=15)
	{
		float degInRad = i*DEG2RAD;
		glVertex3f(-cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.01f);

		degInRad = (i+8)*DEG2RAD;
		glVertex3f(-cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.01f);

		glVertex3f(0, offset/2, -.01f);
	}
	glEnd();


	//**** Create outlines ****/

	glColor3f(1.0, 1.0, 1.0);

	//Draw one half of Nuke body
	radius = 1;
	offset = -cos(-30.0f*DEG2RAD);

	glBegin(GL_LINES);
	for (int i=-20; i<30; i+=10)
	{
		float degInRad = i*DEG2RAD;
		glVertex3f(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001f);

		degInRad = (i+10)*DEG2RAD;
		glVertex3f(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001f);

		//glVertex3f(0, 0, 0);
	}
	glEnd();

	//Draw the other half of Nuke body
	offset = -cos(150.0f*DEG2RAD);
	glBegin(GL_LINES);
	for (int i=150; i<200; i+=10)
	{
		float degInRad = i*DEG2RAD;
		glVertex3f(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001f);

		degInRad = (i+10)*DEG2RAD;
		glVertex3f(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001f);

		//glVertex3f(0, 0, 0);
	}
	glEnd();

	
	//Fill in bottom triangle
	glBegin(GL_LINES);
		//glVertex3f(0,0,0);
		//glVertex3f(cos(200 * DEG2RAD)*radius - cos(150.0f*DEG2RAD), sin(200 * DEG2RAD)*radius, -.00001f);
		//glVertex3f(cos(-20 * DEG2RAD)*radius - cos(-30.0f*DEG2RAD), sin(-20 * DEG2RAD)*radius, -.00001f);
		glVertex3f(-0.073740899562835693, -0.34192359447479248, -.00001f);
		glVertex3f(0.073662996292114258, -0.34201046824455261, -.00001f);
	glEnd();

	//Make one fin
	offset = sin(200 * DEG2RAD);
	glBegin(GL_LINES);
	radius = .28f;
	for (int i=120; i<180; i+=15)
	{
		float degInRad = i*DEG2RAD;
		glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.001f);

		degInRad = (i+8)*DEG2RAD;
		glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.001f);

		//glVertex3f(0, offset/2, 0);
	}
	glEnd();

	glBegin(GL_LINES);
		glVertex3f(0, offset/2, -0.005f);
		glVertex3f(cos(179*DEG2RAD)*radius,sin(179*DEG2RAD)*radius + offset, -0.005f);
	glEnd();

	glBegin(GL_LINES);
	//Make another fin
	for (int i=120; i<180; i+=15)
	{
		float degInRad = i*DEG2RAD;
		glVertex3f(-cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.001f);

		degInRad = (i+8)*DEG2RAD;
		glVertex3f(-cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.001f);

		//glVertex3f(0, offset/2, 0);
	}
	glEnd();

	glBegin(GL_LINES);
		glVertex3f(0, offset/2, -0.005f);
		glVertex3f(-cos(179*DEG2RAD)*radius,sin(179*DEG2RAD)*radius + offset, -0.005f);
	glEnd();

	//Make some rocket flames!
	//Cycle flame color
	flameColor[1] = flameColor[1] - .1f;
	if(flameColor[1] < 0)
		flameColor[1] += 1.0f;
	glColor3f(flameColor[0], flameColor[1], flameColor[2]);

	animVal++;
	//animVal %=2;

	//int flameSize = (int)((thrust + .3f) / .2f);
	int flameSize = (int)(thrust + .3f) * 2.0;

	if(animVal % 2 == 0)
	{
		glBegin(GL_LINE_LOOP);
			glVertex3f(-0.073740899562835693f, -0.34192359447479248f, -.00001f);
			
			glVertex3f(-0.04424f, -0.34192359447479248f * 4*flameSize/6.0f, -.00001f);
			glVertex3f(-0.01478f, -0.34192359447479248f, -.00001f);
			glVertex3f( 0.01468f, -0.34192359447479248f * 4*flameSize/6.0f, -.00001f);
			glVertex3f( 0.04414f,  -0.34192359447479248f, -.00001f);

			glVertex3f(0.073662996292114258f, -0.34201046824455261f, -.00001f);
		glEnd();
	}

	else if(animVal % 2 == 1)
	{	
		glBegin(GL_LINE_LOOP);
			glVertex3f(-0.073740899562835693f, -0.34192359447479248f, -.00001f);
			
			glVertex3f(-0.04424f, -0.34192359447479248f, -.00001f);
			glVertex3f(-0.01478f, -0.34192359447479248f * 4*flameSize/6.0f, -.00001f);
			glVertex3f(0.01468f, -0.34192359447479248f, -.00001f);
			glVertex3f( 0.04414f,  -0.34192359447479248f * 4*flameSize/6.0f, -.00001f);

			glVertex3f(0.073662996292114258, -0.34201046824455261, -.00001f);
		glEnd();
	}

	glPopMatrix();

#ifdef COLLISION_DEBUG
	glPushMatrix();

	//Draw collision spheres for debug
	for(unsigned int i=0; i<collisionSpheres.size(); i++)
	{
		radius = collisionSpheres[i]->radius * scale[1];
		glColor3f(1, 1, 0);
		glBegin(GL_LINE_LOOP);
			for (int j=0; j<360; j++)
			{
				float degInRad = j*DEG2RAD;
				glVertex3f(cos(degInRad)*radius + collisionSpheres[i]->globalPosition[0] ,sin(degInRad)*radius + collisionSpheres[i]->globalPosition[1], 0 + collisionSpheres[i]->globalPosition[2]);
			}
		glEnd();
	}

	glPopMatrix();
#endif
	
}

void CNuke::ProcessMotion(DWORD milliseconds, Keys * keys)
{
	//Rotate to point to the direction of motion
	rotation[2] = atan2(motionVector[1] , motionVector[0]) / DEG2RAD;

	//update speed due to thrust
	if(milliseconds > 0)
	{
		motionVector[0] += cos(rotation[2] * DEG2RAD) * (thrust / milliseconds);
		motionVector[1] += sin(rotation[2] * DEG2RAD) * (thrust / milliseconds);
	}

	//Add motion based on motion vector and elapsed time
	translation[0] += milliseconds * motionVector[0] / gameParams->nukeSpeedDivider;
	translation[1] += milliseconds * motionVector[1] / gameParams->nukeSpeedDivider;
	translation[2] += milliseconds * motionVector[2] / gameParams->nukeSpeedDivider;

	//Update collision sphere locations
	for(unsigned int i = 0; i < collisionSpheres.size(); i++)
	{
		//First, rotate the sphere
		//collisionSpheres[i]->globalPosition[0] = cos(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[0] + sin(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[1];
		collisionSpheres[i]->globalPosition[0] = cos(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[0] + sin(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[1];
		//collisionSpheres[i]->globalPosition[1] = -sin(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[0] + cos(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[1];
		collisionSpheres[i]->globalPosition[1] = -sin(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[0] + cos(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[1];
		collisionSpheres[i]->globalPosition[2] = 0;

		//Scale by the object's scale
		collisionSpheres[i]->globalPosition[0] *= scale[0];
		collisionSpheres[i]->globalPosition[1] *= scale[1];
		collisionSpheres[i]->globalPosition[2] *= scale[2];

		//Translate by the object's translation
		collisionSpheres[i]->globalPosition[0] += translation[0];
		collisionSpheres[i]->globalPosition[1] += translation[1];
		collisionSpheres[i]->globalPosition[2] += translation[2];
	}

	if(translation[1] > 1.70f)
		translation[1] = -1.70f;
	else if(translation[1] < -1.70f)
		translation[1] = 1.70f;

	if(translation[0] > 7.5f || translation[0] < -2.75f)
		deleteMe = true;
//		translation[0] = -2.75f;
//	else if(translation[0] < -2.75f)
//		translation[0] = 2.75f;

	if(gravityImmunity > 0)
		gravityImmunity -= milliseconds;
}

float *CNuke::GetMotionVector()
{
	tempVec[0] = motionVector[0] / (gameParams->nukeSpeedDivider / 1000);
	tempVec[1] = motionVector[1] / (gameParams->nukeSpeedDivider / 1000);
	tempVec[2] = motionVector[2] / (gameParams->nukeSpeedDivider / 1000);

	return tempVec;
}

int CNuke::GetType()
{
	return TYPE_NUKE;
}

void CNuke::ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells)
{
	float vectorToGravity[3];
	float forceVector[3];
	float distance;


	if(gravityImmunity <= 0)
	{
		//CGameObject::ProcessGravity(milliseconds, gWells);
		
		for(unsigned int i=0; i<gWells.size(); i++)
		{
			//Determine vector from this object to gravity well
			vectorToGravity[0] = gWells[i]->translation[0] - translation[0];
			vectorToGravity[1] = gWells[i]->translation[1] - translation[1];
			vectorToGravity[2] = gWells[i]->translation[2] - translation[2];
			
			//Find distance to gravity well
			distance = sqrt(pow(vectorToGravity[0], 2) + pow(vectorToGravity[1], 2) + pow(vectorToGravity[2], 2));
			
			//Make vectorToGravity a unit vector
			vectorToGravity[0] /= distance;
			vectorToGravity[1] /= distance;
			vectorToGravity[2] /= distance;
			
			//Compute force vector due to gravity
			forceVector[0] = vectorToGravity[0] * (gWells[i]->mass / pow(distance, 2));
			forceVector[1] = vectorToGravity[1] * (gWells[i]->mass / pow(distance, 2));
			forceVector[2] = vectorToGravity[2] * (gWells[i]->mass / pow(distance, 2));

			if(forceVector[0] > gameParams->maxGravityForce)
				forceVector[0] = gameParams->maxGravityForce;
			if(forceVector[1] > gameParams->maxGravityForce)
				forceVector[1] = gameParams->maxGravityForce;
			if(forceVector[2] > gameParams->maxGravityForce)
				forceVector[2] = gameParams->maxGravityForce;

			if(forceVector[0] < -gameParams->maxGravityForce)
				forceVector[0] = -gameParams->maxGravityForce;
			if(forceVector[1] < -gameParams->maxGravityForce)
				forceVector[1] = -gameParams->maxGravityForce;
			if(forceVector[2] < -gameParams->maxGravityForce)
				forceVector[2] = -gameParams->maxGravityForce;

			//Add force vector to this object's motionVector
			motionVector[0] += forceVector[0] * milliseconds / 1000;
			motionVector[1] += forceVector[1] * milliseconds / 1000;
			//motionVector[2] += forceVector[2] * milliseconds / 1000;
		}
		
	}
}

CDebris::CDebris(sGameParams *newGameParams) : CGameObject(newGameParams)
{
	//Pick a random debris type
	debType = gameParams->randoms[gameParams->randIndex++%512] * 4 / 32768;

	sCollisionSphere* cSphere;

	switch(debType)
	{
	case DEBRIS_TYPE_0:
		cSphere  = new sCollisionSphere;
		cSphere->translation[0] = -.05f;
		cSphere->translation[1] = 0.05f;
		cSphere->translation[2] = 0;
		cSphere->radius = .1f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);

		break;
	case DEBRIS_TYPE_1:
		cSphere  = new sCollisionSphere;
		cSphere->translation[0] = 0;
		cSphere->translation[1] = 0.1f;
		cSphere->translation[2] = 0;
		cSphere->radius = .1f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);

		break;
	case DEBRIS_TYPE_2:
		cSphere  = new sCollisionSphere;
		cSphere->translation[0] = 0;
		cSphere->translation[1] = 0.05f;
		cSphere->translation[2] = 0;
		cSphere->radius = .05f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);

		break;
	case DEBRIS_TYPE_3:
		cSphere  = new sCollisionSphere;
		cSphere->translation[0] = -.025f;
		cSphere->translation[1] = 0.025f;
		cSphere->translation[2] = 0;
		cSphere->radius = .05f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);
		break;
	}	

	color[0] = 1;
	color[1] = (float)gameParams->randoms[gameParams->randIndex++%512] / RAND_MAX;;
	color[2] = 0;

	angularVelocity[0] = float(gameParams->randoms[gameParams->randIndex++%512] % 500);
	angularVelocity[1] = float(gameParams->randoms[gameParams->randIndex++%512] % 500);
	angularVelocity[2] = float(gameParams->randoms[gameParams->randIndex++%512] % 500);

	rotation[2] = float(gameParams->randoms[gameParams->randIndex++%512] % 500);

	TTL = 1500 - gameParams->randoms[gameParams->randIndex++%512]%500;
}

CDebris::~CDebris()
{
}

int CDebris::GetType()
{
	return TYPE_DEBRIS;
}

void CDebris::Draw()
{
	glPushMatrix();
	
	glTranslatef(translation[0], translation[1], translation[2]);
	glRotatef(rotation[0], 1, 0, 0);
	glRotatef(rotation[1], 0, 1, 0);
	glRotatef(rotation[2], 0, 0, 1);
	glScalef(scale[0], scale[1], scale[2]);
	
	//Cycle colors
	color[1] = color[1] - .1f;
	if(color[1] < 0)
		color[1] += 1.0f;
	glColor3f(color[0], color[1], color[2]);

	switch(debType)
	{
	case DEBRIS_TYPE_0:
		//L-shaped debris
		glBegin(GL_LINE_STRIP);
			glVertex3f(-.25f, 0, -.25f);
			glVertex3f(0,0,0);
			glVertex3f(0,.25f,0);
		glEnd();
		break;
	case DEBRIS_TYPE_1:
		glBegin(GL_LINE_STRIP);
			glVertex3f(-.1f, 0, .25f);
			glVertex3f(0,0,0);
			glVertex3f(0,.25f,0);
		glEnd();
		break;
	case DEBRIS_TYPE_2:
		glBegin(GL_LINE_STRIP);
			glVertex3f(-.25f, 0, .25f);
			glVertex3f(0,0,0);
			glVertex3f(0,.1f,0);
		glEnd();
		break;
	case DEBRIS_TYPE_3:
		glBegin(GL_LINE_STRIP);
			glVertex3f(-.1f, 0, -.1f);
			glVertex3f(0,0,0);
			glVertex3f(0,.1f,0);
		glEnd();
		break;
	default:
		break;
	}

	glPopMatrix();

#ifdef COLLISION_DEBUG
	glPushMatrix();

	//Draw collision spheres for debug
	float radius;
	for(unsigned int i=0; i<collisionSpheres.size(); i++)
	{
		radius = collisionSpheres[i]->radius * scale[1];
		glColor3f(1, 1, 0);
		glBegin(GL_LINE_LOOP);
			for (int j=0; j<360; j++)
			{
				float degInRad = j*DEG2RAD;
				glVertex3f(cos(degInRad)*radius + collisionSpheres[i]->globalPosition[0] ,sin(degInRad)*radius + collisionSpheres[i]->globalPosition[1], 0 + collisionSpheres[i]->globalPosition[2]);
			}
		glEnd();
	}

	glPopMatrix();
#endif
}

void CDebris::ProcessMotion(DWORD milliseconds, Keys* keys)
{
	//Add motion based on motion vector and elapsed time
	translation[0] += milliseconds * motionVector[0] / 1000;
	translation[1] += milliseconds * motionVector[1] / 1000;
	translation[2] += milliseconds * motionVector[2] / 1000;

	//Rotate based on angular velocity and elapse time
	rotation[0] += milliseconds * angularVelocity[0] / 1000;
	rotation[1] += milliseconds * angularVelocity[1] / 1000;
	rotation[2] += milliseconds * angularVelocity[2] / 1000;

	//Update collision sphere locations
	for(unsigned int i = 0; i < collisionSpheres.size(); i++)
	{
		//First, rotate the sphere
		//collisionSpheres[i]->globalPosition[0] = cos(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[0] + sin(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[1];
		collisionSpheres[i]->globalPosition[0] = cos(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[0] + sin(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[1];
		//collisionSpheres[i]->globalPosition[1] = -sin(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[0] + cos(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[1];
		collisionSpheres[i]->globalPosition[1] = -sin(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[0] + cos(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[1];
		collisionSpheres[i]->globalPosition[2] = 0;

		//Scale by the object's scale
		collisionSpheres[i]->globalPosition[0] *= scale[0];
		collisionSpheres[i]->globalPosition[1] *= scale[1];
		collisionSpheres[i]->globalPosition[2] *= scale[2];

		//Translate by the object's translation
		collisionSpheres[i]->globalPosition[0] += translation[0];
		collisionSpheres[i]->globalPosition[1] += translation[1];
		collisionSpheres[i]->globalPosition[2] += translation[2];
	}

	if(translation[1] > 1.75)
		translation[1] = -1.75;
	else if(translation[1] < -1.75)
		translation[1] = 1.75;

	if(translation[0] > 14 || translation[0] < -14)
		deleteMe = true;
//		translation[0] = -14;
//	else if(translation[0] < -14)
//		translation[0] = 14;

	TTL -= milliseconds;
}

bool CDebris::CanDestroy(int destroyerType)
{
	if(destroyerType == TYPE_DEBRIS)
		return false;
	else
		return true;	
}

CMissileBase::CMissileBase(sGameParams *newGameParams) : CGameObject(newGameParams)
{
	sCollisionSphere *cSphere;

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = .50f;
	cSphere->translation[1] = 0;
	cSphere->translation[2] = 0;
	cSphere->radius = .50f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	loaded = true;
	launchKey = '\0';
	launchReady = false;
	charge = 0;
	destroyed = false;
	hitPoints = 30;
	firedLast = false;
}

CMissileBase::~CMissileBase()
{
}

int CMissileBase::Launch()
{
	int oldCharge = charge;

	loaded = false; 
	launchReady = false; 
	timeToReload = gameParams->nukeReloadTime; 
	charge = 0;

	return oldCharge;
}

bool CMissileBase::LaunchReady()
{
	return launchReady;
}

void CMissileBase::ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells)
{
	int numCities = 0;

	if(mySide == SIDE_GREEN)
	{
		numCities = gameParams->numGreenCities;
	}
	else if(mySide == SIDE_BLUE)
	{
		numCities = gameParams->numBlueCities;
	}

	if(timeToReload > 0)
	{
		timeToReload -= milliseconds;
	}
	else
		loaded = true;

	if(destroyed)
	{
		timeToRebuild -= milliseconds * numCities;
		if(timeToRebuild < 0)
		{
			collisionSpheres[0]->translation[0] = .50f;
			destroyed = false;
			hitPoints = 30;
		}
	}
}

bool CMissileBase::CanDestroy(int destroyerType)
{
	bool retval = false;

	if(destroyerType == TYPE_NUKE || destroyerType == TYPE_BEAM || destroyerType == TYPE_BLACKHOLE)
	{
		destroyed = true;
		timeToRebuild = 30000;

		collisionSpheres[0]->translation[0] = -50;

		retval = true;
	}
	else if(destroyerType == TYPE_BULLET)
	{
		hitPoints--;
		if(hitPoints < 0)
		{
			destroyed = true;
			timeToRebuild = 30000;

			collisionSpheres[0]->translation[0] = -50;

			retval = true;
		}
	}

	return retval;
}

float* CMissileBase::GetNukeTranslation()
{
	float nukeTranslation[3];

	nukeTranslation[0] = .5f * scale[0];
	nukeTranslation[1] = 1.0f * scale[1];
	nukeTranslation[2] = 0;

	rotatedNuke[0] = nukeTranslation[0] *  cos((rotation[2]) * DEG2RAD) + nukeTranslation[1] * -sin((rotation[2]) * DEG2RAD);
	rotatedNuke[1] = nukeTranslation[0] * sin((rotation[2]) * DEG2RAD) + nukeTranslation[1] * cos((rotation[2]) * DEG2RAD);
	rotatedNuke[2] = 0;

	rotatedNuke[0] += translation[0];
	rotatedNuke[1] += translation[1];

	return rotatedNuke;
}

float* CMissileBase::GetNukeVector()
{
	float divisor;
	float nukePos[3];

	//scale then rotate it
	nukePos[0] = .5f*scale[0] * cos(rotation[2] * DEG2RAD) - 1.0f*scale[1] * sin(rotation[2] * DEG2RAD);
	nukePos[1] = .5f*scale[0] * sin(rotation[2] * DEG2RAD) + 1.0f*scale[1] * cos(rotation[2] * DEG2RAD);
	nukePos[2] = 0;

	//translate it
	nukePos[0] += translation[0];
	nukePos[1] += translation[1];

	//compute vector to cursor
	nukeVector[0] = pCursorPos[0] - nukePos[0];
	nukeVector[1] = pCursorPos[1] - nukePos[1];
	
	//Convert to unit vector, then make it sort of small.  This will be the missile's original motion vector.
	divisor = sqrt(nukeVector[0] * nukeVector[0] + nukeVector[1] * nukeVector[1]);
	nukeVector[0] = (nukeVector[0] / divisor / 100);
	nukeVector[1] = (nukeVector[1] / divisor / 100);
	nukeVector[2] = 0;
	
	return nukeVector;
}

void CMissileBase::Draw()
{
	float x, y, z=0;
	float offset = 0;
	float radius;
	float missileRotation = 0;

	if(destroyed)
		return;

	glPushMatrix();
	
	glTranslatef(translation[0], translation[1], translation[2]);
	glRotatef(rotation[0], 1, 0, 0);
	glRotatef(rotation[1], 0, 1, 0);
	glRotatef(rotation[2], 0, 0, 1);
	glScalef(scale[0], scale[1], scale[2]);
	glColor3f(color[0], color[1], color[2]);

	glBegin(GL_QUADS);
		glVertex3f(0, 0, 0);
		glVertex3f(1,0,0);
		glVertex3f(1,.5f,0);
		glVertex3f(0,.5f,0);
	glEnd();

	glBegin(GL_LINE_STRIP);
		glVertex3f(-.2f, 0, 0);
		glVertex3f(-.2f, 1.5f, 0);
		glVertex3f(0, 1.5f, 0);
		glVertex3f(0, 0, 0);

		for(int i=0; i<8; i++)
		{
			if(i % 2)
				x = -.2f;
			else
				x = 0;

			y = i * .2f;

			glVertex3f(x, y, 0);
		}
	glEnd();

	glBegin(GL_LINE_STRIP);
		for(int i=0; i<8; i++)
		{
			if(i % 2)
				x = 0;
			else
				x = -.2f;

			y = i * .2f;

			glVertex3f(x, y, 0);
		}
	glEnd();

	glColor3f(1, 1, 1);
	glBegin(GL_LINE_LOOP);
		glVertex3f(0, 0, 0);
		glVertex3f(1,0,0);
		glVertex3f(1,.5f,0);
		glVertex3f(0,.5f,0);
	glEnd();


	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
		glVertex3f(-1, 0, 0);
		glVertex3f(1,0,0);

		glVertex3f(0, -1, 0);
		glVertex3f(0,1,0);
	glEnd();

	if(loaded && !firedLast)
	{
		//Draw a missile on the launch pad
		//The missile gets its own coordinate system
		glPushMatrix();

		float nukePos[2];

		//rotate it
		nukePos[0] = .5f * cos(rotation[2] * DEG2RAD) - 1.0f * sin(rotation[2] * DEG2RAD);
		nukePos[1] = .5f * sin(rotation[2] * DEG2RAD) + 1.0f * cos(rotation[2] * DEG2RAD);

		//scale it
		nukePos[0] *= scale[0];
		nukePos[1] *= scale[1];

		//translate it
		nukePos[0] += translation[0];
		nukePos[1] += translation[1];

		//Rotate to point toward the cursor
		missileRotation = atan2(pCursorPos[1] - nukePos[1], pCursorPos[0] - nukePos[0]) / DEG2RAD;

		//glRotatef(missileRotation + rotation[2] + 90, 0, 0, 1);
		glTranslatef(.5, 1, 0);
		glRotatef(missileRotation - rotation[2] - 90, 0, 0, 1);

		glColor3f(color[0], color[1], color[2]);  

		//Draw one half of Nuke body
		radius = 1;
		offset = -cos(-30.0f*DEG2RAD);

		glBegin(GL_TRIANGLES);
		for (int i=-20; i<30; i+=5)
		{
			float degInRad = i*DEG2RAD;
			glVertex3f(cos(degInRad)*radius + offset, sin(degInRad)*radius, -.001f);

			degInRad = (i+5)*DEG2RAD;
			glVertex3f(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001f);

			glVertex3f(0, 0, -.001f);
		}
		glEnd();

		//Draw the other half of Nuke body
		offset = -cos(150.0f*DEG2RAD);
		glBegin(GL_TRIANGLES);
		for (int i=150; i<200; i+=5)
		{
			float degInRad = i*DEG2RAD;
			glVertex3f(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001f);

			degInRad = (i+5)*DEG2RAD;
			glVertex3f(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001f);

			glVertex3f(0, 0, -.001f);
		}
		glEnd();

		//Fill in bottom triangle
		glBegin(GL_TRIANGLES);
			glVertex3f(0,0,0);
			glVertex3f(cos(200 * DEG2RAD)*radius - cos(150.0f*DEG2RAD), sin(200 * DEG2RAD)*radius, -.001f);
			glVertex3f(cos(-20 * DEG2RAD)*radius - cos(-30.0f*DEG2RAD), sin(-20 * DEG2RAD)*radius, -.001f);
		glEnd();

		//Make one fin
		offset = sin(200 * DEG2RAD);
		glBegin(GL_TRIANGLES);
		radius = .28f;
		for (int i=120; i<180; i+=4)
		{
			float degInRad = i*DEG2RAD;
			glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.01f);

			degInRad = (i+4)*DEG2RAD;
			glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.01f);

			glVertex3f(0, offset/2, -.01f);
		}
		glEnd();

		glBegin(GL_TRIANGLES);
		//Make another fin
		for (int i=120; i<180; i+=4)
		{
			float degInRad = i*DEG2RAD;
			glVertex3f(-cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.01f);

			degInRad = (i+4)*DEG2RAD;
			glVertex3f(-cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.01f);

			glVertex3f(0, offset/2, -.01f);
		}
		glEnd();


		//**** Create outlines ***

		glColor3f(1.0, 1.0, 1.0);

		//Draw one half of Nuke body
		radius = 1;
		offset = -cos(-30.0f*DEG2RAD);

		glBegin(GL_LINES);
		for (int i=-20; i<30; i+=5)
		{
			float degInRad = i*DEG2RAD;
			glVertex3f(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001f);

			degInRad = (i+5)*DEG2RAD;
			glVertex3f(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001f);

			//glVertex3f(0, 0, 0);
		}
		glEnd();

		//Draw the other half of Nuke body
		offset = -cos(150.0f*DEG2RAD);
		glBegin(GL_LINES);
		for (int i=150; i<200; i+=5)
		{
			float degInRad = i*DEG2RAD;
			glVertex3f(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001f);

			degInRad = (i+5)*DEG2RAD;
			glVertex3f(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001f);

			//glVertex3f(0, 0, 0);
		}
		glEnd();

		
		//Fill in bottom triangle
		glBegin(GL_LINES);
			//glVertex3f(0,0,0);
			glVertex3f(cos(200 * DEG2RAD)*radius - cos(150.0f*DEG2RAD), sin(200 * DEG2RAD)*radius, -.00001f);
			glVertex3f(cos(-20 * DEG2RAD)*radius - cos(-30.0f*DEG2RAD), sin(-20 * DEG2RAD)*radius, -.00001f);
		glEnd();

		//Make one fin
		offset = sin(200 * DEG2RAD);
		glBegin(GL_LINES);
		radius = .28f;
		for (int i=120; i<180; i+=4)
		{
			float degInRad = i*DEG2RAD;
			glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.001f);

			degInRad = (i+4)*DEG2RAD;
			glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.001f);

			//glVertex3f(0, offset/2, 0);
		}
		glEnd();

		glBegin(GL_LINES);
			glVertex3f(0, offset/2, -0.005f);
			glVertex3f(cos(179*DEG2RAD)*radius,sin(179*DEG2RAD)*radius + offset, -0.005f);
		glEnd();

		glBegin(GL_LINES);
		//Make another fin
		for (int i=120; i<180; i+=4)
		{
			float degInRad = i*DEG2RAD;
			glVertex3f(-cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.001f);

			degInRad = (i+4)*DEG2RAD;
			glVertex3f(-cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.001f);

			//glVertex3f(0, offset/2, 0);
		}
		glEnd();

		glBegin(GL_LINES);
			glVertex3f(0, offset/2, -0.005f);
			glVertex3f(-cos(179*DEG2RAD)*radius,sin(179*DEG2RAD)*radius + offset, -0.005f);
		glEnd();
		
		glPopMatrix();

	}	

	glPopMatrix();


#ifdef COLLISION_DEBUG
	glPushMatrix();

	//Draw collision spheres for debug
	for(unsigned int i=0; i<collisionSpheres.size(); i++)
	{
		radius = collisionSpheres[i]->radius * scale[1];
		glColor3f(1, 1, 0);
		glBegin(GL_LINE_LOOP);
			for (int j=0; j<360; j++)
			{
				float degInRad = j*DEG2RAD;
				glVertex3f(cos(degInRad)*radius + collisionSpheres[i]->globalPosition[0] ,sin(degInRad)*radius + collisionSpheres[i]->globalPosition[1], 0 + collisionSpheres[i]->globalPosition[2]);
			}
		glEnd();
	}

	glPopMatrix();
#endif
}

int CMissileBase::GetType()
{
	return TYPE_MISSILEBASE;
}


CCity::CCity(sGameParams *newGameParams) : CGameObject(newGameParams)
{
	sCollisionSphere *cSphere;

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = .9f;
	cSphere->translation[1] = .4f;
	cSphere->translation[2] = 0;
	cSphere->radius = .4f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = .9f;
	cSphere->translation[1] = .7f;
	cSphere->translation[2] = 0;
	cSphere->radius = .4f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = 0;
	cSphere->translation[1] = .25f;
	cSphere->translation[2] = 0;
	cSphere->radius = .25f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = 0;
	cSphere->translation[1] = .45f;
	cSphere->translation[2] = 0;
	cSphere->radius = .25f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = .35f;
	cSphere->translation[1] = .25f;
	cSphere->translation[2] = 0;
	cSphere->radius = .25f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	hitPoints = 30;
}

CCity::~CCity()
{
}

void CCity::ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells)
{
}

bool CCity::CanDestroy(int destroyerType)
{
	bool retval = false;

	if(destroyerType == TYPE_NUKE || destroyerType == TYPE_BEAM || destroyerType == TYPE_BLACKHOLE)
		retval = true;
	else if(destroyerType == TYPE_BULLET)
	{
		hitPoints--;
		if(hitPoints < 0)
			retval = true;
	}

	return retval;
}

int	 CCity::GetType()
{
	return TYPE_CITY;
}

void CCity::Draw()
{
	glPushMatrix();
	
	glTranslatef(translation[0], translation[1], translation[2]);
	glRotatef(rotation[0], 1, 0, 0);
	glRotatef(rotation[1], 0, 1, 0);
	glRotatef(rotation[2], 0, 0, 1);
	glScalef(scale[0], scale[1], scale[2]);
	glColor3f(color[0], color[1], color[2]);

	glBegin(GL_QUADS);

		glColor3f(color[0] *.6f, color[1]*.6f, color[2]*.6f);
		
		glVertex3f(0, 0, -.01f);
		glVertex3f(1, 0, -.01f);
		glVertex3f(1, .5f, -.01f);
		glVertex3f(0, .5f, -.01f);

		glColor3f(color[0] *.8f, color[1]*.8f, color[2]*.8f);

		glVertex3f(-.25f, 0, -.001f);
		glVertex3f(.25f, 0, -.001f);
		glVertex3f(.25f, .7f, -.001f);
		glVertex3f(-.25f, .7f, -.001f);

		glColor3f(color[0], color[1], color[2]);
		
		glVertex3f(.5f, 0, -.001f);
		glVertex3f(1.3f, 0, -.001f);
		glVertex3f(1.3f, 1.1f, -.001f);
		glVertex3f(.5f, 1.1f, -.001f);
	glEnd();

	glColor3f(1, 1, 1);
	glBegin(GL_LINE_LOOP);
		glVertex3f(0, 0, -.01f);
		glVertex3f(1, 0, -.01f);
		glVertex3f(1, .5f, -.01f);
		glVertex3f(0, .5f, -.01f);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3f(-.25f, 0, -.001f);
		glVertex3f(.25f, 0, -.001f);
		glVertex3f(.25f, .7f, -.001f);
		glVertex3f(-.25f, .7f, -.001f);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3f(.5, 0, -.001f);
		glVertex3f(1.3f, 0, -.001f);
		glVertex3f(1.3f, 1.1f, -.001f);
		glVertex3f(.5, 1.1f, -.001f);
	glEnd();

	//Draw some windows
	glColor3f(0, 0, 0);

	glBegin(GL_QUADS);
		
		glVertex3f(.6f, .2f, 0);
		glVertex3f(.8f, .2f, 0);
		glVertex3f(.8f, .4f ,0);
		glVertex3f(.6f, .4f ,0);

		glVertex3f(1,   .2f, 0);
		glVertex3f(1.2f, .2f, 0);
		glVertex3f(1.2f, .4f, 0);
		glVertex3f(1,   .4f, 0);

		glVertex3f(.6f, .5f, 0);
		glVertex3f(.8f, .5f, 0);
		glVertex3f(.8f, .7f, 0);
		glVertex3f(.6f, .7f, 0);

		glVertex3f(1,   .5f, 0);
		glVertex3f(1.2f, .5f, 0);
		glVertex3f(1.2f, .7f, 0);
		glVertex3f(1,   .7f, 0);

		glVertex3f(.6f, .8f, 0);
		glVertex3f(.8f, .8f, 0);
		glVertex3f(.8f, 1, 0);
		glVertex3f(.6f, 1, 0);

		glVertex3f(1,   .8f, 0);
		glVertex3f(1.2f, .8f, 0);
		glVertex3f(1.2f, 1, 0);
		glVertex3f(1,   1, 0);

		glVertex3f(-.2f, .1f, 0);
		glVertex3f(.2f, .1f, 0);
		glVertex3f(.2f, .2f, 0);
		glVertex3f(-.2f, .2f, 0);

		glVertex3f(-.2f, .3f, 0);
		glVertex3f(.2f, .3f, 0);
		glVertex3f(.2f, .4f, 0);
		glVertex3f(-.2f, .4f, 0);

		glVertex3f(-.2f, .5f, 0);
		glVertex3f(.2f, .5f, 0);
		glVertex3f(.2f, .6f, 0);
		glVertex3f(-.2f, .6f, 0);

	glEnd();


	glPopMatrix();


#ifdef COLLISION_DEBUG
	glPushMatrix();

	//Draw collision spheres for debug
	float radius;
	for(unsigned int i=0; i<collisionSpheres.size(); i++)
	{
		radius = collisionSpheres[i]->radius * scale[1];
		glColor3f(1, 1, 0);
		glBegin(GL_LINE_LOOP);
			for (int j=0; j<360; j++)
			{
				float degInRad = j*DEG2RAD;
				glVertex3f(cos(degInRad)*radius + collisionSpheres[i]->globalPosition[0] ,sin(degInRad)*radius + collisionSpheres[i]->globalPosition[1], 0 + collisionSpheres[i]->globalPosition[2]);
			}
		glEnd();
	}

	glPopMatrix();
#endif

}


CFlakCannon::CFlakCannon(sGameParams *newGameParams) : CGameObject(newGameParams)
{
	sCollisionSphere *cSphere;

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = 0;
	cSphere->translation[1] = 0;
	cSphere->translation[2] = 0;
	cSphere->radius = .5f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	fireKey = '\0';
	loaded = true;
	timeToReload = 0;
	destroyed = false;
	charge = 0;
	ready = false;

	hitPoints = 30;
}

CFlakCannon::~CFlakCannon()
{
}

void CFlakCannon::ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells)
{
}

bool CFlakCannon::CanDestroy(int destroyerType)
{
	bool retval = false;

	if(destroyerType == TYPE_NUKE || destroyerType == TYPE_BEAM || destroyerType == TYPE_BLACKHOLE)
	{
		collisionSpheres[0]->translation[0] = 50;
		timeToRebuild = 30000;
		destroyed = true;
		retval = true;
	}
	else if(destroyerType == TYPE_BULLET)
	{
		hitPoints--;
		if(hitPoints < 0)
		{
			collisionSpheres[0]->translation[0] = 50;
			timeToRebuild = 30000;
			destroyed = true;
			retval = true;
		}
	}

	return retval;
}

int	CFlakCannon::GetType()
{
	return TYPE_FLAKCANNON;
}

void CFlakCannon::Fire()
{
	loaded = false; 
	timeToReload = gameParams->flakReloadTime;
}

bool CFlakCannon::HasSuperWeapon()
{
	bool result = false;

	switch(mySide)
	{
	case SIDE_BLUE:
		if(gameParams->blueSuperAmmo > gameParams->blueSuperFires)
			result = true;
		else
			result = false;
		break;
	case SIDE_GREEN:
		if(gameParams->greenSuperAmmo > gameParams->greenSuperFires)
			result = true;
		else
			result = false;
		break;
	}

	return result;
}

void CFlakCannon::FireSuperWeapon()
{
	loaded = false; 
	timeToReload = gameParams->flakReloadTime * 100;
	charge = 0;

	switch(mySide)
	{
	case SIDE_BLUE:
		gameParams->blueSuperFires++;
		break;
	case SIDE_GREEN:
		gameParams->greenSuperFires++;
		break;
	}
}

void CFlakCannon::GetProjVector(int* TTL, float* projVector)
{
	projVector[0] = (pCursorPos[0] - translation[0]);
	projVector[1] = (pCursorPos[1] - translation[1]);
	projVector[2] = 0;//pCursorPos[2] - translation[2]/3.0;

	float magnitude = sqrt(projVector[0] * projVector[0] + projVector[1] * projVector[1]) / 8.0f;

	projVector[0] /= magnitude;
	projVector[1] /= magnitude;

	*TTL = (int)(1000 * magnitude);
}

float CFlakCannon::GetProjAngle()
{
	float projVector[3];

	projVector[0] = (pCursorPos[0] - translation[0]);
	projVector[1] = (pCursorPos[1] - translation[1]);
	projVector[2] = 0;//pCursorPos[2] - translation[2]/3.0;

	return atan((pCursorPos[1] - translation[1]) / (pCursorPos[0] - translation[0])) /  DEG2RAD;
}

float* CFlakCannon::GetProjTranslation()
{
	projTranslation[0] = translation[0];
	projTranslation[1] = translation[1];
	projTranslation[2] = -.0010f;

	return projTranslation;
}

void CFlakCannon::AddTimeSinceFired(DWORD milliseconds)
{
	int numCities = 0;

	if(mySide == SIDE_GREEN)
	{
		numCities = gameParams->numGreenCities;
	}
	else if(mySide == SIDE_BLUE)
	{
		numCities = gameParams->numBlueCities;
	}

	timeToReload -= milliseconds;

	if(timeToReload < 0)
	{
		loaded = true; 
	}

	if(destroyed)
	{
		timeToRebuild -= milliseconds * numCities;
		if(timeToRebuild < 0)
		{
			collisionSpheres[0]->translation[0] = 0;
			destroyed = false;
			hitPoints = 30;
		}
	}
}

void CFlakCannon::Draw()
{
	float cannonRotation = 0;

	if(destroyed)
		return;

	glPushMatrix();
	
	glTranslatef(translation[0], translation[1], translation[2]);
	glRotatef(rotation[0], 1, 0, 0);
	glRotatef(rotation[1], 0, 1, 0);
	glRotatef(rotation[2], 0, 0, 1);
	glScalef(scale[0], scale[1], scale[2]);
	glColor3f(color[0], color[1], color[2]);

	glBegin(GL_QUADS);
		glVertex3f(-.5f, .25f, -.001f);	
		glVertex3f(-.5f, 0, -.001f);
		glVertex3f(.5f, 0, -.001f);
		glVertex3f(.5f, .25f, -.001f);
	glEnd();

	glBegin(GL_TRIANGLES);
		for (int i=0; i<180; i+=4)
		{
			float degInRad = i*DEG2RAD;
			float radius = .4f;
			glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius, -.001f);

			degInRad = (i+4)*DEG2RAD;
			glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius, -.001f);

			glVertex3f(0, 0, -.001f);
		}
	glEnd();

	glColor3f(1, 1, 1);
	glBegin(GL_LINE_STRIP);
		glVertex3f(-.5f, .25f, -.001f);	
		glVertex3f(-.5f, 0, -.001f);
		glVertex3f(.5f, 0, -.001f);
		glVertex3f(.5f, .25f, -.001f);
		glVertex3f(-.5f, .25f, -.001f);	
	glEnd();

	glBegin(GL_LINE_STRIP);
		for (int i=0; i<180; i+=4)
		{
			float degInRad = i*DEG2RAD;
			float radius = .4f;
			glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius, -.00125f);

			degInRad = (i+4)*DEG2RAD;
			glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius, -.00125f);
		}
	glEnd();

	glColor3f(color[0], color[1], color[2]);

	cannonRotation = atan2(pCursorPos[1] - translation[1], pCursorPos[0] - translation[0]) / DEG2RAD;

	glPushMatrix();

	glRotatef(cannonRotation - rotation[2] - 90, 0, 0, 1);

	//Draw the cannon
	glBegin(GL_QUADS);
		glVertex3f(-.05f, 0, -.00125f);
		glVertex3f(.05f, 0, -.00125f);
		glVertex3f(.03f, .8f, -.00125f);
		glVertex3f(-.03f, .8f, -.00125f);
	glEnd();

	//Draw the cannon outline
	glColor3f(1, 1, 1);
	glBegin(GL_LINE_STRIP);
		glVertex3f(-.05f, 0, -.00125f);
		glVertex3f(.05f, 0, -.00125f);
		glVertex3f(.03f, .8f, -.00125f);
		glVertex3f(-.03f, .8f, -.00125f);
		glVertex3f(-.05f, 0, -.00125f);
	glEnd();

	glPopMatrix();

	glPopMatrix();



#ifdef COLLISION_DEBUG
	glPushMatrix();

	//Draw collision spheres for debug
	float radius;
	for(unsigned int i=0; i<collisionSpheres.size(); i++)
	{
		radius = collisionSpheres[i]->radius * scale[1];
		glColor3f(1, 1, 0);
		glBegin(GL_LINE_LOOP);
			for (int j=0; j<360; j++)
			{
				float degInRad = j*DEG2RAD;
				glVertex3f(cos(degInRad)*radius + collisionSpheres[i]->globalPosition[0] ,sin(degInRad)*radius + collisionSpheres[i]->globalPosition[1], 0 + collisionSpheres[i]->globalPosition[2]);
			}
		glEnd();
	}

	glPopMatrix();
#endif
}

CProjectile::CProjectile(sGameParams *newGameParams) : CGameObject(newGameParams)
{
	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;

	timeToLive = 1000;
}

CProjectile::~CProjectile()
{
}

void CProjectile::Draw()
{
	float projRotation = 0;

	glPushMatrix();
	
	glTranslatef(translation[0], translation[1], translation[2]);
	glRotatef(rotation[0], 1, 0, 0);
	glRotatef(rotation[1], 0, 1, 0);
	glRotatef(rotation[2], 0, 0, 1);
	glScalef(scale[0], scale[1], scale[2]);
	glColor3f(color[0], color[1], color[2]);

	projRotation = atan2(motionVector[1], motionVector[0]) / DEG2RAD;

	glRotatef(projRotation - 90, 0, 0, 1);

	glBegin(GL_QUADS);
		glVertex3f(-.05f, -.05f, -.01f);	
		glVertex3f(.05f, -.05f, -.01f);
		glVertex3f(.05f, .05f, -.01f);
		glVertex3f(-.05f, .05f, -.01f);
	glEnd();

	glBegin(GL_TRIANGLES);
		glVertex3f(.05f, .05f, -.01f);
		glVertex3f(-.05f, .05f, -.01f);
		glVertex3f(0, .2f, -.001f);
	glEnd();

	glColor3f(1, 1, 1);
	glBegin(GL_LINE_LOOP);
		glVertex3f(-.05f, -.05f, -.01f);	
		glVertex3f(.05f, -.05f, -.01f);
		glVertex3f(.05f, .05f, -.01f);
		glVertex3f(0, .2f, -.01f);
		glVertex3f(-.05f, .05f, -.01f);
	glEnd();

	glPopMatrix();

	//Draw a line from her to this object's origination point
	glColor3f(.2f, .2f, .2f);
	glBegin(GL_LINES);
		glVertex3f(origin[0], origin[1], -.002f);
		glVertex3f(translation[0], translation[1], -.002f);
	glEnd();
}

CMothership::CMothership(sGameParams *newGameParams) : CGameObject(newGameParams)
{
	sCollisionSphere *cSphere;

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = 0;
	cSphere->translation[1] = .25;
	cSphere->translation[2] = 0;
	cSphere->radius = 1.0f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = -1.1f;
	cSphere->translation[1] = 0;
	cSphere->translation[2] = 0;
	cSphere->radius = 0.8f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = 1.1f;
	cSphere->translation[1] = 0;
	cSphere->translation[2] = 0;
	cSphere->radius = 0.8f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	countDown = 15000 - gameParams->randoms[gameParams->randIndex++%512]%5000;
	myBeam = NULL;

	hitPoints = 30;
}

CMothership::~CMothership()
{
}

bool CMothership::IsFiring()
{
	if(countDown < 0)
	{
		countDown = 15000 - gameParams->randoms[gameParams->randIndex++%512]%5000;
		return true;
	}
	else
		return false;
}

void CMothership::Draw()
{
	glPushMatrix();
	
	glTranslatef(translation[0], translation[1], translation[2]);
	glRotatef(rotation[0], 1, 0, 0);
	glRotatef(rotation[1], 0, 1, 0);
	glRotatef(rotation[2], 0, 0, 1);
	glScalef(scale[0], scale[1], scale[2]);
	glColor3f(color[0], color[1], color[2]);

	glBegin(GL_TRIANGLES);
		for (int i=0; i<360; i+=36)
		{
			float degInRad = i*DEG2RAD;
			float radius = 1.0f;

			glColor3f(color[0], color[1], color[2]);
			glVertex3f(cos(degInRad) * radius * 2, sin(degInRad) * radius/2.0f + .1f, -.001f);

			degInRad = (i+36)*DEG2RAD;
			glVertex3f(cos(degInRad) * radius * 2, sin(degInRad) * radius/2.0f + .1f, -.001f);

			glColor3f(0, 0, 0);
			glVertex3f(0, .1f, -.001f);
		}
	glEnd();

	
	glColor3f(color[0], color[1], color[2]);
	glBegin(GL_TRIANGLES);
		for (int i=20; i<160; i+=10)
		{
			float degInRad = i*DEG2RAD;
			float radius = 1.0f;
			glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius + .2f, -.002f);

			degInRad = (i+10)*DEG2RAD;
			glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius + .2f, -.002f);

			glVertex3f(0, .2f, -.001f);
		}
	glEnd();

	glColor3f(1, 1, 1);

	glBegin(GL_LINE_STRIP);
		for (int i=0; i<360; i+=36)
		{
			float degInRad = i*DEG2RAD;
			float radius = 1.0f;
			glVertex3f(cos(degInRad) * radius * 2, sin(degInRad) * radius/2.0f + .1f, -.001f);

			degInRad = (i+36)*DEG2RAD;
			glVertex3f(cos(degInRad) * radius * 2, sin(degInRad) * radius/2.0f + .1f, -.001f);
		}
	glEnd();

	glBegin(GL_LINE_STRIP);
		for (int i=20; i<160; i+=10)
		{
			float degInRad = i*DEG2RAD;
			float radius = 1.0f;
			glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius + .2f, -.001f);

			degInRad = (i+10)*DEG2RAD;
			glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius + .2f, -.001f);
		}
	glEnd();

	glPopMatrix();

#ifdef COLLISION_DEBUG
	glPushMatrix();

	//Draw collision spheres for debug
	float radius;
	for(unsigned int i=0; i<collisionSpheres.size(); i++)
	{
		radius = collisionSpheres[i]->radius * scale[1];
		glColor3f(1, 1, 0);
		glBegin(GL_LINE_LOOP);
			for (int j=0; j<360; j++)
			{
				float degInRad = j*DEG2RAD;
				glVertex3f(cos(degInRad)*radius + collisionSpheres[i]->globalPosition[0] ,sin(degInRad)*radius + collisionSpheres[i]->globalPosition[1], 0 + collisionSpheres[i]->globalPosition[2]);
			}
		glEnd();
	}

	glPopMatrix();
#endif
}

bool CMothership::CanDestroy(int destroyerType)
{
	bool retval = false;

	if(destroyerType == TYPE_NUKE || destroyerType == TYPE_BEAM || destroyerType == TYPE_BLACKHOLE)
	{
		if(myBeam != NULL)
			myBeam->Kill();
		retval = true;
	}
	else if(destroyerType == TYPE_BULLET)
	{
		hitPoints--;
		if(hitPoints < 0)
		{
			if(myBeam != NULL)
				myBeam->Kill();
			retval = true;
		}
	}

	return retval;
}


CBeam::CBeam(sGameParams *newGameParams) : CGameObject(newGameParams)
{
	sCollisionSphere *cSphere;
	float trans=0;

	for(int i=0; i<40; i++)
	{
		trans -= (i/16.0f);
		cSphere  = new sCollisionSphere;
		cSphere->translation[0] = 0;
		cSphere->translation[1] = trans;
		cSphere->translation[2] = 0;
		cSphere->radius = i / 48.0f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);
	}

	animVal = 0;
	TTL = 2000;
	myParent = NULL;
}

CBeam::~CBeam()
{

}

void CBeam::Draw()
{
	glPushMatrix();
	
	glTranslatef(translation[0], translation[1], translation[2]);
	glRotatef(rotation[0], 1, 0, 0);
	glRotatef(rotation[1], 0, 1, 0);
	glRotatef(rotation[2], 0, 0, 1);
	glScalef(scale[0], scale[1], scale[2]);
	glColor3f(color[0], color[1], color[2]);
	animVal++;

	glBegin(GL_LINES);

	for(int i=0; i<3; i++)
	{
		glColor3f(1.0, 1.0, 1.0);
		glVertex3f(0, 0, -.002f);
		glColor3f(animVal%6 / 6.0f * color[0], animVal%6 / 6.0f * color[1], animVal%6 / 6.0f * color[2]);
		glVertex3f(0, -60, -.002f);

		glColor3f(1.0, 1.0, 1.0);
		glVertex3f(0, 0, -.002f);
		glColor3f(animVal%6 / 6.0f * color[0], animVal%6 / 6.0f * color[1], animVal%6 / 6.0f * color[2]);
		glVertex3f(((i+1)*animVal%100) / 100.0f, -60, -.002f);

		glColor3f(1.0, 1.0, 1.0);
		glVertex3f(0, 0, -.002f);
		glColor3f(animVal%6 / 6.0f * color[0], animVal%6 / 6.0f * color[1], animVal%6 / 6.0f * color[2]);
		glVertex3f(((i+1)*animVal%100) / -100.0f, -60, -.002f);

		glColor3f(1.0, 1.0, 1.0);
		glVertex3f(0, 0, -.002f);
		glColor3f(animVal%6 / 6.0f * color[0], animVal%6 / 6.0f * color[1], animVal%6 / 6.0f * color[2]);
		glVertex3f(((i+1)*animVal%100) / 200.0f, -60, -.002f);

		glColor3f(1.0, 1.0, 1.0);
		glVertex3f(0, 0, -.002f);
		glColor3f(animVal%6 / 6.0f * color[0], animVal%6 / 6.0f * color[1], animVal%6 / 6.0f * color[2]);
		glVertex3f(((i+1)*animVal%100) / -200.0f, -60, -.002f);
	}

	glEnd();

	glPopMatrix();

#ifdef COLLISION_DEBUG
	glPushMatrix();

	//Draw collision spheres for debug
	float radius;
	for(unsigned int i=0; i<collisionSpheres.size(); i++)
	{
		radius = collisionSpheres[i]->radius * scale[1];
		glColor3f(1, 1, 0);
		glBegin(GL_LINE_LOOP);
			for (int j=0; j<360; j++)
			{
				float degInRad = j*DEG2RAD;
				glVertex3f(cos(degInRad)*radius + collisionSpheres[i]->globalPosition[0] ,sin(degInRad)*radius + collisionSpheres[i]->globalPosition[1], 0 + collisionSpheres[i]->globalPosition[2]);
			}
		glEnd();
	}

	glPopMatrix();
#endif

}

CBlackHole::CBlackHole(sGameParams *newGameParams) : CGameObject(newGameParams)
{
	sCollisionSphere *cSphere  = new sCollisionSphere();
	cSphere->translation[0] = 0;
	cSphere->translation[1] = 0;
	cSphere->translation[2] = 0;
	cSphere->radius = 1;
	cSphere->globalPosition[0] = -10;
	cSphere->globalPosition[1] = -10;
	cSphere->globalPosition[2] = -10;
	collisionSpheres.push_back(cSphere);

	TTL = 30000;
	animVal = 0;
}

CBlackHole::~CBlackHole()
{
	myGravity->mass = 0;
}

void CBlackHole::ProcessMotion(DWORD milliseconds, Keys* keys)
{
	CGameObject::ProcessMotion(milliseconds, keys);

	TTL -= milliseconds;

	myGravity->translation[0] = translation[0];
	myGravity->translation[1] = translation[1];
	myGravity->translation[2] = translation[2];

	if(translation[0] > 2.75f || translation[0] < -2.75f)
		KillGravity();
}

bool CBlackHole::CanDestroy(int destroyerType)
{
	if(destroyerType == TYPE_MISSILEBASE || destroyerType == TYPE_CITY || destroyerType == TYPE_FLAKCANNON || destroyerType == TYPE_PLANET)
	{
		motionVector[0] = 0;
		motionVector[1] = 0;
		motionVector[2] = 0;
	}
	return false;
}

void CBlackHole::Draw()
{
	glPushMatrix();
	
	glTranslatef(translation[0], translation[1], translation[2]);
	glRotatef(rotation[0], 1, 0, 0);
	glRotatef(rotation[1], 0, 1, 0);
	glRotatef(rotation[2], 0, 0, 1);
	glScalef(scale[0], scale[1], scale[2]);	
	animVal++;

	//Draw a black circle
	glColor3f(0, 0, 0);
	glBegin(GL_TRIANGLES);
	for (int i=0; i<360; i+=4)
	{
		float degInRad = i*DEG2RAD;
		float radius = 1.0f;

		glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius, -.001f);

		degInRad = (i+4)*DEG2RAD;
		glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius, -.001f);

		glVertex3f(0, .1f, -.001f);
	}
	glEnd();

	//Draw some electricity around it
	glColor3f(color[0], color[1], color[2]);
	glBegin(GL_LINE_STRIP);
	for (int i=0; i<=360; i+=20)
	{
		float degInRad = i*DEG2RAD;
		float radius = 1.0f;

		glVertex3f(cos(degInRad) * radius + ((gameParams->randoms[gameParams->randIndex++%1024]/(float)RAND_MAX) - .5f)/ 4.0f,
			sin(degInRad) * radius + ((gameParams->randoms[gameParams->randIndex++%1024]/(float)RAND_MAX) - .5f)/4.0f, -.001f);

		degInRad = (i+4)*DEG2RAD;

		glVertex3f(cos(degInRad) * radius + ((gameParams->randoms[gameParams->randIndex++%1024]/(float)RAND_MAX) - .5f)/ 4.0f,
			sin(degInRad) * radius + ((gameParams->randoms[gameParams->randIndex++%1024]/(float)RAND_MAX) - .5f)/4.0f, -.001f);

	}
	glEnd();

	//Draw some electricity around it
	glColor3f(1, 1, 1);
	glBegin(GL_LINE_STRIP);
	for (int i=0; i<=360; i+=20)
	{
		float degInRad = i*DEG2RAD;
		float radius = 1.0f;

		glVertex3f(cos(degInRad) * radius + ((gameParams->randoms[gameParams->randIndex++%1024]/(float)RAND_MAX) - .5f)/ 4.0f,
			sin(degInRad) * radius + ((gameParams->randoms[gameParams->randIndex++%1024]/(float)RAND_MAX) - .5f)/4.0f, -.001f);

		degInRad = (i+4)*DEG2RAD;

		glVertex3f(cos(degInRad) * radius + ((gameParams->randoms[gameParams->randIndex++%1024]/(float)RAND_MAX) - .5f)/ 4.0f,
			sin(degInRad) * radius + ((gameParams->randoms[gameParams->randIndex++%1024]/(float)RAND_MAX) - .5f)/4.0f, -.001f);

	}
	glEnd();

	//Draw some particles flying into it
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	for (int i=0; i<=7200; i+=163)
	{
		float degInRad = i*DEG2RAD;
		float radius = (2-((animVal*i)%1000000)/1000000.0f);

		glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius, -.001f);
		glVertex3f(cos(degInRad) * radius * 0.9f, sin(degInRad) * radius * 0.9f, -.001f);
	}
	glEnd();

	glPopMatrix();
}

CShip::CShip(sGameParams *newGameParams) : CGameObject(newGameParams)
{
	sCollisionSphere *cSphere  = new sCollisionSphere();
	cSphere->translation[0] = 0;
	cSphere->translation[1] = 0;
	cSphere->translation[2] = 0;
	cSphere->radius = 1;
	cSphere->globalPosition[0] = -10;
	cSphere->globalPosition[1] = -10;
	cSphere->globalPosition[2] = -10;
	collisionSpheres.push_back(cSphere);

	//Cycle colors
	flameColor[0] = 1.0f;
	flameColor[1] = (float)(gameParams->randoms[gameParams->randIndex++%512] / RAND_MAX);
	flameColor[2] = 0;

	timeSinceLastFire = 0;

	hitPoints = 15;
}

CShip::~CShip()
{
}

void CShip::Draw()
{
	glPushMatrix();

	glTranslatef(translation[0], translation[1], translation[2]);
	glRotatef(rotation[0], 1, 0, 0);
	glRotatef(rotation[1], 0, 1, 0);
	glRotatef(rotation[2], 0, 0, 1);
	glScalef(scale[0], scale[1], scale[2]);
	
	//The engines
	glBegin (GL_QUADS);								
		glColor3f (color[0], color[1], color[2]);	glVertex3f( -0.4, -0.5f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f(-.5f, -.9f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( -.2f, -.9f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( -0.3f, -.5f, 0.0f);
	glEnd ();	

	glBegin (GL_QUADS);								
		glColor3f (color[0], color[1], color[2]);	glVertex3f( 0.4, -0.5f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( 0.5f, -.9f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( 0.2f, -.9f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( 0.3f, -.5f, 0.0f);
	glEnd ();

	glBegin (GL_LINE_LOOP);								
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( -0.4, -0.5f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f(-.5f, -.9f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( -.2f, -.9f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( -0.3f, -.5f, 0.0f);
	glEnd ();	

	glBegin (GL_LINE_LOOP);								
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 0.4, -0.5f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 0.5f, -.9f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 0.2f, -.9f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 0.3f, -.5f, 0.0f);
	glEnd ();

	//Laser guns!
	glBegin(GL_QUADS);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( -0.35, 0.5f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f(-.3f, 0.5f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( -.3f, 0.3f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( -0.35f, 0.3f, 0.0f);		

		glColor3f (color[0], color[1], color[2]);	glVertex3f( 0.35, 0.5f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f(.3f, 0.5f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( .3f, 0.3f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( 0.35f, 0.3f, 0.0f);

		glColor3f (color[0], color[1], color[2]);	glVertex3f( -0.55, 0.1f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f(-.5f, 0.1f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( -.5f, -0.1f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( -0.55f, -0.1f, 0.0f);

		glColor3f (color[0], color[1], color[2]);	glVertex3f( 0.55, 0.1f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f(.5f, 0.1f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( .5f, -0.1f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( 0.55f, -0.1f, 0.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glColor3f (1, 1, 1);	
		glVertex3f( -0.35, 0.5f, 0.0f);
		glVertex3f(-.3f, 0.5f, 0.0f);
		glVertex3f( -.3f, 0.3f, 0.0f);
		glVertex3f( -0.35f, 0.3f, 0.0f);		
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3f( 0.35, 0.5f, 0.0f);
		glVertex3f(.3f, 0.5f, 0.0f);
		glVertex3f( .3f, 0.3f, 0.0f);
		glVertex3f( 0.35f, 0.3f, 0.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3f( -0.55, 0.1f, 0.0f);
		glVertex3f(-.5f, 0.1f, 0.0f);
		glVertex3f( -.5f, -0.1f, 0.0f);
		glVertex3f( -0.55f, -0.1f, 0.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3f( 0.55, 0.1f, 0.0f);
		glVertex3f(.5f, 0.1f, 0.0f);
		glVertex3f( .5f, -0.1f, 0.0f);
		glVertex3f( 0.55f, -0.1f, 0.0f);
	glEnd();

	//The body of the ship
	glBegin (GL_QUADS);								
		glColor3f (color[0], color[1], color[2]);	glVertex3f( 0.0f, 1.0f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f(1.0f, -1.0f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( 0.0f, -0.5f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glVertex3f( -1.0f,-1.0f, 0.0f);
	glEnd ();	

	glBegin(GL_LINE_LOOP);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 0.0f, 1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f(1.0f, -1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 0.0f, -0.5f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( -1.0f,-1.0f, 0.0f);
	glEnd();

	//Some little windows
	glBegin (GL_TRIANGLES);								
		glColor3f (0, 0, 0);	glVertex3f( 0.02f, 0.9f, 0.0f);
		glColor3f (0, 0, 0);	glVertex3f(0.15f, 0.6f, 0.0f);
		glColor3f (0, 0, 0);	glVertex3f( 0.02f, 0.6f, 0.0f);

		glColor3f (0, 0, 0);	glVertex3f( -0.02f, 0.9f, 0.0f);
		glColor3f (0, 0, 0);	glVertex3f(-0.15f, 0.6f, 0.0f);
		glColor3f (0, 0, 0);	glVertex3f( -0.02f, 0.6f, 0.0f);
	glEnd ();	

	//Some wing decorations
	glBegin (GL_LINE_LOOP);								
		glColor3f (0, 0, 0);	glVertex3f( -0.05f, 0.55f, 0.0f);
		glColor3f (0, 0, 0);	glVertex3f(-.9f, -.9f, 0.0f);
		glColor3f (0, 0, 0);	glVertex3f( -0.05f, -0.45f, 0.0f);
	glEnd ();	

	glBegin (GL_LINE_LOOP);								
		glColor3f (0, 0, 0);	glVertex3f( 0.05f, 0.55f, 0.0f);
		glColor3f (0, 0, 0);	glVertex3f( 0.9f, -.9f, 0.0f);
		glColor3f (0, 0, 0);	glVertex3f( 0.05f, -0.45f, 0.0f);
	glEnd();


	if(joystate->rgbButtons[1] & 0x80)
	{
		//Make some rocket flames!
		//Cycle flame color
		for(int i=0; i<2; i++)
		{
			glPushMatrix();
			glScalef(1.2, 1.2, 1);
			if(i==0)
				glTranslatef(-.3, 0, 0);
			else
				glTranslatef(.3, 0, 0);

			flameColor[1] = flameColor[1] - .1f;
			if(flameColor[1] < 0)
				flameColor[1] += 1.0f;
			glColor3f(flameColor[0], flameColor[1], flameColor[2]);

			animVal++;

			int flameSize = 8;

			if(animVal % 2 == 0)
			{
				glBegin(GL_LINE_LOOP);
					glVertex3f(-0.073740899562835693f, -0.34192359447479248f, .1f);
					
					glVertex3f(-0.04424f, -0.34192359447479248f * 4*flameSize/6.0f, .1f);
					glVertex3f(-0.01478f, -0.34192359447479248f, -.1f);
					glVertex3f( 0.01468f, -0.34192359447479248f * 4*flameSize/6.0f, -.1f);
					glVertex3f( 0.04414f,  -0.34192359447479248f, -.1f);

					glVertex3f(0.073662996292114258f, -0.34201046824455261f, -.1f);
				glEnd();
			}

			else if(animVal % 2 == 1)
			{	
				glBegin(GL_LINE_LOOP);
					glVertex3f(-0.073740899562835693f, -0.34192359447479248f, -.1f);
					
					glVertex3f(-0.04424f, -0.34192359447479248f, -.1f);
					glVertex3f(-0.01478f, -0.34192359447479248f * 4*flameSize/6.0f, -.1f);
					glVertex3f(0.01468f, -0.34192359447479248f, -.1f);
					glVertex3f( 0.04414f,  -0.34192359447479248f * 4*flameSize/6.0f, -.1f);

					glVertex3f(0.073662996292114258, -0.34201046824455261, -.1f);
				glEnd();
			}
			glPopMatrix();
		}
	}
	
	glPopMatrix();
}

bool CShip::CanDestroy(int destroyerType)
{
	bool retval = false;

	if(destroyerType == TYPE_NUKE || destroyerType == TYPE_BEAM || destroyerType == TYPE_BLACKHOLE ||
		destroyerType == TYPE_PLANET)
	{
		retval = true;
	}
	else if(destroyerType == TYPE_BULLET)
	{
		hitPoints--;

		if(hitPoints < 0)
			retval = true;
	}

	return retval;
}

void CShip::ProcessMotion(DWORD milliseconds, Keys *keys)
{
	//Add motion based on motion vector and elapsed time
	translation[0] += milliseconds * motionVector[0] / 1000;
	translation[1] += milliseconds * motionVector[1] / 1000;
	translation[2] += milliseconds * motionVector[2] / 1000;

	//Rotate based on angular velocity and elapse time
	rotation[0] += milliseconds * angularVelocity[0] / 1000;
	rotation[1] += milliseconds * angularVelocity[1] / 1000;
	rotation[2] += milliseconds * angularVelocity[2] / 1000;

	//Update collision sphere data
	//Update collision sphere locations
	for(unsigned int i = 0; i < collisionSpheres.size(); i++)
	{
		//First, rotate the sphere
		collisionSpheres[i]->globalPosition[0] = cos(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[0] + sin(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[1];
		collisionSpheres[i]->globalPosition[1] = -sin(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[0] + cos(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[1];
		collisionSpheres[i]->globalPosition[2] = 0;

		//Scale by the object's scale
		collisionSpheres[i]->globalPosition[0] *= scale[0];
		collisionSpheres[i]->globalPosition[1] *= scale[1];
		collisionSpheres[i]->globalPosition[2] *= scale[2];

		//Translate by the object's translation
		collisionSpheres[i]->globalPosition[0] += translation[0];
		collisionSpheres[i]->globalPosition[1] += translation[1];
		collisionSpheres[i]->globalPosition[2] += translation[2];
	}

	//Loop at edge of screen
	if(translation[1] > 1.70f)
		translation[1] = -1.70f;
	else if(translation[1] < -1.70f)
		translation[1] = 1.70f;

	//Turn around if way off in no-mans-land
	if(translation[0] > 2.75f || translation[0] < -2.75f)
		motionVector[0] *= -1;

	timeSinceLastFire += milliseconds;
}

void CShip::ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells)
{
	float vectorToGravity[3];
	float forceVector[3];
	float distance;
	float tangent;
	float pointingVector[3];
	float vectorLen;
	int direction = 1;

	for(unsigned int i=0; i<gWells.size(); i++)
	{
		//Determine vector from this object to gravity well
		vectorToGravity[0] = gWells[i]->translation[0] - translation[0];
		vectorToGravity[1] = gWells[i]->translation[1] - translation[1];
		vectorToGravity[2] = gWells[i]->translation[2] - translation[2];
		
		//Find distance to gravity well
		distance = sqrt(pow(vectorToGravity[0], 2) + pow(vectorToGravity[1], 2) + pow(vectorToGravity[2], 2));
		
		//Make vectorToGravity a unit vector
		vectorToGravity[0] /= distance;
		vectorToGravity[1] /= distance;
		vectorToGravity[2] /= distance;
		
		//Compute force vector due to gravity
		forceVector[0] = vectorToGravity[0] * (gWells[i]->mass / pow(distance, 2));
		forceVector[1] = vectorToGravity[1] * (gWells[i]->mass / pow(distance, 2));
		forceVector[2] = vectorToGravity[2] * (gWells[i]->mass / pow(distance, 2));

		if(forceVector[0] > gameParams->maxGravityForce)
			forceVector[0] = gameParams->maxGravityForce;
		if(forceVector[1] > gameParams->maxGravityForce)
			forceVector[1] = gameParams->maxGravityForce;
		if(forceVector[2] > gameParams->maxGravityForce)
			forceVector[2] = gameParams->maxGravityForce;

		if(forceVector[0] < -gameParams->maxGravityForce)
			forceVector[0] = -gameParams->maxGravityForce;
		if(forceVector[1] < -gameParams->maxGravityForce)
			forceVector[1] = -gameParams->maxGravityForce;
		if(forceVector[2] < -gameParams->maxGravityForce)
			forceVector[2] = -gameParams->maxGravityForce;

		//Add force vector to this object's motionVector
		motionVector[0] += forceVector[0] * milliseconds / 1000;
		motionVector[1] += forceVector[1] * milliseconds / 1000;
		//motionVector[2] += forceVector[2] * milliseconds / 1000;		
	}

	//Add motion due to joystick
	if(joystate->lX < 28768 || joystate->lX > 36768)
		angularVelocity[2] = -(joystate->lX - 32768) / 100;
	else
		angularVelocity[2] = 0;

	if(joystate->rgbButtons[1] & 0x80)
	{
		pointingVector[0] = cos((rotation[2] + 90) * DEG2RAD);
		pointingVector[1] = sin((rotation[2] + 90) * DEG2RAD);
		
		motionVector[0] += pointingVector[0] / 50;
		motionVector[1] += pointingVector[1] / 50;
	}
}

bool CShip::IsFiring() 
{ 
	bool retval;

	if(timeSinceLastFire > 100)
	{
		retval = joystate->rgbButtons[0]; 
		timeSinceLastFire = 0;
	}
	else
		retval = false;

	return retval;
}


CBullet::CBullet(sGameParams *newGameParams) : CGameObject(newGameParams)
{
	sCollisionSphere *cSphere  = new sCollisionSphere();
	cSphere->translation[0] = 0;
	cSphere->translation[1] = 0;
	cSphere->translation[2] = 0;
	cSphere->radius = 1;
	cSphere->globalPosition[0] = -10;
	cSphere->globalPosition[1] = -10;
	cSphere->globalPosition[2] = -10;
	collisionSpheres.push_back(cSphere);

	TTL = 3000;
}

CBullet::~CBullet()
{
}

void CBullet::Draw()
{
	glPushMatrix();
	
	glTranslatef(translation[0], translation[1], translation[2]);
	glRotatef(rotation[0], 1, 0, 0);
	glRotatef(rotation[1], 0, 1, 0);
	glRotatef(rotation[2], 0, 0, 1);
	glScalef(scale[0], scale[1], scale[2]);
	glColor3f(color[0], color[1], color[2]);

	glBegin(GL_QUADS);
		glVertex3f(-.05f, -.05f, -.01f);	
		glVertex3f(.05f, -.05f, -.01f);
		glVertex3f(.05f, .05f, -.01f);
		glVertex3f(-.05f, .05f, -.01f);
	glEnd();

	glBegin(GL_TRIANGLES);
		glVertex3f(.05f, .05f, -.01f);
		glVertex3f(-.05f, .05f, -.01f);
		glVertex3f(0, .2f, -.001f);
	glEnd();

	glColor3f(1, 1, 1);
	glBegin(GL_LINE_LOOP);
		glVertex3f(-.05f, -.05f, -.01f);	
		glVertex3f(.05f, -.05f, -.01f);
		glVertex3f(.05f, .05f, -.01f);
		glVertex3f(0, .2f, -.01f);
		glVertex3f(-.05f, .05f, -.01f);
	glEnd();

	glPopMatrix();

}


void CBullet::ProcessMotion(DWORD milliseconds, Keys* keys)
{
	//Add motion based on motion vector and elapsed time
	translation[0] += milliseconds * motionVector[0] / 1000;
	translation[1] += milliseconds * motionVector[1] / 1000;
	translation[2] += milliseconds * motionVector[2] / 1000;

	//Rotate based on angular velocity and elapse time
	rotation[0] += milliseconds * angularVelocity[0] / 1000;
	rotation[1] += milliseconds * angularVelocity[1] / 1000;
	rotation[2] += milliseconds * angularVelocity[2] / 1000;

	//Update collision sphere data
	//Update collision sphere locations
	for(unsigned int i = 0; i < collisionSpheres.size(); i++)
	{
		//First, rotate the sphere
		collisionSpheres[i]->globalPosition[0] = cos(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[0] + sin(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[1];
		collisionSpheres[i]->globalPosition[1] = -sin(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[0] + cos(-(rotation[2]) * DEG2RAD)*collisionSpheres[i]->translation[1];
		collisionSpheres[i]->globalPosition[2] = 0;

		//Scale by the object's scale
		collisionSpheres[i]->globalPosition[0] *= scale[0];
		collisionSpheres[i]->globalPosition[1] *= scale[1];
		collisionSpheres[i]->globalPosition[2] *= scale[2];

		//Translate by the object's translation
		collisionSpheres[i]->globalPosition[0] += translation[0];
		collisionSpheres[i]->globalPosition[1] += translation[1];
		collisionSpheres[i]->globalPosition[2] += translation[2];
	}

	if(translation[1] > 1.70f)
		translation[1] = -1.70f;
	else if(translation[1] < -1.70f)
		translation[1] = 1.70f;

	if(translation[0] > 2.75f || translation[0] < -2.75f)
		deleteMe = true;

	TTL -= milliseconds;

	if(TTL < 0)
		deleteMe = true;
}