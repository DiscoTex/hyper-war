/*************************************************************/
/*	CEffects.h: Class for rendering global effects			 */
/*															 */
/*	July 2009												 */
/*************************************************************/
#include <windows.h>
//#include "glBase.h"
#include "gl/gl.h"
#include "gl/glu.h"

class CEffects
{
public:
	CEffects();
	~CEffects();

	//Get or set the starfield position
	void		SetStarFieldPosition(int newPos);
	int			GetStarFieldPosition() {return starFieldPosition;}
	void		CEffects::SetTranslation(float x, float y, float z);

	//Draw the starfield effect
	void		DrawStarfield();
private:
	float		translation[3];
	int			starFieldPosition;
};