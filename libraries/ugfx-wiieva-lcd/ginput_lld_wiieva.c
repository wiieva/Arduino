#include "gfx.h"
#include "aioiface.h"

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE

#define GMOUSE_DRIVER_VMT		GMOUSEVMT_MCU
#include "src/ginput/ginput_driver_mouse.h"

void wiieva_lcd_read_input (AIO_InputState *input_state);
static bool_t init_board(GMouse *m, unsigned driverinstance) {

}

static bool_t read_xyz(GMouse *m, GMouseReading *prd) {
    AIO_InputState input_state;
    wiieva_lcd_read_input (&input_state);
    prd->x = input_state.x;
    prd->y = input_state.y;
    prd->z = input_state.z;
    return true;
}

// Resolution and Accuracy Settings
#define GMOUSE_MCU_PEN_CALIBRATE_ERROR		8
#define GMOUSE_MCU_PEN_CLICK_ERROR			6
#define GMOUSE_MCU_PEN_MOVE_ERROR			4
#define GMOUSE_MCU_FINGER_CALIBRATE_ERROR	14
#define GMOUSE_MCU_FINGER_CLICK_ERROR		18
#define GMOUSE_MCU_FINGER_MOVE_ERROR		14
#define GMOUSE_MCU_Z_MIN                    0
#define GMOUSE_MCU_Z_MAX                    4095
#define GMOUSE_MCU_Z_TOUCHON                1000
#define GMOUSE_MCU_Z_TOUCHOFF               500
#define GMOUSE_MCU_BOARD_DATA_SIZE          0

void calsave (GMouse *m, const void *buf, size_t sz)
{
    GMouseCalibration *c = (GMouseCalibration *)buf;
    ets_printf( "cal ax=%d,bx=%d,cx=%d,ay=%d,by=%d,cy=%d\r\n",(int)(c->ax*10000.0),
    (int)(c->bx*10000.0),(int)(c->cx*10000.0),(int)(c->ay*10000.0),(int)(c->by*10000.0),(int)(c->cy*10000.0));
}

bool_t calload (GMouse *m, void *buf, size_t sz)
{
    GMouseCalibration *c = (GMouseCalibration *)buf;
    c->ax=0.0486;
    c->bx=0.0005;
    c->cx=-8.7076;
    c->ay=0.0004;
    c->by=0.0703;
    c->cy=-16.8264;
    return TRUE;
}

const GMouseVMT GMOUSE_DRIVER_VMT[1] = {{
    {
        GDRIVER_TYPE_TOUCH,
        GMOUSE_VFLG_TOUCH|GMOUSE_VFLG_CALIBRATE|GMOUSE_VFLG_CAL_TEST|GMOUSE_VFLG_ONLY_DOWN|GMOUSE_VFLG_POORUPDOWN,
        sizeof(GMouse) + GMOUSE_MCU_BOARD_DATA_SIZE,
        _gmouseInitDriver, _gmousePostInitDriver, _gmouseDeInitDriver
    },
    GMOUSE_MCU_Z_MAX,			// z_max
    GMOUSE_MCU_Z_MIN,			// z_min
    GMOUSE_MCU_Z_TOUCHON,		// z_touchon
    GMOUSE_MCU_Z_TOUCHOFF,		// z_touchoff
    {				// pen_jitter
        GMOUSE_MCU_PEN_CALIBRATE_ERROR,			// calibrate
        GMOUSE_MCU_PEN_CLICK_ERROR,				// click
        GMOUSE_MCU_PEN_MOVE_ERROR				// move
    },
    {				// finger_jitter
        GMOUSE_MCU_FINGER_CALIBRATE_ERROR,		// calibrate
        GMOUSE_MCU_FINGER_CLICK_ERROR,			// click
        GMOUSE_MCU_FINGER_MOVE_ERROR			// move
    },
    init_board,		// init
    0,				// deinit
    read_xyz,		// get
    calsave,				// calsave
    calload				// calload
}};

#endif /* GFX_USE_GINPUT && GINPUT_NEED_MOUSE */

#if GFX_USE_GINPUT && GINPUT_NEED_KEYBOARD

#define GKEYBOARD_DRIVER_VMT            GKEYBOARDVMT_WIIEVA
#include "src/ginput/ginput_driver_keyboard.h"

static bool_t keyboard_init(GKeyboard *k, unsigned driverinstance) 
{
	return true;
}

static uint16_t last_keys = 0;



static int keyboard_getgata(GKeyboard *k, uint8_t *pch, int sz)
{

    return 0;
    AIO_InputState input_state;

    if (!pch || sz == 0)
        return 0;

    wiieva_lcd_read_input (&input_state);
    int i;

    uint16_t changed_keys = last_keys ^ input_state.keys;

    if (!changed_keys)
        return 0;

    for (i = 0; i < 16; ++i)
        if (changed_keys & (1<<i)) {
            switch (i) {
                case AIO_KEY_CODE_DOWN: *pch = GKEY_DOWN; break;
                case AIO_KEY_CODE_UP:   *pch = GKEY_UP; break;
                case AIO_KEY_CODE_LEFT: *pch = GKEY_LEFT; break;
                case AIO_KEY_CODE_RIGHT: *pch = GKEY_RIGHT; break;
                case AIO_KEY_CODE_OK: *pch = GKEY_ENTER; break;
                case AIO_KEY_CODE_KB2: *pch = GKEY_FN1; break;
                case AIO_KEY_CODE_PWR: *pch = GKEY_BACKSPACE; break;
                default:
                    return 0;
            }
            k->keystate = 0;
            if (*pch & 0x80)
                k->keystate = GKEYSTATE_SPECIAL;
            if (!(input_state.keys & (1<<i)))
                k->keystate |= GKEYSTATE_KEYUP;
            last_keys ^= (1<<i);
            return 1;
    }

	return 0;
}


const GKeyboardVMT GKEYBOARD_DRIVER_VMT[1] = {{
	{
		GDRIVER_TYPE_KEYBOARD,
		0,                  //  GKEYBOARD_VFLG_DYNAMICONLY
		sizeof(GKeyboard),
		_gkeyboardInitDriver, _gkeyboardPostInitDriver, _gkeyboardDeInitDriver
	},
	0,
	keyboard_init,                       // init
	0,                                              // deinit
	keyboard_getgata,            // getdata
	0                                               // putdata              void    (*putdata)(GKeyboard *k, char ch);              Optional
}};

#endif

