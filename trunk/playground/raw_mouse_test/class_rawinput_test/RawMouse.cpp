#include "StdAfx.h"
#include "RawMouse.h"

CRawMouse::CRawMouse(void)
{
}

CRawMouse::~CRawMouse(void)
{
}

// get the relative position of the mouse since the last time
unsigned long CRawMouse::get_x_delta(int index)
{
	return 0;
}


// get the relative position of the mouse since the last time
unsigned long CRawMouse::get_y_delta(int index)
{
	return 0;
}

// get the relative position of the mouse since the last time
unsigned long CRawMouse::get_z_delta(int index)
{
	return 0;
}

// return the actual number of mice
int CRawMouse::raw_mouse_count(void)
{
	return 0;
}

// Every time the WM_INPUT message is received, the lparam must be passed to this function to 
//keep a running tally of every mouse move to maintain accurate results
bool CRawMouse::process_raw_mouse(HANDLE device)
{
	return false;
}
