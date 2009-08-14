#include "StdAfx.h"
#include "RawMouse.h"
#include "stdio.h"

CRawMouse::CRawMouse(void)
: nraw_mouse_count(0)
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
	return nraw_mouse_count;
}

// Every time the WM_INPUT message is received, the lparam must be passed to this function to 
//keep a running tally of every mouse move to maintain accurate results
bool CRawMouse::process_raw_mouse(HANDLE device)
{
	return false;
}

// init the raw mouses
int CRawMouse::init_raw_mouse(void)
{
	int i;
	unsigned int nInputDevices;
	int nSize;
	RAWINPUTDEVICELIST *pRawInputDeviceList

	nraw_mouse_count = 0;
	// 1st call to GetRawInputDeviceList: Pass NULL to get the number of devices.
	if (GetRawInputDeviceList(NULL, &nInputDevices, sizeof(RAWINPUTDEVICELIST)) != 0) {
		fprintf(stderr, "ERROR: Unable to count raw input devices.\n");
		return 0;
	}
	// Allocate the array to hold the DeviceList
	if ((pRawInputDeviceList = malloc(sizeof(RAWINPUTDEVICELIST) * nInputDevices)) == NULL) {
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
		        if (GetRawInputDeviceInfo (pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, NULL, &nSize) != 0) {
				fprintf(stderr, "ERROR: Unable to get size of raw input device name.\n");
				return 0;
			}
				
			// Allocate the array to hold the name
			if ((psName = (char *)malloc(sizeof(TCHAR) * nSize)) == NULL)  {
				fprintf(stderr, "ERROR: Unable to allocate memory for device name.\n");
				return 0;
			}

			// 2nd call to GetRawInputDeviceInfo: Pass our pointer to get the device name
			if ((int)GetRawInputDeviceInfo (pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, psName, &nSize) < 0)  {
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



	return 0;
}
