#include "StdAfx.h"
#include "RawMouse.h"
#include "stdio.h"

CRawMouse::CRawMouse(void)
: nraw_mouse_count(0)
, include_sys_mouse(false)
, include_rdp_mouse(false)
, include_individual_mice(false)
, excluded_sysmouse_devices_count(0)
, bHasBeenInitialized(false)
{


}

CRawMouse::~CRawMouse(void)
{
	free(raw_mice);
}

// get the relative position of the mouse since the last time
long CRawMouse::get_x_delta(int mousenum)
{
	long nReturn = 0;

	if (raw_mice != NULL && mousenum < nraw_mouse_count) {
		nReturn = raw_mice[mousenum].x;
		if(!raw_mice[mousenum].is_absolute) raw_mice[mousenum].x = 0;
	}

	return nReturn;

}


// get the relative position of the mouse since the last time
 long CRawMouse::get_y_delta(int mousenum)
{
		long nReturn = 0;

		if (raw_mice != NULL && mousenum < nraw_mouse_count) {
		nReturn = raw_mice[mousenum].y;
		if(!raw_mice[mousenum].is_absolute) raw_mice[mousenum].y = 0;
		}
		return nReturn;

}

// get the relative position of the mouse since the last time
 long CRawMouse::get_z_delta(int mousenum)
{
		long nReturn = 0;

 		if (raw_mice != NULL && mousenum < nraw_mouse_count) {
			nReturn = raw_mice[mousenum].z;
			if(!raw_mice[mousenum].is_absolute) raw_mice[mousenum].z = 0;
		}

		return nReturn;
}

// return the actual number of mice
int CRawMouse::raw_mouse_count(void)
{
	return nraw_mouse_count;
}

// Every time the WM_INPUT message is received, the lparam must be passed to this function to 
//keep a running tally of every mouse move to maintain accurate results
bool CRawMouse::process_raw_mouse(HANDLE in_device_handle)
{
		// When the WM_INPUT message is received, the lparam must be passed to this function to keep a running tally of
	//     every mouse moves to maintain accurate results for get_raw_mouse_?_delta().
	// This function will take the HANDLE of the device and find the device in the raw_mice arrayand add the 
	//      x and y mousemove values according to the information stored in the RAWINPUT structure.

	LPBYTE lpb;
	int dwSize;

	if (GetRawInputData ((HRAWINPUT)in_device_handle, RID_INPUT, NULL, (PUINT)&dwSize, sizeof(RAWINPUTHEADER)) == -1) {
		fprintf(stderr, "ERROR: Unable to add to get size of raw input header.\n");
		return 0;
	}

	lpb = (LPBYTE)malloc(sizeof(LPBYTE) * dwSize);
	if (lpb == NULL) {
		fprintf(stderr, "ERROR: Unable to allocate memory for raw input header.\n");
		return 0;
	} 
  
	if (GetRawInputData((HRAWINPUT)in_device_handle, RID_INPUT, lpb,  (PUINT)&dwSize, sizeof(RAWINPUTHEADER)) != dwSize ) {
		fprintf(stderr, "ERROR: Unable to add to get raw input header.\n");
		return 0;
	} 

	read_raw_input((RAWINPUT*)lpb);

	free(lpb); 

	return 1;

}

// init the raw mouses
int CRawMouse::init_raw_mouse(BOOL in_include_sys_mouse, BOOL in_include_rdp_mouse, BOOL in_include_individual_mice)
{
	unsigned int i,j;
	unsigned int nInputDevices;
	unsigned int nSize;
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	char *psName;
	char buffer[80];
	int currentmouse = 0;
	nraw_mouse_count = 0;

	include_sys_mouse = in_include_sys_mouse;
	include_rdp_mouse = in_include_rdp_mouse;
	include_individual_mice = in_include_individual_mice;

	if (bHasBeenInitialized) {
	fprintf(stderr, "WARNING: rawmouse init called after initialization already completed.");
		bHasBeenInitialized = 1;
		return 0;
	}


	// 1st call to GetRawInputDeviceList: Pass NULL to get the number of devices.
	if (GetRawInputDeviceList(NULL, &nInputDevices, sizeof(RAWINPUTDEVICELIST)) != 0) {
		fprintf(stderr, "ERROR: Unable to count raw input devices.\n");
		return 0;
	}
	// Allocate the array to hold the DeviceList
	if ((pRawInputDeviceList =(PRAWINPUTDEVICELIST) malloc(sizeof(RAWINPUTDEVICELIST) * nInputDevices)) == NULL) {
		fprintf(stderr, "ERROR: Unable to allocate memory for raw input device list.\n");
		return 0;
	}

	// 2nd call to GetRawInputDeviceList: Pass the pointer to our DeviceList and GetRawInputDeviceList() will fill the array
	if (GetRawInputDeviceList(pRawInputDeviceList, &nInputDevices, sizeof(RAWINPUTDEVICELIST)) == -1)  {
		fprintf(stderr, "ERROR: Unable to get raw input device list.\n");
		return 0;
	}


	// Loop through all devices and count the mice
	for (i = 0; i < nInputDevices; i++) {
		if (pRawInputDeviceList[i].dwType == RIM_TYPEMOUSE) {
		        /* Get the device name and use it to determine if it's the RDP Terminal Services virtual device. */

			// 1st call to GetRawInputDeviceInfo: Pass NULL to get the size of the device name 
	        if (GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, NULL, &nSize) != 0) {
				fprintf(stderr, "ERROR: Unable to get size of raw input device name.\n");
				return 0;
			}
				
			// Allocate the array to hold the name
			if ((psName = (char *)malloc(sizeof(TCHAR) * nSize)) == NULL)  {
				fprintf(stderr, "ERROR: Unable to allocate memory for device name.\n");
				return 0;
			}

			// 2nd call to GetRawInputDeviceInfo: Pass our pointer to get the device name
			if ((int) GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, psName, &nSize) < 0)  {
				fprintf(stderr, "ERROR: Unable to get raw input device name.\n");
				return 0;
			} 

			// Count this mouse for allocation if it's not an RDP mouse or if we want to include the rdp mouse
			if (is_rm_rdp_mouse(psName)) {
				if (include_rdp_mouse) nraw_mouse_count++;
			}
			else { // It's an ordinary mouse
				nraw_mouse_count++;
				if (!include_individual_mice) excluded_sysmouse_devices_count++;     // Don't count this in the final nraw_mouse_count value
			}
		}
	}

	if (include_sys_mouse)
		nraw_mouse_count++;

	// Allocate the array for the raw mice
	if ((raw_mice = (PRAW_MOUSE)malloc(sizeof(RAW_MOUSE) * nraw_mouse_count)) == NULL)  {
		fprintf(stderr, "ERROR: Unable to allocate memory for raw input mice.\n");
		return 0;
	}

	// Define the sys mouse
	if (include_sys_mouse) {
		raw_mice[RAW_SYS_MOUSE].device_handle = 0;
		raw_mice[RAW_SYS_MOUSE].x = 0;
		raw_mice[RAW_SYS_MOUSE].y = 0;
		raw_mice[RAW_SYS_MOUSE].z = 0;
		raw_mice[RAW_SYS_MOUSE].is_absolute = 0;
		raw_mice[RAW_SYS_MOUSE].is_virtual_desktop = 0;

		currentmouse++;
	}
	// Loop through all devices and set the device handles and initialize the mouse values
	for (i = 0; i < nInputDevices; i++) {
		if (pRawInputDeviceList[i].dwType == RIM_TYPEMOUSE) {
			// 1st call to GetRawInputDeviceInfo: Pass NULL to get the size of the device name 
		        if (GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, NULL, &nSize) != 0)  {
				fprintf(stderr, "ERROR: Unable to get size of raw input device name (2).\n");
				return 0;
			}
			
			// Allocate the array to hold the name
			if ((psName = (char *)malloc(sizeof(TCHAR) * nSize)) == NULL) {
				fprintf(stderr, "ERROR: Unable to allocate memory for raw input device name (2).\n");
				return 0;
			}
		  
			// 2nd call to GetRawInputDeviceInfo: Pass our pointer to get the device name
			if ((int) GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, psName, &nSize) < 0) {
				fprintf(stderr, "ERROR: Unable to get raw input device name (2).\n");
				return 0;
			} 

			// Add this mouse to the array if it's not an RDPMouse or if we wish to include the RDP mouse
			if ((!is_rm_rdp_mouse(psName)) || include_rdp_mouse ) {
				raw_mice[currentmouse].device_handle = pRawInputDeviceList[i].hDevice;
				raw_mice[currentmouse].x = 0;
				raw_mice[currentmouse].y = 0;
				raw_mice[currentmouse].z = 0;
				raw_mice[currentmouse].is_absolute = 0;
				raw_mice[currentmouse].is_virtual_desktop = 0;

				currentmouse++;
			}
		}
	}
    
	// free the RAWINPUTDEVICELIST
	free(pRawInputDeviceList);



	
	for (i = 0; i <(unsigned int) nraw_mouse_count; i++) {
		for (j = 0; j < MAX_RAW_MOUSE_BUTTONS; j++) {
			raw_mice[i].buttonpressed[j] = 0;

			// Create the name for this button
			sprintf(buffer, "Button %i", j);
			raw_mice[i].button_name[j] = (char *)malloc(strlen(buffer) + 1);
			sprintf(raw_mice[i].button_name[j], "%s", buffer);
		}
	}

	nraw_mouse_count -= excluded_sysmouse_devices_count;
	// finally, register to recieve raw input WM_INPUT messages
	if (!register_raw_mouse()) {
		fprintf(stderr, "ERROR: Unable to register raw input (2).\n");
		return 0;
	}

	bHasBeenInitialized = 1;
	return 1;  

}

// is_rm_rdp_mouse
bool CRawMouse::is_rm_rdp_mouse(char* cDeviceString)
{
	  int i;
	  char cRDPString[] = "\\??\\Root#RDP_MOU#0000#";

	  if (strlen(cDeviceString) < 22) {
			return 0;
	  }

	  for (i = 0; i < 22; i++) {
			if (cRDPString[i] != cDeviceString[i]) {
				return 0;
			}
	  }  

	  return 1;

}

// This function registers to receive the WM_INPUT messages
bool CRawMouse::register_raw_mouse(void)
{
	  // This function registers to receive the WM_INPUT messages
	  RAWINPUTDEVICE Rid[1]; // Register only for mouse messages from wm_input.  

	  //register to get wm_input messages
	  Rid[0].usUsagePage = 0x01; 
	  Rid[0].usUsage = 0x02; 
	  Rid[0].dwFlags = 0;// RIDEV_NOLEGACY;   // adds HID mouse and also ignores legacy mouse messages
	  Rid[0].hwndTarget = NULL;

	  // Register to receive the WM_INPUT message for any change in mouse (buttons, wheel, and movement will all generate the same message)
          if (!RegisterRawInputDevices(Rid, 1, sizeof (Rid[0])))
			return 0;
 
	  return 1;
}

// read raw input
bool CRawMouse::read_raw_input(PRAWINPUT raw)
{
		  // should be static when I get around to it

	  int i;

	  // mouse 0 is sysmouse, so if there is not sysmouse start loop @0
	  i = 0;
	  if (include_sys_mouse) i++; 

	  for ( ; i < (nraw_mouse_count + excluded_sysmouse_devices_count); i++) {
			if (raw_mice[i].device_handle == raw->header.hDevice)
			{
				// Update the values for the specified mouse
				if (include_individual_mice) {
					if (raw_mice[i].is_absolute) {
						raw_mice[i].x = raw->data.mouse.lLastX;
						raw_mice[i].y = raw->data.mouse.lLastY;
					}
					else { // relative
						raw_mice[i].x += raw->data.mouse.lLastX;
						raw_mice[i].y += raw->data.mouse.lLastY;
					}
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) raw_mice[i].buttonpressed[0] = 1;
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP)   raw_mice[i].buttonpressed[0] = 0;
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) raw_mice[i].buttonpressed[1] = 1;
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP)   raw_mice[i].buttonpressed[1] = 0;
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) raw_mice[i].buttonpressed[2] = 1;
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP)   raw_mice[i].buttonpressed[2] = 0;

					if (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)          raw_mice[i].is_absolute = 1;
					else if (raw->data.mouse.usFlags & MOUSE_MOVE_RELATIVE)     raw_mice[i].is_absolute = 0;
					if (raw->data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP)        raw_mice[i].is_virtual_desktop = 1;
					else                                                        raw_mice[i].is_virtual_desktop = 0;

					if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) {      // If the current message has a mouse_wheel message
						if ((SHORT)raw->data.mouse.usButtonData > 0) {
							raw_mice[i].z++;
						}
						if ((SHORT)raw->data.mouse.usButtonData < 0) {
							raw_mice[i].z--;
						}
					}
				}

				// Feed the values for every mouse into the system mouse
				if (include_sys_mouse) { 
					if (raw_mice[i].is_absolute) {
						raw_mice[RAW_SYS_MOUSE].x = raw->data.mouse.lLastX;
						raw_mice[RAW_SYS_MOUSE].y = raw->data.mouse.lLastY;
					}
					else { // relative
						raw_mice[RAW_SYS_MOUSE].x += raw->data.mouse.lLastX;
						raw_mice[RAW_SYS_MOUSE].y += raw->data.mouse.lLastY;
					}
			  
					// This is innacurate:  If 2 mice have their buttons down and I lift up on one, this will register the
					//   system mouse as being "up".  I checked out on my windows desktop, and Microsoft was just as
					//   lazy as I'm going to be.  Drag an icon with the 2 left mouse buttons held down & let go of one.

					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) raw_mice[RAW_SYS_MOUSE].buttonpressed[0] = 1;
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP) raw_mice[RAW_SYS_MOUSE].buttonpressed[0] = 0;
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) raw_mice[RAW_SYS_MOUSE].buttonpressed[1] = 1;
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP) raw_mice[RAW_SYS_MOUSE].buttonpressed[1] = 0;
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) raw_mice[RAW_SYS_MOUSE].buttonpressed[2] = 1;
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP) raw_mice[RAW_SYS_MOUSE].buttonpressed[2] = 0;
				  
					// If an absolute mouse is triggered, sys mouse will be considered absolute till the end of time.
					if (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)          raw_mice[RAW_SYS_MOUSE].is_absolute = 1;
					// Same goes for virtual desktop
					if (raw->data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP)        raw_mice[RAW_SYS_MOUSE].is_virtual_desktop = 1;

					if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) {      // If the current message has a mouse_wheel message
						if ((SHORT)raw->data.mouse.usButtonData > 0) {
							raw_mice[RAW_SYS_MOUSE].z++;
						}
						if ((SHORT)raw->data.mouse.usButtonData < 0) {
							raw_mice[RAW_SYS_MOUSE].z--;
						}
					}

				}
			}
	  }
	  
	  return 1;

}

// ask if buttons are pressed 
bool CRawMouse::mouse_button_pressed(int mousenum, int buttonnum)
{
		if (mousenum >= nraw_mouse_count || buttonnum >= MAX_RAW_MOUSE_BUTTONS || raw_mice == NULL) return 0;
	return (raw_mice[mousenum].buttonpressed[buttonnum]);
}
