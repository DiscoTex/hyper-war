/*************************************************************/
/*	CEffects.cpp: Class for rendering global effects			 */
/*															 */
/*	July 2009												 */
/*************************************************************/

#include "CEffects.h"

CEffects::CEffects()
{
	starFieldPosition = 0;
}

CEffects::~CEffects()
{
}

void CEffects::DrawStarfield()
{
	//Draw the starfield using starFieldPosition as an input
	//it should look the same in position=0 and postion=(num possible positions+1) which we modded by above
	/*
	glPushMatrix();

	//glTranslatef(translation[0], translation[1], translation[2]);
	//glRotatef(rotation[0], 1, 0, 0);
	//glRotatef(rotation[1], 0, 1, 0);
	//glRotatef(rotation[2], 0, 0, 1);
	//glScalef(scale[0], scale[1], scale[2]);

	float color[3] = {1, 1, 1};
	glTranslatef(0, 0, -1);
	
	glBegin (GL_QUADS);								
		glColor3f (color[0], color[1], color[2]);	glColor3f( -1.0f, 1.0f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glColor3f(1.0f, 1.0f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glColor3f( 1.0f,-1.0f, 0.0f);
		glColor3f (color[0], color[1], color[2]);	glColor3f( -1.0f,-1.0f, 0.0f);
	glEnd ();	

	glBegin(GL_LINE_LOOP);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( -1.0f, 1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f(1.0f, 1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 1.0f,-1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( -1.0f,-1.0f, 0.0f);
	glEnd();

	glPopMatrix();
	*/
}