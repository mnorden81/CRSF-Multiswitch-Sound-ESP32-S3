/*
 * sound.h  -  Soundteil des RC-Multiswitch-Sound (ESP32-S3)
 *
 * 1:1 aus ESP32-Sound-Basis_Ziege-One v7.21 uebernommen (ESP32-RC-Sound.ino,
 * config.h/.cpp, WebServerManager.cpp), ohne die Teile, die hier der
 * Schaltteil macht oder die es auf dieser Platine nicht gibt:
 *   - RC-System, MKan-Gruppen, WLAN, Geraetename, UART B/C, OTA und das
 *     Flash-Backup gehoeren dem Schaltteil (RC_Multiswitch_Sound.ino) und
 *     gelten fuer beide Teile gemeinsam.
 *   - Hardware V1/V2 (PWM- und Pin-Eingaenge), PWM-Skalierung, Port C als
 *     SoftwareSerial und der WifiPin (GPIO13) entfallen.
 * Eigene Einstellungen des Soundteils: Motor, Sound 1-24 (Quelle, Lautstaerke,
 * Modus, Name), Sound-Adresse (CRSF), Einkanal Kanal/Mode (SBUS) und die
 * S.Port Poll-IDs. Gespeichert im NVS-Namensraum "snd".
 */
#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include "src/RC_Common/lua_menu.h"

// Motor (0) + 24 Zusatzsounds = 25 Slots, wie in der Sound-Basis
static constexpr int SND_NUM_SLOTS = 25;
static constexpr int SND_NAME_LEN  = 17;   // 16 Zeichen + Null

// Im Lua-Menue hoechstens 23 Sounds: das Menue hat max. 255 Eintraege mit
// einer 8-Bit-Feld-ID (CRSF), ab Feld 112 mit je 6 Feldern endet Sound 23
// bei Feld 249. In der Web-UI sind immer alle 24 Sounds einstellbar.
static constexpr uint8_t SND_LUA_FIRST_ID  = 112;
static constexpr uint8_t SND_LUA_MAX       = 23;

struct SoundConfig {
    int Source_Start_Sound[SND_NUM_SLOTS];   // Quelle Motor(0) + Sound 1-24
    int Volumen_Sound[SND_NUM_SLOTS];        // Lautstaerke 0-200
    int Mode_Sound[SND_NUM_SLOTS];           // 0=Normal, 1=Loop, 2=Tippbetrieb

    int Source_Speed_Sound_0;    // Quelle Motorgeschwindigkeit
    int throttle_mode;           // 0=Eine Richtung, 1=Zwei Richtungen
    int Min_Speed_Sound_0;       // Mindestgeschwindigkeit Motor (%)
    int Max_Speed_Sound_0;       // Maximalgeschwindigkeit Motor (%)
    int shutdowndelay;           // Motor aus Standgas Verzoegerung (s)
    int engine_on_toggle;        // 0=Normal, 1=Toggle
    int throttle_ramp;           // Gas-Rampe (%/s)
    int throttle_dead_band;      // Standgas-Totband (%)

    int Einkanal_Channel;        // SBUS-Kanal fuer Einkanal-Sounds (999=aus)
    int Einkanal_mode;           // 0=Normal, 10-13=WM-Adresse 0-3
    int modul_adress;            // Sound-Adresse: Einkanal-Sounds (EK) bei CRSF

    uint8_t sport_poll_id[2];    // S.Port Physical Poll-ID Sensor 1+2

    // NEU (Entwurf 28.10.2026): eigener Name je Sound, Index 0 = Motor
    char Sound_Name[SND_NUM_SLOTS][SND_NAME_LEN];
};

extern SoundConfig scfg;

// Speichern: wie in der Sound-Basis 500 ms nach der letzten Aenderung,
// zurueckgehalten solange nach einer SD-Schreibaktion der Cooldown laeuft.
void soundMarkDirty();
void soundLoadConfig();
void soundSaveNow();           // sofort (Neustart, /api/save)
void soundSavePending();       // aus loop()
void soundResetConfig();       // Werkseinstellung des Soundteils

// Ablauf (siehe setup()/loop() in RC_Multiswitch_Sound.ino)
void soundSetupEarly();        // SD-Karte, Motor-Dateien (vor dem WLAN-Start)
void soundSetupLate();         // SD-Lese-Task, Sample-Puffer (nach dem WLAN-Start)
void soundLoop();

// Einkanal-Sounds (Quelle "EK", 70-77)
void soundOnSwitchBits(uint8_t addr, uint8_t bits);  // CRSF, WM-Protokoll
void soundOnSbus(uint16_t ch);                       // SBUS, Kanal scfg.Einkanal_Channel
uint16_t soundEinkanalData();

bool isValidEinSource(int c);

// Lua-Menue: Motor (86-98) und Sound 1..count (ab 112). Die Felder im Ordner
// "Einstellungen" baut der Schaltteil, die Lese-/Schreibfunktionen des
// Soundteils stehen hier.
void soundLuaMotor(LuaMenu& m, uint8_t folderId);
void soundLuaSounds(LuaMenu& m, uint8_t count);
uint8_t sndLuaGetAddr(uint8_t);
void    sndLuaSetAddr(uint8_t, uint8_t v);
uint8_t sndLuaGetEkChannel(uint8_t);
void    sndLuaSetEkChannel(uint8_t, uint8_t v);
uint8_t sndLuaGetEkMode(uint8_t);
void    sndLuaSetEkMode(uint8_t, uint8_t v);

// Web: Routen des Soundteils am gemeinsamen Webserver (sound_web.cpp)
void soundWebRegister(WebServer& server);
bool soundSdCooldownActive();
