# CRSF-Multiswitch-Sound-ESP32-S3

CRSF-Multiswitch mit 8 Kanaelen (ULN2803A) plus Soundmodul (I2S mit PCM5102A oder MAX98357A, microSD) auf einem ESP32-S3-DevKitC-1 N16R8, UART A, B und C.

## Aufbau
- `hardware/`: KiCad-Projekt (Schaltplan, PCB, PDF, eigene Footprints). Details, Pinbelegung und offene Punkte in [hardware/README-hardware.md](hardware/README-hardware.md).
- `firmware/`: Firmware, noch nicht begonnen.

## Stand
- Hardware v0.1: nur Schaltplan, PCB noch leer. ERC: 1 Fehler (3V3 von U1 und PWR_FLAG beide Power output), 65 Warnungen (siehe hardware/ERC.rpt).
- Firmware: offen.
