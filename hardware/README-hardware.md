# RC-Multiswitch-Sound-ESP32-S3 (v0.9, Entwurf)
RC-Multiswitch (8 Kanaele, TBD62083APG als Nachfolger des ULN2803A; ein ULN2803A passt weiterhin) plus Soundmodul auf einem ESP32-S3-DevKitC-1 (N16R8 oder Clone). Nur Schaltplan, das PCB-Layout wird selbst erstellt.

## Teile
- Last 3S LiPo 9-12,6V (CN2, JST PH 2 horizontal), TVS P6KE15A (D1), 470u/35V (C1), max. 500mA je Kanal. R9-R16 (THT, ohne Wert: Anwender legt sie je Verbraucher aus) zwischen Treiber-Ausgang und J1; R1-R8 + LED1-8 als Kontroll-LEDs (Wert vom Anwender, Empfehlung siehe Schaltplan).
- UART A (CN1, Eingang), UART B (J5), UART C (J6): JST PH 4-polig horizontal (S4B-PH-K). Keine Vorwiderstaende. ESP32 wird aus +5V von CN1 versorgt.
- Audio:
  - PCM5102A-Modul auf J2 (VIN, GND, LCK, DIN, BCK, SCK), SCK fest auf GND. Das TPA3118-Modul (HW-327, mono, 8-24 V) wird direkt aus VLOAD gespeist (Pads PWR+/PWR- am Modul, kein Stecker CN3 mehr). Audio: Die Quelle ist mono, deshalb geht nur der Ausgang L (J2 Pin 10) per Leiterbahn direkt an J11 (IN) des Verstaerkers, die G-Pads (J2 Pin 7, 9) an GND, das R-Pad (J2 Pin 8) bleibt unbenutzt (kein Mono-Mix, keine R18/R19). Der MUTE-Header des Moduls bleibt offen. Loetbruecken H1L-H4L am PCM5102A-Modul lt. Datenblatt pruefen (ueblich: FLT=L, DEMP=L, XSMT=H, FMT=L).
  - I2S-Leitungen BCLK/LRCK/DOUT gehen an das PCM5102A-Modul (J2).
- microSD-TF-Kartenmodul (6-polig, J9: VCC, CS, MOSI, SCK, MISO, GND, gleiche Reihenfolge wie auf deinem RC-Sound-Board) an IO10-13. Versorgung fest 3,3 V vom 3V3-Pin des ESP32-Boards (J8 entfaellt). Das Modul muss mit 3,3 V arbeiten: Standard-"Micro SD TF Card Module" mit Spannungsregler und Pegelwandler sind fuer 5 V gebaut und funktionieren mit 3,3 V nicht zuverlaessig (Regler-Dropout). Pull-ups und Entkopplung sind auf dem Modul.

## GPIO-Belegung (DevKitC-1 N16R8)
| Funktion | GPIO |
|---|---|
| S0..S7 | 4, 5, 6, 7, 15, 16, 1, 2 |
| UART A | TX 17, RX 18 |
| UART B | TX 39, RX 40 |
| UART C | TX 41, RX 42 |
| SD (SPI) | CS 10, MOSI 11, SCK 12, MISO 13 |
| I2S | BCLK 14, LRCK 21, DOUT 47 |
Gesperrt: IO0/3/45/46 (Strapping), IO19/20 (USB), IO26-37 (Flash/PSRAM), IO43/44 (UART0). IO38/48 koennen je nach Board-Revision an der RGB-LED haengen.

## Footprint
- `footprints.pretty/ESP32-S3-DevKitC-1_THT`: 2x22, 2,54 mm, Reihenabstand **25,4 mm**, Pad 1-22 links von oben (USB unten), 23-44 rechts. Die Espressif-Bibliothek (PCM_Espressif) soll laut KiCad-Forum 22,86 mm haben und damit vermutlich falsch sein. Reihenabstand und Aussenmasse VOR dem Layout am echten Board nachmessen, die Pinbelegung gegen das Pinout des gekauften Boards pruefen.
- Pad-Zuordnung (Pad 1..22 = J1 links, 23..44 = J3 rechts) ist im Footprint und im Symbol identisch.
- `footprints.pretty/Mod_PCM5102A_GY_THT`: PCM5102A-Modul FLACH (Draufsicht): 6 Pins am linken kurzen Rand, Pad 1..6 = VIN (unten), GND, LCK, DIN, BCK, SCK (oben), wie J2. Pinraster exakt; Umriss ca. 31,5 x 16,5 mm inkl. Klinkenbuchse, aus dem Foto geschaetzt, am echten Modul nachmessen. Pad 7..10 = G, R, G, L an der oberen Langkante (Raster 2,54 mm), wie J2 Pin 7-10; die fuenf weiteren Loecher der Oberkante sind unbestueckt und nur auf F.Fab als Kreise markiert. Zum Uebernehmen in J2 im Schaltplan als Footprint `ESP32-S3-DevKitC-1_THT:Mod_PCM5102A_GY_THT` eintragen (Bibliotheksname siehe fp-lib-table).
- `footprints.pretty/Mod_microSD_SPI_THT`: microSD-/TF-Modul (Platine ca. 17,7 x 17,6 mm, Slot ragt rechts ca. 2,5 mm ueber), Pad 1..6 = VCC, CS, MOSI, SCK, MISO, GND (wie J9, Pad 1 eckig oben), zwei Befestigungsloecher 2,2 mm. Masse aus dem Foto geschaetzt, Pinreihenfolge laut Beschriftung des echten Moduls pruefen.
- `footprints.pretty/Mod_TPA3118_HW327_THT`: TPA3118-Modul HW-327 zum Einloeten (ca. 45,4 x 35 mm, 4 Befestigungsloecher 3 mm, Raster 39,7 x 29,1 mm). Pads: 1 IN (eckig), 2 GND, 3 PWR+ (eckig), 4 PWR-, 5 OUT+, 6 OUT-, 7/8 MUTE (nicht im Schaltplan, offen). Lage aus dem Foto geschaetzt (Raster der IN-Pads 2,54 mm als Massstab, OUT/PWR-Pads ca. 3,8 mm): am echten Modul nachmessen. Belegung IN/GND per Multimeter pruefen (GND-Pad hat Durchgang zu PWR-). Die alte Datei `Mod_TPA3118_HW327_Keepout.kicad_mod` ist damit ueberholt und kann geloescht werden.
- CN4 (Lautsprecher): Standard-Footprint Connector_JST:JST_PH_S2B-PH-K_1x02_P2.00mm_Horizontal.

## Masse und Leiterbahnbreiten (ab v0.9)
- Lastmasse getrennt: U2 (TBD62083APG) Pin 9 haengt am Netz **GNDPWR**, nicht an GND. GND (ESP, PCM5102A, microSD, TPA3118) und GNDPWR sind nur ueber den NetTie **NT1** verbunden. NT1 im PCB direkt an CN2 Pin 1 legen. So fliesst der Strom der Verbraucher nicht ueber die Masse von ESP und Audio.
- Netzklassen (Projekteinstellungen): **Power** 1,5 mm (Vias 1,0/0,5 mm) fuer GND, GNDPWR, /VLOAD und die Lautsprecherleitungen an CN4; **Kanal** 0,5 mm fuer /OUT1../OUT8 und /OUT1L../OUT8L; alles andere 0,2 mm (Default). Der Router (auch Freerouting) nimmt diese Breiten automatisch.
- Regeldatei `RC-Multiswitch-Sound-ESP32-S3.kicad_dru`: DRC meldet Bahnen von VLOAD, GNDPWR und Lautsprecher unter 1,0 mm und Kanal-Bahnen unter 0,4 mm. GND darf duenner sein (z. B. LEDs), breite GND-Wege zu TPA3118 und CN2 von Hand pruefen.
- Strom: U2 insgesamt max. ca. 1,5 A (mit ULN2803A wegen Waerme eher 1 A), nicht 8 x 500 mA gleichzeitig. Belastbarkeit von R9-R16 (Watt) zum Verbraucherstrom passend waehlen.

## Hinweise
- TPA3118-Modul: Betriebsspannung 3S (9-12,6 V) liegt im Bereich des Moduls (8-24 V), Elkos 25 V unkritisch; unter 8 V schaltet es nicht zuverlaessig. Leistung bei 12 V nur einige Watt an 4-8 Ohm. Der Verstaerker kann mehrere Ampere ziehen, die Leiterbahnen (Breite!) von VLOAD zu den Modul-Pads und CN4 (JST PH, ca. 2 A je Kontakt) pruefen. Mono: ein Lautsprecher (4-8 Ohm), BTL-Ausgang, keine Masse am Lautsprecher.
- Aenderungen v0.2 bis v0.8: TPA3118 statt TPA3116D2, MAX98357A samt J3/J4/R17 entfernt, J7 (Setup) und J8 (SD-Jumper) entfernt, SD fest an 3V3, PCM5102A-Ausgangspads als J2 Pin 7-10, J10/R18/R19 entfallen (mono), J11 Eingang des TPA3118. Im PCB per F8 uebernehmen, hardware/*.pdf ist noch v0.1.
- ESP wird ueber den 5V-Pin versorgt. Nicht gleichzeitig USB und BEC anschliessen, ohne zu pruefen, ob das Board eine Schutzdiode hat.
- Kein ERC/DRC im Entstehungs-Sandkasten (nur KiCad-CLI 7) und keine Firmware getestet. Bitte ERC in KiCad ausfuehren.
- fp-lib-table liegt neben der .kicad_pro, damit der Projekt-Footprint gefunden wird.
