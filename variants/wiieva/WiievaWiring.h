#pragma once

#include "aioiface.h"

namespace wiieva
{
    const uint16_t SEND_CMD_FL_ARG3 = 0x1;
    const uint16_t SEND_CMD_FL_ANSWER = 0x2;
    const uint16_t SEND_CMD_FL_TX_BUF = 0x4;
    const uint16_t SEND_CMD_FL_RX_BUF = 0x8;
    uint16_t sendCommand (uint16_t cmd,int flags, uint16_t arg1 = 0,uint16_t arg2 = 0,uint16_t arg3 = 0,uint16_t *buf=0);
    uint16_t sendBuffer (uint16_t cmd, uint8_t size, uint8_t *buf);
    uint16_t recvBuffer (uint16_t cmd, uint8_t size, uint8_t *buf);
} 

