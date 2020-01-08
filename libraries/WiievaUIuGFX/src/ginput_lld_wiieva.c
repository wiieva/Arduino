#include "gfx.h"
#include "aioiface.h"

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE

#define GMOUSE_DRIVER_VMT		GMOUSEVMT_MCU
#include "src/ginput/ginput_driver_mouse.h"

static bool_t mouse_init (GMouse *m, unsigned driverinstance) {
    return TRUE;
}

extern void wiieva_read_input (AIO_InputState *input_state);
static bool_t mouse_read_xyz(GMouse *m, GMouseReading *prd) {
    AIO_InputState input_state;
    wiieva_read_input (&input_state);
    prd->x = input_state.x;
    prd->y = input_state.y;
    prd->z = input_state.z;
    return TRUE;
}
static GMouseCalibration calibration = {0.0486,0.0005,-8.7076,0.0004,0.0703,-16.8264};

static void mouse_calsave (GMouse *m, const void *buf, size_t sz) {
    calibration = *((GMouseCalibration *)buf);
    //ets_printf( "cal ax=%d,bx=%d,cx=%d,ay=%d,by=%d,cy=%d\r\n",(int)(c->ax*10000.0),
    //(int)(c->bx*10000.0),(int)(c->cx*10000.0),(int)(c->ay*10000.0),(int)(c->by*10000.0),(int)(c->cy*10000.0));
}

static bool_t mouse_calload (GMouse *m, void *buf, size_t sz) {
    *((GMouseCalibration *)buf) = calibration;
    return TRUE;
}

uint16_t wiievaGetTouchCalibrationData (void *buf,size_t bufSize) {
    if (bufSize < sizeof (GMouseCalibration))
        return 0;
    memcpy (buf,&calibration,sizeof (calibration));
    return sizeof (calibration);
}

uint16_t wiievaSetTouchCalibrationData (void *buf,size_t bufSize) {
    if (bufSize < sizeof (GMouseCalibration))
        return 0;
    memcpy (&calibration,buf,sizeof (calibration));
    return sizeof (calibration);
}



const GMouseVMT GMOUSE_DRIVER_VMT[1] = {{
    {
        GDRIVER_TYPE_TOUCH,
        GMOUSE_VFLG_TOUCH|GMOUSE_VFLG_CALIBRATE|GMOUSE_VFLG_CAL_TEST|GMOUSE_VFLG_ONLY_DOWN|GMOUSE_VFLG_POORUPDOWN,
        sizeof(GMouse),
        _gmouseInitDriver, _gmousePostInitDriver, _gmouseDeInitDriver
    },
    4095,			// z_max
    0,			    // z_min
    1000,		    // z_touchon
    500,		    // z_touchoff
    {				// pen_jitter
        8,			// calibrate
        6,			// click
        4			// move
    },
    {				// finger_jitter
        14,		    // calibrate
        18,			// click
        14          // move
    },
    mouse_init,		// init
    0,				        // deinit
    mouse_read_xyz,	// get
    mouse_calsave,  // calsave
    mouse_calload	// calload
}};

#endif /* GFX_USE_GINPUT && GINPUT_NEED_MOUSE */

#if GFX_USE_GINPUT && GINPUT_NEED_KEYBOARD

#define GKEYBOARD_DRIVER_VMT            GKEYBOARDVMT_WIIEVA
#include "src/ginput/ginput_driver_keyboard.h"

static bool_t keyboard_init(GKeyboard *k, unsigned driverinstance) {
	return TRUE;
}

static uint16_t last_keys = 0;
static int keyboard_getgata(GKeyboard *k, uint8_t *pch, int sz) {
    AIO_InputState input_state;

    if (!pch || sz == 0)
        return 0;

    wiieva_read_input (&input_state);
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
		0,
		sizeof(GKeyboard),
		_gkeyboardInitDriver, _gkeyboardPostInitDriver, _gkeyboardDeInitDriver
	},
	0,
	keyboard_init,      // init
	0,                  // deinit
	keyboard_getgata,   // getdata
	0                   // putdata
}};

#endif

