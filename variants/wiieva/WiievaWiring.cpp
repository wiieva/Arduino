
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
    const uint16_t WIIEVA_STM32_DELAY_DATA_US = 2;
    const uint16_t cmd_wait_times_us []={1,5,5,3,3,2,2,20,20,30,30,3,5,4,4,
                                         0,0,4,4,0,0,3,0,0,0,0,0,0,0,0};
    uint16_t pinModes [ESP_PINS_OFFSET] = {0};
    uint32_t stmWaitTillMicros = 0;
    SPISettings spiSettings (8000000,MSBFIRST,SPI_MODE0);

    void inline waitSTM () {
        uint32_t d = stmWaitTillMicros - system_get_time ();
        if (d < 40)
            delayMicroseconds (d+1);
    }

    void inline scheduleWaitSTM (uint32_t micros) {
        stmWaitTillMicros = system_get_time() + micros;
    }

    void inline sendWordSTM (uint16_t word,uint16_t micros) {
        waitSTM ();
        SPI.write16 (word,1);
        scheduleWaitSTM (micros);
    }

    uint16_t inline recvWordSTM (uint16_t micros) {
        uint16_t ret = 0;
        waitSTM ();
        SPI.transferBytes (0,(uint8_t *)&ret,2);
        scheduleWaitSTM (micros);
        ret = (ret >> 8 | ret << 8);
        return ret;
    }

    uint16_t sendCommand (uint16_t cmd,int flags, uint16_t arg1,uint16_t arg2,uint16_t arg3,uint16_t *buf) {
        uint32_t ret = 0;
        // Additional delay for commands, what can return answer
        if (flags & (SEND_CMD_FL_ANSWER|SEND_CMD_FL_RX_BUF|SEND_CMD_FL_TX_BUF)) {
            stmWaitTillMicros += 2;
        }
        waitSTM ();
        InterruptLock lock;

        SPI.beginTransaction (spiSettings);

        // CS low
        *portOutputRegister (0) &= ~digitalPinToBitMask(WIIEVA_STM_CS);

        // Write command word
        sendWordSTM (cmd | (arg1<<8) | (arg2 <<5),cmd_wait_times_us[cmd]);

        // Check and send second word (arg3)
        if (flags & SEND_CMD_FL_ARG3) {
            sendWordSTM (arg3,5);
        }

        // Recv answer to command
        // For recv buffer commands it is count of available words in STM32 buffer to read
        // For send buffer commands it is count of free words it STM32 buffer to write
        if (flags & (SEND_CMD_FL_ANSWER|SEND_CMD_FL_RX_BUF|SEND_CMD_FL_TX_BUF)) {
            ret = recvWordSTM  (WIIEVA_STM32_DELAY_DATA_US);
        }

        // Send buffer to STM32
        if (flags & SEND_CMD_FL_TX_BUF) {
            ret = min(ret,arg1);
            for (int i = 0; i < (ret+1)/2 ; ++i) {
                sendWordSTM (buf[i],WIIEVA_STM32_DELAY_DATA_US);
            }
        }

        // Recv buffer from STM32
        if (flags & SEND_CMD_FL_RX_BUF) {
            ret = min(ret,arg1);
            waitSTM ();
            for (int i = 0; i < (ret+1)/2; ++i)  {
                // Here we do not need delay
                buf[i] = recvWordSTM (0);
            }
            scheduleWaitSTM (WIIEVA_STM32_DELAY_DATA_US+2);
        }

        // CS high
        *portOutputRegister (0) |=  digitalPinToBitMask(WIIEVA_STM_CS);

        return ret;
    }

    uint16_t sendBuffer (uint16_t cmd, uint8_t size, uint8_t *buf)
    {
        return sendCommand (cmd, SEND_CMD_FL_TX_BUF, size,0,0,(uint16_t*)buf);
    }
    uint16_t recvBuffer (uint16_t cmd, uint8_t size, uint8_t *buf)
    {
        return sendCommand (cmd, SEND_CMD_FL_RX_BUF, size,0,0,(uint16_t *)buf);
    }

    void pinMode(int stmPin, int mode)
    {
        pinModes[stmPin] =  mode?AIO_PIN_DIGITAL_OUT:AIO_PIN_DIGITAL_IN;
        sendCommand (AIO_CMD_PIN_MODE,0,stmPin,pinModes[stmPin]);
    }

    void digitalWrite(int stmPin, int value)
    {
        if (pinModes[stmPin] == AIO_PIN_ANALOG_OUT) {
            sendCommand (AIO_CMD_PIN_MODE,0,stmPin,AIO_PIN_DIGITAL_OUT);
            pinModes[stmPin] = AIO_PIN_DIGITAL_OUT;
        }
        sendCommand (AIO_CMD_DIGITAL_WRITE,0,stmPin,value);
    }

    int digitalRead(int stmPin)
    {
        return sendCommand (AIO_CMD_DIGITAL_READ,SEND_CMD_FL_ANSWER,stmPin);
    }

    void analogWrite(int stmPin, int value)
    {
        if (pinModes[stmPin] == AIO_PIN_DIGITAL_OUT) {
            sendCommand (AIO_CMD_PIN_MODE,0,stmPin,AIO_PIN_ANALOG_OUT);
            pinModes[stmPin] = AIO_PIN_ANALOG_OUT;
        }
        sendCommand (AIO_CMD_ANALOG_WRITE,SEND_CMD_FL_ARG3, stmPin,0,value);
    }

    int analogRead(int stmPin)
    {
       if (stmPin < A0)
            stmPin += A0;
        return sendCommand (AIO_CMD_ANALOG_READ,SEND_CMD_FL_ANSWER,stmPin);
    }
} // namespace wiieva

extern "C" {

void pinMode(uint8_t pin, uint8_t mode)
{
    if (pin >= ESP_PINS_OFFSET)
        __pinMode(pin - ESP_PINS_OFFSET, mode);
    else
        wiieva::pinMode(pin, mode);
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    if (pin >= ESP_PINS_OFFSET)
        __digitalWrite(pin - ESP_PINS_OFFSET, value);
    else
        wiieva::digitalWrite(pin, value);
}

int digitalRead(uint8_t pin)
{
    if (pin >= ESP_PINS_OFFSET)
        return __digitalRead(pin - ESP_PINS_OFFSET);
    else
        return wiieva::digitalRead(pin);
}

int analogRead(uint8_t pin)
{
    if (pin >= ESP_PINS_OFFSET)
        return __analogRead(pin - ESP_PINS_OFFSET);
    else
        return wiieva::analogRead(pin);
}

void analogWrite(uint8_t pin, int value)
{
    if (pin >= ESP_PINS_OFFSET)
        __analogWrite(pin - ESP_PINS_OFFSET, value);
    else
        wiieva::analogWrite(pin, value);
}


void initVariant()
{
   wiieva::stmWaitTillMicros = system_get_time();

   SPI.begin ();

   pinMode (WIIEVA_STM_CS, OUTPUT);
   digitalWrite (WIIEVA_STM_CS, 1);
   pinMode (WIIEVA_LCD_CS, OUTPUT);
   digitalWrite (WIIEVA_LCD_CS, 1);
   pinMode (WIIEVA_SD_CS, OUTPUT);
   digitalWrite (WIIEVA_SD_CS, 1);

   pinMode (WIIEVA_KB2,INPUT);
   pinMode (WIIEVA_KB_PWR,INPUT);
   pinMode (WIIEVA_CHRG,INPUT);

   wiieva::sendCommand (AIO_CMD_AUDIO_IN_MODE,0,AIO_AUDIO_OFF);
   wiieva::sendCommand (AIO_CMD_AUDIO_OUT_MODE,0,AIO_AUDIO_OFF);

}

}
