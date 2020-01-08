#pragma once

#include <Stream.h>
#include <cbuf.h>
#include <gfx.h>
#include "WiievaPlotter.h"

class WiievaConsole : public Stream {
public:
    enum FontSize {
        FontSmallest,
        FontSmall,
        FontNormal,
        FontBig
    };
    enum Orientation {
        Portrait,
        Landscape,
    };

public:
    ~WiievaConsole ();
    void begin (WiievaConsole::FontSize fs=FontSmall,
        WiievaConsole::Orientation orient=Portrait,
        int backlight=50,
        uint32_t fgColor=0x00C000,
        uint32_t bgColor=0x00);
    void end ();
    void begin (int baudRate) {
        begin ();
    }
    void setDebugOutput (bool) {
    }
    int available(void) override;
    int peek(void) override;
    int read(void) override;
    void flush(void) override {};
    size_t write(uint8_t) override;
    inline size_t write(unsigned long n) {
        return write((uint8_t) n);
    }
    inline size_t write(long n) {
        return write((uint8_t) n);
    }
    inline size_t write(unsigned int n) {
        return write((uint8_t) n);
    }
    inline size_t write(int n) {
        return write((uint8_t) n);
    }

    void toggleKeyboard (bool show=true);
    void togglePlotter (bool show=true);
    void drawUsage ();

    void setPlotterMode (WiievaPlotter::Mode mode);

    using Print::write; // pull in write(str) and write(buf, size) from Print

    WiievaConsole &black () { 
        write ("\x1B""0",2); 
        return *this;
    }
    WiievaConsole &red () { 
        write ("\x1B""1",2); 
        return *this;
    }
    WiievaConsole &green () { 
        write ("\x1B""2",2); 
        return *this;
    }
    WiievaConsole &yellow () { 
        write ("\x1B""3",2); 
        return *this;
    }
    WiievaConsole &blue () {
        write ("\x1B""4",2);
        return *this;
    }
    WiievaConsole &magenta () {
        write ("\x1B""5",2); 
        return *this;
    }
    WiievaConsole &cyan () { 
        write ("\x1B""6",2); 
        return *this;
    }
    WiievaConsole &white () { 
        write ("\x1B""7",2); 
        return *this;
    }
    WiievaConsole &bold () { 
        write ("\x1B""b",2); 
        return *this;
    }
    WiievaConsole &underline () { 
        write ("\x1B""u",2); 
        return *this;
    }
    WiievaConsole &normal () {
        write ("\x1B""C\x1B""U\x1B""B",6); 
        return *this;
    }
    WiievaConsole &clear () { 
        write ("\x1B""J",2); 
        return *this;
    }

protected:
    void onEvent(GEvent *ev);

// Console variables
    GHandle ghConsole,ghKeyboard;
    GListener glistener;
    bool keyboardVisible = false,plotterVisible = false, plotterPaused = false;
    cbuf buf = cbuf(64);
    WiievaPlotter plotter;
    font_t font = nullptr;
};
