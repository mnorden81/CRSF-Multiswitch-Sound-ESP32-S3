# Gehaeuse fuer RC-Multiswitch-Sound-ESP32-S3 (Platine v0.9)
# Parametrisch mit CadQuery. Aufruf: python gehaeuse.py  -> STL/STEP im Ordner "export"
#
# Koordinaten: X = KiCad-x - 151, Y = 147.75 - KiCad-y (Platinen-Ecke links unten = 0/0),
# Z nach oben, Z = 0 = Unterseite Gehaeuseboden.
import os, sys
import cadquery as cq

# ---------------- Parameter (alles in mm) ----------------
PCB_W, PCB_H, PCB_T = 106.0, 68.15, 1.6      # Platine aus Edge.Cuts
WAND      = 2.2      # Wandstaerke
BODEN     = 2.0      # Bodenstaerke
DECKEL    = 2.0      # Deckelstaerke
SPALT     = 0.6      # Luft Platine <-> Innenwand (oben/rechts/unten)
SPALT_L   = 3.0      # links mehr Luft: microSD-Slot ragt 2,6 mm ueber die Kante
UNTEN     = 5.0      # Platz unter der Platine (Loetstellen)
INNEN_H   = 24.0     # Platz ueber der Platine (Elko C1 10x22 stehend = hoechstes Teil, +2 mm Luft)
ESP_Z     = 13.0     # Unterkante ESP-Board ueber Platine (Annahme 11 mm + 2 mm laut Martin)
SCHRAUBE_D = 2.5     # Kernloch fuer M3-Blech-/Kunststoffschraube (Platine, 4x an den TPA-Loechern)
DECKEL_SCHR_D = 2.2  # Kernloch Deckelschrauben (M2,5 x 8 selbstschneidend)
DECKEL_SCHR_K = 2.8  # Durchgangsloch im Deckel
LIPPE     = 3.0      # Hoehe der Zentrierlippe am Deckel
TOL       = 0.25     # Spiel Lippe <-> Innenwand

Z_PCB = BODEN + UNTEN            # Unterkante Platine
Z_TOP = Z_PCB + PCB_T            # Oberkante Platine
H_BASE = Z_TOP + INNEN_H         # Hoehe Unterteil (Deckel liegt oben drauf)

def kx(x): return x - 151.0
def ky(y): return 147.75 - y

# Innenraum
IX0, IY0 = -SPALT_L, -SPALT
IX1, IY1 = PCB_W + SPALT, PCB_H + SPALT
# Deckelschrauben: diagonal ausserhalb der Platinenecken
SCHR = [(-SPALT_L - 0.2, -2.0), (PCB_W + 2.0, -2.0), (-SPALT_L - 0.2, PCB_H + 2.0), (PCB_W + 2.0, PCB_H + 2.0)]
BOSS_R = 3.0
# Aussenkontur: Rechteck um den Innenraum + Wand, Ecken rund ueber die Schraubendome
OX0, OY0 = min(IX0 - WAND, SCHR[0][0] - BOSS_R), min(IY0 - WAND, SCHR[0][1] - BOSS_R)
OX1, OY1 = max(IX1 + WAND, SCHR[3][0] + BOSS_R), max(IY1 + WAND, SCHR[3][1] + BOSS_R)
ECKE_R = 4.0

def box(x0, y0, z0, x1, y1, z1):
    return cq.Workplane("XY").box(x1 - x0, y1 - y0, z1 - z0, centered=False).translate((x0, y0, z0))

def rbox(x0, y0, z0, x1, y1, z1, r):
    return (cq.Workplane("XY").workplane(offset=z0).center((x0 + x1) / 2, (y0 + y1) / 2)
            .rect(x1 - x0, y1 - y0).extrude(z1 - z0).edges("|Z").fillet(r))

def cyl(x, y, z0, z1, r):
    return cq.Workplane("XY").workplane(offset=z0).center(x, y).circle(r).extrude(z1 - z0)

# ---------------- Oeffnungen in den Waenden ----------------
# (Name, Wand, von, bis (entlang der Wand, Platinenkoordinaten), z_unten, z_oben ueber Platinen-Oberkante)
# Werte aus den Fab-Umrissen der Footprints + 0,8 mm Luft je Seite.
def kicad_span_y(a, b): return (ky(b), ky(a))
OEFFNUNGEN = [
    # linke Wand (Kante x = 0)
    ("CN2 Akku 3S",      "L", *kicad_span_y(114.8 - 0.8, 120.7 + 0.8), -0.2, 6.8),
    ("CN1 UART A",       "L", *kicad_span_y(124.25 - 0.8, 134.15 + 0.8), -0.2, 6.8),
    ("microSD",          "L", *kicad_span_y(94.4 - 8.0, 94.4 + 8.0), 1.8, 8.6),
    ("USB-C ESP",        "L", *kicad_span_y(94.8 - 13.0, 94.8 + 13.0), ESP_Z - 0.6, ESP_Z + 8.4),
    # untere Wand (Kante y = 0)
    ("J5 UART B",        "U", kx(151.95 - 0.8), kx(161.85 + 0.8), -0.2, 6.8),
    ("J6 UART C",        "U", kx(163.65 - 0.8), kx(173.55 + 0.8), -0.2, 6.8),
    ("CN4 Lautsprecher", "U", kx(175.35 - 0.8), kx(181.25 + 0.8), -0.2, 6.8),
    ("J1 Kabel",         "U", kx(231.4), kx(253.6), 10.0, INNEN_H + 1),   # Kabel der Ausgaenge nach unten raus
]

def wand_schnitt(w, a, b, z0, z1):
    za, zb = Z_TOP + z0, Z_TOP + z1
    if w == "L": return box(OX0 - 1, a, za, IX0 + 0.5, b, zb)
    if w == "R": return box(IX1 - 0.5, a, za, OX1 + 1, b, zb)
    if w == "U": return box(a, OY0 - 1, za, b, IY0 + 0.5, zb)
    if w == "O": return box(a, IY1 - 0.5, za, b, OY1 + 1, zb)

# Platinen-Auflagen: 4 Saeulen an den TPA3118-Befestigungsloechern (3 mm, gehen durch Modul + Platine)
SAEULEN = [(kx(x), ky(y)) for x, y in [(186.21, 113.3), (225.91, 113.3), (186.21, 142.4), (225.91, 142.4)]]
# zusaetzliche Stuetzen (nur aufliegen), Stellen ohne Loetpads
STUETZEN = [(kx(152.6), ky(81.2)), (kx(152.6), ky(111.0)), (kx(255.4), ky(81.2)),
            (kx(255.4), ky(146.2)), (kx(152.6), ky(146.3)), (kx(226.0), ky(81.2))]

def unterteil():
    b = rbox(OX0, OY0, 0, OX1, OY1, H_BASE, ECKE_R)
    b = b.cut(box(IX0, IY0, BODEN, IX1, IY1, H_BASE + 1))
    # Schraubendome fuer den Deckel (an der Platine freigeschnitten)
    for x, y in SCHR:
        d = cyl(x, y, BODEN, H_BASE, BOSS_R).cut(box(-0.4, -0.4, 0, PCB_W + 0.4, PCB_H + 0.4, H_BASE + 1))
        b = b.union(d).cut(cyl(x, y, BODEN + 2, H_BASE + 1, DECKEL_SCHR_D / 2))
    # Saeulen + Stuetzen
    for x, y in SAEULEN:
        b = b.union(cyl(x, y, 0.5, Z_PCB, 3.2)).cut(cyl(x, y, 1.0, Z_PCB + 1, SCHRAUBE_D / 2))
    for x, y in STUETZEN:
        b = b.union(box(x - 1.6, y - 1.6, 0.5, x + 1.6, y + 1.6, Z_PCB))
    for name, w, a, c, z0, z1 in OEFFNUNGEN:
        b = b.cut(wand_schnitt(w, a, c, z0, z1))
    # Griffmulde fuer die microSD-Karte (aussen, von unten nach oben auslaufend)
    b = b.cut(box(OX0 - 1, ky(94.4 + 5), Z_TOP + 1.8, IX0 + 0.5, ky(94.4 - 5), Z_TOP + ESP_Z - 0.6))
    # Lueftungsschlitze Seitenwand rechts
    for i in range(6):
        y = 12 + i * 8
        b = b.cut(box(IX1 - 0.5, y, Z_TOP + 6, OX1 + 1, y + 2.0, Z_TOP + 18))
    # Befestigungslaschen unten (2x M3, fuer Kabelbinder/Schraube ins Modell)
    for x in (OX0 + 14, OX1 - 14):
        for y, s in ((OY0, -1), (OY1, 1)):
            t = rbox(x - 6, y - 7 if s < 0 else y - 1, 0, x + 6, y + 1 if s < 0 else y + 7, 3.0, 2.0)
            b = b.union(t).cut(cyl(x, y + s * 3.8, -1, 5, 1.7))
    return b

# ---------------- Deckel ----------------
LED_KICAD = [(248.0, 94.3), (243.03, 94.3), (237.91, 94.3), (233.03, 94.3),
             (248.0, 112.2), (243.03, 112.2), (237.91, 112.2), (233.03, 112.2)]
LED_OFF = 1.27   # LED_D3.0mm: Mitte liegt 1,27 mm rechts von Pad 1

def deckel():
    d = rbox(OX0, OY0, 0, OX1, OY1, DECKEL, ECKE_R)
    # Zentrierlippe innen, an den Domen und an der J1-Kabeloeffnung ausgespart
    lip = box(IX0 + TOL, IY0 + TOL, -LIPPE, IX1 - TOL, IY1 - TOL, 0.01)
    lip = lip.cut(box(IX0 + TOL + 1.2, IY0 + TOL + 1.2, -LIPPE - 1, IX1 - TOL - 1.2, IY1 - TOL - 1.2, 1))
    for x, y in SCHR:
        lip = lip.cut(cyl(x, y, -LIPPE - 1, 1, BOSS_R + 0.4))
    for name, w, a, c, z0, z1 in OEFFNUNGEN:
        if z1 > INNEN_H - LIPPE:
            lip = lip.cut(box(a - 0.3, IY0 - 1, -LIPPE - 1, c + 0.3, IY0 + 3, 1))
    d = d.union(lip)
    # Schraubenloecher mit Senkung
    for x, y in SCHR:
        d = d.cut(cyl(x, y, -LIPPE - 1, DECKEL + 1, DECKEL_SCHR_K / 2))
        d = d.cut(cq.Workplane("XY").workplane(offset=DECKEL - 1.4).center(x, y)
                  .circle(DECKEL_SCHR_K / 2).workplane(offset=1.41).circle(2.6).loft())
    # LED-Fenster (Kontroll-LEDs 1-8)
    for x, y in LED_KICAD:
        d = d.cut(cyl(kx(x + LED_OFF), ky(y), -1, DECKEL + 1, 1.7))
    # Fenster ueber dem USB-Ende des ESP: BOOT/RESET mit Stift druecken, RGB-LED sichtbar
    d = d.cut(rbox(kx(152.6), ky(94.8 + 12.5), -LIPPE - 1, kx(161.0), ky(94.8 - 12.5), DECKEL + 1, 1.5))
    # Ausschnitt ueber J1 (Ausgaenge, Stiftleiste 2x8)
    d = d.cut(rbox(kx(231.4), ky(146.6), -LIPPE - 1, kx(253.6), ky(139.4), DECKEL + 1, 1.0))
    # Lueftungsschlitze ueber dem TPA3118
    for i in range(9):
        x = kx(190.0) + i * 4.0
        d = d.cut(rbox(x, ky(140.0), -1, x + 2.0, ky(116.0), DECKEL + 1, 0.9))
    # Beschriftung (0,6 mm vertieft)
    def txt(s, x, y, size=3.2, rot=0):
        return (cq.Workplane("XY").workplane(offset=DECKEL - 0.6).center(x, y)
                .transformed(rotate=(0, 0, rot)).text(s, size, 1.0, halign="center", valign="center"))
    for s, x, y, sz, r in [("RC Multiswitch Sound S3", 40, 62.5, 4.0, 0),
                           ("USB", kx(165.0), ky(94.8), 3.0, 90),
                           ("A", 3.0, ky(129.2), 3.2, 0), ("3S", 3.5, ky(117.75), 3.2, 0),
                           ("B", kx(156.9), 3.5, 3.2, 0), ("C", kx(168.6), 3.5, 3.2, 0),
                           ("SPK", kx(178.3), 3.5, 3.0, 0),
                           ("1  2  3  4", kx(240.5 + LED_OFF), ky(98.6), 2.6, 0),
                           ("5  6  7  8", kx(240.5 + LED_OFF), ky(116.5), 2.6, 0),
                           ("AUSGAENGE", kx(242.5), ky(137.0), 2.6, 0)]:
        d = d.cut(txt(s, x, y, sz, r))
    return d

def platine_dummy():
    """Platine mit vereinfachten Bauteilen (nur fuer Vorschau / Kollisionskontrolle)."""
    p = box(0, 0, Z_PCB, PCB_W, PCB_H, Z_TOP)
    T = Z_TOP
    def fab(x0, y0, x1, y1, z0, z1):
        return box(kx(x0), ky(y1), T + z0, kx(x1), ky(y0), T + z1)
    teile = {
        "ESP32-S3": fab(152.03, 80.83, 221.83, 108.77, ESP_Z, ESP_Z + 1.6),
        "ESP-Modul": fab(195.0, 85.8, 219.0, 103.8, ESP_Z + 1.6, ESP_Z + 4.8),
        "USB": fab(151.3, 82.3, 158.6, 107.3, ESP_Z + 1.6, ESP_Z + 4.8),
        "Leisten": fab(159.0, 81.0, 215.0, 83.2, 0, ESP_Z).union(fab(159.0, 106.4, 215.0, 108.6, 0, ESP_Z)),
        "PCM5102A": fab(177.1, 85.85, 208.2, 103.85, 2.5, 4.1),
        "microSD": fab(148.9, 85.58, 169.2, 103.18, 2.5, 4.1).union(fab(148.4, 87.4, 160.7, 101.4, 4.1, 5.9)),
        "TPA3118": fab(183.23, 110.51, 228.57, 145.48, 3.0, 4.6),
        "C1": cyl(kx(170.48), ky(127.85), T, T + 20, 5.0),
        "U2": fab(231.28, 117.13, 254.26, 127.29, 0, 8.0),
        "J1": fab(232.37, 140.48, 252.69, 145.56, 0, 8.5),
        "D1": fab(163.85, 114.9, 176.55, 118.5, 0, 3.6),
    }
    for x, y in [(217.69, 140.56), (207.84, 140.56), (218.2, 116.21), (208.36, 116.21)]:   # Elkos auf dem Modul
        teile["TPA-C%.0f%.0f" % (x, y)] = cyl(kx(x), ky(y), T + 4.6, T + 16.6, 4.1)
    for ref, (x0, y0, x1, y1) in {"CN1": (151.9, 124.25, 159.5, 134.15), "CN2": (151.9, 114.8, 159.5, 120.7),
                                  "J5": (151.95, 137.95, 161.85, 145.55), "J6": (163.65, 137.95, 173.55, 145.55),
                                  "CN4": (175.35, 137.95, 181.25, 145.55)}.items():
        teile[ref] = fab(x0, y0, x1, y1, 0, 6.0)
    for x, y in LED_KICAD:
        teile["LED%.0f%.0f" % (x, y)] = cyl(kx(x + LED_OFF), ky(y), T, T + 5.3, 1.5)
    for p_ in teile.values():
        p = p.union(p_)
    return p

if __name__ == "__main__":
    out = sys.argv[1] if len(sys.argv) > 1 else "export"
    os.makedirs(out, exist_ok=True)
    u, d = unterteil(), deckel()
    print("Aussenmasse: %.1f x %.1f x %.1f mm (mit Deckel)" % (OX1 - OX0, OY1 - OY0, H_BASE + DECKEL))
    cq.exporters.export(u, f"{out}/Gehaeuse_Unterteil.stl", tolerance=0.05, angularTolerance=0.1)
    cq.exporters.export(u, f"{out}/Gehaeuse_Unterteil.step")
    # Deckel fuer den Druck umdrehen (Oberseite aufs Bett)
    d_druck = d.rotate((0, 0, 0), (1, 0, 0), 180)
    cq.exporters.export(d_druck, f"{out}/Gehaeuse_Deckel.stl", tolerance=0.05, angularTolerance=0.1)
    cq.exporters.export(d, f"{out}/Gehaeuse_Deckel.step")
    if "--vorschau" in sys.argv:
        pd = platine_dummy()
        cq.exporters.export(pd, f"{out}/vorschau_platine.stl", tolerance=0.1, angularTolerance=0.3)
        cq.exporters.export(d.translate((0, 0, H_BASE)), f"{out}/vorschau_deckel.stl", tolerance=0.1, angularTolerance=0.3)
        # Kollisionskontrolle
        cu, cd = pd.intersect(u).val().Volume(), pd.intersect(d.translate((0, 0, H_BASE))).val().Volume()
        print("Kollision Platine/Unterteil: %.2f mm3, Platine/Deckel: %.2f mm3" % (cu, cd))
