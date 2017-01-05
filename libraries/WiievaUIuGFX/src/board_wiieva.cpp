#include <Arduino.h>
#include <SPI.h>

#include "aioiface.h"
#include "WiievaWiring.h"

extern "C" {
#include "user_interface.h"
}

#define SPILOWSPEED 1000000
#define SPIHIGHSPEED 32000000

static SPISettings spiSettings(SPILOWSPEED, MSBFIRST, SPI_MODE0);

static inline void cmdmode() {
  digitalWrite(WIIEVA_LCD_RS, 0);
}

static inline void datamode() {
  digitalWrite(WIIEVA_LCD_RS, 1);
}

extern "C" void wiieva_lcd_init_board(void) {
  Serial.println("init board");
  pinMode(WIIEVA_LCD_CS, OUTPUT);
  digitalWrite(WIIEVA_LCD_CS, 1);
  pinMode(WIIEVA_LCD_RS, OUTPUT);
  datamode();
}

extern "C" void wiieva_lcd_post_init_board(void) {
  spiSettings = SPISettings(SPIHIGHSPEED, MSBFIRST, SPI_MODE0);
}

extern "C" void wiieva_lcd_setpin_reset(int state) {
}

static int aquire_count = 0;

extern "C" void wiieva_lcd_aquirebus(void) {
    if (!aquire_count++) {
        SPI.beginTransaction(spiSettings);
        digitalWrite(WIIEVA_LCD_CS, 0);
    }
}

extern "C" void wiieva_lcd_releasebus(void) {
    if (aquire_count && !--aquire_count) {
        digitalWrite(WIIEVA_LCD_CS, 1);
        SPI.endTransaction();
    }
}

extern "C" void wiieva_lcd_write_cmd(uint8_t cmd) {
  cmdmode();
  SPI.write(cmd);
  datamode();
}

extern "C" void wiieva_lcd_write_data(uint16_t data) {
  SPI.write16(data,1);
}

extern "C" void wiieva_lcd_write_data_byte (uint8_t data) {
  SPI.write(data);
}

extern "C" void wiieva_lcd_write_data_repeat (uint16_t data,int cnt) {
    uint32_t _data = data >> 8 | data<<8;
    SPI.writePattern((uint8_t*)&_data,sizeof(uint16_t),cnt);
}

extern "C" void wiieva_lcd_write_data_buf (uint16_t *data,int cnt) {
    SPI.writeBytes  ((uint8_t*)data,cnt*sizeof(*data));
}

extern "C" void wiieva_lcd_set_backlight (uint16_t percent) {
	pinMode (WIIEVA_LCD_BK,OUTPUT);
	analogWrite (WIIEVA_LCD_BK,percent*255/100);
}

extern "C" void wiieva_lcd_read_input (AIO_InputState *input_state) {
    wiieva::recvBuffer (AIO_CMD_GET_INPUT_STATE,sizeof (*input_state),(uint8_t*)input_state);
    system_soft_wdt_feed ();
}
