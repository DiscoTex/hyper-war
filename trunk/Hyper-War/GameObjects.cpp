/*************************************************************/
/*	GameObjects.cpp: definitions of game objects, including	 */
/*  the base class, CGameObject and several derived classes	 */					
/*  Aaron E. Wegner											 */
/*	July 2009												 */
/*************************************************************/

#include "GameObjects.h"

//Generic Game Object
CGameObject::CGameObject()
{
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
		if(translation[1] > 1.75)
			translation[1] = -1.75;
		else if(translation[1] < -1.75)
			translation[1] = 1.75;

		if(translation[0] > 2.75)
			translation[0] = -2.75;
		else if(translation[0] < -2.75)
			translation[0] = 2.75;
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

		if(forceVector[0] > MAX_GRAVITY_FORCE)
			forceVector[0] = MAX_GRAVITY_FORCE;
		if(forceVector[1] > MAX_GRAVITY_FORCE)
			forceVector[1] = MAX_GRAVITY_FORCE;
		if(forceVector[2] > MAX_GRAVITY_FORCE)
			forceVector[2] = MAX_GRAVITY_FORCE;

		if(forceVector[0] < -MAX_GRAVITY_FORCE)
			forceVector[0] = -MAX_GRAVITY_FORCE;
		if(forceVector[1] < -MAX_GRAVITY_FORCE)
			forceVector[1] = -MAX_GRAVITY_FORCE;
		if(forceVector[2] < -MAX_GRAVITY_FORCE)
			forceVector[2] = -MAX_GRAVITY_FORCE;

		//Add force vector to this object's motionVector
		motionVector[0] += forceVector[0] * milliseconds / 1000;
		//motionVector[1] += forceVector[1] * milliseconds / 1000;
		//motionVector[2] += forceVector[2] * milliseconds / 1000;
	}
}

//Return the index of collided object
int CGameObject::CheckCollision(vector< CGameObject* > gObjects, DWORD milliseconds, unsigned int checkAfterIndex)
{
	float relVelVector[3];
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
			if(otherType == TYPE_DEBRIS)
				continue;
			break;
		case TYPE_PLANET:
			if(otherType == TYPE_PLANET || otherType == TYPE_MISSILEBASE || otherType ==  TYPE_CITY || otherType == TYPE_FLAKCANNON)
				continue;
			break;
		case TYPE_MISSILEBASE:
			if(otherType == TYPE_PLANET || otherType ==  TYPE_DEBRIS)
				continue;
		case TYPE_CITY:
			if(otherType == TYPE_PLANET || otherType ==  TYPE_DEBRIS)
				continue;
			break;
		case TYPE_FLAKCANNON:
			if(otherType == TYPE_PLANET || otherType ==  TYPE_DEBRIS)
				continue;
			break;
		}

		if(gObjects[i] == this)
			continue;
		
		for(unsigned int k=0; k<collisionSpheres.size(); k++)
		{
			for(unsigned int j=0; j<gObjects[i]->GetCollisionSpheres().size(); j++)
			{
				// Relative velocity
				relVelVector[0] = (gObjects[i]->GetMotionVector()[0] - motionVector[0]);
				relVelVector[1] = (gObjects[i]->GetMotionVector()[1] - motionVector[1]);
				relVelVector[2] = (gObjects[i]->GetMotionVector()[2] - motionVector[2]);

				//Motion vector is in units per second
				//Assume we are getting at least 60 fps
				relVelVector[0] /= 60;
				relVelVector[1] /= 60;
				relVelVector[2] /= 60;
				
				// Relative position
				relPosVector[0] = gObjects[i]->GetCollisionSpheres()[j]->globalPosition[0] - collisionSpheres[k]->globalPosition[0];
				relPosVector[1] = gObjects[i]->GetCollisionSpheres()[j]->globalPosition[1] - collisionSpheres[k]->globalPosition[1];
				relPosVector[2] = gObjects[i]->GetCollisionSpheres()[j]->globalPosition[2] - collisionSpheres[k]->globalPosition[2];

				//Minimal distance to avoid collision
				float r = collisionSpheres[k]->radius * scale[0] + gObjects[i]->GetCollisionSpheres()[j]->radius * gObjects[i]->GetScale()[0];

				//(Distance between the objects)^2 - (Minimal distance)^2
				float pp = relPosVector[0] * relPosVector[0] + relPosVector[1] * relPosVector[1] + relPosVector[2] * relPosVector[2] - r*r;

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

CPlanet::CPlanet()
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
		for (int i=0; i<360; i+=2)
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
		for (int i=0; i<360; i+=2)
		{
			float degInRad = i*DEG2RAD;
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

CNuke::CNuke()
{
	//Create collision spheres
	sCollisionSphere* cSphere = new sCollisionSphere;
	cSphere->translation[0] = .25;
	cSphere->translation[1] = 0;
	cSphere->translation[2] = 0;
	cSphere->radius = .2f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	cSphere = new sCollisionSphere;
	cSphere->translation[0] = -.15f;
	cSphere->translation[1] = 0.0f;
	cSphere->translation[2] = 0.0f;
	cSphere->radius = .25;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	thrust = .3;

	//Cycle colors
	flameColor[0] = 1.0f;
	flameColor[1] = rand() / INT_MAX;
	flameColor[2] = 0;

	animVal = 0;
	lastLaunch = 0;
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
		//glVertex3f(cos(200 * DEG2RAD)*radius - cos(150.0f*DEG2RAD), sin(200 * DEG2RAD)*radius, -.001f);
		//glVertex3f(cos(-20 * DEG2RAD)*radius - cos(-30.0f*DEG2RAD), sin(-20 * DEG2RAD)*radius, -.001f);
		glVertex3f(-0.073740899562835693, -0.34192359447479248, -.001);
		glVertex3f(0.073662996292114258, -0.34201046824455261, -.001);
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


	//**** Create outlines ****/

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
		//glVertex3f(cos(200 * DEG2RAD)*radius - cos(150.0f*DEG2RAD), sin(200 * DEG2RAD)*radius, -.00001f);
		//glVertex3f(cos(-20 * DEG2RAD)*radius - cos(-30.0f*DEG2RAD), sin(-20 * DEG2RAD)*radius, -.00001f);
		glVertex3f(-0.073740899562835693, -0.34192359447479248, -.00001f);
		glVertex3f(0.073662996292114258, -0.34201046824455261, -.00001f);
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

	//Make some rocket flames!
	//Cycle flame color
	flameColor[1] = flameColor[1] - .1f;
	if(flameColor[1] < 0)
		flameColor[1] += 1.0f;
	glColor3f(flameColor[0], flameColor[1], flameColor[2]);

	animVal++;

	int flameSize = (thrust + .3) / .2;

	if(animVal % 6 == 0)
	{
		glBegin(GL_LINE_LOOP);
			glVertex3f(-0.073740899562835693, -0.34192359447479248, -.00001f);
			
			glVertex3f(-0.053740899562835693, -0.36192359447479248 * flameSize, -.00001f);
			glVertex3f(-0.033740899562835693, -0.34192359447479248, -.00001f);
			glVertex3f(-0.013740899562835693, -0.36192359447479248 * flameSize, -.00001f);
			glVertex3f( 0.05740899562835693,  -0.34192359447479248, -.00001f);

			glVertex3f(0.073662996292114258, -0.34201046824455261, -.00001f);
		glEnd();
	}

	else if(animVal % 6 == 1)
	{	
		glBegin(GL_LINE_LOOP);
			glVertex3f(-0.073740899562835693, -0.34192359447479248, -.00001f);
			
			glVertex3f(-0.053740899562835693, -0.34192359447479248, -.00001f);
			glVertex3f(-0.033740899562835693, -0.36192359447479248 * flameSize, -.00001f);
			glVertex3f(-0.013740899562835693, -0.34192359447479248, -.00001f);
			glVertex3f( 0.05740899562835693,  -0.36192359447479248 * flameSize, -.00001f);

			glVertex3f(0.073662996292114258, -0.34201046824455261, -.00001f);
		glEnd();
	}

	else if(animVal % 6 == 2)
	{
		glBegin(GL_LINE_LOOP);
			glVertex3f(-0.073740899562835693, -0.34192359447479248, -.00001f);
			
			glVertex3f(-0.053740899562835693, -0.37192359447479248 * flameSize, -.00001f);
			glVertex3f(-0.033740899562835693, -0.34192359447479248, -.00001f);
			glVertex3f(-0.013740899562835693, -0.37192359447479248 * flameSize, -.00001f);
			glVertex3f( 0.05740899562835693,  -0.34192359447479248, -.00001f);

			glVertex3f(0.073662996292114258, -0.34201046824455261, -.00001f);
		glEnd();
	}

	if(animVal % 6 == 3)
	{	
		glBegin(GL_LINE_LOOP);
			glVertex3f(-0.073740899562835693, -0.34192359447479248, -.00001f);
			
			glVertex3f(-0.053740899562835693, -0.34192359447479248, -.00001f);
			glVertex3f(-0.033740899562835693, -0.38192359447479248 * flameSize, -.00001f);
			glVertex3f(-0.013740899562835693, -0.34192359447479248, -.00001f);
			glVertex3f( 0.05740899562835693,  -0.38192359447479248 * flameSize, -.00001f);

			glVertex3f(0.073662996292114258, -0.34201046824455261, -.00001f);
		glEnd();
	}

	if(animVal % 6 == 4)
	{
		glBegin(GL_LINE_LOOP);
			glVertex3f(-0.073740899562835693, -0.34192359447479248, -.00001f);
			
			glVertex3f(-0.053740899562835693, -0.39192359447479248 * flameSize, -.00001f);
			glVertex3f(-0.033740899562835693, -0.34192359447479248, -.00001f);
			glVertex3f(-0.013740899562835693, -0.39192359447479248 * flameSize, -.00001f);
			glVertex3f( 0.05740899562835693,  -0.34192359447479248, -.00001f);

			glVertex3f(0.073662996292114258, -0.34201046824455261, -.00001f);
		glEnd();
	}

	if(animVal % 6 == 5)
	{	
		glBegin(GL_LINE_LOOP);
			glVertex3f(-0.073740899562835693, -0.34192359447479248, -.00001f);
			
			glVertex3f(-0.053740899562835693, -0.34192359447479248, -.00001f);
			glVertex3f(-0.033740899562835693, -0.39192359447479248 * flameSize, -.00001f);
			glVertex3f(-0.013740899562835693, -0.34192359447479248, -.00001f);
			glVertex3f( 0.01740899562835693,  -0.39192359447479248 * flameSize, -.00001f);

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
	translation[0] += milliseconds * motionVector[0] / 10000;
	translation[1] += milliseconds * motionVector[1] / 10000;
	translation[2] += milliseconds * motionVector[2] / 10000;

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

	if(translation[0] > 2.75)
		translation[0] = -2.75;
	else if(translation[0] < -2.75)
		translation[0] = 2.75;
}

float *CNuke::GetMotionVector()
{
	tempVec[0] = motionVector[0]/10.0f;
	tempVec[1] = motionVector[1]/10.0f;
	tempVec[2] = motionVector[2]/10.0f;

	return tempVec;
}

int CNuke::GetType()
{
	return TYPE_NUKE;
}

CDebris::CDebris()
{
	//Pick a random debris type
	debType = rand() * 4 / 32768;

	sCollisionSphere* cSphere;

	switch(debType)
	{
	case DEBRIS_TYPE_0:
		cSphere  = new sCollisionSphere;
		cSphere->translation[0] = -.19f;
		cSphere->translation[1] = 0;
		cSphere->translation[2] = 0;
		cSphere->radius = .05f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);

		cSphere = new sCollisionSphere;
		cSphere->translation[0] = 0;
		cSphere->translation[1] = .19f;
		cSphere->translation[2] = 0;
		cSphere->radius = .05f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);

		cSphere  = new sCollisionSphere;
		cSphere->translation[0] = -.06f;
		cSphere->translation[1] = 0;
		cSphere->translation[2] = 0;
		cSphere->radius = .05f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);

		cSphere = new sCollisionSphere;
		cSphere->translation[0] = 0;
		cSphere->translation[1] = .06f;
		cSphere->translation[2] = 0;
		cSphere->radius = .05f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);
		break;
	case DEBRIS_TYPE_1:
		cSphere = new sCollisionSphere;
		cSphere->translation[0] = 0;
		cSphere->translation[1] = .19f;
		cSphere->translation[2] = 0;
		cSphere->radius = .05f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);

		cSphere  = new sCollisionSphere;
		cSphere->translation[0] = -.06f;
		cSphere->translation[1] = 0;
		cSphere->translation[2] = 0;
		cSphere->radius = .05f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);

		cSphere = new sCollisionSphere;
		cSphere->translation[0] = 0;
		cSphere->translation[1] = .06f;
		cSphere->translation[2] = 0;
		cSphere->radius = .05f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);
		break;
	case DEBRIS_TYPE_2:
		cSphere  = new sCollisionSphere;
		cSphere->translation[0] = -.19f;
		cSphere->translation[1] = 0;
		cSphere->translation[2] = 0;
		cSphere->radius = .05f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);

		cSphere  = new sCollisionSphere;
		cSphere->translation[0] = -.06f;
		cSphere->translation[1] = 0;
		cSphere->translation[2] = 0;
		cSphere->radius = .05f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);

		cSphere = new sCollisionSphere;
		cSphere->translation[0] = 0;
		cSphere->translation[1] = .06f;
		cSphere->translation[2] = 0;
		cSphere->radius = .05f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);
		break;
	case DEBRIS_TYPE_3:
		cSphere  = new sCollisionSphere;
		cSphere->translation[0] = -.06f;
		cSphere->translation[1] = 0;
		cSphere->translation[2] = 0;
		cSphere->radius = .05f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);

		cSphere = new sCollisionSphere;
		cSphere->translation[0] = 0;
		cSphere->translation[1] = .06f;
		cSphere->translation[2] = 0;
		cSphere->radius = .05f;
		cSphere->globalPosition[0] = 0;
		cSphere->globalPosition[1] = 0;
		cSphere->globalPosition[2] = 0;
		collisionSpheres.push_back(cSphere);
		break;
	}	

	color[0] = 1;
	color[1] = 0;
	color[2] = 0;

	angularVelocity[0] = float(rand() % 500);
	angularVelocity[1] = float(rand() % 500);
	angularVelocity[2] = float(rand() % 500);

	rotation[2] = float(rand() % 500);

	TTL = 1500 - rand()%500;
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

	if(translation[0] > 14)
		translation[0] = -14;
	else if(translation[0] < -14)
		translation[0] = 14;

	TTL -= milliseconds;
}

bool CDebris::CanDestroy(int destroyerType)
{
	if(destroyerType == TYPE_DEBRIS)
		return false;
	else
		return true;	
}

CMissileBase::CMissileBase()
{
	sCollisionSphere *cSphere;

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = .50;
	cSphere->translation[1] = .1;
	cSphere->translation[2] = 0;
	cSphere->radius = .50f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	loaded = true;
	launchKey = '\0';
	launchReady = false;
}

CMissileBase::~CMissileBase()
{
}

int CMissileBase::Launch()
{
	int oldCharge = charge;

	loaded = false; 
	launchReady = false; 
	timeToReload = 5000; 
	charge = 0;

	return oldCharge;
}

void CMissileBase::ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells)
{
	if(timeToReload > 0)
	{
		timeToReload -= milliseconds;
	}
	else
		loaded = true;
}

bool CMissileBase::CanDestroy(int destroyerType)
{
	//if(destroyerType != TYPE_NUKE)
		return false;
	//else
	//	return true;
}

float* CMissileBase::GetNukeTranslation()
{
	float nukeTranslation[3];
	float rotatedNuke[3];

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
	float nukeVector[3];
	float divisor;
	float nukePos[3];

	//scale then rotate it
	nukePos[0] = .5*scale[0] * cos(rotation[2] * DEG2RAD) - 1.0*scale[1] * sin(rotation[2] * DEG2RAD);
	nukePos[1] = .5*scale[0] * sin(rotation[2] * DEG2RAD) + 1.0*scale[1] * cos(rotation[2] * DEG2RAD);
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

	if(loaded)
	{
		//Draw a missile on the launch pad
		//The missile gets its own coordinate system
		glPushMatrix();

		float nukePos[2];

		//rotate it
		nukePos[0] = .5 * cos(rotation[2] * DEG2RAD) - 1.0 * sin(rotation[2] * DEG2RAD);
		nukePos[1] = .5 * sin(rotation[2] * DEG2RAD) + 1.0 * cos(rotation[2] * DEG2RAD);

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


CCity::CCity()
{
	sCollisionSphere *cSphere;

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = .9;
	cSphere->translation[1] = .4;
	cSphere->translation[2] = 0;
	cSphere->radius = .4f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = .9f;
	cSphere->translation[1] = .7;
	cSphere->translation[2] = 0;
	cSphere->radius = .4f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = 0;
	cSphere->translation[1] = .25;
	cSphere->translation[2] = 0;
	cSphere->radius = .25f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = 0;
	cSphere->translation[1] = .45;
	cSphere->translation[2] = 0;
	cSphere->radius = .25f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	cSphere  = new sCollisionSphere;
	cSphere->translation[0] = .35;
	cSphere->translation[1] = .25;
	cSphere->translation[2] = 0;
	cSphere->radius = .25f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);
}

CCity::~CCity()
{
}

void CCity::ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells)
{
}

bool CCity::CanDestroy(int destroyerType)
{
	if(destroyerType != TYPE_NUKE)
		return false;
	else
		return true;
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

		glColor3f(color[0] *.6, color[1]*.6, color[2]*.6);
		
		glVertex3f(0, 0, -.01);
		glVertex3f(1, 0, -.01);
		glVertex3f(1, .5f, -.01);
		glVertex3f(0, .5f, -.01);

		glColor3f(color[0] *.8, color[1]*.8, color[2]*.8);

		glVertex3f(-.25, 0, -.001);
		glVertex3f(.25, 0, -.001);
		glVertex3f(.25, .7f, -.001);
		glVertex3f(-.25, .7f, -.001);

		glColor3f(color[0], color[1], color[2]);
		
		glVertex3f(.5, 0, -.001);
		glVertex3f(1.3, 0, -.001);
		glVertex3f(1.3, 1.1 , -.001);
		glVertex3f(.5, 1.1 , -.001);
	glEnd();

	glColor3f(1, 1, 1);
	glBegin(GL_LINE_LOOP);
		glVertex3f(0, 0, -.01);
		glVertex3f(1, 0, -.01);
		glVertex3f(1, .5f, -.01);
		glVertex3f(0, .5f, -.01);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3f(-.25, 0, -.001);
		glVertex3f(.25, 0, -.001);
		glVertex3f(.25, .7f, -.001);
		glVertex3f(-.25, .7f, -.001);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3f(.5, 0, -.001);
		glVertex3f(1.3, 0, -.001);
		glVertex3f(1.3, 1.1, -.001);
		glVertex3f(.5, 1.1, -.001);
	glEnd();

	//Draw some windows
	glColor3f(0, 0, 0);

	glBegin(GL_QUADS);
		
		glVertex3f(.6, .2, 0);
		glVertex3f(.8, .2, 0);
		glVertex3f(.8, .4 ,0);
		glVertex3f(.6, .4 ,0);

		glVertex3f(1,   .2, 0);
		glVertex3f(1.2, .2, 0);
		glVertex3f(1.2, .4, 0);
		glVertex3f(1,   .4, 0);

		glVertex3f(.6, .5, 0);
		glVertex3f(.8, .5, 0);
		glVertex3f(.8, .7, 0);
		glVertex3f(.6, .7, 0);

		glVertex3f(1,   .5, 0);
		glVertex3f(1.2, .5, 0);
		glVertex3f(1.2, .7, 0);
		glVertex3f(1,   .7, 0);

		glVertex3f(.6, .8, 0);
		glVertex3f(.8, .8, 0);
		glVertex3f(.8, 1, 0);
		glVertex3f(.6, 1, 0);

		glVertex3f(1,   .8, 0);
		glVertex3f(1.2, .8, 0);
		glVertex3f(1.2, 1, 0);
		glVertex3f(1,   1, 0);

		glVertex3f(-.2, .1, 0);
		glVertex3f(.2, .1, 0);
		glVertex3f(.2, .2f, 0);
		glVertex3f(-.2, .2f, 0);

		glVertex3f(-.2, .3, 0);
		glVertex3f(.2, .3, 0);
		glVertex3f(.2, .4f, 0);
		glVertex3f(-.2, .4f, 0);

		glVertex3f(-.2, .5, 0);
		glVertex3f(.2, .5, 0);
		glVertex3f(.2, .6f, 0);
		glVertex3f(-.2, .6f, 0);

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


CFlakCannon::CFlakCannon()
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
}

CFlakCannon::~CFlakCannon()
{
}

void CFlakCannon::ProcessGravity(DWORD milliseconds, vector< sGravityWell* > gWells)
{
}

bool CFlakCannon::CanDestroy(int destroyerType)
{
	//if(destroyerType != TYPE_NUKE)
		return false;
	//else
	//	return true;
}

int	CFlakCannon::GetType()
{
	return TYPE_FLAKCANNON;
}

void CFlakCannon::Fire()
{
	loaded = false; 
	timeToReload = 25; 
}

void CFlakCannon::GetProjVector(int* TTL, float* projVector)
{
	projVector[0] = (pCursorPos[0] - translation[0]);
	projVector[1] = (pCursorPos[1] - translation[1]);
	projVector[2] = 0;//pCursorPos[2] - translation[2]/3.0;

	float magnitude = sqrt(projVector[0] * projVector[0] + projVector[1] * projVector[1]) / 6.0;

	projVector[0] /= magnitude;
	projVector[1] /= magnitude;

	*TTL = 1000 * magnitude;
}

float* CFlakCannon::GetProjTranslation()
{
	float projTranslation[3];

	projTranslation[0] = translation[0];
	projTranslation[1] = translation[1];
	projTranslation[2] = -.0010;

	return projTranslation;
}

void CFlakCannon::AddTimeSinceFired(DWORD milliseconds)
{
	timeToReload -= milliseconds;

	if(timeToReload < 0)
	{
		loaded = true; 
	}
}

void CFlakCannon::Draw()
{
	float cannonRotation = 0;

	glPushMatrix();
	
	glTranslatef(translation[0], translation[1], translation[2]);
	glRotatef(rotation[0], 1, 0, 0);
	glRotatef(rotation[1], 0, 1, 0);
	glRotatef(rotation[2], 0, 0, 1);
	glScalef(scale[0], scale[1], scale[2]);
	glColor3f(color[0], color[1], color[2]);

	glBegin(GL_QUADS);
		glVertex3f(-.5, .25, -.001);	
		glVertex3f(-.5, 0, -.001);
		glVertex3f(.5, 0, -.001);
		glVertex3f(.5, .25, -.001);
	glEnd();

	glBegin(GL_TRIANGLES);
		for (int i=0; i<180; i+=4)
		{
			float degInRad = i*DEG2RAD;
			float radius = .4;
			glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius, -.001);

			degInRad = (i+4)*DEG2RAD;
			glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius, -.001);

			glVertex3f(0, 0, -.001);
		}
	glEnd();

	glColor3f(1, 1, 1);
	glBegin(GL_LINE_STRIP);
		glVertex3f(-.5, .25, -.001);	
		glVertex3f(-.5, 0, -.001);
		glVertex3f(.5, 0, -.001);
		glVertex3f(.5, .25, -.001);
		glVertex3f(-.5, .25, -.001);	
	glEnd();

	glBegin(GL_LINE_STRIP);
		for (int i=0; i<180; i+=4)
		{
			float degInRad = i*DEG2RAD;
			float radius = .4;
			glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius, -.00125);

			degInRad = (i+4)*DEG2RAD;
			glVertex3f(cos(degInRad) * radius, sin(degInRad) * radius, -.00125);
		}
	glEnd();

	glColor3f(color[0], color[1], color[2]);

	cannonRotation = atan2(pCursorPos[1] - translation[1], pCursorPos[0] - translation[0]) / DEG2RAD;

	glPushMatrix();

	glRotatef(cannonRotation - rotation[2] - 90, 0, 0, 1);

	//Draw the cannon
	glBegin(GL_QUADS);
		glVertex3f(-.05f, 0, -.00125);
		glVertex3f(.05f, 0, -.00125);
		glVertex3f(.03f, .8f, -.00125);
		glVertex3f(-.03f, .8f, -.00125);
	glEnd();

	//Draw the cannon outline
	glColor3f(1, 1, 1);
	glBegin(GL_LINE_STRIP);
		glVertex3f(-.05f, 0, -.00125);
		glVertex3f(.05f, 0, -.00125);
		glVertex3f(.03f, .8f, -.00125);
		glVertex3f(-.03f, .8f, -.00125);
		glVertex3f(-.05f, 0, -.00125);
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

CProjectile::CProjectile()
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
		glVertex3f(-.05f, -.05f, -.01);	
		glVertex3f(.05f, -.05f, -.01);
		glVertex3f(.05f, .05f, -.01);
		glVertex3f(-.05f, .05f, -.01);
	glEnd();

	glBegin(GL_TRIANGLES);
		glVertex3f(.05f, .05f, -.01f);
		glVertex3f(-.05f, .05f, -.01f);
		glVertex3f(0, .2f, -.001f);
	glEnd();

	glColor3f(1, 1, 1);
	glBegin(GL_LINE_LOOP);
		glVertex3f(-.05f, -.05f, -.01);	
		glVertex3f(.05f, -.05f, -.01);
		glVertex3f(.05f, .05f, -.01);
		glVertex3f(0, .2f, -.01f);
		glVertex3f(-.05f, .05f, -.01);
	glEnd();

	glPopMatrix();

	//Draw a line from her to this object's origination point
	glColor3f(.2f, .2f, .2f);
	glBegin(GL_LINES);
		glVertex3f(origin[0], origin[1], -.002);
		glVertex3f(translation[0], translation[1], -.002);
	glEnd();
}