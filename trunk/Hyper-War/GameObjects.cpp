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

	glTranslated(translation[0], translation[1], translation[2]);
	glRotated(rotation[0], 1, 0, 0);
	glRotated(rotation[1], 0, 1, 0);
	glRotated(rotation[2], 0, 0, 1);
	glScaled(scale[0], scale[1], scale[2]);
	
	glBegin (GL_QUADS);								
		glColor3d (color[0], color[1], color[2]);	glVertex3d( -1.0f, 1.0f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d(1.0f, 1.0f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( 1.0f,-1.0f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( -1.0f,-1.0f, 0.0f);
	glEnd ();	

	glBegin(GL_LINE_LOOP);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( -1.0f, 1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f(1.0f, 1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 1.0f,-1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( -1.0f,-1.0f, 0.0f);
	glEnd();

	glPopMatrix();
}

void CGameObject::SetScale(double x, double y, double z)
{
	scale[0] = x;
	scale[1] = y;
	scale[2] = z;
}

void CGameObject::SetRotation(double x, double y, double z)
{
	rotation[0] = x;
	rotation[1] = y;
	rotation[2] = z;
}

void CGameObject::SetTranslation(double x, double y, double z)
{
	translation[0] = x;
	translation[1] = y;
	translation[2] = z;
}

void CGameObject::SetColor(double r, double g, double b)
{
	color[0] = r;
	color[1] = g;
	color[2] = b;
}

void CGameObject::SetMotionVector(double x, double y, double z)
{
	motionVector[0] = x;
	motionVector[1] = y;
	motionVector[2] = z;
}

void CGameObject::SetAngularVelocity(double x, double y, double z)
{
	angularVelocity[0] = x;
	angularVelocity[1] = y;
	angularVelocity[2] = z;
}

double* CGameObject::GetScale(double &x, double &y, double &z)
{
	x = scale[0];
	y = scale[1];
	z = scale[2];

	return scale;
}

double* CGameObject::GetRotation(double &x, double &y, double &z)
{
	x = rotation[0];
	y = rotation[1];
	z = rotation[2];

	return rotation;
}

double* CGameObject::GetTranslation(double &x, double &y, double &z)
{
	x = translation[0];
	y = translation[1];
	z = translation[2];

	return translation;
}

double* CGameObject::GetColor(double &r, double &g, double &b)
{
	r = color[0];
	g = color[1];
	b = color[2];

	return color;
}

double* CGameObject::GetMotionVector(double &x, double &y, double &z)
{
	x = motionVector[0];
	y = motionVector[1];
	z = motionVector[2];

	return motionVector;
}

double* CGameObject::GetAngularVelocity(double &x, double &y, double &z)
{
	x = angularVelocity[0];
	y = angularVelocity[1];
	z = angularVelocity[2];

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
	double vectorToGravity[3];
	double forceVector[3];
	double distance;

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
	double relVelVector[3];
	double relPosVector[3];
	double x, y, z;	

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
					double r = collisionSpheres[k]->radius * scale[0] + gObjects[i]->GetCollisionSpheres()[j]->radius * gObjects[i]->GetScale(x, y, z)[0];

					//dP^2-r^2
					double pp = relPosVector[0] * relPosVector[0] + relPosVector[1] * relPosVector[1] + relPosVector[2] * relPosVector[2] - r*r;

					//(1)Check if the spheres are already intersecting
					if ( pp < 0 ) 
						return i;
/*
					//Now check to see if the spheres will intersect within 1/60th of a second
					//dP*dV
					double pv = relPosVector[0] * relVelVector[0] + relPosVector[1] * relVelVector[1] + relPosVector[2] * relVelVector[2];
					//(2)Check if the spheres are moving away from each other
					if ( pv < 0 ) 
					{
						//dV^2
						double vv = relVelVector[0] * relVelVector[0] + relVelVector[1] * relVelVector[1] + relVelVector[2] * relVelVector[2];

						//(3)Check if the spheres can intersect within 1 frame
						if ( (pv + vv) <= 0 && (vv + 2 * pv + pp) >= 0 )
						{
							return -1;
						}
						else
						{
							//Discriminant/4
							double D    = pv * pv - pp * vv; 
							if ( D > 0 )
								return -1;

							// tmin = -dP*dV/dV*2 
							//the time when the distance between the spheres is minimal
							//double tmin = -pv/vv;

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
	
	glTranslated(translation[0], translation[1], translation[2]);
	glRotated(rotation[0], 1, 0, 0);
	glRotated(rotation[1], 0, 1, 0);
	glRotated(rotation[2], 0, 0, 1);
	glScaled(scale[0], scale[1], scale[2]);

	float radius = 1;
	glBegin(GL_TRIANGLES);
		for (int i=0; i<360; i++)
		{
			float degInRad = (float)(i*DEG2RAD);
			glColor3d(color[0], color[1], color[2]);  glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius, 0);

			degInRad = (float)((i+1)*DEG2RAD);
			glColor3d(color[0], color[1], color[2]);  glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius, 0);

			glColor3d(color[0], color[1], color[2]);  glVertex3f(0, 0, 0);
		}
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
		for (int i=0; i<360; i++)
		{
			float degInRad = (float)(i*DEG2RAD);
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
	cSphere->radius = .2;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

	cSphere = new sCollisionSphere;
	cSphere->translation[0] = 0;
	cSphere->translation[1] = -.15;
	cSphere->translation[2] = 0;
	cSphere->radius = .25;
	cSphere->globalPosition[0] = 0;
	cSphere->globalPosition[1] = 0;
	cSphere->globalPosition[2] = 0;
	collisionSpheres.push_back(cSphere);

}

void CNuke::Draw()
{
	double offset = 0;
	double radius;

	glPushMatrix();
	
	glTranslated(translation[0], translation[1], translation[2]);
	glRotated(rotation[0], 1, 0, 0);
	glRotated(rotation[1], 0, 1, 0);
	glRotated(rotation[2] - 90, 0, 0, 1);
	glScaled(scale[0], scale[1], scale[2]);

	glColor3d(color[0], color[1], color[2]);  

	//Draw one half of Nuke body
	radius = 1;
	offset = -cos(-30.0*DEG2RAD);

	glBegin(GL_TRIANGLES);
	for (int i=-20; i<30; i++)
	{
		float degInRad = (float)(i*DEG2RAD);
		glVertex3d(cos(degInRad)*radius + offset, sin(degInRad)*radius, -.001);

		degInRad = (float)((i+1)*DEG2RAD);
		glVertex3d(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001);

		glVertex3d(0, 0, -.001);
	}
	glEnd();

	//Draw the other half of Nuke body
	offset = -cos(150.0*DEG2RAD);
	glBegin(GL_TRIANGLES);
	for (int i=150; i<200; i++)
	{
		float degInRad = (float)(i*DEG2RAD);
		glVertex3d(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001);

		degInRad = (float)((i+1)*DEG2RAD);
		glVertex3d(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001);

		glVertex3d(0, 0, -.001);
	}
	glEnd();

	//Fill in bottom triangle
	glBegin(GL_TRIANGLES);
		glVertex3d(0,0,0);
		glVertex3d(cos(200 * DEG2RAD)*radius - cos(150.0*DEG2RAD), sin(200 * DEG2RAD)*radius, -.001);
		glVertex3d(cos(-20 * DEG2RAD)*radius - cos(-30.0*DEG2RAD), sin(-20 * DEG2RAD)*radius, -.001);
	glEnd();

	//Make one fin
	offset = sin(200 * DEG2RAD);
	glBegin(GL_TRIANGLES);
	radius = .28;
	for (int i=120; i<180; i++)
	{
		float degInRad = (float)(i*DEG2RAD);
		glVertex3d(cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.01);

		degInRad = (float)((i+1)*DEG2RAD);
		glVertex3d(cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.01);

		glVertex3d(0, offset/2, -.01);
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	//Make another fin
	for (int i=120; i<180; i++)
	{
		float degInRad = (float)(i*DEG2RAD);
		glVertex3d(-cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.01);

		degInRad = (float)((i+1)*DEG2RAD);
		glVertex3d(-cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.01);

		glVertex3d(0, offset/2, -.01);
	}
	glEnd();


	//**** Create outlines ****/

	glColor3d(1.0, 1.0, 1.0);

	//Draw one half of Nuke body
	radius = 1;
	offset = -cos(-30.0*DEG2RAD);

	glBegin(GL_LINES);
	for (int i=-20; i<30; i++)
	{
		float degInRad = (float)(i*DEG2RAD);
		glVertex3d(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001);

		degInRad = (float)((i+1)*DEG2RAD);
		glVertex3d(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001);

		//glVertex3d(0, 0, 0);
	}
	glEnd();

	//Draw the other half of Nuke body
	offset = -cos(150.0*DEG2RAD);
	glBegin(GL_LINES);
	for (int i=150; i<200; i++)
	{
		float degInRad = (float)(i*DEG2RAD);
		glVertex3d(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001);

		degInRad = (float)((i+1)*DEG2RAD);
		glVertex3d(cos(degInRad)*radius + offset,sin(degInRad)*radius, -.001);

		//glVertex3d(0, 0, 0);
	}
	glEnd();

	
	//Fill in bottom triangle
	glBegin(GL_LINES);
		//glVertex3d(0,0,0);
		glVertex3d(cos(200 * DEG2RAD)*radius - cos(150.0*DEG2RAD), sin(200 * DEG2RAD)*radius, -.00001);
		glVertex3d(cos(-20 * DEG2RAD)*radius - cos(-30.0*DEG2RAD), sin(-20 * DEG2RAD)*radius, -.00001);
	glEnd();

	//Make one fin
	offset = sin(200 * DEG2RAD);
	glBegin(GL_LINES);
	radius = .28;
	for (int i=120; i<180; i++)
	{
		float degInRad = (float)(i*DEG2RAD);
		glVertex3d(cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.001);

		degInRad = (float)((i+1)*DEG2RAD);
		glVertex3d(cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.001);

		//glVertex3d(0, offset/2, 0);
	}
	glEnd();

	glBegin(GL_LINES);
		glVertex3d(0, offset/2, -0.005);
		glVertex3d(cos(179*DEG2RAD)*radius,sin(179*DEG2RAD)*radius + offset, -0.005);
	glEnd();

	glBegin(GL_LINES);
	//Make another fin
	for (int i=120; i<180; i++)
	{
		float degInRad = (float)(i*DEG2RAD);
		glVertex3d(-cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.001);

		degInRad = (float)((i+1)*DEG2RAD);
		glVertex3d(-cos(degInRad)*radius,sin(degInRad)*radius + offset, -0.001);

		//glVertex3d(0, offset/2, 0);
	}
	glEnd();

	glBegin(GL_LINES);
		glVertex3d(0, offset/2, -0.005);
		glVertex3d(-cos(179*DEG2RAD)*radius,sin(179*DEG2RAD)*radius + offset, -0.005);
	glEnd();

	glPopMatrix();

	/*
	glPushMatrix();

	//Draw collision spheres for debug
	for(unsigned int i=0; i<collisionSpheres.size(); i++)
	{
		radius = collisionSpheres[i]->radius * scale[1];
		glColor3f(1, 1, 0);
		glBegin(GL_LINE_LOOP);
			for (int j=0; j<360; j++)
			{
				float degInRad = (float)(j*DEG2RAD);
				glVertex3d(cos(degInRad)*radius + collisionSpheres[i]->globalPosition[0] ,sin(degInRad)*radius + collisionSpheres[i]->globalPosition[1], 0 + collisionSpheres[i]->globalPosition[2]);
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