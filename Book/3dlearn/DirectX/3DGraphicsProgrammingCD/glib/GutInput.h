#ifndef _GUTINPUT_
#define _GUTINPUT_

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

struct GutMouseInfo
{
	// 滑鼠相對移動的位置
	int x,y;
	// 滑鼠滾輪的滾動狀態
	int z;
	// 滑鼠按鈕的狀態
	unsigned char button[3];
};

struct GutJoystickInfo
{
	int x, y, z;
	int rx, ry, rz;
	int pov[4];
	unsigned char button[32];
};

enum GutInputDevice
{
	GINPUT_KEYBOARD,
	GINPUT_MOUSE,
	GINPUT_JOYSTICK
};

int GutCheckInputDevice(GutInputDevice input);
int GutInputInit(void);
int GutInputClose(void);
int GutInputRestore(void);

int GutReadKeyboard(char buffer[256]);
int GutReadMouse(GutMouseInfo *mouse);
int GutReadJoystick(GutJoystickInfo *jostick);
int GutReadJoystick(GutJoystickInfo *jostick, int whichone);

void GutReadKeyboard(void);
void GutRegisterKeyDown(int key, void (*pKeyDownFunc)(void));
void GutRegisterKeyUp  (int key, void (*pKeyDownUp)(void));
void GutRegisterKeyPressed(int key, void (*pKeyDownPressed)(void));

#define GUTKEY_ESCAPE          0x01
#define GUTKEY_1               0x02
#define GUTKEY_2               0x03
#define GUTKEY_3               0x04
#define GUTKEY_4               0x05
#define GUTKEY_5               0x06
#define GUTKEY_6               0x07
#define GUTKEY_7               0x08
#define GUTKEY_8               0x09
#define GUTKEY_9               0x0A
#define GUTKEY_0               0x0B
#define GUTKEY_MINUS           0x0C    /* - on main keyboard */
#define GUTKEY_EQUALS          0x0D
#define GUTKEY_BACK            0x0E    /* backspace */
#define GUTKEY_TAB             0x0F
#define GUTKEY_Q               0x10
#define GUTKEY_W               0x11
#define GUTKEY_E               0x12
#define GUTKEY_R               0x13
#define GUTKEY_T               0x14
#define GUTKEY_Y               0x15
#define GUTKEY_U               0x16
#define GUTKEY_I               0x17
#define GUTKEY_O               0x18
#define GUTKEY_P               0x19
#define GUTKEY_LBRACKET        0x1A
#define GUTKEY_RBRACKET        0x1B
#define GUTKEY_RETURN          0x1C    /* Enter on main keyboard */
#define GUTKEY_LCONTROL        0x1D
#define GUTKEY_A               0x1E
#define GUTKEY_S               0x1F
#define GUTKEY_D               0x20
#define GUTKEY_F               0x21
#define GUTKEY_G               0x22
#define GUTKEY_H               0x23
#define GUTKEY_J               0x24
#define GUTKEY_K               0x25
#define GUTKEY_L               0x26
#define GUTKEY_SEMICOLON       0x27
#define GUTKEY_APOSTROPHE      0x28
#define GUTKEY_GRAVE           0x29    /* accent grave */
#define GUTKEY_LSHIFT          0x2A
#define GUTKEY_BACKSLASH       0x2B
#define GUTKEY_Z               0x2C
#define GUTKEY_X               0x2D
#define GUTKEY_C               0x2E
#define GUTKEY_V               0x2F
#define GUTKEY_B               0x30
#define GUTKEY_N               0x31
#define GUTKEY_M               0x32
#define GUTKEY_COMMA           0x33
#define GUTKEY_PERIOD          0x34    /* . on main keyboard */
#define GUTKEY_SLASH           0x35    /* / on main keyboard */
#define GUTKEY_RSHIFT          0x36
#define GUTKEY_MULTIPLY        0x37    /* * on numeric keypad */
#define GUTKEY_LMENU           0x38    /* left Alt */
#define GUTKEY_SPACE           0x39
#define GUTKEY_CAPITAL         0x3A
#define GUTKEY_F1              0x3B
#define GUTKEY_F2              0x3C
#define GUTKEY_F3              0x3D
#define GUTKEY_F4              0x3E
#define GUTKEY_F5              0x3F
#define GUTKEY_F6              0x40
#define GUTKEY_F7              0x41
#define GUTKEY_F8              0x42
#define GUTKEY_F9              0x43
#define GUTKEY_F10             0x44
#define GUTKEY_NUMLOCK         0x45
#define GUTKEY_SCROLL          0x46    /* Scroll Lock */
#define GUTKEY_NUMPAD7         0x47
#define GUTKEY_NUMPAD8         0x48
#define GUTKEY_NUMPAD9         0x49
#define GUTKEY_SUBTRACT        0x4A    /* - on numeric keypad */
#define GUTKEY_NUMPAD4         0x4B
#define GUTKEY_NUMPAD5         0x4C
#define GUTKEY_NUMPAD6         0x4D
#define GUTKEY_ADD             0x4E    /* + on numeric keypad */
#define GUTKEY_NUMPAD1         0x4F
#define GUTKEY_NUMPAD2         0x50
#define GUTKEY_NUMPAD3         0x51
#define GUTKEY_NUMPAD0         0x52
#define GUTKEY_DECIMAL         0x53    /* . on numeric keypad */
#define GUTKEY_OEM_102         0x56    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define GUTKEY_F11             0x57
#define GUTKEY_F12             0x58
#define GUTKEY_F13             0x64    /*                     (NEC PC98) */
#define GUTKEY_F14             0x65    /*                     (NEC PC98) */
#define GUTKEY_F15             0x66    /*                     (NEC PC98) */
#define GUTKEY_KANA            0x70    /* (Japanese keyboard)            */
#define GUTKEY_ABNT_C1         0x73    /* /? on Brazilian keyboard */
#define GUTKEY_CONVERT         0x79    /* (Japanese keyboard)            */
#define GUTKEY_NOCONVERT       0x7B    /* (Japanese keyboard)            */
#define GUTKEY_YEN             0x7D    /* (Japanese keyboard)            */
#define GUTKEY_ABNT_C2         0x7E    /* Numpad . on Brazilian keyboard */
#define GUTKEY_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define GUTKEY_PREVTRACK       0x90    /* Previous Track (GUTKEY_CIRCUMFLEX on Japanese keyboard) */
#define GUTKEY_AT              0x91    /*                     (NEC PC98) */
#define GUTKEY_COLON           0x92    /*                     (NEC PC98) */
#define GUTKEY_UNDERLINE       0x93    /*                     (NEC PC98) */
#define GUTKEY_KANJI           0x94    /* (Japanese keyboard)            */
#define GUTKEY_STOP            0x95    /*                     (NEC PC98) */
#define GUTKEY_AX              0x96    /*                     (Japan AX) */
#define GUTKEY_UNLABELED       0x97    /*                        (J3100) */
#define GUTKEY_NEXTTRACK       0x99    /* Next Track */
#define GUTKEY_NUMPADENTER     0x9C    /* Enter on numeric keypad */
#define GUTKEY_RCONTROL        0x9D
#define GUTKEY_MUTE            0xA0    /* Mute */
#define GUTKEY_CALCULATOR      0xA1    /* Calculator */
#define GUTKEY_PLAYPAUSE       0xA2    /* Play / Pause */
#define GUTKEY_MEDIASTOP       0xA4    /* Media Stop */
#define GUTKEY_VOLUMEDOWN      0xAE    /* Volume - */
#define GUTKEY_VOLUMEUP        0xB0    /* Volume + */
#define GUTKEY_WEBHOME         0xB2    /* Web home */
#define GUTKEY_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define GUTKEY_DIVIDE          0xB5    /* / on numeric keypad */
#define GUTKEY_SYSRQ           0xB7
#define GUTKEY_RMENU           0xB8    /* right Alt */
#define GUTKEY_PAUSE           0xC5    /* Pause */
#define GUTKEY_HOME            0xC7    /* Home on arrow keypad */
#define GUTKEY_UP              0xC8    /* UpArrow on arrow keypad */
#define GUTKEY_PRIOR           0xC9    /* PgUp on arrow keypad */
#define GUTKEY_LEFT            0xCB    /* LeftArrow on arrow keypad */
#define GUTKEY_RIGHT           0xCD    /* RightArrow on arrow keypad */
#define GUTKEY_END             0xCF    /* End on arrow keypad */
#define GUTKEY_DOWN            0xD0    /* DownArrow on arrow keypad */
#define GUTKEY_NEXT            0xD1    /* PgDn on arrow keypad */
#define GUTKEY_INSERT          0xD2    /* Insert on arrow keypad */
#define GUTKEY_DELETE          0xD3    /* Delete on arrow keypad */
#define GUTKEY_LWIN            0xDB    /* Left Windows key */
#define GUTKEY_RWIN            0xDC    /* Right Windows key */
#define GUTKEY_APPS            0xDD    /* AppMenu key */
#define GUTKEY_POWER           0xDE    /* System Power */
#define GUTKEY_SLEEP           0xDF    /* System Sleep */
#define GUTKEY_WAKE            0xE3    /* System Wake */
#define GUTKEY_WEBSEARCH       0xE5    /* Web Search */
#define GUTKEY_WEBFAVORITES    0xE6    /* Web Favorites */
#define GUTKEY_WEBREFRESH      0xE7    /* Web Refresh */
#define GUTKEY_WEBSTOP         0xE8    /* Web Stop */
#define GUTKEY_WEBFORWARD      0xE9    /* Web Forward */
#define GUTKEY_WEBBACK         0xEA    /* Web Back */
#define GUTKEY_MYCOMPUTER      0xEB    /* My Computer */
#define GUTKEY_MAIL            0xEC    /* Mail */
#define GUTKEY_MEDIASELECT     0xED    /* Media Select */

/*
*  Alternate names for keys, to facilitate transition from DOS.
*/
#define GUTKEY_BACKSPACE       GUTKEY_BACK            /* backspace */
#define GUTKEY_NUMPADSTAR      GUTKEY_MULTIPLY        /* * on numeric keypad */
#define GUTKEY_LALT            GUTKEY_LMENU           /* left Alt */
#define GUTKEY_CAPSLOCK        GUTKEY_CAPITAL         /* CapsLock */
#define GUTKEY_NUMPADMINUS     GUTKEY_SUBTRACT        /* - on numeric keypad */
#define GUTKEY_NUMPADPLUS      GUTKEY_ADD             /* + on numeric keypad */
#define GUTKEY_NUMPADPERIOD    GUTKEY_DECIMAL         /* . on numeric keypad */
#define GUTKEY_NUMPADSLASH     GUTKEY_DIVIDE          /* / on numeric keypad */
#define GUTKEY_RALT            GUTKEY_RMENU           /* right Alt */
#define GUTKEY_UPARROW         GUTKEY_UP              /* UpArrow on arrow keypad */
#define GUTKEY_PGUP            GUTKEY_PRIOR           /* PgUp on arrow keypad */
#define GUTKEY_LEFTARROW       GUTKEY_LEFT            /* LeftArrow on arrow keypad */
#define GUTKEY_RIGHTARROW      GUTKEY_RIGHT           /* RightArrow on arrow keypad */
#define GUTKEY_DOWNARROW       GUTKEY_DOWN            /* DownArrow on arrow keypad */
#define GUTKEY_PGDN            GUTKEY_NEXT            /* PgDn on arrow keypad */

#endif // _GUTINPUT_