/* 
	hello world how many raw input does system have.
	code to see how many raw input we have on the system.



d.p.nielsen@gmail.com
Aug 2009

*/
#include <stdio.h>
#include <conio.h>			//  Provides getch()
#include <windows.h>		//  Provides rawinput

main()
{
	
	UINT nDevices,i;
	PRAWINPUTDEVICELIST pRawInputDeviceList;

	// 1st call to GetRawInputDeviceList: Pass NULL to get the size of the list.
	//function enumerates the raw input devices attached to the system
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) return 0; 
	printf("Number of raw input devices: %i\n\n", nDevices);
	// todo list them out

	// get memory for the list 
	if ((pRawInputDeviceList = malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL) {
	//we had a error what to do.
	
	}else{
		// let get our list of hardware... 
		if (GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) == -1) {
		//we had a error what to do.
		
		
		}else
		{
			// do the job...
			for ( i = 0; i < nDevices; ++i)
			{
				printf( "Device %i:", i);
				switch(pRawInputDeviceList[i].dwType){
						case RIM_TYPEHID:
							printf( "HID\n");
							break;
						case RIM_TYPEKEYBOARD:
							printf( "KEYBOARD\n");
							break;
						case RIM_TYPEMOUSE:
							printf( "MOUSE\n");
							break;
						default:
							break;
				}

			
		}
			// after the job, free the RAWINPUTDEVICELIST
		}
	}

	free(pRawInputDeviceList);

	printf("Press any key...");
	getch();

	return 1;
}
