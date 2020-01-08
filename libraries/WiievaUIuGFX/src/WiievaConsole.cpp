
#include "WiievaConsole.h"
#include "WiievaPlotter.h"

void WiievaConsole::begin (WiievaConsole::FontSize fontSize,
    WiievaConsole::Orientation orient,
    int backlight,
    uint32_t fgColor,uint32_t bgColor) {

    gfxInit ();
    gdispSetBacklight (backlight);
    gdispSetOrientation (orient==Landscape?GDISP_ROTATE_90:GDISP_ROTATE_0);

    // Setup defaults for GUI
    gwinSetDefaultFont(gdispOpenFont("DejaVuSans12"));
    gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
    gwinSetDefaultColor(HTML2COLOR(fgColor));
    gwinSetDefaultBgColor(HTML2COLOR(bgColor));

    GWindowInit wi;
    gwinClearInit(&wi);

    wi.show = TRUE;
    wi.width = gdispGetWidth ();
    wi.height = gdispGetHeight ();
    switch (fontSize) {
        case FontSmallest:
            font = gdispOpenFont("fixed_5x8");
            break;
        case FontSmall:
            font = gdispOpenFont("DejaVuSans10");
            break;
        default:
        case FontNormal:
            font = gdispOpenFont("DejaVuSans12");
            break;
        case FontBig:
            font = gdispOpenFont("DejaVuSans16");
            break;
    }

    drawUsage ();

    ghConsole = gwinConsoleCreate (0,&wi);
    gwinSetFont (ghConsole,font);
    gwinConsoleSetBuffer (ghConsole,TRUE);

    gwinShow (ghConsole);

    GWidgetInit wgi;
    gwinWidgetClearInit(&wgi);
    wgi.g.show = FALSE;
    wgi.g.x = 0;
    wgi.g.y = gdispGetHeight()-80;
    wgi.g.width = gdispGetWidth();
    wgi.g.height = 80;
    wgi.customDraw = gwinKeyboardDraw_Normal;
    ghKeyboard = gwinKeyboardCreate(0, &wgi);

    geventListenerInit(&glistener);
    geventAttachSource(&glistener, ginputGetMouse(GMOUSE_ALL_INSTANCES), GLISTEN_MOUSEMETA|GLISTEN_MOUSEDOWNMOVES);
    geventAttachSource(&glistener, ginputGetKeyboard(GKEYBOARD_ALL_INSTANCES), 0);
    geventRegisterCallback (&glistener,[] (void *p,GEvent *ev) { ((WiievaConsole*)p)->onEvent(ev);} ,this);
    plotter.begin ();
}

void WiievaConsole::end () {
    plotter.end ();
    gwinDestroy (ghKeyboard);
    gwinDestroy (ghConsole);
    gdispSetBacklight (0);
    gdispCloseFont (font);
    gfxDeinit ();
}

void WiievaConsole::drawUsage () {
    int h = gdispGetHeight (), w = gdispGetWidth ();
    uint32_t c = gwinGetDefaultColor ();

    gdispClear (gwinGetDefaultBgColor());
    gdispDrawBox (0,0,w,h,c);
    gdispDrawLine (w/2,0,w/2,h-80,c);
    gdispDrawLine (0,h-80,w,h-80,c);

    gdispDrawStringBox (1,1,w/2-2,h-82,"Tap here to show plotter",font,c,justifyCenter);
    gdispDrawStringBox (w/2+1,1,w/2-2,h-82,"Tap here to show keyboard or pause plotter",font,c,justifyCenter);
    gdispDrawStringBox (1,h-78,w,77,"Wiieva console usage.",font,c,justifyCenter);
    gfxSleepMilliseconds (2000);
}

WiievaConsole::~WiievaConsole () {
    end ();
}


int WiievaConsole::available(void) {
    gfxYield ();
    return buf.available();
}

int WiievaConsole::peek(void) {
    gfxYield ();
    return buf.peek();
}

int WiievaConsole::read(void) {
    gfxYield ();
    return buf.read();
}

size_t WiievaConsole::write(uint8_t c) {
    if (!plotterVisible) {
        gwinPutChar (ghConsole,c);
        gfxYield ();
    } else if (!plotterPaused) {
        plotter.write (c);
    } else
        gfxYield ();
}

void WiievaConsole::setPlotterMode (WiievaPlotter::Mode _mode) {
    plotter.setMode(_mode);
}

void WiievaConsole::toggleKeyboard (bool show) {
    if (plotterVisible)
        return;
    keyboardVisible = show;
    if (show) {
        gwinResize (ghConsole,gdispGetWidth(),gdispGetHeight()-80);
        gwinShow (ghKeyboard);
    } else {
        gwinResize (ghConsole,gdispGetWidth(),gdispGetHeight());
        gwinHide (ghKeyboard);
   }
}

void WiievaConsole::togglePlotter (bool show) {
    plotterVisible = show;
    if (show) {
        plotterPaused = false;
        gwinHide (ghConsole);
        gwinHide (ghKeyboard);
        plotter.draw ();
    } else {
        gwinShow (ghConsole);
        toggleKeyboard (keyboardVisible);
    }
}

void WiievaConsole::onEvent(GEvent *pe) {
    switch (pe->type) {
        case GEVENT_MOUSE:
        case GEVENT_TOUCH: {
            GEventMouse *me = (GEventMouse *)pe;
            if  ((me->buttons & GINPUT_MOUSE_BTN_LEFT) && (me->y < gdispGetHeight()-80))
                if (me->x > gdispGetWidth()/2) {
                    if (plotterVisible)
                        plotterPaused = !plotterPaused;
                    else
                        toggleKeyboard (!keyboardVisible);
                }
                else
                    togglePlotter (!plotterVisible);
            break;
        }
        case GEVENT_GWIN_KEYBOARD:
        case GEVENT_KEYBOARD: {
            GEventKeyboard *ke = (GEventKeyboard *)pe;
            buf.write (ke->c[0]);
        }
    }
}
