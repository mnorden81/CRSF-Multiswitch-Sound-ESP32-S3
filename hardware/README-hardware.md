# RC-Multiswitch-Sound-ESP32-S3 (v0.1, Entwurf)
RC-Multiswitch (8 Kanaele, ULN2803A) plus Soundmodul auf einem ESP32-S3-DevKitC-1 (N16R8 oder Clone). Nur Schaltplan, das PCB-Layout wird selbst erstellt.

## Teile
- Last 12-24V (CN2, JST PH 2 horizontal), TVS P6KE33A (D1), 470u/35V (C1), max. 500mA je Kanal. R9-R16 (150R, THT) zwischen ULN-Ausgang und J1; R1-R8 + LED1-8 als Kontroll-LEDs (Tabelle siehe Schaltplan).
- UART A (CN1, Eingang), UART B (J5), UART C (J6): JST PH 4-polig horizontal (S4B-PH-K). Keine Vorwiderstaende. ESP32 wird aus +5V von CN1 versorgt.
- Audio, nur EINE Variante bestuecken:
  - A) PCM5102A-Modul auf J2 (VIN, GND, LCK, DIN, BCK, SCK), SCK fest auf GND. Line-Out per Kabel auf das TPA3116D2-Modul, das ueber CN3 direkt aus VLOAD gespeist wird.
  - B) MAX98357A-Modul auf J3 (VIN, GND, SD, GAIN, DIN, BCLK, LRC), GAIN per Jumper J4 (GND 12dB / offen 9dB / +5V 6dB), SD an IO8 mit R17 (100k) Pull-down.
  - I2S-Leitungen BCLK/LRCK/DOUT gehen parallel an beide Module.
- microSD-TF-Kartenmodul (6-polig, J9: VCC, CS, MOSI, SCK, MISO, GND, gleiche Reihenfolge wie auf deinem RC-Sound-Board) an IO10-13. Versorgung per Jumper J8: 5V fuer Module mit Spannungsregler/Pegelwandler (Standard-"Micro SD TF Card Module"), 3V3 fuer reine 3,3V-Module. Pull-ups und Entkopplung sind auf dem Modul.
- Setup-Taster auf J7 (IO9, interner Pull-up in Firmware).

## GPIO-Belegung (DevKitC-1 N16R8)
| Funktion | GPIO |
|---|---|
| S0..S7 | 4, 5, 6, 7, 15, 16, 1, 2 |
| UART A | TX 17, RX 18 |
| UART B | TX 39, RX 40 |
| UART C | TX 41, RX 42 |
| SD (SPI) | CS 10, MOSI 11, SCK 12, MISO 13 |
| I2S | BCLK 14, LRCK 21, DOUT 47 |
| Amp-Mute (MAX98357A SD) | 8 |
| Setup | 9 |
Gesperrt: IO0/3/45/46 (Strapping), IO19/20 (USB), IO26-37 (Flash/PSRAM), IO43/44 (UART0). IO38/48 koennen je nach Board-Revision an der RGB-LED haengen.

## Footprint
- `footprints.pretty/ESP32-S3-DevKitC-1_THT`: 2x22, 2,54 mm, Reihenabstand **25,4 mm**, Pad 1-22 links von oben (USB unten), 23-44 rechts. Die Espressif-Bibliothek (PCM_Espressif) soll laut KiCad-Forum 22,86 mm haben und damit vermutlich falsch sein. Reihenabstand und Aussenmasse VOR dem Layout am echten Board nachmessen, die Pinbelegung gegen das Pinout des gekauften Boards pruefen.
- Pad-Zuordnung (Pad 1..22 = J1 links, 23..44 = J3 rechts) ist im Footprint und im Symbol identisch.

## Hinweise
- ESP wird ueber den 5V-Pin versorgt. Nicht gleichzeitig USB und BEC anschliessen, ohne zu pruefen, ob das Board eine Schutzdiode hat.
- Kein ERC/DRC im Entstehungs-Sandkasten (nur KiCad-CLI 7) und keine Firmware getestet. Bitte ERC in KiCad ausfuehren.
- fp-lib-table liegt neben der .kicad_pro, damit der Projekt-Footprint gefunden wird.
