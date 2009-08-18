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
	  long x;
	  long y;
      long z;

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

} RAW_MOUSE, *PRAW_MOUSE;


class CRawMouse
{
public:
	CRawMouse(void);
	~CRawMouse(void);

	// get the relative position of the mouse since the last time
	long get_x_delta(int index); 
	long get_y_delta(int index);
	long get_z_delta(int index);
	// return the actual number of mice
	 int raw_mouse_count(void);
	// Every time the WM_INPUT message is received, the lparam must be passed to this function to keep
	// a running tally of every mouse move to maintain accurate results
	bool process_raw_mouse(HANDLE in_device_handle);
private:
	int excluded_sysmouse_devices_count;
	// number of raw mouse found
	 int nraw_mouse_count;

	// did we include the ?  mouse
	bool include_sys_mouse;
	bool include_rdp_mouse;
	bool include_individual_mice; 

	// Pointer to our array of raw mice.  Created with .. init_raw_mouse()
	PRAW_MOUSE raw_mice;
public:
	// init the raw mouses
	int init_raw_mouse(BOOL in_include_sys_mouse, BOOL in_include_rdp_mouse, BOOL in_include_individual_mice);
private:
	// is_rm_rdp_mouse
	static bool is_rm_rdp_mouse(char* cDeviceString);
public:
private:

	// This function registers to receive the WM_INPUT messages
	bool register_raw_mouse(void);
	bool bHasBeenInitialized;
	// read raw input
	bool read_raw_input(PRAWINPUT raw);
public:
	// ask if buttons are pressed 
	bool mouse_button_pressed(int mousenum, int buttonnum);
	void destroy_raw_mouse(void);
};
