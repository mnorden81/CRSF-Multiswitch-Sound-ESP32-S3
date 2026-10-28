/*
 * sound_web.cpp  -  Web-API des Soundteils (RC-Multiswitch-Sound, ESP32-S3)
 *
 * Die Handler stammen aus WebServerManager.cpp der ESP32-Sound-Basis v7.21
 * und laufen hier am gemeinsamen Webserver des Schaltteils (webui.h). Die
 * Seite selbst (Reiter Motor, Sounds, LiPo und die Sound-Karten in RC-System,
 * WLAN und Debug) liegt in webui_page.h.
 *
 * Unterschiede zur Sound-Basis:
 *   - /api/config der Sound-Basis heisst hier /api/sconfig (/api/config
 *     gehoert dem Schaltteil). GET liefert das gewohnte Format; RC-System,
 *     MKan-Gruppen, WLAN und Port B/C sind dort nur gespiegelt, sie werden
 *     im Schaltteil eingestellt. POST nimmt nur die Werte des Soundteils:
 *     modul_adr (Sound-Adresse), ek_channel, ek_mode, poll_id0/1.
 *   - NEU: Name je Sound (/api/sound {"id":n,"name":"..."}, max. 16 Zeichen).
 *   - /api/sdexport schreibt den mitgeschickten Inhalt (Gesamtsicherung aus
 *     Schalt- und Soundteil, wie beim Export als Datei). Ohne Inhalt nur den
 *     Soundteil, wie bisher.
 *   - Die alten URL-Parameter (/?VolumenSound=... usw.) und die PWM-/Pin-
 *     Eingaenge der Hardware V1/V2 entfallen.
 */
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include "sound.h"
#include "XT_I2S_Audio.h"
#include "src/RC_Common/CRSF_ESP32/crsf_esp32.h"
#include "src/RC_Common/uart_roles.h"
#include "src/RC_Common/uart_ports.h"
#include "src/RC_Common/sport_lipo.h"
#include "src/RC_Common/gps_speed.h"
#include "src/RC_Common/web_common.h"

// ── Soundteil (sound.cpp) ───────────────────────────────────────────────
extern bool          Sound_on_web[SND_NUM_SLOTS];
extern XT_Wav_Class  Sound_loop;
extern XT_Wav_Class  Sound_shut;
extern XT_Wav_Class  Sound_start;
extern XT_Wav_Class* Sounds[SND_NUM_SLOTS];

// ── Schaltteil (RC_Multiswitch_Sound.ino, webui.h) ─────────────────────
extern CRSF     crsf;
extern uint16_t channel_output[16];
extern bool     BUS_OK;
extern int      RC_System;
extern int      EK_Gruppen_Adresse[2];
extern int      SBUS_Gruppen_Channel[2];
extern int      SBUS_Gruppen_Mode[2];
extern uint8_t  g_port_role[2];
extern uint32_t g_port_baud[2];
extern UartPort g_ports[2];
extern char     g_wifi_ssid[33];
extern char     g_wifi_pass[64];
extern char     g_wifi_ip[16];
extern char     g_device_name[24];
extern bool     g_wifi_auto;
extern uint16_t g_wifi_auto_timeout;
extern bool     g_wifi_always_on;
uint16_t fwVersion();
bool webui_isApActive();
void webui_enableAP();
void webui_disableAP();

static WebServer* sv = nullptr;

// v7.16: nach einer SD-Schreiboperation 3 s kein NVS-Schreiben (Brownout
// ohne Stuetzkondensator, siehe Sound-Basis WebServerManager.cpp)
static constexpr unsigned long SD_SAVE_COOLDOWN_MS = 3000;
static unsigned long lastSdActivityMs = 0;
static void markSdActivity() { lastSdActivityMs = millis(); }
bool soundSdCooldownActive() {
    return lastSdActivityMs != 0 && (millis() - lastSdActivityMs) < SD_SAVE_COOLDOWN_MS;
}

static String versionString() {
    char b[8]; snprintf(b, sizeof(b), "%u.%02u", fwVersion() / 100, fwVersion() % 100);
    return String(b);
}

static void sendJson(const String& json) {
    sv->sendHeader("Cache-Control", "no-cache");
    sv->send(200, "application/json", json);
}
static void sendOk()  { sendJson("{\"ok\":true}"); }
static void sendErr() { sv->send(400, "application/json", "{\"error\":\"bad request\"}"); }

// ── /sport JSON-Endpunkt (LiPo Live-Daten) ──────────────────────────────
static void handleSport() {
    char json[512]; int pos = 0;
    pos += snprintf(json+pos, sizeof(json)-pos, "{");
    for (uint8_t i = 0; i < 2; i++) {
        pos += snprintf(json+pos, sizeof(json)-pos,
            "\"s%u\":{\"online\":%s,\"cells\":%u,\"total\":%.2f,\"min\":%.3f,\"soc\":%u,\"cv\":[",
            i, lipoSensor[i].online ? "true" : "false",
            lipoSensor[i].cellCount, lipoSensor[i].totalVoltage,
            lipoSensor[i].minCell, sportCalcSoC(lipoSensor[i].minCell));
        for (uint8_t c = 0; c < 6; c++)
            pos += snprintf(json+pos, sizeof(json)-pos, "%.3f%s",
                lipoSensor[i].cellVoltage[c], c < 5 ? "," : "");
        pos += snprintf(json+pos, sizeof(json)-pos, "]}%s", i < 1 ? "," : "");
    }
    pos += snprintf(json+pos, sizeof(json)-pos,
        ",\"pid0\":\"%02X\",\"pid1\":\"%02X\"}",
        scfg.sport_poll_id[0], scfg.sport_poll_id[1]);
    sv->send(200, "application/json", json);
}

// ── POST /api/sconfig ────────────────────────────────────────────────────
static void handleConfigPost() {
    if (!sv->hasArg("plain")) { sendErr(); return; }
    const String& body = sv->arg("plain");
    bool changed = false;
    int  v;
    if (webJsonInt(body, "ek_channel", v)) { scfg.Einkanal_Channel = (v==999)?999:constrain(v,0,15); changed=true; }
    if (webJsonInt(body, "ek_mode",    v)) { scfg.Einkanal_mode    = v;                            changed=true; }
    if (webJsonInt(body, "modul_adr",  v)) { scfg.modul_adress     = constrain(v,0,20);            changed=true; }
    String s;
    if (webJsonString(body, "poll_id0", s)) { scfg.sport_poll_id[0]=(uint8_t)strtol(s.c_str(),nullptr,16); changed=true; }
    if (webJsonString(body, "poll_id1", s)) { scfg.sport_poll_id[1]=(uint8_t)strtol(s.c_str(),nullptr,16); changed=true; }
    if (changed) soundMarkDirty();
    sendOk();
}

// ── GET /api/sconfig ─────────────────────────────────────────────────────
// Gleiches Format wie /api/config der Sound-Basis (motor, sounds, settings,
// sport, wifi, version), dazu "name" je Sound.
static String buildSoundJson() {
    String j = "{";
    j += "\"motor\":{";
    j += "\"source\":"     + String(scfg.Source_Start_Sound[0]) + ",";
    j += "\"speed_src\":"  + String(scfg.Source_Speed_Sound_0)  + ",";
    j += "\"vol\":"        + String(scfg.Volumen_Sound[0])       + ",";
    j += "\"mode\":"       + String(scfg.throttle_mode)          + ",";
    j += "\"toggle\":"     + String(scfg.engine_on_toggle)       + ",";
    j += "\"rpm_min\":"    + String(scfg.Min_Speed_Sound_0)      + ",";
    j += "\"rpm_max\":"    + String(scfg.Max_Speed_Sound_0)      + ",";
    j += "\"shutdown_s\":" + String(scfg.shutdowndelay)          + ",";
    j += "\"ramp\":"       + String(scfg.throttle_ramp)          + ",";
    j += "\"deadband\":"   + String(scfg.throttle_dead_band)     + "},";

    j += "\"sounds\":[";
    for (int i = 1; i < SND_NUM_SLOTS; i++) {
        if (i > 1) j += ",";
        j += "{\"id\":"     + String(i)                            + ",";
        j += "\"source\":"  + String(scfg.Source_Start_Sound[i]) + ",";
        j += "\"vol\":"     + String(scfg.Volumen_Sound[i])       + ",";
        j += "\"mode\":"    + String(scfg.Mode_Sound[i])          + ",";
        j += "\"name\":\""  + webJsonEscape(scfg.Sound_Name[i])   + "\"}";
    }
    j += "],";

    // Einstellungen: Hardware wie V3 der Sound-Basis (keine PWM-/Pin-Eingaenge)
    j += "\"settings\":{";
    j += "\"hw_config\":2,\"hw_config_pending\":2,\"restart_pending\":0,";
    j += "\"portb_role\":" + String(g_port_role[0]) + ",";
    j += "\"portb_baud\":" + String(g_port_baud[0]) + ",";
    j += "\"portb_options\":\"" + String(uartRoleOptions()) + "\",";
    j += "\"portb_baud_defaults\":[";
    for (uint8_t i = 0; i < UROLE_COUNT; i++) {
        if (i > 0) j += ",";
        j += String(uartRoleDefaultBaud(i));
    }
    j += "],";
    j += "\"portc_gps\":"  + String(g_port_role[1] == UROLE_GPS ? 1 : 0) + ",";
    j += "\"rc_system\":"  + String(RC_System)              + ",";
    j += "\"ek_channel\":" + String(scfg.Einkanal_Channel)  + ",";
    j += "\"ek_mode\":"    + String(scfg.Einkanal_mode)     + ",";
    j += "\"modul_adr\":"  + String(scfg.modul_adress)      + ",";
    j += "\"mkan_adr0\":"  + String(EK_Gruppen_Adresse[0])  + ",";
    j += "\"mkan_adr1\":"  + String(EK_Gruppen_Adresse[1])  + ",";
    j += "\"sbusgrp_ch0\":"   + String(SBUS_Gruppen_Channel[0]) + ",";
    j += "\"sbusgrp_mode0\":" + String(SBUS_Gruppen_Mode[0])    + ",";
    j += "\"sbusgrp_ch1\":"   + String(SBUS_Gruppen_Channel[1]) + ",";
    j += "\"sbusgrp_mode1\":" + String(SBUS_Gruppen_Mode[1])    + ",";
    j += "\"pwm_min\":1000,\"pwm_max\":2000},";

    char p0[4], p1[4];
    snprintf(p0, sizeof(p0), "%02X", scfg.sport_poll_id[0]);
    snprintf(p1, sizeof(p1), "%02X", scfg.sport_poll_id[1]);
    j += "\"sport\":{\"poll_id0\":\"" + String(p0) + "\",\"poll_id1\":\"" + String(p1) + "\"},";

    j += "\"wifi\":{";
    j += "\"pass\":\""   + webJsonEscape(g_wifi_pass)   + "\",";
    j += "\"ssid\":\""   + webJsonEscape(g_wifi_ssid)   + "\",";
    j += "\"ip\":\""     + webJsonEscape(g_wifi_ip)     + "\",";
    j += "\"device\":\"" + webJsonEscape(g_device_name) + "\",";
    j += "\"auto_enable\":"  + String(g_wifi_auto ? 1 : 0)      + ",";
    j += "\"auto_timeout\":" + String(g_wifi_auto_timeout)      + ",";
    j += "\"always_on\":"    + String(g_wifi_always_on ? 1 : 0) + "},";

    j += "\"version\":\"" + versionString() + "\"";
    j += "}";
    return j;
}

static void handleConfigGet() { sendJson(buildSoundJson()); }

// ── Sicherungen auf der SD-Karte (/cfgbkup) ─────────────────────────────
static constexpr const char* SD_BACKUP_DIR = "/cfgbkup";

static bool isSafeBackupFileName(const String& name) {
    if (name.length() == 0 || name.length() > 40) return false;
    if (name.indexOf('/') >= 0 || name.indexOf("..") >= 0) return false;
    for (size_t i = 0; i < name.length(); i++) {
        char c = name[i];
        bool ok = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
                  (c >= 'A' && c <= 'Z') || c == '_' || c == '.' || c == '-';
        if (!ok) return false;
    }
    return true;
}

// Fortlaufende Nummer fuer die Dateinamen (keine Uhr im Modul)
static uint32_t nextSdBackupCounter() {
    Preferences p; p.begin("snd", false);
    uint32_t n = p.getUInt("sdbkupn", 0) + 1;
    p.putUInt("sdbkupn", n);
    p.end();
    return n;
}

// POST /api/sdexport: mitgeschickte Gesamtsicherung (Web-UI) oder, ohne
// Inhalt, nur der Soundteil (/api/sconfig)
static void handleSdExport() {
    String j;
    if (sv->hasArg("plain") && sv->arg("plain").length() > 2) j = sv->arg("plain");
    else j = buildSoundJson();
    if (j.length() > 32768) { sv->send(413, "application/json", "{\"ok\":false,\"error\":\"too_large\"}"); return; }
    if (!sdMutexTake(3000)) {
        sv->send(503, "application/json", "{\"ok\":false,\"error\":\"sd_busy\"}");
        return;
    }
    if (!SD.exists(SD_BACKUP_DIR)) SD.mkdir(SD_BACKUP_DIR);
    uint32_t n = nextSdBackupCounter();
    char fname[48];
    snprintf(fname, sizeof(fname), "%s/config_%04u_v%u.json", SD_BACKUP_DIR, (unsigned)n, (unsigned)fwVersion());
    bool ok = false;
    File f = SD.open(fname, FILE_WRITE);
    if (f) {
        ok = (f.print(j) == (int)j.length());
        f.close();
    }
    sdMutexGive();
    markSdActivity();
    if (ok) sendJson("{\"ok\":true,\"file\":\"" + String(fname + 1) + "\"}");
    else    sv->send(500, "application/json", "{\"ok\":false,\"error\":\"sd_write\"}");
}

static void handleSdList() {
    String j = "{\"ok\":true,\"files\":[";
    bool first = true;
    if (sdMutexTake(3000)) {
        if (SD.exists(SD_BACKUP_DIR)) {
            File dir = SD.open(SD_BACKUP_DIR);
            if (dir && dir.isDirectory()) {
                File entry = dir.openNextFile();
                while (entry) {
                    if (!entry.isDirectory()) {
                        String nm = String(entry.name());
                        int slash = nm.lastIndexOf('/');
                        if (slash >= 0) nm = nm.substring(slash + 1);
                        if (nm.endsWith(".json") && isSafeBackupFileName(nm)) {
                            if (!first) j += ",";
                            j += "{\"name\":\"" + nm + "\",\"size\":" + String(entry.size()) + "}";
                            first = false;
                        }
                    }
                    entry.close();
                    entry = dir.openNextFile();
                }
                dir.close();
            }
        }
        sdMutexGive();
    }
    j += "]}";
    sendJson(j);
}

static void handleSdConfig() {
    String name = sv->arg("file");
    if (!isSafeBackupFileName(name)) { sv->send(400, "application/json", "{\"ok\":false,\"error\":\"bad_name\"}"); return; }
    String path = String(SD_BACKUP_DIR) + "/" + name;
    if (!sdMutexTake(3000)) { sv->send(503, "application/json", "{\"ok\":false,\"error\":\"sd_busy\"}"); return; }
    File f = SD.open(path, FILE_READ);
    if (!f) {
        sdMutexGive();
        sv->send(404, "application/json", "{\"ok\":false,\"error\":\"not_found\"}");
        return;
    }
    String content = f.readString();
    f.close();
    sdMutexGive();
    sendJson(content);
}

static void handleSdDelete() {
    String name = sv->arg("file");
    if (!isSafeBackupFileName(name)) { sv->send(400, "application/json", "{\"ok\":false,\"error\":\"bad_name\"}"); return; }
    String path = String(SD_BACKUP_DIR) + "/" + name;
    if (!sdMutexTake(3000)) { sv->send(503, "application/json", "{\"ok\":false,\"error\":\"sd_busy\"}"); return; }
    bool ok = SD.remove(path);
    sdMutexGive();
    markSdActivity();
    if (ok) sendJson("{\"ok\":true,\"file\":\"" + name + "\"}");
    else    sv->send(500, "application/json", "{\"ok\":false,\"error\":\"sd_delete\"}");
}

// ── Sound-Dateien hochladen/loeschen (WLAN -> SD-Karte) ──────────────────
static bool isValidSoundUploadName(const String& name) {
    if (name == "loop.wav" || name == "shut.wav" || name == "start.wav") return true;
    if (!name.startsWith("sound") || !name.endsWith(".wav")) return false;
    String numPart = name.substring(5, name.length() - 4);
    if (numPart.length() == 0 || numPart.length() > 2) return false;
    for (size_t i = 0; i < numPart.length(); i++) if (!isDigit(numPart[i])) return false;
    int n = numPart.toInt();
    return n >= 1 && n <= 24;
}

static XT_Wav_Class* soundObjectFor(const String& name) {
    if (name == "loop.wav")  return &Sound_loop;
    if (name == "shut.wav")  return &Sound_shut;
    if (name == "start.wav") return &Sound_start;
    if (name.startsWith("sound") && name.endsWith(".wav")) {
        int n = name.substring(5, name.length() - 4).toInt();
        if (n >= 1 && n <= 24) return Sounds[n];
    }
    return nullptr;
}

// v7.11/v7.12: nur 5 Datei-Deskriptoren (max_files, siehe SDCardInit()).
// Waehrend Upload/Loeschen die gerade nicht spielenden Handles schliessen
// und danach genau diese wieder oeffnen. UnLoadWavFile()/LoadWavFile()
// nehmen den SD-Mutex selbst: nur AUSSERHALB von sdMutexTake() aufrufen.
// Bit 0-2 = Motor (loop/shut/start), Bit 3-26 = Sounds[1..24].
static XT_Wav_Class* const kMotorSounds[3] = { &Sound_loop, &Sound_shut, &Sound_start };

static uint32_t releaseIdleSoundHandles(XT_Wav_Class* except) {
    uint32_t mask = 0;
    for (uint8_t i = 0; i < 3; i++) {
        XT_Wav_Class* s = kMotorSounds[i];
        if (s == except || s->Playing || !s->WavFile) continue;
        s->UnLoadWavFile();
        mask |= (1ul << i);
    }
    for (uint8_t n = 1; n <= 24; n++) {
        XT_Wav_Class* s = Sounds[n];
        if (s == nullptr || s == except || s->Playing || !s->WavFile) continue;
        s->UnLoadWavFile();
        mask |= (1ul << (n + 2));
    }
    return mask;
}

static void restoreSoundHandles(uint32_t mask) {
    for (uint8_t i = 0; i < 3; i++)
        if (mask & (1ul << i)) kMotorSounds[i]->LoadWavFile();
    for (uint8_t n = 1; n <= 24; n++)
        if ((mask & (1ul << (n + 2))) && Sounds[n]) Sounds[n]->LoadWavFile();
}

static void logSdUploadFail(const char* name, const char* err, uint32_t motorMask, uint32_t bytesSoFar) {
    uint64_t total = SD.totalBytes(), used = SD.usedBytes();
    Serial.printf("[SD-Upload] FEHLER %s bei /%s | geschrieben=%u B | "
                  "Karte: %llu frei von %llu B | freigegebene Handles=0x%08X | Heap=%u\n",
                  err, name, (unsigned)bytesSoFar,
                  (unsigned long long)(total > used ? total - used : 0),
                  (unsigned long long)total, (unsigned)motorMask,
                  (unsigned)ESP.getFreeHeap());
}

static File     uploadFile;
static bool     uploadMutexHeld   = false;
static bool     uploadFailed      = false;
static String   uploadErrCode;
static uint32_t uploadBytes       = 0;
static XT_Wav_Class* uploadTargetSound = nullptr;
static uint32_t uploadMotorMask   = 0;

// Upload-Callback (START -> mehrfach WRITE -> END/ABORTED). Die Datei geht
// haeppchenweise direkt auf die SD-Karte; der SD-Mutex bleibt fuer die
// ganze Uebertragung genommen. Vor dem Schreiben wird die alte Datei
// geloescht und das geprueft (v7.09: sonst haengt FILE_WRITE hinten an,
// v7.12: schreibgeschuetzte Datei -> sd_locked).
static void handleSoundUploadData() {
    HTTPUpload& upload = sv->upload();

    if (upload.status == UPLOAD_FILE_START) {
        uploadFailed = false; uploadErrCode = ""; uploadBytes = 0; uploadMutexHeld = false;
        uploadTargetSound = nullptr;
        uploadMotorMask = 0;

        String name = sv->arg("name");
        if (!isValidSoundUploadName(name)) { uploadFailed = true; uploadErrCode = "bad_name"; return; }

        XT_Wav_Class* target = soundObjectFor(name);
        if (target) target->UnLoadWavFile();
        uint32_t motorMask = releaseIdleSoundHandles(target);

        if (!sdMutexTake(3000)) {
            uploadFailed = true; uploadErrCode = "sd_busy";
            logSdUploadFail(name.c_str(), "sd_busy", motorMask, 0);
            if (target) target->LoadWavFile();
            restoreSoundHandles(motorMask);
            return;
        }
        uploadMutexHeld = true;
        uploadTargetSound = target;
        uploadMotorMask   = motorMask;

        SD.remove("/" + name);
        if (SD.exists("/" + name)) {
            // Datei schreibgeschuetzt (FAT-Attribut, oft von Windows); am PC: attrib -r X:\*.wav
            uploadFailed = true; uploadErrCode = "sd_locked";
            logSdUploadFail(name.c_str(), "sd_locked (Datei schreibgeschuetzt?)", motorMask, 0);
            sdMutexGive(); uploadMutexHeld = false;
            uploadTargetSound = nullptr; uploadMotorMask = 0;
            if (target) target->LoadWavFile();
            restoreSoundHandles(motorMask);
            return;
        }
        uploadFile = SD.open("/" + name, FILE_WRITE);
        if (!uploadFile) {
            uploadFailed = true; uploadErrCode = "sd_write";
            logSdUploadFail(name.c_str(), "sd_write (SD.open fehlgeschlagen)", motorMask, 0);
            sdMutexGive(); uploadMutexHeld = false;
            uploadTargetSound = nullptr; uploadMotorMask = 0;
            restoreSoundHandles(motorMask);
            return;
        }
        Serial.printf("[SD-Upload] Start: /%s\n", name.c_str());

    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (uploadFailed || !uploadFile) return;
        size_t written = uploadFile.write(upload.buf, upload.currentSize);
        uploadBytes += written;
        if (written != upload.currentSize) {
            uploadFailed = true; uploadErrCode = "sd_write";   // z. B. Karte voll
            logSdUploadFail(sv->arg("name").c_str(), "sd_write (Teilschreibvorgang)", uploadMotorMask, uploadBytes);
        }

    } else if (upload.status == UPLOAD_FILE_END || upload.status == UPLOAD_FILE_ABORTED) {
        if (uploadFile) uploadFile.close();
        if (upload.status == UPLOAD_FILE_ABORTED) { uploadFailed = true; uploadErrCode = "aborted"; }
        if (uploadFailed) {
            String name = sv->arg("name");
            if (isValidSoundUploadName(name)) SD.remove("/" + name);
        } else {
            Serial.printf("[SD-Upload] Fertig: %u Bytes\n", (unsigned)uploadBytes);
        }
        XT_Wav_Class* target = uploadTargetSound;
        uint32_t motorMask   = uploadMotorMask;
        uploadTargetSound = nullptr;
        uploadMotorMask   = 0;
        if (uploadMutexHeld) { sdMutexGive(); uploadMutexHeld = false; }
        if (target) target->LoadWavFile();
        restoreSoundHandles(motorMask);
        markSdActivity();
    }
}

static void handleSoundUpload() {
    String name = sv->arg("name");
    if (!isValidSoundUploadName(name)) { sv->send(400, "application/json", "{\"ok\":false,\"error\":\"bad_name\"}"); return; }
    if (uploadFailed) { sv->send(200, "application/json", "{\"ok\":false,\"error\":\"" + uploadErrCode + "\"}"); return; }
    sendJson("{\"ok\":true,\"file\":\"" + name + "\",\"size\":" + String(uploadBytes) + "}");
}

static bool sdFileInfo(const String& path, uint32_t& outSize) {
    File f = SD.open(path, FILE_READ);
    if (!f) return false;
    outSize = f.size();
    f.close();
    return true;
}

static void handleSoundFiles() {
    static const char* const fixedNames[3] = { "loop.wav", "shut.wav", "start.wav" };
    bool haveSd = sdMutexTake(3000);
    String j = "{\"ok\":true,\"files\":{";
    for (int i = 0; i < 3 + 24; i++) {
        String name = (i < 3) ? String(fixedNames[i]) : ("sound" + String(i - 2) + ".wav");
        uint32_t size = 0;
        bool exists = haveSd && sdFileInfo("/" + name, size);
        if (i) j += ",";
        j += "\"" + name + "\":{\"exists\":" + String(exists ? "true" : "false") + ",\"size\":" + String(size) + "}";
    }
    j += "}}";
    if (haveSd) sdMutexGive();
    sendJson(j);
}

static void handleSoundDelete() {
    String name = sv->arg("name");
    if (!isValidSoundUploadName(name)) { sv->send(400, "application/json", "{\"ok\":false,\"error\":\"bad_name\"}"); return; }
    XT_Wav_Class* target = soundObjectFor(name);
    if (target) target->UnLoadWavFile();
    uint32_t motorMask = releaseIdleSoundHandles(target);
    if (!sdMutexTake(3000)) {
        sv->send(503, "application/json", "{\"ok\":false,\"error\":\"sd_busy\"}");
        if (target) target->LoadWavFile();
        restoreSoundHandles(motorMask);
        return;
    }
    bool existed = SD.exists("/" + name);
    bool ok = !existed || SD.remove("/" + name);
    sdMutexGive();
    if (target) target->LoadWavFile();
    restoreSoundHandles(motorMask);
    markSdActivity();
    if (ok) sendJson("{\"ok\":true,\"file\":\"" + name + "\",\"existed\":" + String(existed ? "true" : "false") + "}");
    else    sv->send(500, "application/json", "{\"ok\":false,\"error\":\"sd_delete\"}");
}

// ── POST /api/wifi_enable {"on":true/false} (Sound-Seite, wie /api/wifi) ──
static void handleWifiEnable() {
    bool on = false;
    if (sv->hasArg("plain")) {
        const String& body = sv->arg("plain");
        on = body.indexOf("\"on\":true") >= 0 || body.indexOf("\"on\": true") >= 0 || body.indexOf("\"on\":1") >= 0;
    }
    if (on) {
        webui_enableAP();
        sendJson("{\"ok\":true,\"active\":true}");
    } else {
        sendJson("{\"ok\":true,\"active\":false}");
        sv->client().flush();
        delay(200);
        webui_disableAP();
    }
}

// ── GET /api/debug: Kanaele, WAV-Status, S.Port, GPS, CRSF, WLAN ────────
static void handleDebug() {
    String j = "{";
    j += "\"channels\":[";
    for (int i = 0; i < 16; i++) { if (i) j += ","; j += String(channel_output[i]); }
    j += "],";
    // Keine PWM-Eingaenge auf dieser Platine (Feld bleibt fuer die Seite)
    j += "\"pwm\":[";
    for (int i = 0; i < 6; i++) { if (i) j += ","; j += "{\"us\":0,\"pct\":0}"; }
    j += "],";
    j += "\"wav\":{";
    j += "\"loop\":"  + String(Sound_loop.FileOK)  + ",";
    j += "\"shut\":"  + String(Sound_shut.FileOK)  + ",";
    j += "\"start\":" + String(Sound_start.FileOK) + ",";
    j += "\"s\":[";
    for (int i = 1; i < SND_NUM_SLOTS; i++) {
        if (i > 1) j += ",";
        j += String(Sounds[i] ? Sounds[i]->FileOK : 0);
    }
    j += "]},";
    j += "\"hw_config\":2,";
    j += "\"portb_role\":" + String(g_ports[0].activeRole) + ",";
    j += "\"sport\":{";
    j += "\"total_v\":"  + String(sportGetTotalVoltage(), 2)      + ",";
    j += "\"min_cell\":" + String(sportGetMinCell(), 3)           + ",";
    j += "\"cells\":"    + String(sportGetTotalCells())           + ",";
    j += "\"soc\":"      + String(sportCalcSoC(sportGetMinCell()))+ ",";
    j += "\"raw_bytes\":"    + String(sportGetRawBytesRx())      + ",";
    j += "\"polls_sent\":"   + String(sportGetPollsSent())       + ",";
    j += "\"valid_frames\":" + String(sportGetValidFrames())     + ",";
    j += "\"crc_errors\":"   + String(sportGetCrcErrors())       + ",";
    j += "\"no_response\":"  + String(sportGetNoResponseCount()) + ",";
    char lastPollHex[5];
    snprintf(lastPollHex, sizeof(lastPollHex), "0x%02X", sportGetLastPollId());
    j += "\"last_poll_id\":\"" + String(lastPollHex) + "\",";
    j += "\"packs\":[";
    for (uint8_t i = 0; i < 2; i++) {
        if (i) j += ",";
        j += "{\"online\":"  + String(lipoSensor[i].online ? "true" : "false") + ",";
        j += "\"cells\":"    + String(lipoSensor[i].cellCount)      + ",";
        j += "\"total\":"    + String(lipoSensor[i].totalVoltage, 2)+ ",";
        j += "\"min\":"      + String(lipoSensor[i].minCell, 3)     + ",";
        j += "\"cv\":[";
        for (uint8_t c = 0; c < 6; c++) {
            if (c) j += ",";
            j += String(lipoSensor[i].cellVoltage[c], 3);
        }
        j += "]}";
    }
    j += "]}";
    j += ",\"gps\":{";
    j += "\"active\":" + String(gpsIsActive() ? 1 : 0) + ",";
    j += "\"speed\":"  + String(gpsGetSpeedKmh(), 1) + ",";
    j += "\"sats\":"   + String(gpsGetSatellites())  + ",";
    j += "\"fix\":"    + String(gpsHasFix() ? 1 : 0)  + ",";
    j += "\"baud\":"   + String(gpsGetBaud()) + ",";
    j += "\"auto\":"   + String(gpsBaudAuto() ? 1 : 0) + ",";
    j += "\"locked\":" + String(gpsBaudLocked() ? 1 : 0) + ",";
    j += "\"pinConflict\":0";
    j += "}";
    j += ",\"crsf_diag\":{";
    j += "\"raw_bytes\":"     + String(crsf.getRawBytesRx())  + ",";
    j += "\"valid_frames\":"  + String(crsf.getValidFrames()) + ",";
    j += "\"crc_errors\":"    + String(crsf.getCrcErrors())   + ",";
    j += "\"device_pings\":"  + String(crsf.getDevicePings()) + ",";
    j += "\"param_reads\":"   + String(crsf.getParamReads())  + ",";
    j += "\"param_writes\":"  + String(crsf.getParamWrites());
    j += "}";
    j += ",\"wifi\":{";
    j += "\"active\":" + String(webui_isApActive() ? "true" : "false") + ",";
    j += "\"bus_ok\":" + String(BUS_OK ? "true" : "false");
    j += "}";
    j += "}";
    sendJson(j);
}

// ── POST /api/sound ───────────────────────────────────────────────────────
// Test:    {"id":1,"test":true}
// Werte:   {"id":1,"source":70,"vol":100,"mode":0,"name":"Hupe"}
// Motor:   {"id":0,"speed_src":1,"motor_mode":0,"rpm_min":100,...}
static void handleSound() {
    if (!sv->hasArg("plain")) { sendErr(); return; }
    const String& body = sv->arg("plain");
    int id = -1;
    if (!webJsonInt(body, "id", id) || id < 0 || id > SND_NUM_SLOTS - 1) { sendErr(); return; }
    bool changed = false;
    int  v;
    if (body.indexOf("\"test\":true") >= 0) Sound_on_web[id] = true;
    if (webJsonInt(body, "source", v)) {
        if (!isValidEinSource(v)) {
            Serial.printf("[API] Sound %d: ungueltiger Quellwert %d -> Deaktiviert (999)\n", id, v);
            v = 999;
        }
        scfg.Source_Start_Sound[id] = v; changed = true;
    }
    if (webJsonInt(body, "vol",  v)) { scfg.Volumen_Sound[id] = constrain(v,0,200); changed = true; }
    if (webJsonInt(body, "mode", v)) { scfg.Mode_Sound[id]    = constrain(v,0,2);   changed = true; }
    String nm;
    if (id > 0 && webJsonString(body, "name", nm)) {
        nm.trim();
        if (nm.length() == 0) nm = "Sound " + String(id);
        strncpy(scfg.Sound_Name[id], nm.c_str(), SND_NAME_LEN - 1);
        scfg.Sound_Name[id][SND_NAME_LEN - 1] = '\0';
        changed = true;
    }
    if (id == 0) {
        if (webJsonInt(body, "speed_src",  v)) { scfg.Source_Speed_Sound_0 = v;                     changed = true; }
        if (webJsonInt(body, "motor_mode", v)) { scfg.throttle_mode        = constrain(v,0,1);     changed = true; }
        if (webJsonInt(body, "toggle",     v)) { scfg.engine_on_toggle     = constrain(v,0,1);     changed = true; }
        if (webJsonInt(body, "rpm_min",    v)) { scfg.Min_Speed_Sound_0    = constrain(v,0,200);   changed = true; }
        if (webJsonInt(body, "rpm_max",    v)) { scfg.Max_Speed_Sound_0    = constrain(v,100,600); changed = true; }
        if (webJsonInt(body, "shutdown_s", v)) { scfg.shutdowndelay        = constrain(v,0,60);    changed = true; }
        if (webJsonInt(body, "ramp",       v)) { scfg.throttle_ramp        = constrain(v,0,50);    changed = true; }
        if (webJsonInt(body, "deadband",   v)) { scfg.throttle_dead_band   = constrain(v,0,50);    changed = true; }
    }
    if (changed) soundMarkDirty();
    sendOk();
}

void soundWebRegister(WebServer& server) {
    sv = &server;
    server.on("/sport",          HTTP_GET,  handleSport);
    server.on("/api/sconfig",    HTTP_GET,  handleConfigGet);
    server.on("/api/sconfig",    HTTP_POST, handleConfigPost);
    server.on("/api/debug",      HTTP_GET,  handleDebug);
    server.on("/api/sound",      HTTP_POST, handleSound);
    server.on("/api/sdexport",   HTTP_POST, handleSdExport);
    server.on("/api/sdlist",     HTTP_GET,  handleSdList);
    server.on("/api/sdconfig",   HTTP_GET,  handleSdConfig);
    server.on("/api/sddelete",   HTTP_POST, handleSdDelete);
    server.on("/api/soundupload", HTTP_POST, handleSoundUpload, handleSoundUploadData);
    server.on("/api/soundfiles", HTTP_GET,  handleSoundFiles);
    server.on("/api/sounddelete", HTTP_POST, handleSoundDelete);
    server.on("/api/wifi_enable", HTTP_POST, handleWifiEnable);
    // Ende eines Sound-Imports: nur als geaendert markieren, gespeichert wird
    // entprellt in soundSavePending()
    server.on("/save", []() { soundMarkDirty(); sendOk(); });
}
