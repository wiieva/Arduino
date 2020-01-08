
#include "wiring_private.h"
#include "interrupts.h"
#include "aioiface.h"
#include <SPI.h>
#include <WiievaWiring.h>

extern "C" {
#include "user_interface.h"
}

extern "C" void __pinMode(uint8_t pin, uint8_t mode);
extern "C" void __digitalWrite(uint8_t pin, uint8_t val);
extern "C" int  __digitalRead(uint8_t pin);
extern "C" void __analogWrite(uint8_t pin, int val);
extern "C" void __initPins();
extern "C" int  __analogRead(uint8_t pin);

namespace wiieva
{
    bool initDone = false;
    SPISettings spiSettings (16000000,MSBFIRST,SPI_MODE0);

    inline uint32_t REV(uint32_t w) {
        return (((w)>>8)&0xFF00FF)|(((w)<<8)&0xFF00FF00);
    }
    inline void setDataBits(uint16_t bits) {
        const uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
        bits--;
        SPI1U1 = ((SPI1U1 & mask) | ((bits << SPILMOSI) | (bits << SPILMISO)));
    }

    // SPI routines with reversed words
    void transferBytes_(uint8_t * out, uint8_t * in, uint8_t size) {
        while(SPI1CMD & SPIBUSY) {}
        // Set in/out Bits to transfer

        setDataBits(size * 8);

        volatile uint32_t * fifoPtr = &SPI1W0;
        uint8_t dataSize = ((size + 3) / 4);

        if(out) {
            uint32_t * dataPtr = (uint32_t*) out;
            while(dataSize--)
                *fifoPtr++ = REV(*dataPtr++);
        } else {
            // no out data only read fill with dummy data!
            while(dataSize--) {
                *fifoPtr++ = 0;
            }
        }

        SPI1CMD |= SPIBUSY;
        while(SPI1CMD & SPIBUSY) {}

        if(in) {
            volatile uint8_t * fifoPtr8 = (volatile uint8_t *) &SPI1W0;
            dataSize = size;
            while(dataSize--) {
                *in = *(volatile uint8_t *)((uint32_t)fifoPtr8^1);
                in++;
                fifoPtr8++;
            }
        }
    }

    void transferBytes(uint8_t * out, uint8_t * in, uint32_t size) {
        while(size) {
            if(size > 64) {
                transferBytes_(out, in, 64);
                size -= 64;
                if(out) out += 64;
                if(in) in += 64;
            } else {
                transferBytes_(out, in, size);
                size = 0;
            }
        }
    }

    bool inline waitSTM (int state,int cnt=1000) {
        while (((*portInputRegister (0) & digitalPinToBitMask(WIIEVA_STM_RDY))?HIGH:LOW) != state && cnt--){};
        return bool (cnt != 0);
    }

    void inline sendWordSTM (uint32_t word) {
        // Wait for STM get ready
        waitSTM (HIGH);
        // Send data
        transferBytes ((uint8_t *)&word,0,2);
        // Wait for STM get busy
        waitSTM (LOW,100);
    }

    uint16_t inline recvWordSTM () {
        // Wait for STM get ready
        uint32_t ret = 0;
        waitSTM (HIGH);
        transferBytes (0,(uint8_t *)&ret,2);
        waitSTM (LOW,100);
        return ret;
    }

    uint16_t sendCommandWait (uint16_t cmd, uint16_t arg1,uint16_t arg2,uint16_t *out,uint16_t *in) {
        uint32_t ret = 0;
        esp8266::InterruptLock lock;

        SPI.beginTransaction (spiSettings);
        // CS low
        *portOutputRegister (0) &= ~digitalPinToBitMask(WIIEVA_STM_CS);
        // Write command word
        sendWordSTM (cmd | (arg1<<8) | (arg2 <<5));

        // Read answer
        ret = recvWordSTM  ();

        // Send or recv buffer to STM32
        if (out || in) {
            waitSTM (HIGH);
            transferBytes ((uint8_t*)out,(uint8_t*)in,(ret+1)&~1);
            waitSTM (LOW);
        }

        // CS high
        *portOutputRegister (0) |=  digitalPinToBitMask(WIIEVA_STM_CS);
        return ret;
    }

    void sendCommand (uint16_t cmd, uint16_t arg1,uint16_t arg2) {
        esp8266::InterruptLock lock;
        SPI.beginTransaction (spiSettings);
        // CS low
        *portOutputRegister (0) &= ~digitalPinToBitMask(WIIEVA_STM_CS);
        // Write command word
        sendWordSTM (cmd | (arg1<<8) | (arg2 <<5));

        // CS high
        *portOutputRegister (0) |=  digitalPinToBitMask(WIIEVA_STM_CS);
    }

    uint16_t sendBuffer (uint16_t cmd, uint8_t size, uint8_t *buf) {
        return sendCommandWait (cmd, size,0,(uint16_t*)buf,0);
    }

    uint16_t recvBuffer (uint16_t cmd, uint8_t size, uint8_t *buf) {
        return sendCommandWait (cmd, size,0,0,(uint16_t *)buf);
    }
    void pinMode(int stmPin, int mode) {
        sendCommand (AIO_CMD_PIN_MODE,stmPin,mode);
    }

    void digitalWrite(int stmPin, int value) {
        sendCommand (AIO_CMD_DIGITAL_WRITE,stmPin,value);
    }

    int digitalRead(int stmPin) {
        return sendCommandWait (AIO_CMD_DIGITAL_READ,stmPin);
    }

    void analogWrite(int stmPin, int value) {
        uint16_t val = value;
        sendCommandWait (AIO_CMD_ANALOG_WRITE, stmPin,0,&val);
    }

    int analogRead(int stmPin)
    {
       if (stmPin < A0)
            stmPin += A0; 
        return sendCommandWait (AIO_CMD_ANALOG_READ,stmPin);
    }
} // namespace wiieva

extern "C" {

void pinMode(uint8_t pin, uint8_t mode)
{
    if (pin >= ESP_PINS_OFFSET || !wiieva::initDone)
        __pinMode(pin - ESP_PINS_OFFSET, mode);
    else
        wiieva::pinMode(pin, mode);
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    if (pin >= ESP_PINS_OFFSET || !wiieva::initDone)
        __digitalWrite(pin - ESP_PINS_OFFSET, value);
    else
        wiieva::digitalWrite(pin, value);
}

int digitalRead(uint8_t pin)
{
    if (pin >= ESP_PINS_OFFSET || !wiieva::initDone)
        return __digitalRead(pin - ESP_PINS_OFFSET);
    else
        return wiieva::digitalRead(pin);
}

int analogRead(uint8_t pin)
{
    if (pin >= ESP_PINS_OFFSET || !wiieva::initDone)
        return __analogRead(pin - ESP_PINS_OFFSET);
    else
        return wiieva::analogRead(pin);
}

void analogWrite(uint8_t pin, int value)
{
    if (pin >= ESP_PINS_OFFSET || !wiieva::initDone)
        __analogWrite(pin - ESP_PINS_OFFSET, value);
    else
        wiieva::analogWrite(pin, value);
}


void initVariant()
{
   SPI.begin ();

   pinMode (WIIEVA_STM_RDY,INPUT);
   pinMode (WIIEVA_STM_CS, OUTPUT);
   digitalWrite (WIIEVA_STM_CS, 1);
   pinMode (WIIEVA_LCD_CS, OUTPUT);
   digitalWrite (WIIEVA_LCD_CS, 1);
   pinMode (WIIEVA_SD_CS, OUTPUT);
   digitalWrite (WIIEVA_SD_CS, 1);

   pinMode (WIIEVA_KB2,INPUT);
   wiieva::initDone = true;

   pinMode (WIIEVA_KB_PWR,INPUT);
   pinMode (WIIEVA_CHRG,INPUT);

   wiieva::sendCommand (AIO_CMD_AUDIO_IN_MODE,0,AIO_AUDIO_OFF);
   wiieva::sendCommand (AIO_CMD_AUDIO_OUT_MODE,0,AIO_AUDIO_OFF);

}

}
