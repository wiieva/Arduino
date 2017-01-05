/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis
  Modified for ESP8266 WIFIO board by Ivan Grokhotkov, 2015.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.h 249 2007-02-03 16:52:51Z mellis $
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#define NUM_DIGITAL_PINS  48
#define NUM_ANALOG_INPUTS 6
#define ESP_PINS_OFFSET 32


// ESP8266 pins
static const uint8_t WIIEVA_SD_CS  = ESP_PINS_OFFSET + 2;
static const uint8_t WIIEVA_LCD_CS = ESP_PINS_OFFSET + 4;
static const uint8_t WIIEVA_STM_CS = ESP_PINS_OFFSET + 5;
static const uint8_t WIIEVA_LCD_RS = ESP_PINS_OFFSET + 15;
static const uint8_t WIIEVA_KB2    = ESP_PINS_OFFSET + 16;

// *******
// This block must be in sync with GPIO table in ardino_wiring.c
// Standart analog inputs
static const uint8_t A0 = 14;
static const uint8_t A1 = 15;
static const uint8_t A2 = 16;
static const uint8_t A3 = 17;
static const uint8_t A4 = 18;
static const uint8_t A5 = 19;

// Wiieva additional analog inputs
static const uint8_t WIIEVA_BATM = 20;
//static const uint8_t WIIEVA_TC_YU = 21;
//static const uint8_t WIIEVA_TC_XL = 22;
//static const uint8_t WIIEVA_TC_YD = 23;
//static const uint8_t WIIEVA_TC_XR = 24;
//static const uint8_t WIIEVA_4DIR  = 24;

// Wiieva additional digital pins
static const uint8_t WIIEVA_LCD_BK = 21;
static const uint8_t WIIEVA_KB_PWR = 22;
static const uint8_t WIIEVA_CHRG   = 23;

static const uint8_t E0 = ESP_PINS_OFFSET + 0;
static const uint8_t E1 = ESP_PINS_OFFSET + 1;
static const uint8_t E2 = ESP_PINS_OFFSET + 2;
static const uint8_t E3 = ESP_PINS_OFFSET + 3;
static const uint8_t E4 = ESP_PINS_OFFSET + 4;
static const uint8_t E5 = ESP_PINS_OFFSET + 5;
static const uint8_t E11 = ESP_PINS_OFFSET + 11;
static const uint8_t E12 = ESP_PINS_OFFSET + 12;
static const uint8_t E13 = ESP_PINS_OFFSET + 13;
static const uint8_t E14 = ESP_PINS_OFFSET + 14;
static const uint8_t E15 = ESP_PINS_OFFSET + 15;
static const uint8_t E16 = ESP_PINS_OFFSET + 16;

static const uint8_t SDA = 4+ESP_PINS_OFFSET;
static const uint8_t SCL = 5+ESP_PINS_OFFSET;
static const uint8_t SS    = ESP_PINS_OFFSET + 15;
static const uint8_t MOSI  = ESP_PINS_OFFSET + 13;
static const uint8_t MISO  = ESP_PINS_OFFSET + 12;
static const uint8_t SCK   = ESP_PINS_OFFSET + 14;

static const uint8_t LED_BUILTIN_LED = 2;
static const uint8_t BUILTIN_LED = 2;

#define SERIAL_PORT_MONITOR        Serial
#define SERIAL_PORT_USBVIRTUAL     Serial
#define SERIAL_PORT_HARDWARE       Serial
#define SERIAL_PORT_HARDWARE_OPEN  Serial


#undef digitalPinToBitMask
#define digitalPinToBitMask(pin)    (1UL << ((pin)-ESP_PINS_OFFSET))


#endif /* Pins_Arduino_h */
