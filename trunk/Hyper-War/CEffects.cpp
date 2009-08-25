/*************************************************************/
/*	CEffects.cpp: Class for rendering global effects			 */
/*															 */
/*	July 2009												 */
/*************************************************************/

#include "CEffects.h"

CEffects::CEffects()
{
	starFieldPosition = 0;

	Star newStar;

	for(int i=0; i<10; i++)
	{
		newStar.xpos = ((float)rand() / INT_MAX) * 6 - 3;
		newStar.ypos = ((float)rand() / INT_MAX) * 4 - 2;

		newStar.xpos *= 20;
		newStar.ypos *= 20;
		stars.push_back(newStar);
	}
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

void CEffects::DrawStar()
{
	glColor3f(1, 1, 1);

	glBegin(GL_POLYGON);
		glVertex3d( 0.25f, 0.25f, 0.0f);
		glVertex3d( 1.0f, 0.0f, 0.0f);
		glVertex3d( 0.25f,-0.25f, 0.0f);
		glVertex3d( 0.0f,-1.0f, 0.0f);
		glVertex3d( 0.0f, 1.0f, 0.0f);
		glVertex3d( -0.25f, -0.25f, 0.0f);
		glVertex3d(-1.0f, 0.0f, 0.0f);
		glVertex3d( -0.25f, 0.25f, 0.0f);
		glVertex3d(-1.0f, 0.0f, 0.0f);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex3f(-1.0f, 0.0f, 0.0f);
		glVertex3f( -0.25f, 0.25f, 0.0f);
		glVertex3f( 0.0f, 1.0f, 0.0f);
		glVertex3f( 0.25f, 0.25f, 0.0f);
		glVertex3f( 1.0f, 0.0f, 0.0f);
		glVertex3f( 0.25f,-0.25f, 0.0f);
		glVertex3f( 0.0f,-1.0f, 0.0f);
		glVertex3f( -0.25f, -0.25f, 0.0f);
		glVertex3f(-1.0f, 0.0f, 0.0f);
	glEnd();

}
void CEffects::DrawStarfield()
{
	glPushMatrix();

	glTranslated(0, starFieldPosition / 1000.0, 0);	

	//glRotated(33, 0, 0, 1);
	glScaled(.05,.05,1);
	double color[3] = {1, 1, 1};
	glTranslated(0, 0, -1);	
	
	for(unsigned int i=0; i<stars.size(); i++)
	{
		glPushMatrix();
		glTranslatef(stars[i].xpos, stars[i].ypos, 0);
		DrawStar();
		glPopMatrix();
	}	

	glPopMatrix();
}