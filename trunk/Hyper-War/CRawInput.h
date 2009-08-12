/*************************************************************/
/*	CRawInput.h: Class for handling raw input				 */
/*															 */
/*	August 2009												 */
/*************************************************************/

#include <windows.h>
#include "glBase.h"
//#include "GameObjects.h"

using namespace std;

//Define raw input structure here


class CRawInput
{
public:
	CRawInput();
	~CRawInput();

	void		ProcessRawInput();
	void		GetMouse(int devIndex, int& x, int& y);
private:
	
	int			numDevices;
	int			*_x;
	int			*_y;
};