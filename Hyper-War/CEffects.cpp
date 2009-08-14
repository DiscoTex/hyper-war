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
void CEffects::SetTranslation(float x, float y, float z)
{
	translation[0] = x;
	translation[1] = y;
	translation[2] = z;
}
void CEffects::DrawStarfield()
{
	//Draw the starfield using starFieldPosition as an input
	//it should look the same in position=0 and postion=(num possible positions+1) which we modded by above
	glPushMatrix();

	//glTranslated(translation[0], translation[1], translation[2]);
	//glRotated(rotation[0], 1, 0, 0);
	glRotated(33, 1, 1, 1);
	//glRotated(rotation[1], 0, 1, 0);
	//glRotated(45, 0, 1, 0);
	//glRotated(rotation[2], 0, 0, 1);
	//glScaled(scale[0], scale[1], scale[2]);
	glScaled(.1,.1,.1);
	double color[3] = {1, 1, 1};
	glTranslated(0, 0, -1);
	
	//glBegin (GL_POLYGON);								
	//	glColor3d (color[0], color[1], color[2]);	glVertex3d( -1.0f, 1.0f, 0.0f);
	//	glColor3d (color[0], color[1], color[2]);	glVertex3d(1.0f, 1.0f, 0.0f);
	//	glColor3d (color[0], color[1], color[2]);	glVertex3d( 1.0f,-1.0f, 0.0f);
	//	glColor3d (color[0], color[1], color[2]);	glVertex3d( -1.0f,-1.0f, 0.0f);
	//glEnd ();	
	glBegin(GL_POLYGON);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( 0.25f, 0.25f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( 1.0f, 0.0f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( 0.25f,-0.25f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( 0.0f,-1.0f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( 0.0f, 1.0f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( -0.25f, -0.25f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d(-1.0f, 0.0f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( -0.25f, 0.25f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d(-1.0f, 0.0f, 0.0f);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f(-1.0f, 0.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( -0.25f, 0.25f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 0.0f, 1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 0.25f, 0.25f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 1.0f, 0.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 0.25f,-0.25f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 0.0f,-1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( -0.25f, -0.25f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f(-1.0f, 0.0f, 0.0f);
	glEnd();

	glPopMatrix();

	//Draw the starfield using starFieldPosition as an input
	//it should look the same in position=0 and postion=(num possible positions+1) which we modded by above
	glPushMatrix();

	//glTranslated(translation[0], translation[1], translation[2]);
	//glRotated(rotation[0], 1, 0, 0);
	glRotated(45, 1, 1, 1);
	//glRotated(rotation[1], 0, 1, 0);
	//glRotated(45, 0, 1, 0);
	//glRotated(rotation[2], 0, 0, 1);
	//glScaled(scale[0], scale[1], scale[2]);
	glScaled(1,1,1);
	
	glTranslated(0, 0, -1);
	
	//glBegin (GL_POLYGON);								
	//	glColor3d (color[0], color[1], color[2]);	glVertex3d( -1.0f, 1.0f, 0.0f);
	//	glColor3d (color[0], color[1], color[2]);	glVertex3d(1.0f, 1.0f, 0.0f);
	//	glColor3d (color[0], color[1], color[2]);	glVertex3d( 1.0f,-1.0f, 0.0f);
	//	glColor3d (color[0], color[1], color[2]);	glVertex3d( -1.0f,-1.0f, 0.0f);
	//glEnd ();	
	glBegin(GL_POLYGON);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( 0.25f, 0.25f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( 1.0f, 0.0f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( 0.25f,-0.25f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( 0.0f,-1.0f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( 0.0f, 1.0f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( -0.25f, -0.25f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d(-1.0f, 0.0f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d( -0.25f, 0.25f, 0.0f);
		glColor3d (color[0], color[1], color[2]);	glVertex3d(-1.0f, 0.0f, 0.0f);
	glEnd();

	glBegin(GL_LINE_STRIP);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f(-1.0f, 0.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( -0.25f, 0.25f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 0.0f, 1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 0.25f, 0.25f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 1.0f, 0.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 0.25f,-0.25f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( 0.0f,-1.0f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f( -0.25f, -0.25f, 0.0f);
		glColor3f (1.f, 1.f, 1.f);	glVertex3f(-1.0f, 0.0f, 0.0f);
	glEnd();

	glPopMatrix();
	
}