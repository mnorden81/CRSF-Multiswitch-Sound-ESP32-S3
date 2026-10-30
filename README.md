# RC-Multiswitch-Sound-ESP32-S3

RC-Multiswitch mit 8 Kanaelen plus Soundmodul auf einem ESP32-S3-DevKitC-1.

Frueher `CRSF-Multiswitch-Sound-ESP32-S3`, umbenannt passend zu RC-Multiswitch-ESP32-C3 und -S3.

Kombiniert den RC-Multiswitch mit einem Soundmodul auf einer Platine: 8 Schaltkanaele ueber TBD62083APG (Nachfolger des ULN2803A) und Soundausgabe ueber I2S, Sounds von microSD. Ziel ist ein Modul fuer Modelle, die Licht und Sound zusammen brauchen.

Ausfuehrliche Projektbeschreibung: [docs/Projektbeschreibung.docx](docs/Projektbeschreibung.docx)

## Merkmale
- 8 Schaltkanaele ueber TBD62083APG, Kontroll-LED je Kanal
- UART A (Eingang), UART B und UART C als JST PH 4
- Audio: PCM5102A-DAC mit externem TPA3118-Verstaerker (HW-327, mono, aus VLOAD; Quelle mono: Kanal L direkt an den Verstaerker, kein Mono-Mix)
- microSD-Kartenmodul (6-polig) ueber SPI, Versorgung fest 3,3 V vom ESP32-Board (nur reine 3,3-V-Module verwenden)

## Hardware (v0.8, Entwurf, nur Schaltplan)
Board: ESP32-S3-DevKitC-1 N16R8 (oder Clone)

- Lastspannung 3S LiPo, 9-12,6 V (VLOAD) an CN2 (JST PH 2), TVS-Diode P6KE15A als Schutz.
- Treiber TBD62083APG (DMOS, Nachfolger des ULN2803A, gleiche Pinbelegung; ein ULN2803A passt weiterhin in den Sockel), 8 Kanaele, max. 500 mA je Kanal, Summe aller Kanaele ca. 1,5 A. Spannungsabfall am Ausgang nur ca. 0,2-0,4 V statt ca. 1 V beim ULN2803A.
- R9-R16 (THT 1/4 W) zwischen Treiber-Ausgang und Stiftleiste J1: Es ist kein Wert vorgegeben. Der Anwender muss die Widerstaende selbst passend zu Verbraucher und Lastspannung (3S, 9-12,6 V) auslegen, Strom und Verlustleistung beachten.
- R1-R8 und LED1-LED8 (3 mm) als Kontroll-LED je Kanal, vom Steuersignal Sn (3,3 V) gegen GND. Auch hier legt der Anwender die Werte selbst fest. Eine Empfehlung fuer die Kontroll-LEDs steht im Schaltplan.
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

### Anschluesse
- CN1, J5, J6: UART A/B/C als JST PH 4 horizontal (S4B-PH-K)
- CN2 Last 3S LiPo (9-12,6 V) (JST PH 2), CN4 Lautsprecherausgang (JST PH 2, SPK+ / SPK-)
- J2 PCM5102A-Modul: Pin 1-6 = Stiftleiste (VIN, GND, LCK, DIN, BCK, SCK), Pin 7-10 = Ausgangspads G, R, G, L. Die Quelle ist mono: Pad L (Pin 10) und G (Pin 7, 9) gehen per Leiterbahn direkt an J11, Pad R (Pin 8) bleibt unbenutzt (No-Connect). Kein Mono-Mix, keine Mischwiderstaende.
- J11 TPA3118-Modul (HW-327), wird auf die Platine geloetet: Pad 1 IN, 2 GND, 3 PWR+ (VLOAD), 4 PWR- (GND), 5 OUT+ und 6 OUT- (zu CN4); die MUTE-Pads bleiben offen
- J9 microSD-Modul (6 Pins, VCC an 3V3 des ESP32-Boards, eigener Footprint mit Befestigungsloechern)

### Hinweise
- Gesperrte GPIOs: IO0/3/45/46 (Strapping), IO19/20 (USB), IO26-37 (Flash/PSRAM), IO43/44 (UART0). IO38/48 je nach Board an der RGB-LED.
- Footprint ESP32-S3-DevKitC-1_THT mit Reihenabstand 25,4 mm. Vor dem Layout am echten Board nachmessen. Eigene Footprints fuer PCM5102A, microSD-Modul und TPA3118-Modul liegen in hardware/footprints.pretty; die Masse sind aus Fotos geschaetzt und vor dem Layout nachzumessen.
- Nicht gleichzeitig USB und BEC anschliessen, ohne die Schutzdiode des Boards zu pruefen.

## Firmware (v1.00, nicht auf Hardware getestet)
Arduino-Sketch [firmware/RC_Multiswitch_Sound](firmware/RC_Multiswitch_Sound), Beschreibung in [firmware/README.md](firmware/README.md).

Reine Zusammenfuehrung von zwei bestehenden Firmwares:
- Schaltteil: RC-Multiswitch-ESP32-S3 v4.06 (= ESP32-Multiswitch-Basis V3/TXRX mit S3-Teilen): 8 Ausgaenge, Blinken, PWM, MWprop, bis 3 Konfigurationen je Ausgang, MKan-Gruppen, CRSF-Modulkette.
- Soundteil: ESP32-Sound-Basis_Ziege-One v7.21: Motor (start/loop/shut) und 24 Sounds von der SD-Karte, Upload der WAV-Dateien per WLAN.
- Gemeinsam: RC-System (CRSF oder SBUS auf UART A), WLAN und Web-UI, Lua-Menue am Sender, UART B/C (CRSF-Modulkette, S.Port, GPS, Raw), Firmware-Update per WLAN, Einstellungs-Backup im Flash und auf der SD-Karte.

Neu gegenueber den Basis-Firmwares (Entscheidungen 21.-28.10.2026):
- Zwei Adressen: Schalt-Adresse (Ausgaenge, zugleich CRSF-Geraeteadresse) und Sound-Adresse (Einzelkanal-Sounds). Gleiche Zahl = dieselben Schalter schalten beides.
- Status-LED in der Web-UI waehlbar: IO48 (DevKitC-1 v1.0, Vorgabe) oder IO38 (v1.1).
- Lua-Menue zeigt die ersten 8 Sounds, in der Web-UI auf bis zu 23 erhoehbar (Grenze: 255 Lua-Felder). In der Web-UI sind immer alle 24 Sounds einstellbar.
- Eigener Name je Sound und je Ausgang (16 Zeichen), Vorgabe "Sound N" bzw. "Ausgang N".

## Ordner
- `hardware/`: KiCad-Projekt (Schaltplan, PCB, PDF, eigene Footprints, fp-lib-table). Entwurfsnotizen in [hardware/README-hardware.md](hardware/README-hardware.md).
- `firmware/`: Firmware (Arduino-Sketch `RC_Multiswitch_Sound`), Build in GitHub Actions.
- `hardware/gehaeuse/`: 3D-Druck-Gehaeuse (STL, STEP, CadQuery-Quelle), siehe [hardware/gehaeuse/README.md](hardware/gehaeuse/README.md).
- `docs/`: Projektbeschreibung als DOCX.

## Stand
- Hardware: Schaltplan v0.8, PCB in Arbeit. ERC-Stand v0.3 (hardware/ERC.rpt): 1 Fehler (IO8 unbeschaltet, seit v0.4 mit No-Connect-Flag, plus Rest-Leitungsstueck entfernt) und 55 Warnungen (Bibliotheksversionen "power"/"Device"/"local", harmlos). Nach v0.8 ERC neu ausfuehren.
- Firmware: v1.00 (Multiswitch V3 + Sound-Basis v7.21), baut in GitHub Actions, auf Hardware noch nicht getestet.

## Offene Punkte
- R1-R16 vom Anwender je LED bzw. Verbraucher auslegen (3S, ca. 11-12 V); Helligkeit schwankt mit dem Ladezustand.
- Schaltplan v0.8 im PCB per F8 (Aktualisieren aus Schaltplan) uebernehmen: J10, R18, R19, J3, J4, R17, J7, J8 entfallen (Footprints ohne Symbol loeschen lassen), J2, J9 und J11 bekommen die neuen Modul-Footprints (J11 jetzt TPA3118 mit 6 Pads), CN3 entfaellt, CN4 (Lautsprecher) ist neu. hardware/*.pdf ist noch der Stand v0.1.
- ERC mit v0.8 neu ausfuehren; Symbolbibliothek "local" einbinden (reduziert die Warnungen).
- TPA3118-Modul (HW-327): Modul-Mindestspannung 8 V beachten (3S-Akku nicht unter ca. 9 V entladen, Abschaltung ueber Fahrregler/Unterspannungswaechter), MUTE-Header offen, Leiterbahnbreite und Stromaufnahme gegen CN2/CN4 (JST PH, ca. 2 A je Kontakt) pruefen.
- microSD-Modul muss ein reines 3,3-V-Modul sein (Standard-Module mit Spannungsregler bekommen sonst zu wenig Spannung); Pinreihenfolge am Modul gegen J9 (VCC, CS, MOSI, SCK, MISO, GND) pruefen.
- Module nachmessen und Footprints anpassen: PCM5102A, microSD-Modul, TPA3118 (Padlage, Befestigungsloecher, Belegung IN/GND per Multimeter pruefen), ESP32-S3-DevKitC-1.
- PCB-Layout erstellen
- Gehaeuse (hardware/gehaeuse/) am echten Aufbau pruefen: Hoehe Elko C1 (22 mm angenommen), ESP 13 mm ueber der Platine, Lage USB-C und microSD-Slot
- Firmware v1.00 auf der Platine testen (Ausgaenge, Sound, SD-Karte, Status-LED IO48/IO38)
