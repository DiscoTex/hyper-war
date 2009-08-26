/*************************************************************/
/*	CEffects.h: Class for rendering global effects			 */
/*															 */
/*	July 2009												 */
/*************************************************************/
#include <windows.h>
#include "glBase.h"
#include "gl/gl.h"
#include "gl/glu.h"

using namespace std;

typedef struct _Star
{
	float xpos, ypos, zpos;
	float color1, color2, color3;
	int rotation;
	float scale;
} Star;

class CEffects
{
public:
	CEffects();
	~CEffects();

	//Get or set the starfield position
	void		SetStarFieldPosition(int newPos) {starFieldPosition = newPos;}
	int			GetStarFieldPosition() {return starFieldPosition;}
	void		SetTranslation(float x, float y, float z);
	void		DrawStar(Star inStar);

	//Draw the starfield effect
	void		DrawStarfield();
private:
	float		translation[3];
	int			starFieldPosition;
	float		starColor;

	vector < Star >	stars;
};