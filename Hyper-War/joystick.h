// DirectInput Joystick Wrapper
// Copyright 2001 by Jon Parise <jparise@cmu.edu>
//
// $Id: joystick.h,v 1.1.1.1 2001/10/29 19:47:56 jon Exp $

#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include <windows.h>
#include <basetsd.h>
#include <dinput.h>

class Joystick
{
private:
    unsigned int            id;
    unsigned int            device_counter;
    
    LPDIRECTINPUT8          di;
    LPDIRECTINPUTDEVICE8    joystick;

public:
    Joystick(unsigned int id);
    ~Joystick();

    HRESULT deviceName(char* name);

    HRESULT open();
    HRESULT close();

    HRESULT poll(DIJOYSTATE2 *js);

    BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE* instance, VOID* context);

    // Device Querying
    static unsigned int deviceCount();
};

BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE* instance, VOID* context);
BOOL CALLBACK countCallback(const DIDEVICEINSTANCE* instance, VOID* counter);

#endif /* __JOYSTICK_H__ */
