#pragma once

#include "aioiface.h"

namespace wiieva
{
    // Send command, without answer
    void sendCommand (uint16_t cmd, uint16_t arg1 = 0,uint16_t arg2 = 0);
    // Send command, wait answer. return: answer from STM32
    uint16_t sendCommandWait (uint16_t cmd, uint16_t arg1 = 0,uint16_t arg2 = 0,uint16_t *out=0,uint16_t *in=0);
    // Send command and buffer, wait answer. return: sent buffer size in bytes from STM32
    uint16_t sendBuffer (uint16_t cmd, uint8_t size, uint8_t *buf);
    // Send command, receive buffer, wait answer. return: recv buffer size in bytes from STM32
    uint16_t recvBuffer (uint16_t cmd, uint8_t size, uint8_t *buf);
} 
