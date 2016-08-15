#include <windows.h>

#define DIRECTINPUT_VERSION  0x0800

#include <dinput.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutInput.h"

static LPDIRECTINPUT8       g_pDI    = NULL;         
static LPDIRECTINPUTDEVICE8 g_pMouse = NULL;     
static LPDIRECTINPUTDEVICE8 g_pKeyboard = NULL;
static LPDIRECTINPUTDEVICE8 g_pJoystick = NULL;

static POINT g_op;
static POINT g_mouse;

typedef void (*pFunc)(void);

typedef void (*KeyHandlerProc)(void);

static KeyHandlerProc g_pKeyDownFuncs[256];
static KeyHandlerProc g_pKeyUpFuncs[256];
static KeyHandlerProc g_pKeyPressedFuncs[256];

static char g_keyboard_state[256];

static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext )
{
	HRESULT hr;
	// Obtain an interface to the enumerated joystick.
	hr = g_pDI->CreateDevice( pdidInstance->guidInstance, &g_pJoystick, NULL );
	if( FAILED(hr) ) 
		return DIENUM_CONTINUE;
	return DIENUM_STOP;
}

int GutInputRestore(void)
{
	if ( g_pMouse ) g_pMouse->Acquire(); 
	if ( g_pJoystick ) g_pJoystick->Acquire();
	if ( g_pKeyboard ) g_pKeyboard->Acquire(); 

	return 1;
}

int GutInputInit(void)
{
	memset(g_pKeyDownFuncs, 0, sizeof(g_pKeyDownFuncs));
	memset(g_pKeyUpFuncs, 0, sizeof(g_pKeyUpFuncs));
	memset(g_pKeyPressedFuncs, 0, sizeof(g_pKeyPressedFuncs));

	int hr;
	HWND hwnd = GutGetWindowHandleWin32();
	HINSTANCE hinst = GutGetWindowInstanceWin32();
	hr = DirectInput8Create( hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&g_pDI, NULL);
	if ( FAILED(hr) )
		return 0;

	// create keyboard device
	hr = g_pDI->CreateDevice( GUID_SysKeyboard, &g_pKeyboard, NULL );
	if ( FAILED(hr) )
		return 0;

	if ( g_pKeyboard )
	{
		g_pKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
		g_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
		g_pKeyboard->Acquire();
	}

	// create mouse device
	hr = g_pDI->CreateDevice(GUID_SysMouse, &g_pMouse, NULL);
	if ( FAILED(hr) )
		return 0;

	if ( g_pMouse )
	{
		g_pMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
		g_pMouse->SetDataFormat(&c_dfDIMouse2);
		g_pMouse->Acquire();
	}

	GetCursorPos(&g_op);
	ScreenToClient(hwnd, &g_op);
	g_mouse = g_op;

	// create joystick device
	hr = g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);
	if ( FAILED(hr) )
		return 0;

	if ( g_pJoystick )
	{
		g_pJoystick->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
		g_pJoystick->SetDataFormat(&c_dfDIJoystick);
		g_pJoystick->Acquire();
	}

	return 1;
}

int GutInputClose(void)
{
	if ( g_pMouse )	g_pMouse->Unacquire();
	if ( g_pKeyboard ) g_pKeyboard->Unacquire();
	if ( g_pJoystick ) g_pJoystick->Unacquire();

	SAFE_RELEASE(g_pMouse);
	SAFE_RELEASE(g_pKeyboard);
	SAFE_RELEASE(g_pJoystick);

	return 1;
}

int GutReadMouse(GutMouseInfo *info)
{
	if( NULL == g_pMouse ) 
		return 0;

	// DirectInput mouse state structure
	DIMOUSESTATE2 dims2; 

	// Get the input's device state, and put the state in dims
	ZeroMemory( &dims2, sizeof(dims2) );
	int hr = g_pMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &dims2 );
	if ( FAILED(hr) )
	{
		g_pMouse->Acquire();
		g_pMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &dims2 );
	}

	info->x = dims2.lX;
	info->y = dims2.lY;
	info->z = dims2.lZ;

	g_mouse.x += info->x;
	g_mouse.y += info->y;

	info->button[0] = dims2.rgbButtons[0] & 0x80 ? 1 : 0;
	info->button[1] = dims2.rgbButtons[1] & 0x80 ? 1 : 0;
	info->button[2] = dims2.rgbButtons[2] & 0x80 ? 1 : 0;

	return 1;
}

int GutReadKeyboard(char buffer[256])
{
	if ( NULL == g_pKeyboard )
		return 0;

	int hr = g_pKeyboard->GetDeviceState( 256, buffer );

	if ( FAILED(hr) )
	{
		hr = g_pKeyboard->Acquire();
		for(int i=0; hr == DIERR_INPUTLOST && i<10; i++ ) 
		{
			hr = g_pKeyboard->Acquire();
			if ( !FAILED(hr) ) break;
		}
		memset(buffer, 0, 256);
		return 0;
	}
	return 1;
}

int GutReadJoystick(GutJoystickInfo *joystick)
{
	DIJOYSTATE js;
	if ( g_pJoystick==NULL || joystick==NULL )
		return 0;
	// Poll the device to read the current state
	int hr = g_pJoystick->Poll();
	if( FAILED(hr) )
	{
		g_pJoystick->Acquire();
		g_pJoystick->Poll();
	}

	// Get the input's device state
	hr = g_pJoystick->GetDeviceState( sizeof(DIJOYSTATE), &js );
	if( hr == DIERR_INPUTLOST )
	{
		// DInput is telling us that the input stream has been
		// interrupted. We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done. We
		// just re-acquire and try again.
		hr = g_pJoystick->Acquire();
		if( FAILED(hr) )  
			return 0;
	}

	joystick->x = js.lX;
	joystick->y = js.lY;
	joystick->z = js.lZ;
	joystick->rx = js.lRx;
	joystick->ry = js.lRy;
	joystick->rz = js.lRz;
	joystick->pov[0] = js.rgdwPOV[0];
	joystick->pov[1] = js.rgdwPOV[1];
	joystick->pov[2] = js.rgdwPOV[2];
	joystick->pov[3] = js.rgdwPOV[3];

	memcpy(joystick->button, js.rgbButtons, 32);

	return 1;
}

void GutReadKeyboard(void)
{
	static char keyboard[256];
	GutReadKeyboard(keyboard);

	for ( int i=0; i<256; i++ )
	{
		// key down
		if ( keyboard[i] && !g_keyboard_state[i] )
		{
			if ( g_pKeyDownFuncs[i] ) g_pKeyDownFuncs[i]();
		}

		// key up
		if ( !keyboard[i] && g_keyboard_state[i] )
		{
			if ( g_pKeyUpFuncs[i] ) g_pKeyUpFuncs[i]();
		}

		// key pressed
		if ( keyboard[i] )
		{
			if ( g_pKeyPressedFuncs[i] ) g_pKeyPressedFuncs[i]();
		}
	}

	memcpy(g_keyboard_state, keyboard, sizeof(keyboard));
}

void GutRegisterKeyDown(int key, void (*pKeyDownFunc)(void))
{
	g_pKeyDownFuncs[key] = pKeyDownFunc;
}

void GutRegisterKeyUp  (int key, void (*pKeyDownUp)(void))
{
	g_pKeyUpFuncs[key] = pKeyDownUp;
}

void GutRegisterKeyPressed(int key, void (*pKeyPressed)(void))
{
	g_pKeyPressedFuncs[key] = pKeyPressed;
}
