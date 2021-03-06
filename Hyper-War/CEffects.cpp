/*************************************************************/
/*	CEffects.cpp: Class for rendering global effects			 */
/*															 */
/*	July 2009												 */
/*************************************************************/

#include "CEffects.h"
#include "time.h"

CEffects::CEffects()
{
	for(int i=0; i<2000; i++)
	{
		Star newStar;
		newStar.xpos = (((float)rand() / (float)RAND_MAX) - .5f) * 10000;
		newStar.ypos = (((float)rand() / (float)RAND_MAX) - 1.0f) * 10000;
		newStar.zpos = (((float)rand() / (float)RAND_MAX) -  1.0f) * 50.0f - 1;
		newStar.color1 = 1; 
		newStar.color2 = 1; 
		newStar.color3 = 1; 
		newStar.scale = .03f;
		newStar.rotation = (rand()%360)+1;
		stars.push_back(newStar);
	}
	for(int i=0; i<500; i++)
	{
		Star newStar;
		newStar.xpos = (((float)rand() / (float)RAND_MAX) - .5f) * 1000;
		newStar.ypos = (((float)rand() / (float)RAND_MAX) - 1.0f) * 1000;
		newStar.zpos = (((float)rand() / (float)RAND_MAX) -  1.0f) * 50.0f - 1;
		newStar.color1 = 1; 
		newStar.color2 = .7f; 
		newStar.color3 = .7f; 
		newStar.scale = .2f;
		newStar.rotation = (rand()%360)+1;
		stars.push_back(newStar);
	}
	for(int i=0; i<100; i++)
	{
		Star newStar;
		newStar.xpos = (((float)rand() / (float)RAND_MAX) - .5f) * 1000;
		newStar.ypos = (((float)rand() / (float)RAND_MAX) - 1.0f) * 1000;
		newStar.zpos = (((float)rand() / (float)RAND_MAX) -  2.0f) * 50.0f - 1;
		newStar.color1 = 1; 
		newStar.color2 = .7f; 
		newStar.color3 = .7f; 
		newStar.scale = .2f;
		newStar.rotation = (rand()%360)+1;
		stars.push_back(newStar);
	}
	for(int i=0; i<1000; i++)
	{
		Star newStar;
		newStar.xpos = (((float)rand() / (float)RAND_MAX) - .5f) * 10000;
		newStar.ypos = (((float)rand() / (float)RAND_MAX) - .5f) * 10000;
		newStar.zpos = (((float)rand() / (float)RAND_MAX) -  1.0f) * 50.0f - 1;
		newStar.color1 = .7f; 
		newStar.color2 = 0.7f;
		newStar.color3 = 1; 
		newStar.scale = .03f;
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
	float tmp = starFieldPosition / 400.0f;
	double color[3] = {1, 1, 1};
	for(unsigned int i=0; i<stars.size(); i++)
	{
		glPushMatrix();		
		if (stars[i].ypos > 1000)
			stars[i].ypos = -1000;
		glScalef(stars[i].scale,stars[i].scale,1);
		glRotatef((float)stars[i].rotation, 0, 0, 1);
		glTranslatef(stars[i].xpos, stars[i].ypos+tmp, stars[i].zpos);
		glRotatef(stars[i].xpos*tmp/2, 0, 0, 1);
		DrawStar((Star)stars[i]);
		glPopMatrix();
	}	
}