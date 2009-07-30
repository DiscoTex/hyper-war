/*************************************************************/
/*	CEffects.h: Class for rendering global effects			 */
/*															 */
/*	July 2009												 */
/*************************************************************/
#include <windows.h>
#include "glBase.h"

class CEffects
{
public:
	CEffects();
	~CEffects();

	//Get or set the starfield position
	void		SetStarFieldPosition(int newPos);
	int			GetStarFieldPosition() {return starFieldPosition;}

	//Draw the starfield effect
	void		DrawStarfield();
private:

	int			starFieldPosition;
};