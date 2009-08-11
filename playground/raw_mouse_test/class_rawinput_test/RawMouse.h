#pragma once

class CRawMouse
{
public:
	CRawMouse(void);
	~CRawMouse(void);

	// get the relative position of the mouse since the last time
	unsigned long get_x_delta(int index);
	unsigned long get_y_delta(int index);
	unsigned long get_z_delta(int index);
	// return the actual number of mice
	int raw_mouse_count(void);
	// Every time the WM_INPUT message is received, the lparam must be passed to this function to keep
	// a running tally of every mouse move to maintain accurate results
	bool process_raw_mouse(HANDLE device);
};
