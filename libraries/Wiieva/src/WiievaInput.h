#pragma once

#define PORTRAIT                        0
#define LANDSCAPE                       1

#define PREC_LOW                        1
#define PREC_MEDIUM                     2
#define PREC_HI                         3
#define PREC_EXTREME            4

#include "aioiface.h"

class WiievaInput
{
public:
    WiievaInput ();

    void    InitTouch(byte orientation = LANDSCAPE);
    void    read();
    bool    dataAvailable();
    int16_t getX();
    int16_t getY();
    void    setPrecision(byte precision);
    void    calibrateRead();
    int16_t getKeyCode ();

private:
    long    _default_orientation;
    byte    orient;
    byte    prec;
    byte    display_model;
    long    disp_x_size, disp_y_size, default_orientation;
    long    touch_x_left, touch_x_right, touch_y_top, touch_y_bottom;

    AIO_InputState input_state;
};
