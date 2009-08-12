#include "CRawInput.h"

CRawInput::CRawInput()
{
	//Init raw input

	numDevices = 2;

	_x = new int[numDevices];
	_y = new int[numDevices];
}

CRawInput::~CRawInput()
{
	delete [] _x;
	delete [] _y;
}

void CRawInput::ProcessRawInput()
{
}

void CRawInput::GetMouse(int devIndex, int& x, int& y)
{
	if(devIndex < numDevices)
	{
		x = _x[devIndex];
		y = _y[devIndex];
	}
}