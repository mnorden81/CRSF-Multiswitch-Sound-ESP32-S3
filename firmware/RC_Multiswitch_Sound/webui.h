/*
 * webui.h  –  WiFi-Weboberfläche für RC-Multiswitch-Sound S3  v1.00
 * v1.00: Basis RC-Multiswitch-ESP32-S3 v4.06. Die Seite (webui_page.h)
 * enthaelt jetzt Schalt- und Soundteil; die Routen des Soundteils
 * (/api/sconfig, /api/sound, /api/debug, SD-Dateien, ...) haengt
 * soundWebRegister() (sound_web.cpp) an denselben Webserver. /api/config
 * hat zusaetzlich lua_sounds und led_pin.
 * v4.06: Einstellungs-Backup im Flash (RC_Common config_backup, Karte
 * "Konfiguration sichern"), automatisch vor Import und Firmware-Update.
 * v4.05: OTA-Upload, JSON-Helfer und /rc_common.js kommen aus RC_Common
 * (src/RC_Common/web_common.h). Baudrate je UART, Rolle "Raw".
 * NEU v4.04: Karte "UART B/C" im RC-Tab (Rolle Aus/CRSF/S.Port/GPS) und
 * Anzeige von S.Port/GPS im Debug-Tab. g_port_role[], g_port_baud[],
 * g_ports[] stehen in RC_Multiswitch.ino vor dem Include dieser Datei.
 * NEU V3.00: MKan-Gruppen (2 zusaetzliche, unabhaengige Kanalquellen) -
 * siehe Kommentar bei MULTISW_NUM_GROUPS in MultiSwitch_ESP32_V3.ino.
 * NEU V3.10: bis zu 3 Konfigurationen (Kanal-Quelle + Blink/PWM) pro
 * Ausgang, Prioritaet Konfig1 > Konfig2 > Konfig3 - siehe evalSlotActive()/
 * Output() in MultiSwitch_ESP32_V3.ino. Konfig 2/3 sind bewusst NUR hier in
 * der Web-UI verfuegbar, nicht im CRSF-LUA-Menue (siehe README).
 */

#pragma once

#include <ctype.h>
#include <stdlib.h>
#include <WiFi.h>
#include <WebServer.h>

// ============================================================
//  Forward-Deklarationen
// ============================================================

extern CRSF          crsf;
extern int           RC_System;
extern int           RC_System_boot;
extern int           CRSF_Channel;
extern int           einkanal_mode;
extern int           modul_adress;
extern int           Ausgang_Kanal[8];
extern int           pwm_wert[8];
extern int           mode[8];
extern int           Ausgang_Kanal2[8];         // NEU V3.10: Konfig 2 pro Ausgang
extern int           pwm_wert2[8];              // NEU V3.10: Konfig 2 pro Ausgang
extern int           mode2[8];                  // NEU V3.10: Konfig 2 pro Ausgang
extern int           Ausgang_Kanal3[8];         // NEU V3.10: Konfig 3 pro Ausgang
extern int           pwm_wert3[8];              // NEU V3.10: Konfig 3 pro Ausgang
extern int           mode3[8];                  // NEU V3.10: Konfig 3 pro Ausgang
extern int           EK_Gruppen_Adresse[2];      // NEU V3.00: MKan
extern int           SBUS_Gruppen_Channel[2];    // NEU V3.00: MKan
extern int           SBUS_Gruppen_Mode[2];       // NEU V3.00: MKan
extern char          Ausgang_Name[8][17];
extern uint16_t      channel_output[16];
extern uint16_t      einkanal_Data;
extern bool          BUS_OK;
extern bool          Ausgang[8];
extern const uint8_t OutPin[8];
extern char          g_wifi_ssid[33];
extern char          g_wifi_pass[64];
extern char          g_wifi_ip[16];
extern char          g_device_name[24];   // NEU V1.4
extern const uint16_t Version;            // Firmware-Version (z.B. 140 = v1.4)
extern const char*   AP_IP_STR;
extern uint8_t       wm_prop_value[8];   // NEU v0.14
extern void          storageSave();
extern void          nvsSave();
extern void          nvsReset();
extern bool          g_wifi_auto;          // NEU v2.00
extern uint16_t      g_wifi_auto_timeout;  // NEU v2.00
extern bool          g_wifi_always_on;     // NEU v2.02

// ============================================================
//  Debug
// ============================================================
#ifdef SERIAL_DEBUG
template<typename... A> void _dbg(A... a){ (Serial.print(a),...); Serial.println(); }
#define DBG(...) _dbg(__VA_ARGS__)
#else
#define DBG(...) do{}while(0)
#endif

// ============================================================
//  Manuelle Overrides (Web → Output)
// ============================================================
bool g_manual_override[8] = {};
bool g_manual_state[8]    = {};

// ============================================================
//  WebServer
// ============================================================
static WebServer webServer(80);

// AP-Steuerung (NEU v2.00) - Definitionen weiter unten bei webui_init(),
// hier vorab deklariert, weil handleStatus() (weiter oben im File) den
// aktuellen AP-Status schon mit ausliefert.
bool webui_isApActive();
void webui_enableAP();
void webui_disableAP();

// ============================================================
//  HTML (PROGMEM) – siehe webui_page.h
// ============================================================
#include "webui_page.h"   // WEBUI_HTML: Schalt- und Soundseite zusammen (v1.00)

// ============================================================
//  JSON-Helfer
// ============================================================
static void sendJson(const String& json) {
    webServer.sendHeader("Cache-Control","no-cache");
    webServer.send(200, "application/json", json);
}
static void sendOk()  { sendJson("{\"ok\":true}"); }
static void send404() { webServer.send(404,"application/json","{\"error\":\"not found\"}"); }

// v4.05: JSON-Helfer aus RC_Common (webJsonEscape/-Int/-Bool/-String)

// ============================================================
//  RC-System-Name
// ============================================================
// v4.05: Rolle und Baudrate eines UARTs fuer den Debug-Tab, GPS mit "Auto"
static String portStateText(const UartPort& p) {
  String t = uartRoleName(p.activeRole);
  if (p.activeRole == UROLE_OFF) return t;
  if (p.activeRole == UROLE_GPS && gpsBaudAuto())
    return t + " Auto " + (gpsBaudLocked() ? String(gpsGetBaud()) : String("(sucht)"));
  return t + " " + String(p.baud);
}

static const char* rcName(int sys) {
    switch(sys) {
        case 0: return "FrSky SBUS";
        case 1: return "FlySky SBUS";
        case 2: return "ELRS SBUS";
        case 3: return "HoTT SBUS";
        case 4: return "CRSF";
        default: return "Unbekannt";
    }
}

// ============================================================
//  API: /api/status
// ============================================================
static void handleStatus() {
    String j = "{";
    // Firmware-Version dynamisch aus Version-Konstante (z.B. 140 -> "v1.40")
    j += "\"version\":\"v" + String(Version / 100) + "." + String(Version % 100 < 10 ? "0" : "") + String(Version % 100) + "\",";
    j += "\"bus_ok\":" + String(BUS_OK ? "true" : "false") + ",";
    j += "\"rc_system\":" + String(RC_System_boot) + ",";
  j += "\"rc_system_name\":\"" + webJsonEscape(rcName(RC_System_boot)) + "\",";
        j += "\"rc_system_boot\":" + String(RC_System_boot) + ",";

    // Einkanal
    char hex[8]; snprintf(hex, sizeof(hex), "%02X", einkanal_Data);
    j += "\"einkanal_data_hex\":\"" + String(hex) + "\",";

    // Ausgänge
    j += "\"outputs\":[";
    for (int i = 0; i < 8; i++) {
        if (i) j += ",";
        j += "{";
        j += "\"name\":\"" + webJsonEscape(Ausgang_Name[i]) + "\",";
        j += "\"on\":"    + String(Ausgang[i] ? "true" : "false") + ",";
        j += "\"override\":" + String(g_manual_override[i] ? "true" : "false") + ",";
        j += "\"kanal\":" + String(Ausgang_Kanal[i]) + ",";
        j += "\"pwm\":"   + String(pwm_wert[i]) + ",";
        char mhex[6]; snprintf(mhex, sizeof(mhex), "0x%04X", mode[i]);
        j += "\"mode_hex\":\"" + String(mhex) + "\",";
        j += "\"mode\":" + String(mode[i]) + ",";
        // NEU V3.10: Konfig 2/3 pro Ausgang (optional, Prioritaet 1>2>3)
        j += "\"kanal2\":" + String(Ausgang_Kanal2[i]) + ",";
        j += "\"pwm2\":"   + String(pwm_wert2[i]) + ",";
        j += "\"mode2\":"  + String(mode2[i]) + ",";
        j += "\"kanal3\":" + String(Ausgang_Kanal3[i]) + ",";
        j += "\"pwm3\":"   + String(pwm_wert3[i]) + ",";
        j += "\"mode3\":"  + String(mode3[i]);
        j += "}";
    }
    j += "],";

    // Rohkanäle (erste 8)
    j += "\"channels\":[";
    for (int i = 0; i < 8; i++) { if(i)j+=","; j+=channel_output[i]; }
    j += "],";

    // MWprop-Werte (NEU v0.14)
    j += "\"wm_prop\":[";
    for (int i = 0; i < 8; i++) { if(i)j+=","; j+=wm_prop_value[i]; }
    j += "],";

    // CRSF-Diagnose-Zaehler (NEU v2.00) - bei SBUS bleiben sie auf 0
    j += "\"crsf_diag\":{";
    j += "\"raw_bytes\":"    + String(crsf.getRawBytesRx())  + ",";
    j += "\"valid_frames\":" + String(crsf.getValidFrames()) + ",";
    j += "\"crc_errors\":"   + String(crsf.getCrcErrors())   + ",";
    j += "\"device_pings\":" + String(crsf.getDevicePings()) + ",";
    j += "\"param_reads\":"  + String(crsf.getParamReads())  + ",";
    j += "\"param_writes\":" + String(crsf.getParamWrites()) + ",";
    // v4.01: Router-Zaehler
    j += "\"fwd_down\":"    + String(crsf.getFwdDown())    + ",";
    j += "\"fwd_up\":"      + String(crsf.getFwdUp())      + ",";
    j += "\"fwd_dropped\":" + String(crsf.getFwdDropped()) + ",";
    j += "\"down_crc\":"    + String(crsf.getDownCrcErrors()) + ",";
    j += "\"dup_addr\":"    + String(crsf.isDuplicateAddress() ? "true" : "false");
    j += "},";
    // NEU v4.04: UART B/C (Stand beim Booten) + S.Port/GPS-Daten
    j += "\"ports\":{";
    j += "\"b\":\"" + portStateText(g_ports[0]) + "\",";
    j += "\"c\":\"" + portStateText(g_ports[1]) + "\",";
#if defined(UART_C_SERIAL)
    j += "\"has_c\":true,";
#else
    j += "\"has_c\":false,";
#endif
    j += "\"c_dup\":" + String(g_ports[1].duplicate ? "true" : "false");
    if (sportLipoIsActive()) {
        j += ",\"sport\":[";
        for (int i = 0; i < SPORT_NUM_SENSORS; i++) {
            if (i) j += ",";
            j += "{\"online\":" + String(lipoSensor[i].online ? "true" : "false");
            j += ",\"cells\":" + String(lipoSensor[i].cellCount);
            j += ",\"total\":" + String(lipoSensor[i].totalVoltage, 2);
            j += ",\"min\":"   + String(lipoSensor[i].minCell, 2) + "}";
        }
        j += "],\"sport_polls\":" + String(sportGetPollsSent());
        j += ",\"sport_bytes\":"  + String(sportGetRawBytesRx());
        j += ",\"sport_frames\":" + String(sportGetValidFrames());
        j += ",\"sport_crc\":"    + String(sportGetCrcErrors());
    }
    if (gpsIsActive()) {
        j += ",\"gps\":{\"fix\":" + String(gpsHasFix() ? "true" : "false");
        j += ",\"sats\":" + String(gpsGetSatellites());
        j += ",\"kmh\":"  + String(gpsGetSpeedKmh(), 1) + "}";
    }
    j += "},";
    j += "\"wifi_ap_active\":" + String(webui_isApActive() ? "true" : "false");

    j += "}";
    sendJson(j);
}

// ============================================================
//  API: /api/config (GET + POST)
// ============================================================
static void handleConfig() {
    if (webServer.method() == HTTP_GET) {
        String j = "{";
        j += "\"rc_system\":"     + String(RC_System)      + ",";
        j += "\"crsf_channel\":"  + String(CRSF_Channel)   + ",";
        j += "\"einkanal_mode\":" + String(einkanal_mode)  + ",";
        j += "\"modul_adress\":"  + String(modul_adress)   + ",";
        j += "\"ek_gadr0\":"    + String(EK_Gruppen_Adresse[0])   + ",";  // NEU V3.00
        j += "\"ek_gadr1\":"    + String(EK_Gruppen_Adresse[1])   + ",";  // NEU V3.00
        j += "\"sbus_gch0\":"   + String(SBUS_Gruppen_Channel[0]) + ",";  // NEU V3.00
        j += "\"sbus_gch1\":"   + String(SBUS_Gruppen_Channel[1]) + ",";  // NEU V3.00
        j += "\"sbus_gmode0\":" + String(SBUS_Gruppen_Mode[0])    + ",";  // NEU V3.00
        j += "\"sbus_gmode1\":" + String(SBUS_Gruppen_Mode[1])    + ",";  // NEU V3.00
      j += "\"dnam\":\""        + webJsonEscape(g_device_name)  + "\",";
      j += "\"ssid\":\""        + webJsonEscape(g_wifi_ssid)    + "\",";
      j += "\"pass\":\""        + webJsonEscape(g_wifi_pass)    + "\",";
      j += "\"ip\":\""          + webJsonEscape(g_wifi_ip)      + "\",";
        j += "\"wifi_auto\":"      + String(g_wifi_auto ? "true" : "false") + ",";
        j += "\"wifi_auto_timeout\":" + String(g_wifi_auto_timeout) + ",";
        j += "\"wifi_always_on\":" + String(g_wifi_always_on ? "true" : "false") + ",";
        // NEU v4.04: Rollen UART B/C
        j += "\"port_roles\":\"" + String(uartRoleOptions()) + "\",";
        j += "\"portb_role\":" + String(g_port_role[0]) + ",";
        j += "\"portc_role\":" + String(g_port_role[1]) + ",";
        j += "\"portb_baud\":" + String(g_port_baud[0]) + ",";   // v4.05
        j += "\"portc_baud\":" + String(g_port_baud[1]) + ",";
#if defined(UART_C_SERIAL)
        j += "\"has_portc\":true,";
#else
        j += "\"has_portc\":false,";
#endif
        j += "\"portc_dup\":" + String(g_ports[1].duplicate ? "true" : "false") + ",";
        // v1.00: Kombimodul (wirken nach Neustart)
        j += "\"lua_sounds\":" + String(g_lua_sounds) + ",";
        j += "\"lua_sounds_max\":" + String(SND_LUA_MAX) + ",";
        j += "\"led_pin\":" + String(g_led_pin);
        j += "}";
        sendJson(j);
        return;
    }
    // POST
    if (!webServer.hasArg("plain")) { send404(); return; }
    String body = webServer.arg("plain");
    bool changed = false;

    int v = 0;
    if (webJsonInt(body, "rc_system", v)) {
      v = constrain(v, 0, 4);
      if (v != RC_System) { RC_System = v; changed = true; }
    }
    if (webJsonInt(body, "crsf_channel", v)) {
      if ((v >= 0 && v <= 15) || v == 999) {
        if (v != CRSF_Channel) { CRSF_Channel = v; changed = true; }
      }
    }
    if (webJsonInt(body, "einkanal_mode", v)) {
      bool valid = (v == 0 || v == 999 || (v >= 10 && v <= 13));
      if (valid && v != einkanal_mode) { einkanal_mode = v; changed = true; }
    }
    if (webJsonInt(body, "modul_adress", v)) {
      v = constrain(v, 0, 20);
      if (v != modul_adress) { modul_adress = v; changed = true; }
    }
    // NEU V3.00: MKan-Gruppen (CRSF-Adresse bzw. SBUS-Kanal/-Modus je Gruppe)
    if (webJsonInt(body, "ek_gadr0", v)) {
      v = (v == 255) ? 255 : constrain(v, 0, 20);
      if (v != EK_Gruppen_Adresse[0]) { EK_Gruppen_Adresse[0] = v; changed = true; }
    }
    if (webJsonInt(body, "ek_gadr1", v)) {
      v = (v == 255) ? 255 : constrain(v, 0, 20);
      if (v != EK_Gruppen_Adresse[1]) { EK_Gruppen_Adresse[1] = v; changed = true; }
    }
    if (webJsonInt(body, "sbus_gch0", v)) {
      v = (v == 999) ? 999 : constrain(v, 0, 15);
      if (v != SBUS_Gruppen_Channel[0]) { SBUS_Gruppen_Channel[0] = v; changed = true; }
    }
    if (webJsonInt(body, "sbus_gch1", v)) {
      v = (v == 999) ? 999 : constrain(v, 0, 15);
      if (v != SBUS_Gruppen_Channel[1]) { SBUS_Gruppen_Channel[1] = v; changed = true; }
    }
    if (webJsonInt(body, "sbus_gmode0", v)) {
      bool valid = (v == 0 || (v >= 10 && v <= 13));
      if (valid && v != SBUS_Gruppen_Mode[0]) { SBUS_Gruppen_Mode[0] = v; changed = true; }
    }
    if (webJsonInt(body, "sbus_gmode1", v)) {
      bool valid = (v == 0 || (v >= 10 && v <= 13));
      if (valid && v != SBUS_Gruppen_Mode[1]) { SBUS_Gruppen_Mode[1] = v; changed = true; }
    }

    // NEU v2.00: WLAN-Auto-Failsafe
    bool bv = false;
    if (webJsonBool(body, "wifi_auto", bv)) {
      if (bv != g_wifi_auto) { g_wifi_auto = bv; changed = true; }
    }
    if (webJsonInt(body, "wifi_auto_timeout", v)) {
      v = constrain(v, 5, 240);
      if ((uint16_t)v != g_wifi_auto_timeout) { g_wifi_auto_timeout = (uint16_t)v; changed = true; }
    }
    // NEU v2.02: WLAN dauerhaft an
    if (webJsonBool(body, "wifi_always_on", bv)) {
      if (bv != g_wifi_always_on) { g_wifi_always_on = bv; changed = true; }
    }
    // NEU v4.04: Rollen UART B/C, v4.05: Baudrate je UART (wirken nach Neustart)
    const char* roleKey[2] = {"portb_role", "portc_role"};
    const char* baudKey[2] = {"portb_baud", "portc_baud"};
    for (uint8_t i = 0; i < 2; i++) {
      if (webJsonInt(body, roleKey[i], v) && v >= 0 && v < UROLE_COUNT && v != g_port_role[i]) {
        setPortRole(i, (uint8_t)v); changed = true;
      }
      if (webJsonInt(body, baudKey[i], v) && v > 0 && uartBaudValid((uint32_t)v) && (uint32_t)v != g_port_baud[i]) {
        g_port_baud[i] = (uint32_t)v; changed = true;
      }
    }

    // v1.00: Lua-Sounds und Status-LED-Pin (wirken nach Neustart)
    if (webJsonInt(body, "lua_sounds", v)) {
      uint8_t n = clampLuaSounds(v);
      if (n != g_lua_sounds) { g_lua_sounds = n; changed = true; }
    }
    if (webJsonInt(body, "led_pin", v)) {
      int lp = clampLedPin(v);
      if (lp != g_led_pin) { g_led_pin = lp; changed = true; }
    }

    String s;
    if (webJsonString(body, "dnam", s)) {
      s = s.substring(0, sizeof(g_device_name) - 1);
      if (strncmp(g_device_name, s.c_str(), sizeof(g_device_name) - 1) != 0) {
        strncpy(g_device_name, s.c_str(), sizeof(g_device_name) - 1);
        g_device_name[sizeof(g_device_name) - 1] = '\0';
        changed = true;
      }
    }
    if (webJsonString(body, "ssid", s)) {
      s = s.substring(0, sizeof(g_wifi_ssid) - 1);
      if (strncmp(g_wifi_ssid, s.c_str(), sizeof(g_wifi_ssid) - 1) != 0) {
        strncpy(g_wifi_ssid, s.c_str(), sizeof(g_wifi_ssid) - 1);
        g_wifi_ssid[sizeof(g_wifi_ssid) - 1] = '\0';
        changed = true;
      }
    }
    if (webJsonString(body, "pass", s)) {
      s = s.substring(0, sizeof(g_wifi_pass) - 1);
      if (strncmp(g_wifi_pass, s.c_str(), sizeof(g_wifi_pass) - 1) != 0) {
        strncpy(g_wifi_pass, s.c_str(), sizeof(g_wifi_pass) - 1);
        g_wifi_pass[sizeof(g_wifi_pass) - 1] = '\0';
        changed = true;
      }
    }
    if (webJsonString(body, "ip", s)) {
      IPAddress ip;
      if (ip.fromString(s)) {
        s = s.substring(0, sizeof(g_wifi_ip) - 1);
        if (strncmp(g_wifi_ip, s.c_str(), sizeof(g_wifi_ip) - 1) != 0) {
          strncpy(g_wifi_ip, s.c_str(), sizeof(g_wifi_ip) - 1);
          g_wifi_ip[sizeof(g_wifi_ip) - 1] = '\0';
          changed = true;
        }
      }
    }

    if (changed) nvsSave();
    sendOk();
}

// ============================================================
//  API: /api/switch
// ============================================================
static void handleSwitch() {
    if (webServer.method() != HTTP_POST) { send404(); return; }
    if (!webServer.hasArg("plain")) { send404(); return; }
    String body = webServer.arg("plain");

    // ch
    int ch = -1;
    webJsonInt(body, "ch", ch);
    if (ch < 0 || ch > 7) { send404(); return; }

    // release
    bool release = false;
    if (webJsonBool(body, "release", release) && release) {
        g_manual_override[ch] = false;
        sendOk(); return;
    }

    // on/off
    bool on = false;
    if (!webJsonBool(body, "on", on)) { send404(); return; }
    g_manual_override[ch] = true;
    g_manual_state[ch]    = on;
    sendOk();
}

// ============================================================
//  API: /api/output
// ============================================================
static void handleOutput() {
    if (webServer.method() != HTTP_POST) { send404(); return; }
    if (!webServer.hasArg("plain")) { send404(); return; }
    String body = webServer.arg("plain");
    int ch = -1;
    webJsonInt(body, "ch", ch);
    if (ch < 0 || ch > 7) { send404(); return; }
    bool changed = false;
    int val = 0;
    if (webJsonInt(body, "kanal", val)) {
      if (val >= 0 && val <= 95 && val != Ausgang_Kanal[ch]) {  // NEU V3.00: MKan bis 95
        Ausgang_Kanal[ch] = val;
        changed = true;
      }
    }
    int pwm_old = pwm_wert[ch];
    if (webJsonInt(body, "pwm", val)) {
      bool validPwm = (val >= 0 && val <= 255) || (val >= 300 && val <= 315);
      if (validPwm && val != pwm_wert[ch]) {
        pwm_wert[ch] = val;
        changed = true;
      }
    }
    // MWprop aktiviert -> Ausgang automatisch einschalten
    if (pwm_wert[ch] >= 200 && pwm_wert[ch] <= 207 &&
        !(pwm_old >= 200 && pwm_old <= 207)) {
        einkanal_Data |= (1 << ch);
    }
    if (webJsonInt(body, "mode", val)) {
      val = constrain(val, 0, 0xFFFF);
      if (val != mode[ch]) {
        mode[ch] = val;
        changed = true;
      }
    }

    // NEU V3.10: Konfig 2 pro Ausgang (optional - 255 = "nicht verwendet")
    if (webJsonInt(body, "kanal2", val)) {
      if ((val == 255 || (val >= 0 && val <= 95)) && val != Ausgang_Kanal2[ch]) {
        Ausgang_Kanal2[ch] = val;
        changed = true;
      }
    }
    if (webJsonInt(body, "pwm2", val)) {
      bool validPwm = (val >= 0 && val <= 255) || (val >= 300 && val <= 315);
      if (validPwm && val != pwm_wert2[ch]) {
        pwm_wert2[ch] = val;
        changed = true;
      }
    }
    if (webJsonInt(body, "mode2", val)) {
      val = constrain(val, 0, 0xFFFF);
      if (val != mode2[ch]) {
        mode2[ch] = val;
        changed = true;
      }
    }

    // NEU V3.10: Konfig 3 pro Ausgang (optional - 255 = "nicht verwendet")
    if (webJsonInt(body, "kanal3", val)) {
      if ((val == 255 || (val >= 0 && val <= 95)) && val != Ausgang_Kanal3[ch]) {
        Ausgang_Kanal3[ch] = val;
        changed = true;
      }
    }
    if (webJsonInt(body, "pwm3", val)) {
      bool validPwm = (val >= 0 && val <= 255) || (val >= 300 && val <= 315);
      if (validPwm && val != pwm_wert3[ch]) {
        pwm_wert3[ch] = val;
        changed = true;
      }
    }
    if (webJsonInt(body, "mode3", val)) {
      val = constrain(val, 0, 0xFFFF);
      if (val != mode3[ch]) {
        mode3[ch] = val;
        changed = true;
      }
    }

    String nm;
    if (webJsonString(body, "name", nm)) {
      nm = nm.substring(0, 16);
      if (strncmp(Ausgang_Name[ch], nm.c_str(), 16) != 0) {
        strncpy(Ausgang_Name[ch], nm.c_str(), 16);
        Ausgang_Name[ch][16] = '\0';
        changed = true;
      }
    }
    if (changed) nvsSave();
    sendOk();
}

// ============================================================
//  API: /api/restart
// ============================================================
static void handleRestart() {
  storageSave();
    sendOk();
    delay(200);
    ESP.restart();
}

// ============================================================
//  API: /api/reset
// ============================================================
static void handleReset() {
    nvsReset();
    sendOk();
}

// ============================================================
//  API: /api/save  (NEU v2.00)
// ============================================================
// Schreibt die Konfiguration sofort auf Flash, OHNE neu zu starten - anders
// als /api/restart. Wird vom Config-Import gebraucht, der mehrere /api/
// config- und /api/output-Aufrufe hintereinander macht und erst am Ende
// einmal sichern will (statt bei jedem einzelnen Schritt neu zu starten).
static void handleSave() {
    storageSave();
    sendOk();
}

// ============================================================
//  API: /api/wifi  (NEU v2.00)
// ============================================================
// Manueller WLAN-Schalter: wirkt SOFORT ueber webui_enableAP()/
// webui_disableAP(), absichtlich OHNE nvsSave() - wie der bisherige "Test"-
// Schalter fuer Ausgaenge ist das nur ein Laufzeit-Zustand, kein Teil der
// gespeicherten Konfiguration. Fuer dauerhaftes Verhalten siehe wifi_auto/
// wifi_auto_timeout in /api/config.
static void handleWifi() {
    if (webServer.method() != HTTP_POST) { send404(); return; }
    if (!webServer.hasArg("plain")) { send404(); return; }
    String body = webServer.arg("plain");
    bool enable = false;
    if (!webJsonBool(body, "enable", enable)) { send404(); return; }
    if (enable) webui_enableAP(); else webui_disableAP();
    sendOk();
}

// ============================================================
//  webui_init / webui_handle
// ============================================================
// AP-Start/-Stop als eigene Funktionen (NEU v2.00, uebernommen aus dem
// Soundmodul-Projekt) - vorher direkt in webui_init() und damit nur beim
// Booten (GPIO13 == LOW) moeglich. Jetzt auch zur Laufzeit aufrufbar: manuell
// per /api/wifi oder automatisch per wifiFailsafeCheck() in der .ino.
static bool g_apActive = false;

bool webui_isApActive() { return g_apActive; }

void webui_enableAP() {
    if (g_apActive) return;
    IPAddress apIP, gw, subnet(255,255,255,0);
    if (!apIP.fromString(g_wifi_ip)) apIP.fromString("192.168.1.1");
    gw = apIP;
    WiFi.softAPConfig(apIP, gw, subnet);
    WiFi.softAP(g_wifi_ssid, g_wifi_pass);
    g_apActive = true;
    Serial.printf("AP gestartet (Heap frei: %u Bytes), IP: %s\n",
                  (unsigned)ESP.getFreeHeap(), WiFi.softAPIP().toString().c_str());
}

void webui_disableAP() {
    if (!g_apActive) return;
    WiFi.softAPdisconnect(true);
    g_apActive = false;
    Serial.println("AP gestoppt.");
}

void webui_init() {
    // FIX v2.00: WiFi-Treiber (LWIP/Netif) MUSS initialisiert sein, bevor
    // webServer.begin() unten laeuft - sonst existieren dessen interne
    // FreeRTOS-Queues noch nicht und es kommt beim Anlegen des Server-Sockets
    // zum Absturz ("assert failed: xQueueSemaphoreTake"), naemlich immer dann,
    // wenn der AP beim Booten NICHT aktiviert wird (GPIO13 nicht LOW und
    // WLAN-Auto-Failsafe (noch) nicht ausgeloest). WiFi.mode(WIFI_AP) legt nur
    // den Treiber/die Queues an, sendet aber noch nichts sichtbares - das
    // eigentliche Einschalten des Access Points (SSID sichtbar) macht weiterhin
    // ausschliesslich webui_enableAP().
    WiFi.mode(WIFI_AP);

    // NEU v2.02: WLAN dauerhaft an (g_wifi_always_on) uebersteuert den
    // GPIO13-Bootpin vollstaendig - siehe Kommentar bei g_wifi_always_on
    // in der .ino. Default AN, siehe Migrations-Default in nvsLoad().
    // v4.00: WifiPin (GPIO13) entfaellt auf den C3/S3-Platinen.
    // v4.01: Umschalten zur Laufzeit per BOOT-Taster (bootButtonCheck() in der .ino).
    if (g_wifi_always_on) {
        webui_enableAP();
    }
    webServer.on("/",            HTTP_GET,  []{ webServer.send_P(200,"text/html",WEBUI_HTML); });
    webServer.on("/api/status",  HTTP_GET,  handleStatus);
    webServer.on("/api/config",  HTTP_GET,  handleConfig);
    webServer.on("/api/config",  HTTP_POST, handleConfig);
    webServer.on("/api/switch",  HTTP_POST, handleSwitch);
    webServer.on("/api/output",  HTTP_POST, handleOutput);
    webServer.on("/api/reset",   HTTP_POST, handleReset);
    webServer.on("/api/restart", HTTP_POST, handleRestart);
    webServer.on("/api/save",    HTTP_POST, handleSave);
    webServer.on("/api/wifi",    HTTP_POST, handleWifi);
    // v4.05: /api/otaupdate, /rc_common.js, /api/uartroles aus RC_Common
    webCommonRegister(webServer, storageSave);
    // v4.06: Einstellungs-Backups im Flash (/api/backup/*, RC_Common)
    cfgBackupRegister(webServer);
    // v1.00: Routen des Soundteils (sound_web.cpp)
    soundWebRegister(webServer);
    // WebServer.begin() bewusst unabhaengig vom AP-Status: der Server lauscht
    // so schon, sobald der AP spaeter per Auto-Failsafe/manuell aktiviert wird
    // (kein erneutes .begin() noetig).
    webServer.begin();
    Serial.println("WebServer gestartet.");
}

void webui_handle() {
    webServer.handleClient();
}
