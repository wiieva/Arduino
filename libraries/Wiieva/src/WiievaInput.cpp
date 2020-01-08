#include "Arduino.h"
#include "WiievaWiring.h"
#include "WiievaInput.h"

using namespace wiieva;

WiievaInput::WiievaInput ()
{
    touch_x_left = 136*4;
    touch_x_right = 912*4;
    touch_y_top = 104*4;
    touch_y_bottom = 888*4;
}

void WiievaInput::InitTouch(byte orientation)
{
    switch (orientation) {
    case PORTRAIT:
        disp_x_size = 178;
        disp_y_size = 220;
        break;
    case LANDSCAPE:
        disp_y_size = 178;
        disp_x_size = 220;
        break;
    }
    orient = orientation;
}

void WiievaInput::read()
{


}

bool WiievaInput::dataAvailable()
{
    wiieva::recvBuffer (AIO_CMD_GET_INPUT_STATE,sizeof (input_state),(uint8_t*)&input_state);
    return (input_state.z > 500);
}
int16_t WiievaInput::getKeyCode ()
{
    for (int i = 0; i < 16; ++i)
        if (input_state.keys & (1<<i))
            return i;
    return -1;
}


int16_t WiievaInput::getX()
{
    int x = 0;
    if (orient == PORTRAIT)
        x = map(input_state.x, touch_x_left, touch_x_right, 0, disp_x_size);
    else if (orient == LANDSCAPE)
        x = map(input_state.y, touch_y_top, touch_y_bottom, disp_x_size,0);
    if (x < 0)
        x = 0;
    if (x >= disp_x_size)
        x = disp_x_size - 1;
    return x;
}

int16_t WiievaInput::getY()
{
    int y = 0;
    if (orient == PORTRAIT)
        y = map(input_state.y, touch_y_top, touch_y_bottom, 0, disp_y_size);
    else if (orient == LANDSCAPE)
        y = map(input_state.x, touch_x_left, touch_x_right, 0, disp_y_size);
    if (y < 0)
        y = 0;
    if (y >= disp_y_size)
        y = disp_y_size - 1;
    return y;
}

void WiievaInput::setPrecision(byte precision)
{
}

void WiievaInput::calibrateRead()
{
}



