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
	
	UINT nDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;

	// 1st call to GetRawInputDeviceList: Pass NULL to get the size of the list.
	//function enumerates the raw input devices attached to the system
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) return 0; 
	printf("Number of raw input devices: %i\n\n", nDevices);
	// todo list them out

	printf("Press any key...");
	getch();

	return 1;
}


