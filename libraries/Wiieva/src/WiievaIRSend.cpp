#include <Arduino.h>
#include <WiievaIRSend.h>
#include <WiievaWiring.h>

using namespace wiieva;


void WiievaIRSend::sendSony(uint32_t data, int nbits)
{
    send (AIO_SEND_IR_SONY,data,nbits);
}
void WiievaIRSend::sendNEC(uint32_t data, int nbits)
{
    send (AIO_SEND_IR_NEC,data,nbits);
}
void WiievaIRSend::sendWhynter(uint32_t data, int nbits)
{
    send (AIO_SEND_IR_WHYNTER,data,nbits);
}
void WiievaIRSend::sendRC5(uint32_t data, int nbits)
{
    send (AIO_SEND_IR_RC5,data,nbits);
}
void WiievaIRSend::sendRC6(uint32_t data, int nbits)
{
    send (AIO_SEND_IR_RC6,data,nbits);
}
void WiievaIRSend::sendPanasonic(uint16_t addr, uint32_t data)
{
    send (AIO_SEND_IR_PANASONIC,data,0,0,addr);
}
void WiievaIRSend::sendJVC(uint32_t data, int nbits, int repeat)
{
    send (AIO_SEND_IR_JVC,data,nbits,repeat);
}
void WiievaIRSend::sendSAMSUNG(uint32_t data, int nbits)
{
    send (AIO_SEND_IR_SAMSUNG,data,nbits);
}

void WiievaIRSend::send (int mode,uint32_t data,int nbits,int repeat,uint16_t addr)
{
    AIO_IR_Payload payload;
    payload.mode = mode;
    payload.n_bits = nbits;
    payload.repeat = repeat;
    payload.data = data;
    payload.addr = addr;
    int ret = sendBuffer (AIO_CMD_SEND_IR_CMD,sizeof (payload),(uint8_t*)&payload);
    Serial.printf ("ret = %d\r\n",ret);
}

