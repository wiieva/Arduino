#pragma once

class WiievaIRSend {
public:
    void sendSony(uint32_t data, int nbits);
    void sendNEC(uint32_t data, int nbits);
    void sendWhynter(uint32_t data, int nbits);
    void sendRC5(uint32_t data, int nbits);
    void sendRC6(uint32_t data, int nbits);
    void sendPanasonic(uint16_t addr, uint32_t data);
    void sendJVC(uint32_t data, int nbits, int repeat);
    void sendSAMSUNG(uint32_t data, int nbits);
protected:
    void send (int mode,uint32_t data,int nbits,int repeat=0,uint16_t addr=0);

};
