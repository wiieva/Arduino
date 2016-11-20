/* Arduino SdFat Library
 * Copyright (C) 2016 by William Greiman
 *
 * This file is part of the Arduino SdFat Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SdFat Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef SdFat_h
#define SdFat_h
/**
 * \file
 * \brief SdFat class
 */
#include "SysCall.h"
#include "BlockDriver.h"
#ifdef ARDUINO
#include "FatLib/FatLib.h"
#else  // ARDUINO
#include "FatLib.h"
#endif  // ARDUINO
//------------------------------------------------------------------------------
/** SdFat version YYYYMMDD */
#define SD_FAT_VERSION 20160801
//==============================================================================
/**
 * \class SdBaseFile
 * \brief Class for backward compatibility.
 */
class SdBaseFile : public FatFile {
 public:
  SdBaseFile() {}
  /**  Create a file object and open it in the current working directory.
   *
   * \param[in] path A path for a file to be opened.
   *
   * \param[in] oflag Values for \a oflag are constructed by a
   * bitwise-inclusive OR of open flags. see
   * FatFile::open(FatFile*, const char*, uint8_t).
   */
  SdBaseFile(const char* path, uint8_t oflag) : FatFile(path, oflag) {}
};
//-----------------------------------------------------------------------------
#if ENABLE_ARDUINO_FEATURES
/**
 * \class SdFile
 * \brief Class for backward compatibility.
 */
class SdFile : public PrintFile {
 public:
  SdFile() {}
  /**  Create a file object and open it in the current working directory.
   *
   * \param[in] path A path for a file to be opened.
   *
   * \param[in] oflag Values for \a oflag are constructed by a
   * bitwise-inclusive OR of open flags. see
   * FatFile::open(FatFile*, const char*, uint8_t).
   */
  SdFile(const char* path, uint8_t oflag) : PrintFile(path, oflag) {}
};
#endif  // #if ENABLE_ARDUINO_FEATURES
//-----------------------------------------------------------------------------
/**
 * \class SdFileSystem
 * \brief Virtual base class for %SdFat library.
 */
template<class SdDriverClass>
class SdFileSystem : public FatFileSystem {
 public:
  /** Initialize SD card and file system.
   * \param[in] spi SPI object for the card.
   * \param[in] csPin SD card chip select pin.
   * \param[in] spiSettings SPI speed, mode, and bit order.
   * \return true for success else false.
   */
  bool begin(SdSpiDriver* spi, uint8_t csPin, SPISettings spiSettings) {
    return m_card.begin(spi, csPin, spiSettings) &&
           FatFileSystem::begin(&m_card);
  }
  /** \return Pointer to SD card object */
  SdDriverClass *card() {
    m_card.syncBlocks();
    return &m_card;
  }
  /** Initialize SD card for diagnostic use.
   * \param[in] spi SPI object for the card.
   * \param[in] csPin SD card chip select pin.
   * \param[in] spiSettings SPI speed, mode, and bit order.
   * \return true for success else false.
   */  
  bool cardBegin(SdSpiDriver* spi, uint8_t csPin, SPISettings spiSettings) {
    return m_card.begin(spi, csPin, spiSettings);
  }
  /** %Print any SD error code to Serial and halt. */
  void errorHalt() {
    errorHalt(&Serial);
  }
  /** %Print any SD error code and halt.
   *
   * \param[in] pr Print destination.
   */
  void errorHalt(Print* pr) {
    errorPrint(pr);
    SysCall::halt();
  }
  /** %Print msg, any SD error code and halt.
   *
   * \param[in] msg Message to print.
   */
  void errorHalt(char const* msg) {
    errorHalt(&Serial, msg);
  }
  /** %Print msg, any SD error code, and halt.
   *
   * \param[in] pr Print destination.
   * \param[in] msg Message to print.
   */
  void errorHalt(Print* pr, char const* msg) {
    errorPrint(pr, msg);
    SysCall::halt();
  }
  /** %Print any SD error code to Serial */
  void errorPrint() {
    errorPrint(&Serial);
  }
  /** %Print any SD error code.
   * \param[in] pr Print device.
   */
  void errorPrint(Print* pr) {
    if (!cardErrorCode()) {
      return;
    }
    pr->print(F("SD errorCode: 0X"));
    pr->print(cardErrorCode(), HEX);
    pr->print(F(",0X"));
    pr->println(cardErrorData(), HEX);
  }
  /** %Print msg, any SD error code.
   *
   * \param[in] msg Message to print.
   */
  void errorPrint(const char* msg) {
    errorPrint(&Serial, msg);
  }
  /** %Print msg, any SD error code.
   *
   * \param[in] pr Print destination.
   * \param[in] msg Message to print.
   */
  void errorPrint(Print* pr, char const* msg) {
    pr->print(F("error: "));
    pr->println(msg);
    errorPrint(pr);
  }
  /** %Print any SD error code and halt. */
  void initErrorHalt() {
    initErrorHalt(&Serial);
  }
  /** %Print error details and halt after begin fails.
   *
   * \param[in] pr Print destination.
   */
  void initErrorHalt(Print* pr) {
    initErrorPrint(pr);
    SysCall::halt();
  }
  /**Print message, error details, and halt after begin() fails.
   *
   * \param[in] msg Message to print.
   */
  void initErrorHalt(char const *msg) {
    initErrorHalt(&Serial, msg);
  }
  /**Print message, error details, and halt after begin() fails.
   * \param[in] pr Print device.
   * \param[in] msg Message to print.
   */
  void initErrorHalt(Print* pr, char const *msg) {
    pr->println(msg);
    initErrorHalt(pr);
  }

  /** Print error details after begin() fails. */
  void initErrorPrint() {
    initErrorPrint(&Serial);
  }
  /** Print error details after begin() fails.
   *
   * \param[in] pr Print destination.
   */
  void initErrorPrint(Print* pr) {
    if (cardErrorCode()) {
      pr->println(F("Can't access SD card. Do not reformat."));
      if (cardErrorCode() == SD_CARD_ERROR_CMD0) {
        pr->println(F("No card, wrong chip select pin, or SPI problem?"));
      }
      errorPrint(pr);
    } else if (vol()->fatType() == 0) {
      pr->println(F("Invalid format, reformat SD."));
    } else if (!vwd()->isOpen()) {
      pr->println(F("Can't open root directory."));
    } else {
      pr->println(F("No error found."));
    }
  }
  /**Print message and error details and halt after begin() fails.
   *
   * \param[in] msg Message to print.
   */
  void initErrorPrint(char const *msg) {
    initErrorPrint(&Serial, msg);
  }
  /**Print message and error details and halt after begin() fails.
   *
   * \param[in] pr Print destination.
   * \param[in] msg Message to print.
   */
  void initErrorPrint(Print* pr, char const *msg) {
    pr->println(msg);
    initErrorPrint(pr);
  }
#if defined(ARDUINO) || defined(DOXYGEN)
  /** %Print msg, any SD error code, and halt.
   *
   * \param[in] msg Message to print.
   */
  void errorHalt(const __FlashStringHelper* msg) {
    errorHalt(&Serial, msg);
  }
  /** %Print msg, any SD error code, and halt.
   *
   * \param[in] pr Print destination.
   * \param[in] msg Message to print.
   */
  void errorHalt(Print* pr, const __FlashStringHelper* msg) {
    errorPrint(pr, msg);
    SysCall::halt();
  }

  /** %Print msg, any SD error code.
   *
   * \param[in] msg Message to print.
   */
  void errorPrint(const __FlashStringHelper* msg) {
    errorPrint(&Serial, msg);
  }
  /** %Print msg, any SD error code.
   *
   * \param[in] pr Print destination.
   * \param[in] msg Message to print.
   */
  void errorPrint(Print* pr, const __FlashStringHelper* msg) {
    pr->print(F("error: "));
    pr->println(msg);
    errorPrint(pr);
  }
  /**Print message, error details, and halt after begin() fails.
    *
    * \param[in] msg Message to print.
    */
  void initErrorHalt(const __FlashStringHelper* msg) {
    initErrorHalt(&Serial, msg);
  }
  /**Print message, error details, and halt after begin() fails.
   * \param[in] pr Print device for message.
   * \param[in] msg Message to print.
   */
  void initErrorHalt(Print* pr, const __FlashStringHelper* msg) {
    pr->println(msg);
    initErrorHalt(pr);
  }
  /**Print message and error details and halt after begin() fails.
   *
   * \param[in] msg Message to print.
   */
  void initErrorPrint(const __FlashStringHelper* msg) {
    initErrorPrint(&Serial, msg);
  }
  /**Print message and error details and halt after begin() fails.
   *
   * \param[in] pr Print destination.
   * \param[in] msg Message to print.
   */
  void initErrorPrint(Print* pr, const __FlashStringHelper* msg) {
    pr->println(msg);
    initErrorPrint(pr);
  }
#endif  // defined(ARDUINO) || defined(DOXYGEN)
  /** \return The card error code */
  uint8_t cardErrorCode() {
        return m_card.errorCode();
  }
  /** \return the card error data */
  uint8_t cardErrorData() {
    return m_card.errorData();
  }

 private:
  SdDriverClass m_card;
};
//==============================================================================
/**
 * \class SdFat
 * \brief Main file system class for %SdFat library.
 */
class SdFat : public SdFileSystem<SdBlockDriver> {
 public:
#if IMPLEMENT_SPI_PORT_SELECTION || defined(DOXYGEN)
  SdFat() {
    m_spi.setPort(0);
  }
  /** Constructor with SPI port selection.
   * \param[in] spiPort SPI port number.
   */
  explicit SdFat(uint8_t spiPort) {
    m_spi.setPort(spiPort);
  }
#endif  // IMPLEMENT_SPI_PORT_SELECTION
  /** Initialize SD card and file system.
   *
   * \param[in] csPin SD card chip select pin.
   * \param[in] spiSettings SPI speed, mode, and bit order.
   * \return true for success else false.
   */ 
  bool begin(uint8_t csPin = SS, SPISettings spiSettings = SPI_FULL_SPEED) {
    return SdFileSystem::begin(&m_spi, csPin, spiSettings);
  }
  /** Initialize SD card for diagnostic use only.
   *
   * \param[in] csPin SD card chip select pin.
   * \param[in] settings SPI speed, mode, and bit order.
   * \return true for success else false.
   */   
  bool cardBegin(uint8_t csPin = SS, SPISettings settings = SPI_FULL_SPEED) {
    return SdFileSystem::cardBegin(&m_spi, csPin, settings);
  }
  /** Initialize file system for diagnostic use only.
   * \return true for success else false.
   */   
  bool fsBegin() {
    return FatFileSystem::begin(card());
  }

 private:
  SdFatSpiDriver m_spi;
};
//==============================================================================
#if ENABLE_SOFTWARE_SPI_CLASS || defined(DOXYGEN)
/**
 * \class SdFatSoftSpi
 * \brief SdFat class using software SPI.
 */
template<uint8_t MisoPin, uint8_t MosiPin, uint8_t SckPin>
class SdFatSoftSpi : public SdFileSystem<SdBlockDriver>  {
 public:
  /** Initialize SD card and file system.
   *
   * \param[in] csPin SD card chip select pin.
   * \param[in] spiSettings ignored for software SPI..
   * \return true for success else false.
   */
  bool begin(uint8_t csPin = SS, SPISettings spiSettings = SPI_FULL_SPEED) {
    return SdFileSystem::begin(&m_spi, csPin, spiSettings);
  }
 private:
  SdSpiSoftDriver<MisoPin, MosiPin, SckPin> m_spi;
};
#endif  // #if ENABLE_SOFTWARE_SPI_CLASS || defined(DOXYGEN)

#if ENABLE_EXTENDED_TRANSFER_CLASS || defined(DOXYGEN)
//==============================================================================
/**
 * \class SdFatEX
 * \brief SdFat class with extended SD I/O.
 */
class SdFatEX : public SdFileSystem<SdBlockDriverEX> {
 public:
#if IMPLEMENT_SPI_PORT_SELECTION  || defined(DOXYGEN)
  SdFatEX() {
    m_spi.setPort(0);
  }
  /** Constructor with SPI port selection.
   * \param[in] spiPort SPI port number.
   */  
  explicit SdFatEX(uint8_t spiPort) {
    m_spi.setPort(spiPort);
  }
#endif  // IMPLEMENT_SPI_PORT_SELECTION
  /** Initialize SD card and file system.
  *
  * \param[in] csPin SD card chip select pin.
  * \param[in] spiSettings SPI speed, mode, and bit order.
  * \return true for success else false.
  */ 
  bool begin(uint8_t csPin = SS, SPISettings spiSettings = SPI_FULL_SPEED) {
    return SdFileSystem::begin(&m_spi, csPin, spiSettings);
  }

 private:
  SdFatSpiDriver m_spi;
};
//==============================================================================
#if ENABLE_SOFTWARE_SPI_CLASS || defined(DOXYGEN)
/**
 * \class SdFatSoftSpiEX
 * \brief SdFat class using software SPI and extended SD I/O.
 */
template<uint8_t MisoPin, uint8_t MosiPin, uint8_t SckPin>
class SdFatSoftSpiEX : public SdFileSystem<SdBlockDriverEX>  {
 public:
  /** Initialize SD card and file system.
   *
   * \param[in] csPin SD card chip select pin.
   * \param[in] spiSettings ignored for software SPI.
   * \return true for success else false.
   */
  bool begin(uint8_t csPin = SS, SPISettings spiSettings = 2) {
    return SdFileSystem::begin(&m_spi, csPin, spiSettings);
  }
 private:
  SdSpiSoftDriver<MisoPin, MosiPin, SckPin> m_spi;
};
#endif  // #if ENABLE_SOFTWARE_SPI_CLASS || defined(DOXYGEN)
#endif  // ENABLE_EXTENDED_TRANSFER_CLASS || defined(DOXYGEN)
//=============================================================================
/**
 * \class Sd2Card
 * \brief Raw access to SD and SDHC card using default SPI library.
 */
class Sd2Card : public SdSpiCard {
 public:
  /** Initialize the SD card.
   * \param[in] chipSelectPin SD chip select pin.
   * \param[in] spiSettings SPI speed, mode, and bit order.
   * \return true for success else false.
   */
  bool begin(uint8_t chipSelectPin = SS,
             SPISettings spiSettings = SD_SCK_MHZ(50)) {
    m_spi.begin(chipSelectPin);
    m_spi.setSpiSettings(SD_SCK_HZ(250000));
    if (!SdSpiCard::begin(&m_spi)) {
      return false;
    }
    m_spi.setSpiSettings(spiSettings);
    return true;
  }
 private:
  SdFatSpiDriver m_spi;
};
#endif  // SdFat_h