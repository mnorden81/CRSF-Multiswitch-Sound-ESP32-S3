/*
 * sound.cpp  -  Soundteil des RC-Multiswitch-Sound (ESP32-S3)
 *
 * Herkunft: ESP32-Sound-Basis_Ziege-One v7.21
 *   - Einstellungen (Laden/Speichern/Werkseinstellung) aus config.cpp
 *   - Wiedergabe (handleSound, Motor-Zustandsmaschine, Config(), Rampe,
 *     Einkanal) und Lua-Felder aus ESP32-RC-Sound.ino
 * Abweichungen siehe sound.h. Die Ablauflogik ist unveraendert.
 */
#include "sound.h"
#include <Preferences.h>
#include <SPI.h>
#include "XT_I2S_Audio.h"
#include "board_config.h"

// ── Gemeinsame Werte aus dem Schaltteil (RC_Multiswitch_Sound.ino) ──────
extern uint16_t channel_output[16];
extern bool     BUS_OK;
extern int      RC_System;        // 0=FrSky,1=FlySky,2=ELRS SBUS,3=Hott,4=CRSF
extern uint16_t einkanal_Flat16;  // MKan-Gruppen 1/2 (gemeinsam), Quelle 80-95

SoundConfig   scfg;
static bool          sndDirty   = false;
static unsigned long sndDirtyMs = 0;

static constexpr const char* NVS_NS = "snd";

void soundMarkDirty() { sndDirty = true; sndDirtyMs = millis(); }

// ======== Einstellungen ==============================================
static void defaultName(int i, char* out) {
    if (i == 0) strcpy(out, "Motor");
    else        snprintf(out, SND_NAME_LEN, "Sound %d", i);
}

void soundResetConfig() {
    for (int i = 0; i < SND_NUM_SLOTS; i++) {
        scfg.Source_Start_Sound[i] = 999;
        scfg.Volumen_Sound[i]      = 100;
        scfg.Mode_Sound[i]         = 0;
        defaultName(i, scfg.Sound_Name[i]);
    }
    scfg.Source_Speed_Sound_0 = 999;
    scfg.throttle_mode        = 0;
    scfg.Min_Speed_Sound_0    = 100;
    scfg.Max_Speed_Sound_0    = 300;
    scfg.shutdowndelay        = 0;
    scfg.engine_on_toggle     = 0;
    scfg.throttle_ramp        = 20;
    scfg.throttle_dead_band   = 10;
    scfg.Einkanal_Channel     = 999;
    scfg.Einkanal_mode        = 0;
    scfg.modul_adress         = 0;
    scfg.sport_poll_id[0]     = 0xA1;   // Physical ID 0x02 (Werkseinstellung Sensor 1)
    scfg.sport_poll_id[1]     = 0x22;   // Physical ID 0x03 (Sensor 2 umprogrammiert)
    soundMarkDirty();
}

void soundLoadConfig() {
    soundResetConfig();
    sndDirty = false;
    Preferences p; p.begin(NVS_NS, true);
    if (!p.isKey("sssArr")) { p.end(); sndDirty = true; return; }
    // Die drei Sound-Arrays als je ein Blob (wie Sound-Basis v7.x)
    if (p.getBytesLength("sssArr") == sizeof(scfg.Source_Start_Sound)) p.getBytes("sssArr", scfg.Source_Start_Sound, sizeof(scfg.Source_Start_Sound));
    if (p.getBytesLength("vsArr")  == sizeof(scfg.Volumen_Sound))      p.getBytes("vsArr",  scfg.Volumen_Sound,      sizeof(scfg.Volumen_Sound));
    if (p.getBytesLength("msArr")  == sizeof(scfg.Mode_Sound))         p.getBytes("msArr",  scfg.Mode_Sound,         sizeof(scfg.Mode_Sound));
    if (p.getBytesLength("names")  == sizeof(scfg.Sound_Name))         p.getBytes("names",  scfg.Sound_Name,         sizeof(scfg.Sound_Name));
    scfg.Source_Speed_Sound_0 = p.getInt("spd",  999);
    scfg.throttle_mode        = p.getInt("thrm", 0);
    scfg.Min_Speed_Sound_0    = p.getInt("minsp",100);
    scfg.Max_Speed_Sound_0    = p.getInt("maxsp",300);
    scfg.shutdowndelay        = p.getInt("shdly",0);
    scfg.engine_on_toggle     = p.getInt("entog",0);
    scfg.throttle_ramp        = p.getInt("thrr", 20);
    scfg.throttle_dead_band   = p.getInt("thrdb",10);
    scfg.Einkanal_Channel     = p.getInt("ekch", 999);
    scfg.Einkanal_mode        = p.getInt("ekmo", 0);
    scfg.modul_adress         = p.getInt("sadr", 0);
    scfg.sport_poll_id[0]     = (uint8_t)p.getInt("spid0", 0xA1);
    scfg.sport_poll_id[1]     = (uint8_t)p.getInt("spid1", 0x22);
    p.end();
    for (int i = 0; i < SND_NUM_SLOTS; i++) {
        scfg.Sound_Name[i][SND_NAME_LEN - 1] = '\0';
        if (!scfg.Sound_Name[i][0]) defaultName(i, scfg.Sound_Name[i]);
        if (!isValidEinSource(scfg.Source_Start_Sound[i])) {
            Serial.printf("Sound %d: ungueltiger Quellwert %d -> Deaktiviert (999)\n", i, scfg.Source_Start_Sound[i]);
            scfg.Source_Start_Sound[i] = 999;
            sndDirty = true;
        }
    }
    defaultName(0, scfg.Sound_Name[0]);   // Motor behaelt seinen festen Namen
    Serial.println("Sound: Einstellungen aus NVS geladen.");
}

void soundSaveNow() {
    Preferences p; p.begin(NVS_NS, false);
    p.putBytes("sssArr", scfg.Source_Start_Sound, sizeof(scfg.Source_Start_Sound));
    p.putBytes("vsArr",  scfg.Volumen_Sound,       sizeof(scfg.Volumen_Sound));
    p.putBytes("msArr",  scfg.Mode_Sound,          sizeof(scfg.Mode_Sound));
    p.putBytes("names",  scfg.Sound_Name,          sizeof(scfg.Sound_Name));
    p.putInt("spd",  scfg.Source_Speed_Sound_0);
    p.putInt("thrm", scfg.throttle_mode);
    p.putInt("minsp",scfg.Min_Speed_Sound_0);
    p.putInt("maxsp",scfg.Max_Speed_Sound_0);
    p.putInt("shdly",scfg.shutdowndelay);
    p.putInt("entog",scfg.engine_on_toggle);
    p.putInt("thrr", scfg.throttle_ramp);
    p.putInt("thrdb",scfg.throttle_dead_band);
    p.putInt("ekch", scfg.Einkanal_Channel);
    p.putInt("ekmo", scfg.Einkanal_mode);
    p.putInt("sadr", scfg.modul_adress);
    p.putInt("spid0",(int)scfg.sport_poll_id[0]);
    p.putInt("spid1",(int)scfg.sport_poll_id[1]);
    p.end();
    sndDirty = false;
    Serial.println("Sound: Einstellungen in NVS gespeichert.");
}

// 500 ms nach der letzten Aenderung schreiben, aber nicht direkt nach einer
// SD-Schreibaktion (Brownout-Schutz aus Sound-Basis v7.16, siehe
// soundSdCooldownActive() in sound_web.cpp).
void soundSavePending() {
    if (sndDirty && (millis() - sndDirtyMs) >= 500UL && !soundSdCooldownActive())
        soundSaveNow();
}

// ======== Zustand ====================================================
bool Sound_on[SND_NUM_SLOTS]     = {};
bool Sound_play[SND_NUM_SLOTS]   = {};
bool Sound_on_web[SND_NUM_SLOTS] = {};
static bool testSoundActive[SND_NUM_SLOTS] = {};

static bool          Sound_on_Motor       = false;
static bool          Sound_on_Motor_state = false;
static bool          engine_break         = false;
static unsigned long shutdown_timer       = 0;

static constexpr uint8_t ENG_OFF = 0, ENG_STARTING = 1, ENG_RUNNING = 2, ENG_STOPPING = 3;
static volatile uint8_t engine_State = ENG_OFF;

static uint16_t snd_einkanal_Data       = 0;   // 1 Bit/Schalter - Quelle "EK" 70-77
static uint16_t snd_einkanal_SpeicherWM = 0;

static unsigned long previousTime_ramp = 0;
static float last_throttle = 0.0f;
static float throttle      = 0.0f;
static unsigned long currentTime = 0;

uint16_t soundEinkanalData() { return snd_einkanal_Data; }

// ======== I2S Audio ==================================================
// PCM5102A an BCLK/LRCK/DOUT laut board_config.h (Klassenreihenfolge LRCK, BCLK, DOUT)
XT_I2S_Class I2SAudio(I2S_LRC, I2S_BCLK, I2S_DOUT, I2S_NUM_0);

XT_Wav_Class Sound_loop ("/loop.wav");
XT_Wav_Class Sound_shut ("/shut.wav");
XT_Wav_Class Sound_start("/start.wav");
XT_Wav_Class Sound1("/sound1.wav");   XT_Wav_Class Sound2("/sound2.wav");
XT_Wav_Class Sound3("/sound3.wav");   XT_Wav_Class Sound4("/sound4.wav");
XT_Wav_Class Sound5("/sound5.wav");   XT_Wav_Class Sound6("/sound6.wav");
XT_Wav_Class Sound7("/sound7.wav");   XT_Wav_Class Sound8("/sound8.wav");
XT_Wav_Class Sound9 ("/sound9.wav");  XT_Wav_Class Sound10("/sound10.wav");
XT_Wav_Class Sound11("/sound11.wav"); XT_Wav_Class Sound12("/sound12.wav");
XT_Wav_Class Sound13("/sound13.wav"); XT_Wav_Class Sound14("/sound14.wav");
XT_Wav_Class Sound15("/sound15.wav"); XT_Wav_Class Sound16("/sound16.wav");
XT_Wav_Class Sound17("/sound17.wav"); XT_Wav_Class Sound18("/sound18.wav");
XT_Wav_Class Sound19("/sound19.wav"); XT_Wav_Class Sound20("/sound20.wav");
XT_Wav_Class Sound21("/sound21.wav"); XT_Wav_Class Sound22("/sound22.wav");
XT_Wav_Class Sound23("/sound23.wav"); XT_Wav_Class Sound24("/sound24.wav");

XT_Wav_Class* Sounds[SND_NUM_SLOTS] = {
    nullptr,
    &Sound1,&Sound2,&Sound3,&Sound4,&Sound5,&Sound6,&Sound7,&Sound8,
    &Sound9,&Sound10,&Sound11,&Sound12,&Sound13,&Sound14,&Sound15,&Sound16,
    &Sound17,&Sound18,&Sound19,&Sound20,&Sound21,&Sound22,&Sound23,&Sound24
};

// ======== SD-Karte ===================================================
// WICHTIG - max_files NICHT erhoehen (Erfahrung Sound-Basis v7.07..v7.10):
// max_files=15 belegt beim Mounten dauerhaft DRAM, bevor der WLAN-Treiber
// seine Puffer anfordert, und hat den WLAN-Start zerstoert. Es bleibt beim
// Standard 5; Upload/Loeschen geben dafuer ungenutzte Handles frei
// (releaseIdleSoundHandles() in sound_web.cpp).
static void SDCardInit() {
    pinMode(SD_CS, OUTPUT); digitalWrite(SD_CS, HIGH);
    SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, SPI)) Serial.println("FEHLER: SD!");
}
static void LoadFiles() { Sound_loop.LoadWavFile(); Sound_shut.LoadWavFile(); Sound_start.LoadWavFile(); }

// Sample-Puffer aller Sound-Objekte erst NACH dem WLAN-Start vom Heap holen
// (siehe XT_I2S_Audio.h), damit WiFi.softAP() auf einem freien Heap laeuft.
static void AllocateSoundBuffers() {
    Sound_loop.AllocateBuffers();
    Sound_shut.AllocateBuffers();
    Sound_start.AllocateBuffers();
    for (int i = 1; i < SND_NUM_SLOTS; i++) if (Sounds[i]) Sounds[i]->AllocateBuffers();
}

void soundSetupEarly() {
    SDCardInit();
    LoadFiles();
}

void soundSetupLate() {
    // SD-Lese-Task und Puffer bewusst erst nach dem WLAN-AP-Start (Sound-Basis)
    I2SAudio.StartSdReaderTask();
    AllocateSoundBuffers();
}

// ======== Typ-Mapping Quelle Motor/Sound EIN =========================
// Typ: 0=BUS-L(0-15) 1=BUS-H(20-35) 2=PWM-L(40-45) 3=PWM-H(50-55)
//      4=Pin(60-65) 5=Einkanal(70-77) 6=MKan(80-95) 7=Dauerbetrieb(200)
//      8=Deaktiviert(999)
// Diese Platine hat wie Hardware V3 der Sound-Basis keine PWM-/Pin-
// Eingaenge: Typ 2-4 sind gueltige Werte (alte Sicherungen), schalten aber nie.
static uint8_t einTyp(int c) {
    if(c==999)return 8; if(c==200)return 7;
    if(c>=80&&c<=95)return 6; if(c>=70&&c<=77)return 5;
    if(c>=60&&c<=65)return 4; if(c>=50&&c<=55)return 3;
    if(c>=40&&c<=45)return 2; if(c>=20&&c<=35)return 1;
    if(c>=0&&c<=15)return 0; return 8;
}
bool isValidEinSource(int c) {
    return c==999 || c==200 ||
           (c>=80&&c<=95) || (c>=70&&c<=77) || (c>=60&&c<=65) ||
           (c>=50&&c<=55) || (c>=40&&c<=45) || (c>=20&&c<=35) ||
           (c>=0&&c<=15);
}
static uint8_t einNr(int c) {
    if(c>=80&&c<=95)return c-80+1; if(c>=70&&c<=77)return c-70+1;
    if(c>=60&&c<=65)return c-60+1; if(c>=50&&c<=55)return c-50+1;
    if(c>=40&&c<=45)return c-40+1; if(c>=20&&c<=35)return c-20+1;
    if(c>=0&&c<=15)return c+1; return 1;
}
static int setEin(uint8_t t, uint8_t nr) {
    uint8_t i=(nr>0)?nr-1:0;
    if(t==0)return min(i,(uint8_t)15);
    if(t==1)return 20+min(i,(uint8_t)15);
    if(t==5)return 70+min(i,(uint8_t)7);
    if(t==6)return 80+min(i,(uint8_t)15);
    if(t==7)return 200; return 999;
}
static uint8_t einMaxNr(uint8_t t) {
    if(t==0||t==1)return 16; if(t==5)return 8; if(t==6)return 16; return 1;
}

// ── Typ-Mapping Quelle Speed: 0=BUS Kanal(0-15) 2=Deaktiviert(999) ─────
static uint8_t spdTyp(int c) { if(c>=0&&c<=15)return 0; if(c>=20&&c<=25)return 1; return 2; }
static uint8_t spdNr(int c)  { if(c>=0&&c<=15)return c+1; if(c>=20&&c<=25)return c-20+1; return 1; }
static int setSpd(uint8_t t, uint8_t nr) { uint8_t i=(nr>0)?nr-1:0; if(t==0)return min(i,(uint8_t)15); return 999; }

// ======== Lua-Menue (Felder wie Sound-Basis v7.21, Hardware V3) ======
static const char* EIN_SEL = "L;H;EK;MKan;Dauer;Aus";
static uint8_t einSelVal(int cfg) {
    uint8_t t = einTyp(cfg);
    // 0=L,1=H,5->2=EK,6->3=MKan,7->4=Dauer,8->5=Aus; PWM/Pin -> Aus
    if (t == 2 || t == 3 || t == 4) return 5;
    if (t==5) return 2; if (t==6) return 3; if (t==7) return 4; if (t>=8) return 5;
    return t;
}
static int einSelWrite(uint8_t val, int oldCfg) {
    static const uint8_t v3map[] = {0,1,5,6,7,8};
    uint8_t t = (val < 6) ? v3map[val] : 8;
    return setEin(t, einNr(oldCfg));
}

static void luaShowSoundFolder(uint8_t s, LuaView& v, char* buf) {
    snprintf(buf, LUA_VIEW_BUF, "Sound %d", s);
    v.name = buf;
}

static uint8_t luaGetThrottleMode(uint8_t)          { return (uint8_t)constrain(scfg.throttle_mode, 0, 1); }
static void    luaSetThrottleMode(uint8_t, uint8_t v){ scfg.throttle_mode = v; soundMarkDirty(); }
static uint8_t luaGetToggle(uint8_t)                { return (uint8_t)constrain(scfg.engine_on_toggle, 0, 1); }
static void    luaSetToggle(uint8_t, uint8_t v)     { scfg.engine_on_toggle = v; soundMarkDirty(); }
static uint8_t luaGetMinSpeed(uint8_t)              { return (uint8_t)constrain(scfg.Min_Speed_Sound_0, 0, 200); }
static void    luaSetMinSpeed(uint8_t, uint8_t v)   { scfg.Min_Speed_Sound_0 = v; soundMarkDirty(); }
static uint8_t luaGetMaxSpeed(uint8_t)              { return (uint8_t)constrain(scfg.Max_Speed_Sound_0 / 2, 50, 255); }
static void    luaSetMaxSpeed(uint8_t, uint8_t v)   { scfg.Max_Speed_Sound_0 = constrain((int)v * 2, 100, 510); soundMarkDirty(); }
static uint8_t luaGetShutdown(uint8_t)              { return (uint8_t)constrain(scfg.shutdowndelay, 0, 60); }
static void    luaSetShutdown(uint8_t, uint8_t v)   { scfg.shutdowndelay = v; soundMarkDirty(); }
static uint8_t luaGetRamp(uint8_t)                  { return (uint8_t)constrain(scfg.throttle_ramp, 0, 50); }
static void    luaSetRamp(uint8_t, uint8_t v)       { scfg.throttle_ramp = v; soundMarkDirty(); }
static uint8_t luaGetDeadBand(uint8_t)              { return (uint8_t)constrain(scfg.throttle_dead_band, 0, 50); }
static void    luaSetDeadBand(uint8_t, uint8_t v)   { scfg.throttle_dead_band = v; soundMarkDirty(); }

// Quelle Speed: 0=BUS, 1=Deaktiviert
static uint8_t luaGetSpdSrc(uint8_t)                { return spdTyp(scfg.Source_Speed_Sound_0) == 0 ? 0 : 1; }
static void    luaSetSpdSrc(uint8_t, uint8_t v) {
    uint8_t nr = spdNr(scfg.Source_Speed_Sound_0);
    scfg.Source_Speed_Sound_0 = setSpd(v == 0 ? 0 : 2, nr); soundMarkDirty();
}
static void luaShowSpdChannel(uint8_t, LuaView& v, char*) {
    if (spdTyp(scfg.Source_Speed_Sound_0) != 0) { v.type = LUA_INFO; v.name = "Kanal Speed"; v.text = "Deaktiviert"; }
}
static uint8_t luaGetSpdChannel(uint8_t)            { return spdNr(scfg.Source_Speed_Sound_0); }
static void    luaSetSpdChannel(uint8_t, uint8_t v) { scfg.Source_Speed_Sound_0 = setSpd(spdTyp(scfg.Source_Speed_Sound_0), v); soundMarkDirty(); }

// Motor (arg 0) und Sound 1-24 (arg = Sound-Nr.)
static uint8_t luaGetSource(uint8_t s)              { return einSelVal(scfg.Source_Start_Sound[s]); }
static void    luaSetSource(uint8_t s, uint8_t v)   { scfg.Source_Start_Sound[s] = einSelWrite(v, scfg.Source_Start_Sound[s]); soundMarkDirty(); }
static void luaShowChannel(uint8_t s, LuaView& v, char*) {
    uint8_t t = einTyp(scfg.Source_Start_Sound[s]);
    bool noPin = (t == 2 || t == 3 || t == 4);
    if (t == 7 || t == 8 || noPin) {
        v.type = LUA_INFO;
        if (s == 0) { v.name = "Kanal EIN"; v.text = (t == 7 || noPin) ? "Dauerbetrieb/Deakt." : "Deaktiviert"; }
        else        { v.text = (t == 7) ? "Dauerbetrieb" : "Deaktiviert"; }
    } else {
        v.max = einMaxNr(t);
    }
}
static uint8_t luaGetChannel(uint8_t s)             { return einNr(scfg.Source_Start_Sound[s]); }
static void    luaSetChannel(uint8_t s, uint8_t v)  { scfg.Source_Start_Sound[s] = setEin(einTyp(scfg.Source_Start_Sound[s]), v); soundMarkDirty(); }
static uint8_t luaGetVolume(uint8_t s)              { return (uint8_t)constrain(scfg.Volumen_Sound[s], 0, 200); }
static void    luaSetVolume(uint8_t s, uint8_t v)   { scfg.Volumen_Sound[s] = v; soundMarkDirty(); }
static uint8_t luaGetPlayMode(uint8_t s)            { return (uint8_t)constrain(scfg.Mode_Sound[s], 0, 2); }
static void    luaSetPlayMode(uint8_t s, uint8_t v) { scfg.Mode_Sound[s] = v; soundMarkDirty(); }
// "Test Sound" wirkt sofort und wird nicht gespeichert
static uint8_t luaGetTest(uint8_t s)                { return testSoundActive[s] ? 1 : 0; }
static void    luaSetTest(uint8_t s, uint8_t v)     { if (v == 1) Sound_on_web[s] = true; testSoundActive[s] = (v == 1); }

// Einstellungen des Soundteils (Ordner "Einstellungen", gebaut im Schaltteil)
uint8_t sndLuaGetAddr(uint8_t)                { return (uint8_t)constrain(scfg.modul_adress, 0, 20); }
void    sndLuaSetAddr(uint8_t, uint8_t v)     { scfg.modul_adress = v; soundMarkDirty(); }
uint8_t sndLuaGetEkChannel(uint8_t)           { int c = scfg.Einkanal_Channel; return c == 999 ? 255 : (uint8_t)constrain(c, 0, 15); }
void    sndLuaSetEkChannel(uint8_t, uint8_t v){ scfg.Einkanal_Channel = (v == 255) ? 999 : constrain(v, 0, 15); soundMarkDirty(); }
uint8_t sndLuaGetEkMode(uint8_t)              { int m = scfg.Einkanal_mode; return (m >= 10 && m <= 13) ? (uint8_t)(m - 9) : 0; }
void    sndLuaSetEkMode(uint8_t, uint8_t v)   { scfg.Einkanal_mode = (v == 0) ? 0 : constrain((int)v + 9, 10, 13); soundMarkDirty(); }

void soundLuaMotor(LuaMenu& m, uint8_t f) {
    m.folder(f, 0, "Motor");
    m.select(f + 1,  f, "Motor Mode", "Eine Richtung;Zwei Richtungen", luaGetThrottleMode, luaSetThrottleMode);
    m.select(f + 2,  f, "Motor EIN Modus", "Normal;Toggle", luaGetToggle, luaSetToggle);
    m.select(f + 3,  f, "Quelle Motor", EIN_SEL, luaGetSource, luaSetSource, 0);
    m.u8(f + 4,      f, "Kanal Nr EIN", 1, 16, "", luaGetChannel, luaSetChannel, 0)->view = luaShowChannel;
    m.select(f + 5,  f, "Quelle Speed", "BUS Kanal;Deaktiviert", luaGetSpdSrc, luaSetSpdSrc);
    m.u8(f + 6,      f, "Kanal Nr Speed", 1, 16, "", luaGetSpdChannel, luaSetSpdChannel)->view = luaShowSpdChannel;
    m.u8(f + 7,      f, "Volumen Motor", 0, 200, "", luaGetVolume, luaSetVolume, 0);
    m.u8(f + 8,      f, "Drehzahl min %", 0, 200, "%", luaGetMinSpeed, luaSetMinSpeed);
    m.u8(f + 9,      f, "Drehzahl max /2", 50, 255, "%", luaGetMaxSpeed, luaSetMaxSpeed);
    m.u8(f + 10,     f, "Motor aus Standgas", 0, 60, "s", luaGetShutdown, luaSetShutdown);
    m.u8(f + 11,     f, "Motor Rampe %/s", 0, 50, "", luaGetRamp, luaSetRamp);
    m.u8(f + 12,     f, "Standgas Totband", 0, 50, "%", luaGetDeadBand, luaSetDeadBand);
}

// Sound s (1..count): Ordner fi = 112 + (s-1)*6, Felder fi+1 .. fi+5
void soundLuaSounds(LuaMenu& m, uint8_t count) {
    if (count > SND_LUA_MAX) count = SND_LUA_MAX;
    for (uint8_t s = 1; s <= count; s++) {
        uint8_t fi = SND_LUA_FIRST_ID + (s - 1) * 6;
        LuaItem* f = m.folder(fi, 0, "Sound");
        if (!f) return;
        f->arg = s; f->view = luaShowSoundFolder;
        m.select(fi + 1, fi, "Quelle", EIN_SEL, luaGetSource, luaSetSource, s);
        m.u8(fi + 2, fi, "Kanal Nr", 1, 16, "", luaGetChannel, luaSetChannel, s)->view = luaShowChannel;
        m.u8(fi + 3, fi, "Volumen", 0, 200, "", luaGetVolume, luaSetVolume, s);
        m.select(fi + 4, fi, "Wiedergabe Mode", "Normal;Loop;Tippbetrieb", luaGetPlayMode, luaSetPlayMode, s);
        m.select(fi + 5, fi, "Test Sound", "Aus;Ein", luaGetTest, luaSetTest, s);
    }
}

// ======== Wiedergabe =================================================
static float floatMap(float x,float a,float b,float c,float d){if(b==a)return c;return(x-a)*(d-c)/(b-a)+c;}

static float ramp_throttle(float tn){
    unsigned long rt=currentTime-previousTime_ramp; previousTime_ramp=currentTime;
    if(rt>1000)rt=1000; float step=(scfg.throttle_ramp/1000.0f)*(float)rt;
    if(tn>last_throttle){float s=last_throttle+step;tn=(s<tn)?s:tn;}
    else{float s=last_throttle-step;tn=(s>tn)?s:tn;}
    last_throttle=tn; return tn;
}

static void handleSound(uint8_t idx, XT_Wav_Class* snd) {
    if(!snd)return;
    if(Sound_on[idx]||Sound_on_web[idx]){
        snd->Volume=scfg.Volumen_Sound[idx];
        if(!Sound_play[idx]){Sound_play[idx]=true;snd->LoadWavFile();if(scfg.Mode_Sound[idx]==1)snd->RepeatForever=true;I2SAudio.Play(snd);Sound_on_web[idx]=false;}
    }else{
        snd->RepeatForever=false;
        // v7.17: Lautstaerke auch waehrend des Nachspielens aktualisieren
        if(Sound_play[idx]) snd->Volume=scfg.Volumen_Sound[idx];
        if(scfg.Mode_Sound[idx]==2&&Sound_play[idx])I2SAudio.Stop(snd);
        if(!snd->Playing&&Sound_play[idx]){snd->UnLoadWavFile();Sound_play[idx]=false;testSoundActive[idx]=false;}
    }
}

// Quellen auswerten (Sound-Basis Config(), Hardware V3: keine PWM-/Pin-Eingaenge)
static void evalSources() {
    for (int x=0; x<SND_NUM_SLOTS; x++) {
        int src = scfg.Source_Start_Sound[x];
        if      (src>=0   && src<=15)  Sound_on[x] = BUS_OK && (channel_output[src] < 624);
        else if (src>=20  && src<=35)  Sound_on[x] = BUS_OK && (channel_output[src-20] > 1424);
        else if (src>=70  && src<=77)  Sound_on[x] = BUS_OK && bitRead(snd_einkanal_Data, src-70);
        // MKan: 16 Einzelkanaele aus den 2 gemeinsamen Gruppen (Schaltteil)
        else if (src>=80  && src<=95)  Sound_on[x] = BUS_OK && bitRead(einkanal_Flat16, src-80);
        else if (src==200)             Sound_on[x] = true;
        else                           Sound_on[x] = false;
    }

    int spd = scfg.Source_Speed_Sound_0;
    if (spd>=0 && spd<=15) throttle = map(channel_output[spd], 82, 1900, 0, 100);
    else                   throttle = 0;
    throttle = constrain(throttle, 0, 100);

    if (scfg.throttle_mode) {
        if      (throttle > 50+scfg.throttle_dead_band) throttle=map((long)throttle,50+scfg.throttle_dead_band,100,0,100);
        else if (throttle < 50-scfg.throttle_dead_band) throttle=map((long)throttle,50-scfg.throttle_dead_band,0,0,100);
        else throttle=0;
    } else {
        throttle=(throttle>scfg.throttle_dead_band)?map((long)throttle,scfg.throttle_dead_band,100,0,100):0;
    }
    throttle = constrain(throttle, 0, 100);
}

void soundLoop() {
    currentTime = millis();
    evalSources();
    if(currentTime<5000) return;   // 5 s Startverzoegerung wie Sound-Basis
    for(uint8_t i=1;i<=24;i++) handleSound(i,Sounds[i]);

    // Motor Toggle
    if(Sound_on[0]&&!Sound_on_Motor_state&&scfg.engine_on_toggle){Sound_on_Motor=!Sound_on_Motor;Sound_on_Motor_state=true;}
    else if(!Sound_on[0]&&Sound_on_Motor_state&&scfg.engine_on_toggle) Sound_on_Motor_state=false;
    else if(!scfg.engine_on_toggle) Sound_on_Motor=Sound_on[0];

    bool mw=Sound_on_Motor||Sound_on_web[0];
    if((mw&&!engine_break&&!Sound_play[0])||(throttle>0&&engine_break&&!Sound_play[0])||(throttle>0&&Sound_play[0])){Sound_play[0]=true;shutdown_timer=millis();}
    if(!mw){Sound_play[0]=false;engine_break=false;}
    if(scfg.shutdowndelay>0&&Sound_play[0]&&(millis()-shutdown_timer)>((unsigned long)scfg.shutdowndelay*1000UL)){Sound_play[0]=false;engine_break=true;}

    switch(engine_State){
        case ENG_OFF:      if(Sound_play[0]){Sound_start.Volume=scfg.Volumen_Sound[0];I2SAudio.Play(&Sound_start);engine_State=ENG_STARTING;} break;
        case ENG_STARTING: if(!Sound_start.Playing){throttle=0;last_throttle=0;Sound_loop.RepeatForever=true;Sound_loop.Volume=scfg.Volumen_Sound[0];I2SAudio.Play(&Sound_loop);engine_State=ENG_RUNNING;} break;
        case ENG_RUNNING:  Sound_loop.Volume=scfg.Volumen_Sound[0]; if(!Sound_play[0]){throttle=0;if(last_throttle==0){Sound_loop.RepeatForever=false;I2SAudio.Stop(&Sound_loop);engine_State=ENG_STOPPING;}} break;
        case ENG_STOPPING: if(!Sound_loop.Playing){Sound_shut.Volume=scfg.Volumen_Sound[0];I2SAudio.Play(&Sound_shut);engine_State=ENG_OFF;} break;
    }
    throttle=ramp_throttle(throttle);
    if(scfg.Min_Speed_Sound_0!=scfg.Max_Speed_Sound_0)
        Sound_loop.Speed=floatMap(throttle,0,100,scfg.Min_Speed_Sound_0,scfg.Max_Speed_Sound_0)/100.0f;
}

// ======== Einkanal ===================================================
// CRSF: aus dem WM-Protokoll (einkanalFunctionCRSF() im Schaltteil), wenn
// die Adresse die Sound-Adresse ist.
void soundOnSwitchBits(uint8_t addr, uint8_t bits) {
    if (addr == (uint8_t)scfg.modul_adress) snd_einkanal_Data = bits;
}

// SBUS: amplituden-codierte WM-Adresse/Schalter/Status aus einem Kanalwert
static void decodeSbusWm(uint16_t ch, uint8_t &addr, uint8_t &sw, uint8_t &st) {
    uint16_t n=0;uint8_t v=0;
    switch(RC_System){
        case 0:n=(ch>=172)?(ch-172+1):0;v=(uint8_t)(n>>4);break;
        case 1:n=(ch>=220)?(ch-220):0;v=(uint8_t)((n+(n>>6))>>4);break;
        case 2:n=(ch>=172)?(ch-172):0;v=(uint8_t)(n>>4);break;
        case 3:n=(ch>=205)?(ch-205):0;v=(uint8_t)(n>>4);break;
    }
    addr=(v>>4)&0b11; sw=(v>>1)&0b111; st=v&0b1;
}
void soundOnSbus(uint16_t ch) {
    snd_einkanal_Data=ch;
    if(scfg.Einkanal_mode==0){snd_einkanal_Data/=8;}
    else if(scfg.Einkanal_mode<=9){
        if(RC_System==0)snd_einkanal_Data/=8;
        else if(RC_System==1){snd_einkanal_Data=constrain(snd_einkanal_Data,206,1837);snd_einkanal_Data=((snd_einkanal_Data-206)*10+20)/64;}
        else if(RC_System==2){float v=((float)snd_einkanal_Data-172.0f+1.5f)*0.155677655677655f;snd_einkanal_Data=(uint16_t)v;}
    }else{
        uint8_t addr,sw,st; decodeSbusWm(ch,addr,sw,st);
        if(addr==(uint8_t)(scfg.Einkanal_mode-10))bitWrite(snd_einkanal_SpeicherWM,sw,st);
        snd_einkanal_Data=snd_einkanal_SpeicherWM;
    }
}
