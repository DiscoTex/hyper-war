#pragma once

//============================================================
//	PARAMETERS
//============================================================

#define RAW_SYS_MOUSE 0      // The sys mouse combines all the other usb mice into one
#define MAX_RAW_MOUSE_BUTTONS 3

//============================================================
//	DATA TYPES
//============================================================
typedef struct STRUCT_RAW_MOUSE {

	  // Identifier for the mouse.  WM_INPUT passes the device HANDLE as lparam when registering a mousemove
	  HANDLE device_handle;

	  // The running tally of mouse moves received from WM_INPUT (mouse delta).  
	  //    Calling get_raw_mouse_[x | y] will reset the value so that every time
	  //    get_raw_mouse_[x | y] is called, the relative value from the last time
	  //    get_raw_mouse_[x | y] was called will be returned.
	  unsigned long x;
	  unsigned long y;
      unsigned long z;

	  // Used to determine if the HID is using absolute mode or relative mode
	  //    The Act Labs PC USB Light Gun is absolute mode (returns screen coordinates)
	  //    and mice are relative mode (returns delta)
	  // NOTE: this value isn't updated until the device registers a WM_INPUT message
	  bool is_absolute;
	  // This indicates if the coordinates are coming from a multi-monitor setup
	  // NOTE: this value isn't updated until the device registers a WM_INPUT message
	  bool is_virtual_desktop;

	  int buttonpressed[MAX_RAW_MOUSE_BUTTONS];

	  // Identifying the name of the button may be useful in the future as a way to 
	  //   use a mousewheel as a button and other neat tricks (button name: "wheel up", "wheel down")
	  //   -- not a bad way to look at it for a rotary joystick
	  char *button_name[MAX_RAW_MOUSE_BUTTONS];

} RAW_MOUSE;


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
private:
	// number of raw mouse found
	int nraw_mouse_count;
public:
	// init the raw mouses
	int init_raw_mouse(void);
};
