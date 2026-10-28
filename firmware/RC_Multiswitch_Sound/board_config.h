/*
 * board_config.h  -  Pin- und Board-Konfiguration RC-Multiswitch-Sound v1.00
 *
 * Platine RC-Multiswitch-Sound-ESP32-S3 v0.9 mit ESP32-S3-DevKitC-1 N16R8
 * (oder Clone). Pinbelegung laut README.md (Abschnitt "Belegung") und
 * hardware/RC-Multiswitch-Sound-ESP32-S3.kicad_sch.
 *
 * Arduino-IDE-Einstellungen:
 *   Board:            ESP32S3 Dev Module
 *   USB CDC On Boot:  Enabled  (Debug-Ausgaben ueber USB, UART0 = UART A)
 *   Partition Scheme: Minimal SPIFFS (1.9MB APP with OTA/128KB SPIFFS)
 *                     (OTA-Update per WLAN + Einstellungs-Backup im Flash)
 *   PSRAM:            Disabled (wird nicht gebraucht)
 */
#pragma once
#include <Arduino.h>

#if !defined(CONFIG_IDF_TARGET_ESP32S3)
  #error "Bitte Board 'ESP32S3 Dev Module' waehlen"
#endif

#if !ARDUINO_USB_CDC_ON_BOOT
  #error "Bitte 'USB CDC On Boot: Enabled' einstellen - UART0 wird fuer UART A (CRSF/SBUS) gebraucht"
#endif

#define BOARD_S3
#define BOARD_NAME "S3 Kombi"

// Ausgaenge S0..S7 (Kanal 1..8), alle mit LEDC-Kanal
static constexpr uint8_t OutPin[8] = {4, 5, 6, 7, 15, 16, 1, 2};
static constexpr bool    OutPwm[8] = {true, true, true, true, true, true, true, true};

// UART A (CN1, Eingang vom Empfaenger) = UART0, CRSF oder SBUS
#define UART_A_SERIAL Serial0
static constexpr uint8_t UART_A_RX = 18;
static constexpr uint8_t UART_A_TX = 17;
// UART B (J5) = UART1
#define UART_B_SERIAL Serial1
static constexpr uint8_t UART_B_RX = 40;
static constexpr uint8_t UART_B_TX = 39;
// UART C (J6) = UART2
#define UART_C_SERIAL Serial2
static constexpr uint8_t UART_C_RX = 42;
static constexpr uint8_t UART_C_TX = 41;

// microSD (J9) ueber SPI
static constexpr uint8_t SD_CS   = 10;
static constexpr uint8_t SD_MOSI = 11;
static constexpr uint8_t SD_SCK  = 12;
static constexpr uint8_t SD_MISO = 13;

// I2S zum PCM5102A (J2), mono: der Verstaerker haengt am Kanal L
static constexpr uint8_t I2S_BCLK = 14;
static constexpr uint8_t I2S_LRC  = 21;
static constexpr uint8_t I2S_DOUT = 47;

static constexpr uint8_t BOOT_PIN = 0;

// RGB-LED (WS2812) des DevKitC-1: v1.0 an IO48, v1.1 an IO38. Der Pin wird
// in der Web-UI gewaehlt (NVS "ledpin", wirkt nach einem Neustart).
static constexpr int STATUS_LED_PIN_DEFAULT = 48;
static constexpr int STATUS_LED_PIN_ALT     = 38;
#define STATUS_LED_WS2812 1
static constexpr uint8_t STATUS_LED_BRIGHTNESS = 40;  // von 255, ca. 15 %
