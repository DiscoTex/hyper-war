/*************************************************************/
/*	CEffects.cpp: Class for rendering global effects			 */
/*															 */
/*	July 2009												 */
/*************************************************************/

#include "CEffects.h"
#include "time.h"

CEffects::CEffects()
{
	//starFieldPosition = 1000;
	//srand(12422434);
	for(int i=0; i<2000; i++)
	{
		Star newStar;
		
		newStar.xpos = (((float)rand() / (float)RAND_MAX) - .5f) * 10000;
		newStar.ypos = (((float)rand() / (float)RAND_MAX) - 1.0f) * 10000;
		newStar.zpos = (((float)rand() / (float)RAND_MAX) -  1.0f) * 50.0f - 1;
		newStar.color1 = 1; //((float)rand() / (float)RAND_MAX);
		newStar.color2 = 1; //((float)rand() / (float)RAND_MAX);
		newStar.color3 = 1; //((float)rand() / (float)RAND_MAX);
		newStar.scale = .04;//((float)rand()/ (float)RAND_MAX) * .05 ;
		newStar.rotation = (rand()%360)+1;
		stars.push_back(newStar);
	}
	//srand(-4344421);
	for(int i=0; i<1000; i++)
	{
		Star newStar;
		newStar.xpos = (((float)rand() / (float)RAND_MAX) - .5f) * 10000;
		newStar.ypos = (((float)rand() / (float)RAND_MAX) - .5f) * 10000;
		newStar.zpos = (((float)rand() / (float)RAND_MAX) -  1.0f) * 50.0f - 1;
		newStar.color1 = 1; //((float)rand() / (float)RAND_MAX);
		newStar.color2 = 0.7; //((float)rand() / (float)RAND_MAX);
		newStar.color3 = 0.8; //((float)rand() / (float)RAND_MAX);
		newStar.scale = .04;//((float)rand()/ (float)RAND_MAX) * .05 ;
		newStar.rotation = (rand()%360)+1;
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

void CEffects::DrawStar(Star inStar)
{
	glColor3f(inStar.color1, inStar.color2, inStar.color3);
/*
	glBegin(GL_POLYGON);
		glVertex3d( 0.0f, 1.0f, 0.0f);
		glVertex3d( 0.25f, 0.25f, 0.0f);
		glVertex3d( 1.0f, 0.0f, 0.0f);
		glVertex3d( 0.25f,-0.25f, 0.0f);
		glVertex3d( 0.0f,-1.0f, 0.0f);
		glVertex3d( -0.25f, -0.25f, 0.0f);
		glVertex3d( -1.0f, 0.0f, 0.0f);
		glVertex3d( -0.25f, 0.25f, 0.0f);
		
	glEnd();
*/
	glPushMatrix();
	glBegin(GL_LINE_LOOP);
		//glColor3d (, color[1], color[2]);
		glVertex3f( 0.0f, 1.0f, 0.0f);
		glVertex3f( 0.25f, 0.25f, 0.0f);
		glVertex3f( 1.0f, 0.0f, 0.0f);
		glVertex3f( 0.25f,-0.25f, 0.0f);
		glVertex3f( 0.0f,-1.0f, 0.0f);
		glVertex3f( -0.25f, -0.25f, 0.0f);
		glVertex3f( -1.0f, 0.0f, 0.0f);
		glVertex3f( -0.25f, 0.25f, 0.0f);
	glEnd();
	glPopMatrix();

}
void CEffects::DrawStarfield()
{	
	//glTranslated(0, starFieldPosition / 1000.0, -1);	
	float tmp = starFieldPosition / 50.0;
	//glRotated(33, 0, 0, 1);
	//glScalef(.03,.03,1);
	double color[3] = {1, 1, 1};


	for(int i=0; i<stars.size(); i++)
	{
		//if (stars[i].ypos > 1.9)
		//	stars[i].ypos = -1.9;
		//glScaled(stars[i].scale,stars[i].scale,1);		
		glPushMatrix();
		
		if (stars[i].ypos > 1000)
			stars[i].ypos = -1000;

		glScaled(.04,.03,1);
		glTranslatef(stars[i].xpos, stars[i].ypos+tmp, stars[i].zpos);
		
		//printf("x=%f\ty=%f",stars[i].xpos,stars[i].ypos);
		
		DrawStar((Star)stars[i]);
		glPopMatrix();
	}	
}