# CRSF-Multiswitch-Sound-ESP32-S3

CRSF-Multiswitch mit 8 Kanaelen plus Soundmodul auf einem ESP32-S3-DevKitC-1.

Kombiniert den CRSF-Multiswitch mit einem Soundmodul auf einer Platine: 8 Schaltkanaele ueber ULN2803A und Soundausgabe ueber I2S, Sounds von microSD. Ziel ist ein Modul fuer Modelle, die Licht und Sound zusammen brauchen.

Ausfuehrliche Projektbeschreibung: [docs/Projektbeschreibung.docx](docs/Projektbeschreibung.docx)

## Merkmale
- 8 Schaltkanaele ueber ULN2803A, Kontroll-LED je Kanal
- UART A (Eingang), UART B und UART C als JST PH 4
- Audio wahlweise: A) PCM5102A-DAC mit externem TPA3116D2-Verstaerker (aus VLOAD) oder B) MAX98357A mit Gain-Jumper
- microSD-Kartenmodul (6-polig) ueber SPI, Versorgung per Jumper 5 V oder 3,3 V
- Setup-Taster

## Hardware (v0.1 (Entwurf, nur Schaltplan))
Board: ESP32-S3-DevKitC-1 N16R8 (oder Clone)

- Lastspannung 12-24 V (VLOAD) an CN2 (JST PH 2), TVS-Diode P6KE33A als Schutz.
- Treiber ULN2803A, 8 Kanaele, max. 500 mA je Kanal (Verlustleistung des ULN2803A bei mehreren Kanaelen beachten).
- R9-R16 (THT 1/4 W, Standard 150 R) zwischen ULN-Ausgang und Stiftleiste J1, je nach Verbraucher waehlen.
- R1-R8 und LED1-LED8 (3 mm) als Kontroll-LED je Kanal, vom Steuersignal Sn gegen GND.
- Keine Pulldowns und keine Widerstaende in den UART-Leitungen. Der ESP32 wird aus +5 V von CN1 (BEC/Empfaenger) versorgt.

### Belegung
| Funktion | GPIO |
|---|---|
| S0..S7 | IO4, IO5, IO6, IO7, IO15, IO16, IO1, IO2 |
| UART A (CN1, Eingang) | TX IO17, RX IO18 |
| UART B (J5) | TX IO39, RX IO40 |
| UART C (J6) | TX IO41, RX IO42 |
| SD (SPI) | CS IO10, MOSI IO11, SCK IO12, MISO IO13 |
| I2S | BCLK IO14, LRCK IO21, DOUT IO47 |
| Amp-Mute (MAX98357A SD) | IO8 |
| Setup-Taster | IO9 |

### Anschluesse
- CN1, J5, J6: UART A/B/C als JST PH 4 horizontal (S4B-PH-K)
- CN2 Last 12-24 V (JST PH 2), CN3 Versorgung TPA3116D2 aus VLOAD
- J2 PCM5102A-Modul, J3 MAX98357A-Modul, J4 Gain-Jumper
- J8 Jumper SD-Versorgung, J9 microSD-Modul, J7 Setup-Taster

### Hinweise
- Nur eine Audio-Variante bestuecken. Die I2S-Leitungen gehen parallel an beide Modulplaetze.
- Gesperrte GPIOs: IO0/3/45/46 (Strapping), IO19/20 (USB), IO26-37 (Flash/PSRAM), IO43/44 (UART0). IO38/48 je nach Board an der RGB-LED.
- Footprint ESP32-S3-DevKitC-1_THT mit Reihenabstand 25,4 mm. Vor dem Layout am echten Board nachmessen.
- Nicht gleichzeitig USB und BEC anschliessen, ohne die Schutzdiode des Boards zu pruefen.

## Firmware (geplant)
- CRSF als einziges Protokoll, busfaehig ueber die UART-Anschluesse (Eingang plus Weiterleitung zum naechsten Modul).
- Gemeinsame CRSF-Bibliothek als Kopie im Projekt (firmware/lib), keine externe Abhaengigkeit.
- Einstellung der Adresse und Parameter ueber CRSF-Parameter (Lua am Sender) und Web-UI.
- Build mit PlatformIO, Bauweise wie bei den Sound-Mini-Repos.

## Ordner
- `hardware/`: KiCad-Projekt (Schaltplan, PCB, PDF, eigene Footprints, fp-lib-table). Entwurfsnotizen in [hardware/README-hardware.md](hardware/README-hardware.md).
- `firmware/`: Firmware, noch nicht begonnen.
- `docs/`: Projektbeschreibung als DOCX.

## Stand
- Hardware: Schaltplan vorhanden, PCB noch leer. ERC: 1 Fehler (3V3 von U1 und PWR_FLAG beide Power output) und 65 Warnungen, siehe hardware/ERC.rpt.
- Firmware: noch nicht begonnen. Basis kann die Sound-Mini-Firmware (ESP32-Sound-Mini-S3) sein.

## Offene Punkte
- ERC-Fehler beheben (PWR_FLAG an 3V3 entfernen) und Symbolbibliothek "local" einbinden
- PCB-Layout erstellen
- Footprint am echten Board nachmessen
- Firmware schreiben (Multiswitch + Sound)
