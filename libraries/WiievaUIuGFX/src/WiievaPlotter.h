#pragma once

#include <Stream.h>
#include <cbuf.h>
#include <gfx.h>


// moved from class to avoid constexpr compiler lottery
const float fixPointMul = 100.0 ;

class WiievaPlotter  {
public:
    enum Mode {
        Oscilloscope,
        Plotter,
    };

public:
    ~WiievaPlotter ();
    void begin ();
    void end ();
    size_t write(uint8_t c);
    void draw ();
    void setMode (Mode mode);


protected:
    const static int maxGraphs = 4;

    void calcBounds ();
    void parseLine ();

// Plotter variables
    int viewWidth = 0;
    int *vals[maxGraphs] = {0};
    int *prevVals[maxGraphs] = {0};
    uint32_t graphsColors[maxGraphs] = {0xFFFF00,0x00FFFF,0xFF00FF,0x00FF00};
    int graphs =0,
        points =0,
        totalPoints = 0;
    int lowerBound=-1*fixPointMul, higherBound=1*fixPointMul;
    Mode mode = Plotter;
    font_t labelsFont = nullptr;

    char lineBuf[128];
    int linePos = 0;
};
