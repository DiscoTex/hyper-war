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

void CGameObject::ProcessMotion(DWORD milliseconds)
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
		collisionSpheres[i]->globalPosition[0] = cos(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[0] + sin(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[1];
		collisionSpheres[i]->globalPosition[1] = -sin(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[0] + cos(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[1];
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
		motionVector[1] += forceVector[1] * milliseconds / 1000;
		motionVector[2] += forceVector[2] * milliseconds / 1000;
	}
}

//Return the index of collided object
int CGameObject::CheckCollision(vector< CGameObject* > gObjects, DWORD milliseconds)
{
	float relVelVector[3];
	float relPosVector[3];	

	for(unsigned int k=0; k<collisionSpheres.size(); k++)
	{
		//Check for intersection with all other objects in the game
		for(unsigned int i=0; i<gObjects.size(); i++)
		{
			for(unsigned int j=0; j<gObjects[i]->GetCollisionSpheres().size(); j++)
			{
				if(gObjects[i] != this)
				{
					// Relative velocity
					relVelVector[0] = (gObjects[i]->GetMotionVector()[0] - motionVector[0]);
					relVelVector[1] = (gObjects[i]->GetMotionVector()[1] - motionVector[1]);
					relVelVector[2] = (gObjects[i]->GetMotionVector()[2] - motionVector[2]);

					relVelVector[0] /= 60;
					relVelVector[1] /= 60;
					relVelVector[2] /= 60;
					
					// Relative position
					//relPosVector[0] = gObjects[i]->GetPosition()[0] - translation[0];
					//relPosVector[1] = gObjects[i]->GetPosition()[1] - translation[1];
					//relPosVector[2] = gObjects[i]->GetPosition()[2] - translation[2];

					relPosVector[0] = gObjects[i]->GetCollisionSpheres()[j]->globalPosition[0] - collisionSpheres[k]->globalPosition[0];
					relPosVector[1] = gObjects[i]->GetCollisionSpheres()[j]->globalPosition[1] - collisionSpheres[k]->globalPosition[1];
					relPosVector[2] = gObjects[i]->GetCollisionSpheres()[j]->globalPosition[2] - collisionSpheres[k]->globalPosition[2];

					//Minimal distance
					float r = collisionSpheres[k]->radius * scale[0] + gObjects[i]->GetCollisionSpheres()[j]->radius * gObjects[i]->GetScale()[0];

					//dP^2-r^2
					float pp = relPosVector[0] * relPosVector[0] + relPosVector[1] * relPosVector[1] + relPosVector[2] * relPosVector[2] - r*r;

					//(1)Check if the spheres are already intersecting
					if ( pp < 0 ) 
						return i;
/*
					//Now check to see if the spheres will intersect within 1/60th of a second
					//dP*dV
					float pv = relPosVector[0] * relVelVector[0] + relPosVector[1] * relVelVector[1] + relPosVector[2] * relVelVector[2];
					//(2)Check if the spheres are moving away from each other
					if ( pv < 0 ) 
					{
						//dV^2
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
							//float tmin = -pv/vv;

							//Discriminant/(4*dV^2) = -(dp^2-r^2+dP*dV*tmin)
							//if( pp + pv * tmin > 0 )
							//	return i;
						}
					}
					*/
				}
			}
		}
	}

	return -1;
}

int CGameObject::GetType()
{
	return TYPE_GENERIC;
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

	float radius = 1;
	glBegin(GL_TRIANGLES);
		for (int i=0; i<360; i+=4)
		{
			float degInRad = i*DEG2RAD;
			glColor3f(color[0], color[1], color[2]);  glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius, 0);

			degInRad = (i+4)*DEG2RAD;
			glColor3f(color[0], color[1], color[2]);  glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius, 0);

			glColor3f(color[0], color[1], color[2]);  glVertex3f(0, 0, 0);
		}
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
		for (int i=0; i<360; i+=4)
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


// CNuke

CNuke::CNuke()
{
	//Create collision spheres
	sCollisionSphere* cSphere = new sCollisionSphere;
	cSphere->translation[0] = 0;
	cSphere->translation[1] = .25;
	cSphere->translation[2] = 0;
	cSphere->radius = .2f;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	cSphere = new sCollisionSphere;
	cSphere->translation[0] = 0;
	cSphere->translation[1] = -.15f;
	cSphere->translation[2] = 0;
	cSphere->radius = .25;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

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

	
	glPushMatrix();

	/*
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
	*/
	
}

void CNuke::ProcessMotion(DWORD milliseconds)
{
	if(translation[1] > 1.75)
		translation[1] = -1.75;
	else if(translation[1] < -1.75)
		translation[1] = 1.75;

	if(translation[0] > 2.75)
		translation[0] = -2.75;
	else if(translation[0] < -2.75)
		translation[0] = 2.75;

	//Add motion based on motion vector and elapsed time
	translation[0] += milliseconds * motionVector[0] / 1000;
	translation[1] += milliseconds * motionVector[1] / 1000;
	translation[2] += milliseconds * motionVector[2] / 1000;

	//Rotate to point to the direction of motion
	rotation[2] = atan2(motionVector[1] , motionVector[0]) / DEG2RAD;

	//Update collision sphere locations
	for(unsigned int i = 0; i < collisionSpheres.size(); i++)
	{
		//First, rotate the sphere
		collisionSpheres[i]->globalPosition[0] = cos(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[0] + sin(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[1];
		collisionSpheres[i]->globalPosition[1] = -sin(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[0] + cos(-(rotation[2]-90) * DEG2RAD)*collisionSpheres[i]->translation[1];
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
}

int CNuke::GetType()
{
	return TYPE_NUKE;
}