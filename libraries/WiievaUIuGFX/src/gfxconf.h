
#ifndef _GFXCONF_H
#define _GFXCONF_H


/********************************************************/
/* GDISP stuff                                          */
/********************************************************/
#define GFX_USE_GDISP TRUE

#define GFX_USE_OS_ARDUINO TRUE
#define GDISP_NEED_CONTROL TRUE
#define GDISP_NEED_VALIDATION FALSE
#define GDISP_NEED_CLIP TRUE
#define GDISP_NEED_ARC FALSE
#define GDISP_NEED_CONVEX_POLYGON TRUE
#define GDISP_NEED_IMAGE TRUE
#define GDISP_NEED_IMAGE_BMP TRUE
#define GDISP_NEED_IMAGE_GIF FALSE
#define GDISP_NEED_STARTUP_LOGO FALSE
#define GDISP_NEED_CIRCLE TRUE
#define GDISP_NEED_MULTITHREAD TRUE
#define GDISP_DEFAULT_ORIENTATION GDISP_ROTATE_0
#define GDISP_STARTUP_COLOR HTML2COLOR(0xFFFFFF)
#define GDISP_NEED_IMAGE_NATIVE TRUE

/********************************************************/
/* Font stuff                                           */
/********************************************************/
#define GDISP_NEED_TEXT TRUE
#define GDISP_NEED_ANTIALIAS FALSE
#define GDISP_NEED_TEXT_KERNING FALSE
#define GDISP_NEED_UTF8 TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS10 TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS12 TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANSBOLD12 TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS16 TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS20 TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS24 TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS32 TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS32 TRUE
#define GDISP_INCLUDE_FONT_FIXED_10X20  TRUE
#define GDISP_INCLUDE_FONT_FIXED_5X8    TRUE

/********************************************************/
/* GWIN stuff                                           */
/********************************************************/
#define GFX_USE_GWIN TRUE

#define GWIN_NEED_WINDOWMANAGER TRUE
    #define GWIN_REDRAW_IMMEDIATE FALSE
    #define GWIN_REDRAW_SINGLEOP FALSE

#define GWIN_NEED_WIDGET TRUE
    #define GWIN_NEED_LABEL TRUE
    #define GWIN_NEED_CHECKBOX TRUE
    #define GWIN_NEED_BUTTON TRUE
        #define GWIN_BUTTON_LAZY_RELEASE FALSE
    #define GWIN_NEED_IMAGE TRUE
        #define GWIN_NEED_IMAGE_ANIMATION FALSE
    #define GWIN_NEED_LIST TRUE
        #define GWIN_NEED_LIST_IMAGES TRUE
    #define GWIN_FLAT_STYLING FALSE

    #define GWIN_NEED_PROGRESSBAR TRUE
        #define GWIN_PROGRESSBAR_AUTO FALSE
    #define GWIN_NEED_KEYBOARD TRUE
    #define GWIN_NEED_TEXTEDIT TRUE
#define GWIN_NEED_CONTAINERS TRUE
    #define GWIN_NEED_CONTAINER TRUE
    #define GWIN_NEED_FRAME FALSE
    #define GWIN_NEED_TABSET FALSE
#define GWIN_NEED_SLIDER		TRUE


/********************************************************/
/* GTIMER stuff                                         */
/********************************************************/
#define GFX_USE_GTIMER TRUE

#define GTIMER_THREAD_PRIORITY NORMAL_PRIORITY
#define GTIMER_THREAD_WORKAREA_SIZE 2048


/********************************************************/
/* GINPUT stuff                                         */
/********************************************************/
#define GFX_USE_GINPUT TRUE

#define GINPUT_NEED_MOUSE TRUE
#define GINPUT_TOUCH_USER_CALIBRATION_LOAD FALSE
#define GINPUT_TOUCH_NOCALIBRATE_GUI FALSE
#define GINPUT_NEED_KEYBOARD TRUE


/********************************************************/
/* GEVENT stuff                                         */
/********************************************************/
#define GFX_USE_GEVENT TRUE

#define GEVENT_ASSERT_NO_RESOURCE FALSE
#define GEVENT_MAXIMUM_SIZE 32
#define GEVENT_MAX_SOURCE_LISTENERS 32


/********************************************************/
/* GEVENT stuff                                         */
/********************************************************/
#define GFX_USE_GQUEUE TRUE

#define GQUEUE_NEED_ASYNC TRUE


/********************************************************/
/* GFILE stuff                                          */
/********************************************************/
#define GFX_USE_GFILE TRUE

#define GFILE_NEED_NATIVEFS FALSE
#define GFILE_NEED_USERFS TRUE
#define GFILE_MAX_GFILES 17

#define GWIN_NEED_CONSOLE                            TRUE
    #define GWIN_CONSOLE_USE_HISTORY                 TRUE
        #define GWIN_CONSOLE_HISTORY_AVERAGING       TRUE
        #define GWIN_CONSOLE_HISTORY_ATCREATE        TRUE
    #define GWIN_CONSOLE_ESCSEQ                      TRUE
    #define GWIN_CONSOLE_USE_BASESTREAM              TRUE
#define GWIN_NEED_GRAPH                              TRUE

 /* _GFXCONF_H */

// Additional defines

#ifdef __ets__

#define GFX_ROM_DATA_ATTR __attribute__((section(".irom.text")))
#define GFX_NEED_ROM_ALIGN32_READ TRUE
#define _setjmp setjmp
#endif

#define GDISP_NEED_TEXT_WORDWRAP TRUE

#endif

