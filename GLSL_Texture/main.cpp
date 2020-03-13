//=============================================================================
// Sample Application: Texturing Tutorial + Discard Tutorial
//=============================================================================

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glApplication.h"
#include "glutWindow.h"
#include <iostream>
#include "glsl.h"
#include "FreeImage.h"
#include "texture.h"
#include "smartptr.h"

//-----------------------------------------------------------------------------


class myWindow : public cwc::glutWindow
{
protected:
   cwc::glShaderManager SM;
   cwc::glShader *shader;
   GLuint ProgramObject;
   cwc::SmartPtr<cwc::TextureBase>   pTexture;

public:
	myWindow(){}

	virtual void OnRender(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
      if (pTexture) 
      {
         pTexture->bind(0);
         glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);	
	      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // Disable Filtering!
      }

      if (shader) shader->begin();
      if (shader) shader->setUniform1i("myTexture", 0);

      //glutSolidTeapot(1.0);

      glBegin(GL_QUADS);
         glVertex3f(-1.0f, -1.0f,  0.0f);
         glTexCoord2f(1.0, 0.0);
         glVertex3f( 1.0f, -1.0f,  0.0f);
         glTexCoord2f(1.0, 1.0);
         glVertex3f( 1.0f,  1.0f,  0.0f);
         glTexCoord2f(0.0, 1.0);
         glVertex3f(-1.0f,  1.0f,  0.0f);
         glTexCoord2f(0.0, 0.0);
      glEnd();

      if (shader) shader->end();

      glutSwapBuffers();

		//Repaint();
	}

	virtual void OnIdle() {}

	// When OnInit is called, a render context (in this case GLUT-Window) 
	// is already available!
	virtual void OnInit()
	{
		glClearColor(0.5f, 0.5f, 1.0f, 0.0f);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_DEPTH_TEST);
      glEnable(GL_TEXTURE_2D);

		shader = SM.loadfromFile("vertexshader.txt","fragmentshader.txt"); // load (and compile, link) from file
		if (shader==0) 
         std::cout << "Error Loading, compiling or linking shader\n";
      else
      {
         ProgramObject = shader->GetProgramObject();
      }

      // Create a Texture from file.
      pTexture = cwc::TextureFactory::CreateTextureFromFile("texture.bmp");

      if (!pTexture)
         std::cout << "***WARNING: Failed loading texture!!\n";


	}

	virtual void OnResize(int w, int h) 
   {
      if(h == 0) h = 1;
	   float ratio = 1.0f * (float)w / (float)h;

      glMatrixMode(GL_PROJECTION);
	   glLoadIdentity();
	
	   glViewport(0, 0, w, h);

      gluPerspective(45,ratio,1,100);
	   glMatrixMode(GL_MODELVIEW);
	   glLoadIdentity();
	   gluLookAt(0.0f,0.0f,4.0f, 
		          0.0,0.0,-1.0,
			       0.0f,1.0f,0.0f);
   }
	virtual void OnClose(void){}
	virtual void OnMouseDown(int button, int x, int y) {}    
	virtual void OnMouseUp(int button, int x, int y) {}
	virtual void OnMouseWheel(int nWheelNumber, int nDirection, int x, int y){}

	virtual void OnKeyDown(int nKey, char cAscii)
	{       
		if (cAscii == 27) // 0x1b = ESC
		{
			this->Close(); // Close Window!
		} 
	};

	virtual void OnKeyUp(int nKey, char cAscii)
	{
      if (cAscii == 's')      // s: Shader
         shader->enable();
      else if (cAscii == 'f') // f: Fixed Function
         shader->disable();
	};
};

//-----------------------------------------------------------------------------

class myApplication : public cwc::glApplication
{
public:
	virtual void OnInit() {std::cout << "Hello World!\n"; }
};

//-----------------------------------------------------------------------------

int main(void)
{
#ifdef FREEIMAGE_LIB
   FreeImage_Initialise();
#endif
	myApplication*  pApp = new myApplication;
	myWindow* myWin = new myWindow();

	pApp->run();
	delete pApp;
#ifdef FREEIMAGE_LIB 
   FreeImage_DeInitialise();
#endif  
	return 0;
}

//-----------------------------------------------------------------------------

