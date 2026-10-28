// webui_page.h - Web-Seite des RC-Multiswitch-Sound v1.00 (PROGMEM)
// Zusammengefuehrt aus der Seite von RC-Multiswitch-ESP32-S3 v4.06 (webui.h)
// und ESP32-Sound-Basis v7.21 (WebServerManager.cpp). Doppelte Namen im
// Soundteil: Funktionen snd_*, IDs sb-*, Reiter stab-*, /api/sconfig.
#pragma once
#include <pgmspace.h>

static const char WEBUI_HTML[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="de">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>MultiSwitch</title>
<style>
:root{
  --bg:#0d1117;--surf:#161b22;--surf2:#21262d;
  --border:#30363d;--accent:#4493f8;--green:#3fb950;
  --yellow:#d29922;--red:#f85149;--orange:#e3932a;
  --text:#e6edf3;--sub:#8b949e;--radius:8px;
  --mono:'Courier New',monospace;
}
*{box-sizing:border-box;margin:0;padding:0}
body{background:var(--bg);color:var(--text);font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;font-size:14px;min-height:100vh}
header{background:var(--surf);border-bottom:1px solid var(--border);padding:12px 16px;display:flex;align-items:center;gap:10px;position:sticky;top:0;z-index:50}
header h1{font-size:16px;font-weight:700}
.hv{font-size:11px;color:var(--sub);margin-top:2px}
.dot{width:8px;height:8px;border-radius:50%;background:var(--sub);margin-left:auto;transition:.4s}
.dot.ok{background:var(--green);box-shadow:0 0 6px var(--green)}
.dot.warn{background:var(--yellow)}
.dot.err{background:var(--red)}
nav{background:var(--surf);border-bottom:1px solid var(--border);display:flex;overflow-x:auto;scrollbar-width:none}
nav::-webkit-scrollbar{display:none}
.tab{flex-shrink:0;padding:10px 16px;font-size:12px;font-weight:600;color:var(--sub);cursor:pointer;border-bottom:2px solid transparent;transition:.2s;white-space:nowrap;user-select:none}
.tab:hover{color:var(--text)}
.tab.active{color:var(--accent);border-bottom-color:var(--accent)}
main{max-width:560px;margin:0 auto;padding:20px 14px 60px}
.card{background:var(--surf);border:1px solid var(--border);border-radius:var(--radius);padding:16px;margin-bottom:12px}
.card-title{font-weight:600;font-size:13px;margin-bottom:12px;color:var(--sub);text-transform:uppercase;letter-spacing:.04em}
.row{display:flex;align-items:center;gap:8px;margin-bottom:8px}
.row label{flex:0 0 110px;font-size:13px;color:var(--sub)}
.row input,.row select{flex:1;background:var(--surf2);border:1px solid var(--border);color:var(--text);border-radius:6px;padding:6px 10px;font-size:13px}
.row input:focus,.row select:focus{outline:none;border-color:var(--accent)}
.btn{padding:7px 14px;border:none;border-radius:6px;font-size:13px;font-weight:600;cursor:pointer;transition:.15s}
.btn-primary{background:var(--accent);color:#fff}
.btn-primary:hover{filter:brightness(1.15)}
.btn-danger{background:var(--red);color:#fff}
.btn-ghost{background:transparent;border:1px solid var(--border);color:var(--text)}
.btn-ghost:hover{border-color:var(--accent);color:var(--accent)}
.btn-sm{padding:5px 10px;font-size:12px}
.grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(150px,1fr));gap:8px;margin-bottom:12px}
.tile{background:var(--surf2);border:1px solid var(--border);border-radius:var(--radius);padding:12px 10px;text-align:center;cursor:pointer;transition:.2s;position:relative}
.tile:hover{border-color:var(--accent)}
.tile.on{background:#0d2c16;border-color:var(--green)}
.tile.override{border-color:var(--orange)}
.tile-name{font-size:12px;font-weight:600;margin-bottom:4px;word-break:break-word}
.tile-sub{font-size:10px;color:var(--sub)}
.lock{position:absolute;top:6px;right:8px;font-size:10px;color:var(--orange)}
.badge{display:inline-block;padding:2px 6px;border-radius:4px;font-size:10px;font-weight:700}
.badge-ok{background:#0d2c16;color:var(--green)}
.badge-err{background:#2c0d0d;color:var(--red)}
.badge-warn{background:#2c200d;color:var(--yellow)}
.sep{border:none;border-top:1px solid var(--border);margin:12px 0}
.mono{font-family:var(--mono);font-size:12px}
/* Output detail */
#out-detail{display:none}
.out-hdr{display:flex;align-items:center;gap:10px;margin-bottom:16px}
.back-btn{background:none;border:none;color:var(--accent);cursor:pointer;font-size:20px;padding:0 4px}
.state-big{font-size:24px;font-weight:700}
.state-on{color:var(--green)}
.state-off{color:var(--sub)}
.action-row{display:flex;gap:8px;margin-bottom:16px}
.tile-btns{display:flex;gap:4px;margin-top:8px;justify-content:center}
.tile-btns button{flex:1;padding:4px 2px;font-size:10px;font-weight:600;border:none;border-radius:4px;cursor:pointer;transition:.15s}
.tile-btn-on{background:var(--green);color:#0d1117}
.tile-btn-off{background:var(--red);color:#fff}
.tile-btn-rel{background:var(--surf);border:1px solid var(--border)!important;color:var(--sub)}
.tile-btn-cfg{background:var(--accent);color:#fff}

/* Soundteil */
:root{--r:8px}
.snd *{box-sizing:border-box;margin:0;padding:0}
.snd .card{background:var(--surf);border:1px solid var(--border);border-radius:var(--r);padding:14px;margin-bottom:10px}
.snd .card-title{font-weight:600;font-size:11px;color:var(--sub);text-transform:uppercase;letter-spacing:.05em;margin-bottom:10px}
.snd .row{display:flex;align-items:center;gap:8px;margin-bottom:8px}
.snd .lbl{font-size:13px;color:var(--sub);flex:0 0 130px}
.snd select,.snd input[type=text],.snd input[type=password],.snd input[type=number]{
  width:100%;background:var(--surf2);border:1px solid var(--border);
  border-radius:var(--r);color:var(--text);padding:8px 10px;font-size:13px;outline:none;transition:border .2s}
.snd select:focus,.snd input:focus{border-color:var(--accent)}
.snd .sl-wrap{margin-bottom:12px}
.snd .sl-head{display:flex;justify-content:space-between;font-size:12px;color:var(--sub);margin-bottom:5px}
.snd input[type=range]{width:100%;-webkit-appearance:none;height:4px;border-radius:2px;background:var(--surf2);outline:none}
.snd input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:18px;height:18px;border-radius:50%;background:var(--accent);cursor:pointer}
.snd input[type=range]::-moz-range-thumb{width:18px;height:18px;border-radius:50%;background:var(--accent);cursor:pointer;border:none}
.snd .btn{display:inline-flex;align-items:center;justify-content:center;padding:9px 16px;border-radius:var(--r);font-size:13px;font-weight:600;cursor:pointer;border:none;transition:.15s;width:100%;margin-top:6px}
.snd .btn-p{background:var(--accent);color:#fff}
.snd .btn-p:hover{opacity:.85}
.snd .btn-g{background:var(--green);color:#fff}
.snd .btn-g:hover{opacity:.85}
.snd .btn-r{background:var(--red);color:#fff}
.snd .btn-r:hover{opacity:.85}
.snd .btn-s{background:var(--surf2);color:var(--text);border:1px solid var(--border)}
.snd .btn-s:hover{border-color:var(--accent);color:var(--accent)}
.snd .g2{display:grid;grid-template-columns:1fr 1fr;gap:8px}
.snd .badge{display:inline-block;padding:2px 8px;border-radius:20px;font-size:11px;font-weight:600}
.snd .bg-g{background:#1a3a1a;color:var(--green)}
.snd .bg-r{background:#3a1a1a;color:var(--red)}
.snd .bg-s{background:var(--surf2);color:var(--sub)}
.snd .stabs{display:flex;gap:4px;flex-wrap:wrap;margin-bottom:12px}
.snd .stab{padding:5px 10px;border-radius:var(--r);font-size:12px;font-weight:600;cursor:pointer;background:var(--surf2);color:var(--sub);border:1px solid var(--border);transition:.15s}
.snd .stab.active{background:var(--accent);color:#fff;border-color:var(--accent)}
.snd .cell-grid{display:grid;grid-template-columns:repeat(3,1fr);gap:6px;margin-top:8px}
.snd .cell-box{background:var(--surf2);border-radius:var(--r);padding:8px;text-align:center}
.snd .cell-v{font-size:17px;font-weight:700}
.snd .cell-l{font-size:10px;color:var(--sub);margin-top:2px}
.snd .dbg-grid{display:grid;grid-template-columns:repeat(2,1fr);gap:4px}
.snd .dbg-item{background:var(--surf2);border-radius:4px;padding:6px 8px}
.snd .dbg-k{font-size:10px;color:var(--sub)}
.snd .dbg-v{font-size:13px;font-weight:600;margin-top:1px}
.snd hr{border:none;border-top:1px solid var(--border);margin:10px 0}
.snd .mono{font-family:var(--mono);font-size:12px}
.snd .hidden{display:none!important}
.snd /* Toast */
#toast{position:fixed;bottom:24px;left:50%;transform:translateX(-50%);
  background:var(--green);color:#fff;padding:10px 20px;border-radius:var(--r);
  font-size:13px;font-weight:600;opacity:0;transition:opacity .3s;pointer-events:none;z-index:99}
.snd #toast.err{background:var(--red)}
.snd #toast.show{opacity:1}
#toast{position:fixed;bottom:24px;left:50%;transform:translateX(-50%);background:var(--green);color:#fff;padding:10px 20px;border-radius:8px;font-size:13px;font-weight:600;opacity:0;transition:opacity .3s;pointer-events:none;z-index:99}#toast.err{background:var(--red)}#toast.show{opacity:1}
</style>
</head>
<body>
<header>
  <div>
    <h1>⚡ MultiSwitch &#9835; Sound</h1>
    <div class="hv" id="hv">lädt…</div>
  </div>
  <div class="dot" id="dot"></div>
</header>
<nav id="nav">
  <div class="tab active" onclick="showTab('overview')">Übersicht</div>
  <div class="tab" onclick="showTab('outputs')">Ausgänge</div>
  <div class="tab" onclick="showTab('motor')">Motor</div>
  <div class="tab" onclick="showTab('sounds')">Sounds</div>
  <div class="tab" onclick="showTab('rc')">RC-System</div>
  <div class="tab" onclick="showTab('wifi')">WiFi</div>
  <div class="tab" onclick="showTab('lipo')">LiPo</div>
  <div class="tab" onclick="showTab('debug')">Debug</div>
</nav>
<main>
  <!-- Übersicht -->
  <div id="tab-overview">
    <div class="grid" id="tile-grid"></div>
    <div style="display:flex;gap:8px;flex-wrap:wrap;margin-bottom:12px">
      <button class="btn btn-ghost btn-sm" onclick="releaseAll()">🔓 Alle freigeben</button>
    </div>
    <div class="card" id="card-channels" style="display:none">
      <div class="card-title">RC-Kanalwerte</div>
      <div id="ch-grid" style="display:grid;grid-template-columns:repeat(4,1fr);gap:6px;font-size:12px;font-family:var(--mono)"></div>
    </div>
  </div>

  <!-- Ausgänge -->
  <div id="tab-outputs" style="display:none">
    <div id="out-list"></div>
    <div id="out-detail">
      <div class="out-hdr">
        <button class="back-btn" onclick="showOutList()">‹</button>
        <span id="out-title" style="font-weight:700;font-size:15px"></span>
        <span id="out-state-big" class="state-big state-off"></span>
      </div>
      <div class="action-row">
        <button class="btn btn-primary" id="btn-on"  onclick="switchOut(true)">Einschalten</button>
        <button class="btn btn-danger"  id="btn-off" onclick="switchOut(false)">Ausschalten</button>
        <button class="btn btn-ghost"   id="btn-rel" onclick="releaseOut()">Freigeben</button>
      </div>
      <div class="card" id="out-cfg"></div>
    </div>
  </div>

  <!-- RC -->
<div id="tab-motor" style="display:none"><div class="snd"><div id="stab-motor">
  <div class="card">
    <div class="card-title">Motor Mode</div>
    <select id="m-mode">
      <option value="0">Eine Richtung</option>
      <option value="1">Zwei Richtungen</option>
    </select>
  </div>
  <div class="card">
    <div class="card-title">Motor EIN Modus</div>
    <select id="m-toggle">
      <option value="0">Normal</option>
      <option value="1">Toggle</option>
    </select>
  </div>
  <div class="card">
    <div class="card-title">Quelle Einschalten Motor</div>
    <select id="m-src"></select>
  </div>
  <div class="card">
    <div class="card-title">Quelle Motorspeed</div>
    <select id="m-spd"></select>
  </div>
  <div class="card">
    <div class="card-title">Lautst&auml;rke &amp; Drehzahl</div>
    <div class="sl-wrap">
      <div class="sl-head"><span>Volumen</span><span id="lv-m-vol">-</span></div>
      <input type="range" id="m-vol" min="0" max="200" step="5" oninput="slUpd('m-vol','lv-m-vol')">
    </div>
    <div class="sl-wrap">
      <div class="sl-head"><span>Drehzahl min %</span><span id="lv-m-rmin">-</span></div>
      <input type="range" id="m-rmin" min="0" max="200" step="5" oninput="slUpd('m-rmin','lv-m-rmin')">
    </div>
    <div class="sl-wrap">
      <div class="sl-head"><span>Drehzahl max %</span><span id="lv-m-rmax">-</span></div>
      <input type="range" id="m-rmax" min="100" max="600" step="5" oninput="slUpd('m-rmax','lv-m-rmax')">
    </div>
    <div class="sl-wrap">
      <div class="sl-head"><span>Standgas Verz&ouml;gerung s</span><span id="lv-m-sdly">-</span></div>
      <input type="range" id="m-sdly" min="0" max="60" step="2" oninput="slUpd('m-sdly','lv-m-sdly')">
    </div>
    <div class="sl-wrap">
      <div class="sl-head"><span>Motor Rampe %/s</span><span id="lv-m-ramp">-</span></div>
      <input type="range" id="m-ramp" min="0" max="50" step="2" oninput="slUpd('m-ramp','lv-m-ramp')">
    </div>
    <div class="sl-wrap">
      <div class="sl-head"><span>Standgas Totband %</span><span id="lv-m-dead">-</span></div>
      <input type="range" id="m-dead" min="0" max="50" step="2" oninput="slUpd('m-dead','lv-m-dead')">
    </div>
  </div>
  <div class="g2">
    <button class="btn btn-p" onclick="saveMotor()">Speichern</button>
    <button class="btn btn-s" onclick="loadConfig()">Zur&uuml;cksetzen</button>
  </div>
  <div class="card" style="margin-top:10px">
    <div class="card-title">Voreinstellungen</div>
    <div class="g2">
      <button class="btn btn-s" onclick="xget('/setsbus',0,function(){loadConfig();})">SBUS</button>
      <button class="btn btn-s" onclick="xget('/setpwm',0,function(){loadConfig();})">PWM</button>
    </div>
    <button class="btn btn-s" style="margin-top:6px" onclick="xget('/setpin',0,function(){loadConfig();})">PIN</button>
    <button class="btn btn-r" style="margin-top:6px" onclick="if(confirm('Werkseinstellung laden?'))xget('/reset',0,function(){loadConfig();})">Werkseinstellung</button>
  </div>
  <!-- Loop-/Standgas-/Start-Sound-Dateien verwalten (Upload+Loeschen),
       analog zur Sound-Datei-Verwaltung im Tab "Sounds". -->
  <div class="card" style="margin-top:10px">
    <div class="card-title">Motor-Sound-Dateien verwalten</div>
    <div style="font-size:12px;color:var(--sub);margin-bottom:10px">
      Loop-, Standgas- und Start-Sound direkt per WLAN auf die SD-Karte hochladen
      oder von dort l&ouml;schen - wirkt sofort, ohne Neustart des Moduls.
    </div>
    <div class="sl-wrap">
      <div class="sl-head"><span>Loop (loop.wav)</span></div>
      <div style="font-size:12px;color:var(--sub);margin-bottom:6px" id="mf-loop-status">-</div>
      <div class="g2">
        <button class="btn btn-s" onclick="document.getElementById('mf-loop-file').click()">&#8679; Hochladen</button>
        <button class="btn btn-r" onclick="deleteMotorFile('loop.wav')">L&ouml;schen</button>
      </div>
      <input type="file" id="mf-loop-file" accept="audio/wav,.wav" style="display:none" onchange="uploadMotorFile('loop.wav',this.files[0])">
    </div>
    <div class="sl-wrap">
      <div class="sl-head"><span>Standgas (shut.wav)</span></div>
      <div style="font-size:12px;color:var(--sub);margin-bottom:6px" id="mf-shut-status">-</div>
      <div class="g2">
        <button class="btn btn-s" onclick="document.getElementById('mf-shut-file').click()">&#8679; Hochladen</button>
        <button class="btn btn-r" onclick="deleteMotorFile('shut.wav')">L&ouml;schen</button>
      </div>
      <input type="file" id="mf-shut-file" accept="audio/wav,.wav" style="display:none" onchange="uploadMotorFile('shut.wav',this.files[0])">
    </div>
    <div class="sl-wrap">
      <div class="sl-head"><span>Start (start.wav)</span></div>
      <div style="font-size:12px;color:var(--sub);margin-bottom:6px" id="mf-start-status">-</div>
      <div class="g2">
        <button class="btn btn-s" onclick="document.getElementById('mf-start-file').click()">&#8679; Hochladen</button>
        <button class="btn btn-r" onclick="deleteMotorFile('start.wav')">L&ouml;schen</button>
      </div>
      <input type="file" id="mf-start-file" accept="audio/wav,.wav" style="display:none" onchange="uploadMotorFile('start.wav',this.files[0])">
    </div>
    <div class="row hidden" id="mf-upload-progress">
      <div style="font-size:12px;color:var(--sub)" id="mf-upload-status">-</div>
    </div>
  </div>
</div></div></div>
<div id="tab-sounds" style="display:none"><div class="snd"><div id="stab-sounds">
  <div class="stabs" id="stabs"></div>
  <div class="card">
    <div class="card-title">Name</div>
    <div style="display:flex;gap:10px;align-items:center;justify-content:space-between"><span id="s-name" style="font-weight:600;white-space:nowrap;overflow:hidden;text-overflow:ellipsis">Sound 1</span>
    <button class="btn btn-p" style="width:auto;flex:none;padding:8px 16px" onclick="saveSndName()">Name &auml;ndern</button></div>
  </div>
  <div class="card">
    <div class="card-title">Quelle Einschalten</div>
    <select id="s-src"></select>
  </div>
  <div class="card">
    <div class="card-title">Lautst&auml;rke</div>
    <div class="sl-wrap">
      <div class="sl-head"><span>Volumen</span><span id="lv-s-vol">-</span></div>
      <input type="range" id="s-vol" min="0" max="200" step="5" oninput="slUpd('s-vol','lv-s-vol')">
    </div>
  </div>
  <div class="card">
    <div class="card-title">Wiedergabe Modus</div>
    <select id="s-mode">
      <option value="0">Normal</option>
      <option value="1">Loop</option>
      <option value="2">Tippbetrieb</option>
    </select>
  </div>
  <div class="g2">
    <button class="btn btn-p" onclick="saveSound()">Speichern</button>
    <button class="btn btn-g" onclick="testSound()">&#9654; Test Sound</button>
  </div>
  <!-- WAV-Datei fuer den aktuell ausgewaehlten Sound-Slot direkt per WLAN auf
       die SD-Karte im Modul hochladen - kein Ausbau der Karte noetig. Nutzt
       den bestehenden SD-Mutex (siehe handleApiSoundUploadData()), damit
       kein Konflikt mit laufender Sound-Wiedergabe entsteht. Zusaetzlich
       Status (vorhanden/Groesse) und Loeschen. -->
  <div class="card">
    <div class="card-title">Sound-Datei (WAV) verwalten</div>
    <div style="font-size:12px;color:var(--sub);margin-bottom:6px">
      Sound <span id="snd-upload-label">-</span> (<code id="snd-file-name">-</code>)
    </div>
    <div style="font-size:12px;color:var(--sub);margin-bottom:10px" id="snd-file-status">-</div>
    <div class="g2">
      <button class="btn btn-s" onclick="document.getElementById('snd-upload-file').click()">&#8679; WAV hochladen</button>
      <button class="btn btn-r" onclick="deleteSoundSlotFile()">L&ouml;schen</button>
    </div>
    <input type="file" id="snd-upload-file" accept="audio/wav,.wav" style="display:none" onchange="uploadSoundFile(this.files[0])">
    <div class="row hidden" id="snd-upload-progress" style="margin-top:8px">
      <div style="font-size:12px;color:var(--sub)" id="snd-upload-status">-</div>
    </div>
    <div style="font-size:12px;color:var(--sub);margin-top:10px">
      Ersetzt bzw. l&ouml;scht die WAV-Datei direkt auf der SD-Karte im Modul.
      Erwartet 16-Bit-PCM-WAV; die &Auml;nderung wirkt sofort, ohne Neustart.
    </div>
  </div>
</div></div></div>
<div id="tab-lipo" style="display:none"><div class="snd"><div id="stab-lipo">
  <div class="card">
    <div class="card-title">Sensor Adresse (Poll-ID)</div>
    <div class="row" style="margin-bottom:4px">
      <span class="lbl">Sensor 1 (Pack 1)</span>
      <input type="text" id="pid0" maxlength="2" style="width:80px;font-family:var(--mono);text-transform:uppercase">
    </div>
    <div style="font-size:11px;color:var(--sub);margin-bottom:10px">Werkseinstellung: A1 (Physical ID 0x02)</div>
    <div class="row" style="margin-bottom:4px">
      <span class="lbl">Sensor 2 (Pack 2)</span>
      <input type="text" id="pid1" maxlength="2" style="width:80px;font-family:var(--mono);text-transform:uppercase">
    </div>
    <div style="font-size:11px;color:var(--sub);margin-bottom:10px">Werkseinstellung: 22 (Physical ID 0x03)</div>
    <button class="btn btn-p" onclick="saveLipo()">Speichern</button>
  </div>
  <div class="card">
    <div style="display:flex;align-items:center;justify-content:space-between;margin-bottom:10px">
      <span class="card-title" style="margin:0">Live Sensorwerte</span>
      <button class="btn btn-s" style="width:auto;padding:4px 12px;margin:0;font-size:11px" onclick="loadDebug()">&#8635; Aktualisieren</button>
    </div>
    <div id="lipo-packs"><div style="color:var(--sub);font-size:12px">Wird geladen...</div></div>
  </div>
</div></div></div>
  <div id="tab-rc" style="display:none">
    <div class="card">
      <div class="card-title">RC-System</div>

      <!-- RC-Protokoll -->
      <div class="row"><label>RC-Protokoll</label>
        <select id="sel-rc" onchange="onRcChange()">
          <option value="0">FrSky (SBUS)</option>
          <option value="1">FlySky (SBUS)</option>
          <option value="2">ELRS normiert (SBUS)</option>
          <option value="3">HoTT (SBUS)</option>
          <option value="4">CRSF (ELRS/TBS)</option>
        </select>
      </div>

      <!-- SBUS: Einkanal-Kanal (wie RC-Sound: SBUS 1-16 + Deaktiviert) -->
      <div class="row" id="row-sbus-ch" style="display:none"><label>Einkanal-Kanal</label>
        <select id="sel-sbus-ch" onchange="onSbusChChange()">
          <optgroup label="SBUS Kanal">
            <option value="0">SBUS Kanal 01</option>
            <option value="1">SBUS Kanal 02</option>
            <option value="2">SBUS Kanal 03</option>
            <option value="3">SBUS Kanal 04</option>
            <option value="4">SBUS Kanal 05</option>
            <option value="5">SBUS Kanal 06</option>
            <option value="6">SBUS Kanal 07</option>
            <option value="7">SBUS Kanal 08</option>
            <option value="8">SBUS Kanal 09</option>
            <option value="9">SBUS Kanal 10</option>
            <option value="10">SBUS Kanal 11</option>
            <option value="11">SBUS Kanal 12</option>
            <option value="12">SBUS Kanal 13</option>
            <option value="13">SBUS Kanal 14</option>
            <option value="14">SBUS Kanal 15</option>
            <option value="15">SBUS Kanal 16</option>
          </optgroup>
          <optgroup label="Optionen">
            <option value="999">Deaktiviert</option>
          </optgroup>
        </select>
      </div>

      <!-- SBUS: Einkanal-Modus (nur wenn nicht Deaktiviert) -->
      <div class="row" id="row-ek" style="display:none"><label>Einkanal-Modus</label>
        <select id="sel-ek" onchange="onEkChange()">
          <option value="0">Normal</option>
          <option value="10">SBUS WM Adr 0</option>
          <option value="11">SBUS WM Adr 1</option>
          <option value="12">SBUS WM Adr 2</option>
          <option value="13">SBUS WM Adr 3</option>
        </select>
      </div>

      <!-- CRSF: Modul-Adresse (0-20, wie RC-Sound Vorbild) -->
      <div class="row" id="row-adr" style="display:none"><label>Modul-Adresse</label>
        <select id="sel-adr" onchange="onAdrChange()">
          <optgroup label="Modul Adresse">
            <option value="0">0</option>
            <option value="1">1</option>
            <option value="2">2</option>
            <option value="3">3</option>
            <option value="4">4</option>
            <option value="5">5</option>
            <option value="6">6</option>
            <option value="7">7</option>
            <option value="8">8</option>
            <option value="9">9</option>
            <option value="10">10</option>
            <option value="11">11</option>
            <option value="12">12</option>
            <option value="13">13</option>
            <option value="14">14</option>
            <option value="15">15</option>
            <option value="16">16</option>
            <option value="17">17</option>
            <option value="18">18</option>
            <option value="19">19</option>
            <option value="20">20</option>
          </optgroup>
        </select>
      </div>

      <span style="color:var(--sub);font-size:12px;display:block;margin-top:6px" id="rc-hint"></span>
    </div>

    <!-- MKan-Gruppen (NEU V3.00): 2 zusaetzliche, unabhaengige Kanalquellen -->
    <div class="card">
      <div class="card-title">MKan-Gruppen (V3)</div>

      <div class="row" id="row-mkan-adr0" style="display:none"><label>MKan Gruppe 1 Adresse</label>
        <select id="sel-mkan-adr0" onchange="onMkanChange(0)">
          <option value="255">Aus</option>
          <option value="0">0</option><option value="1">1</option><option value="2">2</option>
          <option value="3">3</option><option value="4">4</option><option value="5">5</option>
          <option value="6">6</option><option value="7">7</option><option value="8">8</option>
          <option value="9">9</option><option value="10">10</option><option value="11">11</option>
          <option value="12">12</option><option value="13">13</option><option value="14">14</option>
          <option value="15">15</option><option value="16">16</option><option value="17">17</option>
          <option value="18">18</option><option value="19">19</option><option value="20">20</option>
        </select>
      </div>
      <div class="row" id="row-mkan-adr1" style="display:none"><label>MKan Gruppe 2 Adresse</label>
        <select id="sel-mkan-adr1" onchange="onMkanChange(1)">
          <option value="255">Aus</option>
          <option value="0">0</option><option value="1">1</option><option value="2">2</option>
          <option value="3">3</option><option value="4">4</option><option value="5">5</option>
          <option value="6">6</option><option value="7">7</option><option value="8">8</option>
          <option value="9">9</option><option value="10">10</option><option value="11">11</option>
          <option value="12">12</option><option value="13">13</option><option value="14">14</option>
          <option value="15">15</option><option value="16">16</option><option value="17">17</option>
          <option value="18">18</option><option value="19">19</option><option value="20">20</option>
        </select>
      </div>

      <div class="row" id="row-mkan-ch0" style="display:none"><label>MKan Gruppe 1 SBUS-Kanal</label>
        <select id="sel-mkan-ch0" onchange="onMkanChange(0)">
          <optgroup label="SBUS Kanal">
            <option value="0">SBUS Kanal 01</option><option value="1">SBUS Kanal 02</option>
            <option value="2">SBUS Kanal 03</option><option value="3">SBUS Kanal 04</option>
            <option value="4">SBUS Kanal 05</option><option value="5">SBUS Kanal 06</option>
            <option value="6">SBUS Kanal 07</option><option value="7">SBUS Kanal 08</option>
            <option value="8">SBUS Kanal 09</option><option value="9">SBUS Kanal 10</option>
            <option value="10">SBUS Kanal 11</option><option value="11">SBUS Kanal 12</option>
            <option value="12">SBUS Kanal 13</option><option value="13">SBUS Kanal 14</option>
            <option value="14">SBUS Kanal 15</option><option value="15">SBUS Kanal 16</option>
          </optgroup>
          <optgroup label="Optionen"><option value="999">Deaktiviert</option></optgroup>
        </select>
      </div>
      <div class="row" id="row-mkan-mode0" style="display:none"><label>MKan Gruppe 1 Modus</label>
        <select id="sel-mkan-mode0" onchange="onMkanChange(0)">
          <option value="0">Normal</option>
          <option value="10">SBUS WM Adr 0</option><option value="11">SBUS WM Adr 1</option>
          <option value="12">SBUS WM Adr 2</option><option value="13">SBUS WM Adr 3</option>
        </select>
      </div>
      <div class="row" id="row-mkan-ch1" style="display:none"><label>MKan Gruppe 2 SBUS-Kanal</label>
        <select id="sel-mkan-ch1" onchange="onMkanChange(1)">
          <optgroup label="SBUS Kanal">
            <option value="0">SBUS Kanal 01</option><option value="1">SBUS Kanal 02</option>
            <option value="2">SBUS Kanal 03</option><option value="3">SBUS Kanal 04</option>
            <option value="4">SBUS Kanal 05</option><option value="5">SBUS Kanal 06</option>
            <option value="6">SBUS Kanal 07</option><option value="7">SBUS Kanal 08</option>
            <option value="8">SBUS Kanal 09</option><option value="9">SBUS Kanal 10</option>
            <option value="10">SBUS Kanal 11</option><option value="11">SBUS Kanal 12</option>
            <option value="12">SBUS Kanal 13</option><option value="13">SBUS Kanal 14</option>
            <option value="14">SBUS Kanal 15</option><option value="15">SBUS Kanal 16</option>
          </optgroup>
          <optgroup label="Optionen"><option value="999">Deaktiviert</option></optgroup>
        </select>
      </div>
      <div class="row" id="row-mkan-mode1" style="display:none"><label>MKan Gruppe 2 Modus</label>
        <select id="sel-mkan-mode1" onchange="onMkanChange(1)">
          <option value="0">Normal</option>
          <option value="10">SBUS WM Adr 0</option><option value="11">SBUS WM Adr 1</option>
          <option value="12">SBUS WM Adr 2</option><option value="13">SBUS WM Adr 3</option>
        </select>
      </div>
      <span style="color:var(--sub);font-size:12px;display:block;margin-top:6px">Zusaetzlich zur Einzelkanal-Adresse oben: bis zu 16 weitere, unabhaengige Trigger-Bits fuer die Ausgangs-Kanalquelle "MKan1"/"MKan2".</span>
    </div>

    <!-- UART B/C (NEU v4.04): Rolle je Zusatz-UART, wie beim Soundmodul -->
    <div class="card">
      <div class="card-title">UART B/C</div>
      <div class="row"><label>UART B</label>
        <select id="sel-portb" onchange="onPortChange()"></select>
      </div>
      <div class="row" id="row-portc"><label>UART C</label>
        <select id="sel-portc" onchange="onPortChange()"></select>
      </div>
      <div class="row"><label>UART B Baud</label>
        <select id="sel-portb-baud" onchange="onPortChange()"></select>
      </div>
      <div class="row" id="row-portc-baud"><label>UART C Baud</label>
        <select id="sel-portc-baud" onchange="onPortChange()"></select>
      </div>
      <span style="color:var(--yellow);font-size:12px;display:none;margin-top:6px" id="portc-dup">UART C ist aus: S.Port, GPS und Raw laufen nur auf einem Port.</span>
      <span style="color:var(--sub);font-size:12px;display:block;margin-top:6px">CRSF = Modulkette zum naechsten Modul. S.Port = FrSky LiPo-Sensoren (FLVSS/MLVSS), GPS = NMEA-Empfaenger, Raw = UART offen ohne Auswertung. Beim Wechsel der Rolle wird die passende Baudrate vorgeschlagen. Bei CRSF gehen Zellspannungen und GPS als Telemetrie an den Sender. Wirkt nach einem Neustart.</span>
      <span style="font-size:12px;display:block;margin-top:6px" id="port-hint"></span>
    </div>
  </div>

  <!-- WiFi -->
  <div id="tab-wifi" style="display:none">
    <div class="card">
      <div class="card-title">Zugangspunkt</div>
      <div class="row"><label>Ger&auml;tename (TBS Agent)</label><input type="text" id="inp-dnam" maxlength="23" placeholder="MultiSwitch"></div>
      <div class="row"><label>SSID</label><input type="text" id="inp-ssid" maxlength="32"></div>
      <div class="row"><label>Passwort</label><input type="text" id="inp-pass" maxlength="63"></div>
      <div class="row"><label>IP-Adresse</label>
        <input type="text" id="inp-ip" maxlength="15" placeholder="192.168.1.1"
          pattern="^((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$">
      </div>
      <button class="btn btn-primary" style="margin-top:8px" onclick="saveWifi()">Speichern &amp; Neustart</button>
    </div>
    <div class="card">
      <div class="card-title">WLAN dauerhaft aktiv (empfohlen, NEU v2.02)</div>
      <div class="row"><label>Dauerhaft an</label>
        <select id="sel-wifi-always" onchange="onWifiAlwaysOnChange()">
          <option value="0">Aus</option>
          <option value="1">Ein</option>
        </select>
      </div>
      <span style="color:var(--sub);font-size:12px;display:block">Schaltet den Access Point beim Booten immer ein und nie wieder aus - unabh&auml;ngig vom Auto-Failsafe unten. Wirkt erst nach dem n&auml;chsten Neustart. Default AN.</span>
    </div>
    <div class="card">
      <div class="card-title">WLAN jetzt (NEU v2.00)</div>
      <div class="row"><label>Access Point</label>
        <span id="wifi-ap-state" style="font-weight:600">–</span>
        <button class="btn btn-ghost btn-sm" onclick="toggleWifi(true)">Einschalten</button>
        <button class="btn btn-ghost btn-sm" onclick="toggleWifi(false)">Ausschalten</button>
      </div>
      <div class="row"><label>Auto bei Signalverlust</label>
        <select id="sel-wifi-auto" onchange="saveWifiAuto()">
          <option value="0">Aus</option>
          <option value="1">Ein</option>
        </select>
      </div>
      <div class="row"><label>Timeout (s)</label>
        <input type="number" id="inp-wifi-auto-to" min="5" max="240" step="1" onchange="saveWifiAuto()">
      </div>
      <span style="color:var(--sub);font-size:12px;display:block">Schaltet den Access Point automatisch ein, wenn so lange kein g&uuml;ltiges RC-Signal anliegt (z.&nbsp;B. Sender aus). Schaltet nie automatisch wieder aus.</span>
      <span id="wifi-auto-overridden-hint" style="color:var(--yellow);font-size:12px;display:none;margin-top:4px">&#9888; Wirkungslos, solange "WLAN dauerhaft aktiv" oben eingeschaltet ist - der AP l&auml;uft dann bereits seit dem Booten.</span>
    </div>
    <div class="card">
      <div class="card-title">Konfiguration sichern/wiederherstellen</div>
      <div style="display:flex;gap:8px;flex-wrap:wrap;margin-bottom:8px">
        <button class="btn btn-ghost btn-sm" onclick="exportConfig()">&#8681; Als Datei exportieren</button>
        <button class="btn btn-ghost btn-sm" onclick="document.getElementById('cfg-import-file').click()">&#8679; Aus Datei importieren</button>
        <input type="file" id="cfg-import-file" accept="application/json" style="display:none" onchange="importConfigFile(this.files[0])">
      </div>
      <span style="color:var(--sub);font-size:12px;display:block">Sichert alle Ausg&auml;nge und Einstellungen als JSON-Datei im Browser (kein SD-Slot n&ouml;tig). Import ersetzt die aktuelle Konfiguration inkl. WLAN-Zugangsdaten.</span>
      <div style="border-top:1px solid var(--border);margin:10px 0 8px"></div>
      <div style="display:flex;gap:8px;flex-wrap:wrap;margin-bottom:8px">
        <button class="btn btn-ghost btn-sm" onclick="flashBackupSave()">&#128190; Im Modul sichern</button>
        <button class="btn btn-ghost btn-sm" onclick="flashBackupList()">&#8679; Aus Modul laden</button>
      </div>
      <div id="fb-list" style="display:none;margin-bottom:8px">
        <select id="fb-select" style="width:100%;margin-bottom:6px"></select>
        <div style="display:flex;gap:8px;flex-wrap:wrap">
          <button class="btn btn-ghost btn-sm" onclick="flashBackupRestore()">Wiederherstellen</button>
          <button class="btn btn-ghost btn-sm" onclick="flashBackupDelete()">L&ouml;schen</button>
        </div>
      </div>
      <div id="fb-status" style="font-size:12px;margin-bottom:6px"></div>
      <span style="color:var(--sub);font-size:12px;display:block">v4.06: Sichert dieselben Daten im Flash des Moduls &ndash; ohne PC oder Handy-Datei. Es bleiben die letzten 5 eigenen Sicherungen und 3 automatische (vor jedem Import und Firmware-Update). &Uuml;bersteht Flashen und OTA, nicht aber &bdquo;Erase All Flash&ldquo; oder ein anderes Partitionsschema &ndash; f&uuml;r den Ernstfall zus&auml;tzlich als Datei exportieren.</span>
    </div>
    <div class="card" id="c-ota">
      <div class="card-title">Firmware-Update (OTA, NEU v2.00)</div>
      <span style="color:var(--sub);font-size:12px;display:block;margin-bottom:8px">Spielt eine mit der Arduino-IDE gebaute .bin-Datei direkt per WLAN auf – Ausbauen und USB-Flashen ist damit nicht mehr n&ouml;tig. Setzt eine OTA-f&auml;hige Partitionstabelle voraus (siehe README).</span>
      <button class="btn btn-ghost btn-sm" onclick="document.getElementById('ota-file').click()">&#8679; Firmware-Datei w&auml;hlen (.bin)</button>
      <input type="file" id="ota-file" accept=".bin,application/octet-stream" style="display:none" onchange="uploadOtaFile(this.files[0])">
      <div style="margin-top:8px;font-size:12px" id="ota-status"></div>
      <span style="color:var(--yellow);font-size:12px;display:block;margin-top:8px">&#9888; W&auml;hrend des Uploads die Spannungsversorgung des Moduls NICHT unterbrechen. Update m&ouml;glichst bei &uuml;ber ESC/Akku bestromtem Modul durchf&uuml;hren, nicht nur &uuml;ber USB – manche USB-Anschl&uuml;sse liefern beim Flash-Schreiben nicht genug Strom, ein dadurch ausgel&ouml;ster Reset bricht den Upload ab. Nach Erfolg startet das Modul automatisch neu, die WLAN-Verbindung bricht dabei kurz ab – das ist normal.</span>
    </div>
    <div class="card">
      <div class="card-title">Werkseinstellungen</div>
      <button class="btn btn-danger" onclick="if(confirm('Alles zurücksetzen?'))doReset()">Zurücksetzen</button>
    </div>
  </div>

  <!-- Debug -->
  <div id="tab-debug" style="display:none">
    <div class="card">
      <div class="card-title">Live-Status</div>
      <div id="debug-body" class="mono" style="line-height:1.8"></div>
    </div>
    <div class="card">
      <div class="card-title">CRSF-Diagnose (NEU v2.00)</div>
      <div id="crsf-diag-body" class="mono" style="line-height:1.8"></div>
    </div>
    <div class="card">
      <div class="card-title">UART B/C</div>
      <div id="port-diag-body" class="mono" style="line-height:1.8"></div>
    </div>
    <div class="card">
      <div class="card-title">MWprop-Kanalwerte</div>
      <div id="prop-body" class="mono" style="line-height:1.8"></div>
    </div>
  </div>
<div class="snd" id="snd-rest" hidden><div id="stab-settings">
  <div class="card">
    <div class="card-title">RC-System</div>
    <select id="rc-sys" onchange="onRcSys()">
      <option value="0">FrSky</option>
      <option value="1">FlySky</option>
      <option value="2">ELRS (SBUS)</option>
      <option value="3">Hott</option>
      <option value="4">ELRS (CRSF)</option>
    </select>
  </div>
  <div class="card" id="c-ekch">
    <div class="card-title">Einkanal Kanal</div>
    <select id="ek-ch"></select>
  </div>
  <div class="card" id="c-ekmo">
    <div class="card-title">Einkanal-Mode</div>
    <select id="ek-mode">
      <option value="0">Normal</option>
      <option value="10">SBUS WM Adr 0</option>
      <option value="11">SBUS WM Adr 1</option>
      <option value="12">SBUS WM Adr 2</option>
      <option value="13">SBUS WM Adr 3</option>
    </select>
  </div>
  <div class="card hidden" id="c-madr">
    <div class="card-title">Modul Adresse (CRSF)</div>
    <select id="mod-adr"></select>
  </div>
  <div class="card hidden" id="c-mkan">
    <div class="card-title">Mehrfachadress-Erweiterung (MKan)</div>
    <div class="row" style="margin-bottom:10px">
      <span class="lbl">Gruppe 1 (MKan 01-08)</span>
      <select id="mkan-adr0"></select>
    </div>
    <div class="row">
      <span class="lbl">Gruppe 2 (MKan 09-16)</span>
      <select id="mkan-adr1"></select>
    </div>
  </div>
  <div class="card hidden" id="c-sbusgrp">
    <div class="card-title">Mehrfachadress-Erweiterung (MKan, SBUS)</div>
    <div class="row" style="margin-bottom:6px">
      <span class="lbl">Gruppe 1 Kanal (MKan 01-08)</span>
      <select id="sbusgrp-ch0"></select>
    </div>
    <div class="row" style="margin-bottom:10px">
      <span class="lbl">Gruppe 1 Modus</span>
      <select id="sbusgrp-mode0">
        <option value="0">Normal</option>
        <option value="10">SBUS WM Adr 0</option>
        <option value="11">SBUS WM Adr 1</option>
        <option value="12">SBUS WM Adr 2</option>
        <option value="13">SBUS WM Adr 3</option>
      </select>
    </div>
    <div class="row" style="margin-bottom:6px">
      <span class="lbl">Gruppe 2 Kanal (MKan 09-16)</span>
      <select id="sbusgrp-ch1"></select>
    </div>
    <div class="row">
      <span class="lbl">Gruppe 2 Modus</span>
      <select id="sbusgrp-mode1">
        <option value="0">Normal</option>
        <option value="10">SBUS WM Adr 0</option>
        <option value="11">SBUS WM Adr 1</option>
        <option value="12">SBUS WM Adr 2</option>
        <option value="13">SBUS WM Adr 3</option>
      </select>
    </div>
    <div style="font-size:12px;color:var(--sub);margin-top:10px">
      Nutzt je Gruppe einen eigenen SBUS-Kanal (Deaktiviert = kein Kanal
      zugewiesen) - anders als MKan bei CRSF/ELRS belegt das zusaetzliche
      RC-Kanaele, da SBUS keine adressierten Pakete kennt.
    </div>
  </div>
  <hr>
  <div class="card" id="c-pwm">
    <div class="card-title">PWM Einstellungen</div>
    <div class="row" style="margin-bottom:10px">
      <span class="lbl">PWM min (&micro;s)</span>
      <input type="number" id="pwm-min" min="0" max="4095" style="width:120px">
    </div>
    <div class="row">
      <span class="lbl">PWM max (&micro;s)</span>
      <input type="number" id="pwm-max" min="0" max="4095" style="width:120px">
    </div>
  </div>

  <hr>
  <div class="card">
    <div class="card-title">Hardware Config</div>
    <select id="hw-cfg" onchange="onHwCfg()">
      <option value="0">V1 (GPIO 16,17,22,0,2,4)</option>
      <option value="1">V2 (GPIO 16,17,14,27,32,33)</option>
      <option value="2">V3 (nur BUS+Einkanal, + Port B/C)</option>
    </select>
    <!-- Zeigt "(Neustart noetig!)", sobald Hardware_Config_Pending vom aktiv
         laufenden Hardware_Config abweicht (hardwareConfigRestartPending(),
         siehe config.h) - derselbe Hinweis, den auch das CRSF/Lua-Menue am
         Sender zeigt. Der Wert kommt ueber /api/config als "restart_pending"
         im JSON. -->
    <div id="hw-restart-hint" class="hidden" style="color:var(--yellow);font-size:12px;margin-top:8px">
      &#9888; Neustart erforderlich, damit die neue Hardware Config aktiv wird.
    </div>
  </div>
  <hr>
  <!-- Port B (echter 2. Hardware-UART, GPIO32/33) und Port C (GPS,
       SoftwareSerial GPIO27/14) sind unabhaengig von der Hardware-Version
       waehlbar - nur sichtbar/wirksam auf Hardware V3 (siehe onHwCfg()), da
       V1/V2 diese Pins fuer PWM-Eingaenge brauchen. -->
  <div class="card" id="c-portbc">
    <div class="card-title">Port B / Port C</div>
    <div class="row" style="margin-bottom:10px">
      <span class="lbl">Port B Rolle (GPIO32/33)</span>
      <!-- Optionen kommen zur Laufzeit vom Geraet (s.portb_options in
           /api/config, siehe fillSelFlat() weiter unten) - dieselbe Liste
           wie im CRSF/Lua-Menue und in allen anderen Modulen (RC_Common,
           uart_roles.h). -->
      <select id="portb-mode" onchange="onPortBMode()"></select>
    </div>
    <div class="row" id="row-portb-baud" style="margin-bottom:10px">
      <span class="lbl">Port B Baudrate</span>
      <!-- v7.21: Liste aus /rc_common.js mit "Auto" (GPS sucht die Baudrate
           selbst), ein eigener Wert aus einer Sicherung bleibt erhalten -->
      <select id="portb-baud"></select>
    </div>
    <div class="row">
      <span class="lbl">Port C (GPS, GPIO27/14)</span>
      <select id="portc-gps">
        <option value="0">Aus</option>
        <option value="1">An</option>
      </select>
    </div>
    <div style="color:var(--yellow);font-size:12px;margin-top:8px">
      &#9888; Aenderungen an Port B/C wirken erst nach einem Neustart des Moduls.
      Port C (GPS) kollidiert mit SBUS (GPIO27 wird von beiden belegt) - fuer
      GPS bitte Einkanal/RC-System auf CRSF stellen oder GPS auf Port B legen.
      Steht Port B auf GPS, bleibt Port C aus (nur ein GPS). Port C laeuft per
      SoftwareSerial und kann deshalb nur GPS (kein CRSF, kein S.Port).
    </div>
  </div>
  <button class="btn btn-p" onclick="saveSettings()">Speichern</button>

  <!-- Konfiguration Export/Import: komplette Geraetekonfiguration (Sounds,
       Motor, RC-System, Port B/C, WLAN, ...) als JSON sichern bzw. aus einer
       zuvor exportierten Datei wiederherstellen - z.B. vor einem Firmware-
       Update oder um mehrere Module baugleich zu konfigurieren. -->
  <div class="card" id="c-cfgbackup" style="margin-top:10px">
    <div class="card-title">Konfiguration sichern / wiederherstellen</div>
    <div style="font-size:12px;color:var(--sub);margin-bottom:10px">
      Export speichert die komplette Konfiguration (alle Sounds, Motor,
      RC-System, Port B/C, WLAN, ...) als JSON-Datei auf diesem Geraet.
      Import spielt eine zuvor exportierte Datei wieder ein.
    </div>
    <div class="g2">
      <button class="btn btn-s" onclick="snd_exportConfig()">&#8681; Export</button>
      <button class="btn btn-s" onclick="document.getElementById('cfg-import-file').click()">&#8679; Import</button>
    </div>
    <input type="file" id="sb-cfg-import-file" accept="application/json,.json" style="display:none" onchange="snd_importConfigFile(this.files[0])">
    <div style="color:var(--yellow);font-size:12px;margin-top:8px">
      &#9888; Import ueberschreibt die aktuelle Konfiguration vollstaendig
      (inkl. WLAN-Zugangsdaten). Aenderungen an Port B/C/Hardware wirken wie
      ueblich erst nach einem Neustart des Moduls.
    </div>
    <!-- Zusaetzlich direkt auf/von der im Modul eingebauten SD-Karte - zwei
         eigene, explizite Aktionen (kein automatischer Export bei jedem
         Speichern), Dateiname mit fortlaufender Nummer statt echtem Datum
         (das Modul hat weder RTC noch NTP, siehe Kommentar in
         handleApiSdExport() in WebServerManager.cpp). -->
    <div style="font-size:12px;color:var(--sub);margin-top:14px;margin-bottom:10px">
      Zusaetzlich direkt auf der SD-Karte im Modul (Ordner /cfgbkup),
      unabhaengig vom Browser - z.B. um ein Ersatzmodul per Karte zu
      konfigurieren.
    </div>
    <div class="g2">
      <button class="btn btn-s" onclick="sdExportConfig()">&#8681; Auf SD sichern</button>
      <button class="btn btn-s" onclick="sdListBackups()">&#8679; Von SD laden</button>
    </div>
    <div class="hidden" id="sd-backup-list" style="margin-top:8px">
      <select id="sd-backup-select" style="width:100%;margin-bottom:6px"></select>
      <div class="g2">
        <button class="btn btn-s" onclick="sdImportSelected()">Importieren</button>
        <button class="btn btn-r" onclick="sdDeleteSelected()">L&ouml;schen</button>
      </div>
    </div>
  </div>

  <!-- Firmware-Update direkt per WLAN (OTA) - .bin-Datei hochladen, Firmware
       landet im inaktiven OTA-Segment, Modul startet danach neu. Schlaegt
       der Upload fehl/wird er abgebrochen, bootet unveraendert mit der
       bisherigen Firmware weiter (siehe src/RC_Common/web_common.cpp). -->
  <div class="card" id="sb-c-ota" style="margin-top:10px">
    <div class="card-title">Firmware-Update (OTA)</div>
    <div style="font-size:12px;color:var(--sub);margin-bottom:10px">
      Spielt eine mit PlatformIO gebaute .bin-Datei direkt per WLAN auf das
      Modul auf - Ausbauen und USB-Flashen ist damit nicht mehr noetig.
    </div>
    <button class="btn btn-s" onclick="document.getElementById('ota-file').click()">&#8679; Firmware-Datei waehlen (.bin)</button>
    <input type="file" id="sb-ota-file" accept=".bin,application/octet-stream" style="display:none" onchange="snd_uploadOtaFile(this.files[0])">
    <div class="row hidden" id="ota-progress" style="margin-top:8px">
      <span class="lbl" id="sb-ota-status"></span>
    </div>
    <div style="color:var(--yellow);font-size:12px;margin-top:8px">
      &#9888; Waehrend des Uploads die Spannungsversorgung des Moduls NICHT
      unterbrechen. Update moeglichst bei ueber ESC/Akku bestromtem Modul
      durchfuehren, nicht nur ueber USB - manche USB-Anschluesse liefern
      beim Flash-Schreiben nicht genug Strom, ein dadurch ausgeloester Reset
      bricht den Upload ab. Nach erfolgreichem Upload startet das Modul
      automatisch neu und die WLAN-Verbindung bricht dabei kurz ab - das ist
      normal.
    </div>
  </div>
</div><div id="stab-wifi">
  <div class="card">
    <div class="card-title">WiFi Einstellungen</div>
    <div class="sl-wrap">
      <div class="sl-head"><span>SSID</span></div>
      <input type="text" id="w-ssid" maxlength="31">
    </div>
    <div class="sl-wrap">
      <div class="sl-head"><span>Passwort</span></div>
      <input type="password" id="w-pass" maxlength="63">
    </div>
    <div class="sl-wrap">
      <div class="sl-head"><span>AP IP-Adresse</span></div>
      <input type="text" id="w-ip" maxlength="15" placeholder="192.168.1.1">
      <div style="font-size:11px;color:var(--sub);margin-top:3px">(Neue IP gilt nach Neustart)</div>
    </div>
    <div class="sl-wrap">
      <div class="sl-head"><span>Ger&auml;tename (TBS Agent)</span></div>
      <input type="text" id="w-dev" maxlength="23">
      <div style="font-size:11px;color:var(--sub);margin-top:3px">(Im Agent sichtbar als Name@Adresse)</div>
    </div>
    <button class="btn btn-p" onclick="snd_saveWifi()">Speichern</button>
  </div>

  <!-- "WLAN dauerhaft an" - schaltet den AP beim Booten fest ein und haelt
       ihn dauerhaft an; ueberstimmt GPIO13-Bootpin, den manuellen Schalter
       und den Auto-Failsafe weiter unten komplett (siehe config.WifiAlwaysOn
       in config.h, wifiFailsafeCheck()/setup() in ESP32-RC-Sound.ino).
       Default AN. -->
  <div class="card" id="c-wifi-always" style="margin-top:10px">
    <div class="card-title">WLAN dauerhaft aktiv (empfohlen)</div>
    <div class="row" style="margin-bottom:10px">
      <span class="lbl">Beim Start an, bleibt dauerhaft an</span>
      <input type="checkbox" id="w-always-on" onchange="snd_onWifiAlwaysOnChange()">
    </div>
    <button class="btn btn-p" onclick="snd_saveWifi()">Speichern</button>
    <div style="font-size:12px;color:var(--sub);margin-top:8px">
      Modul ist nach jedem Einschalten sofort per WLAN erreichbar, ohne
      GPIO13-Bootpin oder manuellen Schalter. Ist diese Option aktiv, wirken
      "WLAN automatisch bei Signalverlust" weiter unten sowie der GPIO13-
      Bootpin nicht mehr (dort ausgegraut). Eine Aenderung hier wird zwar
      sofort gespeichert, wirkt sich aber wie die anderen Einstellungen auf
      dieser Seite erst ab dem naechsten Neustart des Moduls aus - das WLAN
      bleibt also bis dahin im aktuellen Zustand.
    </div>
  </div>

  <!-- Manueller WLAN-Schalter - schaltet den AP sofort ein/aus, ohne
       Neustart und ohne das Boot-Verhalten (WifiPin/GPIO13) zu aendern
       (siehe handleApiWifiEnable() in WebServerManager.cpp). -->
  <div class="card" id="c-wifi-now" style="margin-top:10px">
    <div class="card-title">WLAN jetzt</div>
    <div class="row" style="margin-bottom:10px">
      <span class="lbl">Status</span>
      <span id="wifi-status-now">-</span>
    </div>
    <button class="btn btn-s" id="wifi-toggle-btn" onclick="toggleWifiNow()">...</button>
    <div style="font-size:12px;color:var(--sub);margin-top:8px">
      Schaltet den WLAN-Access-Point sofort ein oder aus - wirkt nur fuer die
      aktuelle Laufzeit, ohne Einfluss auf das Verhalten beim naechsten
      Einschalten des Moduls.
    </div>
  </div>

  <!-- WLAN automatisch aktivieren, wenn ueber die eingestellte Zeit kein
       gueltiges RC-Signal anliegt (siehe wifiFailsafeCheck() in
       ESP32-RC-Sound.ino) - z.B. wenn kein Sender gebunden ist. -->
  <div class="card" id="c-wifi-auto" style="margin-top:10px">
    <div class="card-title">WLAN automatisch bei Signalverlust</div>
    <div id="sb-wifi-auto-overridden-hint" class="hidden" style="font-size:12px;color:var(--yellow);margin-bottom:8px">
      Wirkungslos, solange "WLAN dauerhaft aktiv" oben eingeschaltet ist.
    </div>
    <div class="row" style="margin-bottom:10px">
      <span class="lbl">Aktivieren</span>
      <input type="checkbox" id="w-auto-enable">
    </div>
    <div class="row">
      <span class="lbl">Timeout ohne RC-Signal (s)</span>
      <input type="number" id="w-auto-timeout" min="5" max="240" style="width:100px">
    </div>
    <button class="btn btn-p" style="margin-top:10px" onclick="snd_saveWifi()">Speichern</button>
    <div style="font-size:12px;color:var(--sub);margin-top:8px">
      Schaltet das WLAN automatisch ein, wenn ueber die eingestellte Zeit kein
      gueltiges RC-Signal (CRSF/SBUS) anliegt. Bleibt danach an, bis es
      manuell ausgeschaltet wird oder das Modul neu startet - kein
      "Flackern", wenn das Signal zwischenzeitlich zurueckkehrt.
    </div>
  </div>
</div><div id="stab-debug">
  <div class="card">
    <div style="display:flex;align-items:center;justify-content:space-between;margin-bottom:10px">
      <span class="card-title" style="margin:0">BUS Kan&auml;le</span>
      <button class="btn btn-s" style="width:auto;padding:4px 12px;margin:0;font-size:11px" onclick="loadDebug()">&#8635; Aktualisieren</button>
    </div>
    <div class="dbg-grid" id="dbg-ch"></div>
  </div>
  <div class="card">
    <div class="card-title">PWM Pins</div>
    <div id="dbg-pwm"></div>
  </div>
  <div class="card">
    <div class="card-title">WAV Dateien</div>
    <div id="dbg-wav"></div>
  </div>
  <div class="card">
    <div class="card-title">GPS (Port B oder C)</div>
    <div id="dbg-gps"></div>
  </div>
  <div class="card">
    <div class="card-title">S.Port Diagnose</div>
    <div id="dbg-sport-diag"></div>
  </div>
  <div class="card">
    <div class="card-title">CRSF Diagnose</div>
    <div id="dbg-crsf-diag"></div>
  </div>
  <div class="card">
    <div class="card-title">Konfiguration</div>
    <div id="dbg-cfg" class="mono" style="line-height:1.9;color:var(--sub)"></div>
  </div>
</div></div>

  <div class="card" id="c-kombi">
    <div class="card-title">Kombimodul <span style="color:var(--orange,#e3932a);font-size:10px;border:1px solid currentColor;border-radius:4px;padding:0 4px">NEU</span></div>
    <div class="row"><label>Sounds im Lua-Men&uuml;</label><select id="k-luasnd"><option value="8">1-8 (Standard)</option><option value="12">1-12</option><option value="16">1-16</option><option value="20">1-20</option><option value="23">1-23 (Maximum)</option></select></div>
    <div class="row"><label>Status-LED</label><select id="k-led"><option value="48">IO48 (DevKitC-1 v1.0)</option><option value="38">IO38 (DevKitC-1 v1.1)</option></select></div>
    <div style="font-size:12px;color:var(--sub);margin:4px 0 8px">In der Web-UI sind immer alle 24 Sounds einstellbar, im Lua-Men&uuml; h&ouml;chstens 23 (das Men&uuml; hat max. 255 Felder). Bleibt die Status-LED dunkel, auf IO38 umstellen. Beides wirkt nach einem Neustart.</div>
    <button class="btn btn-primary" onclick="saveKombi()">Speichern</button>
  </div>
<span id="sb-hv" hidden></span>
<div class="snd"><div id="toast"></div></div>
</main>


<script src="/rc_common.js"></script>
<script>
let state={},curOut=-1,pollTimer=null;
const $ = id => document.getElementById(id);

function showTab(t){
  ['overview','outputs','rc','wifi','debug'].forEach(n=>{
    $('tab-'+n).style.display=(n===t?'block':'none');
  });
  document.querySelectorAll('.tab').forEach((el,i)=>{
    el.classList.toggle('active',['overview','outputs','rc','wifi','debug'][i]===t);
  });
  if(t==='debug') startPoll(); else stopPoll();
  if(t==='overview'||t==='outputs'||t==='wifi') fetchStatus();
  if(t==='rc'||t==='wifi') fetchConfig();
}

async function api(path,opts={}){
  try{const r=await fetch(path,opts);return r.ok?r.json():null;}catch{return null;}
}

async function fetchStatus(){
  const d=await api('/api/status');
  if(!d)return;
  state=d;
  const dot=$('dot');
  dot.className='dot '+(d.bus_ok?'ok':'err');
  $('hv').textContent=(d.version||'v?')+' · '+d.rc_system_name+(d.bus_ok?' · Signal OK':' · Kein Signal');
  const apEl=$('wifi-ap-state');
  if(apEl) apEl.textContent=d.wifi_ap_active?'an':'aus';
  renderTiles(d);
  if(curOut>=0) renderOutDetail(d,curOut);
  // Kanalwerte auf Startseite
  if(d.channels && d.bus_ok){
    $('card-channels').style.display='';
    $('ch-grid').innerHTML=d.channels.map((v,i)=>
      `<div style="background:var(--surf2);border:1px solid var(--border);border-radius:4px;padding:4px 6px">
       <span style="color:var(--sub)">CH${i+1}</span> ${v}</div>`
    ).join('');
  } else {
    $('card-channels').style.display='none';
  }
}

function renderTiles(d){
  const g=$('tile-grid');
  g.innerHTML='';
  d.outputs.forEach((o,i)=>{
    const t=document.createElement('div');
    t.className='tile'+(o.on?' on':'')+(o.override?' override':'');
    t.innerHTML=
      `<div class="tile-name" style="cursor:pointer" onclick="showOutTab(${i})">${o.name}</div>`+
      `<div class="tile-sub">${o.on
        ?'<span class="badge badge-ok">AN</span>'
        :'<span class="badge badge-err">AUS</span>'}</div>`+
      (o.override?'<span class="lock">🔒</span>':'')+
      `<div class="tile-btns">`+
      `<button class="tile-btn-on"  onclick="event.stopPropagation();tileSwitch(${i},true)">AN</button>`+
      `<button class="tile-btn-off" onclick="event.stopPropagation();tileSwitch(${i},false)">AUS</button>`+
      `<button class="tile-btn-rel" onclick="event.stopPropagation();tileRelease(${i})">Frei</button>`+
      `<button class="tile-btn-cfg" onclick="event.stopPropagation();showOutTab(${i})">Cfg</button>`+
      `</div>`;
    g.appendChild(t);
  });
}

async function tileSwitch(i,on){
  await api('/api/switch',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({ch:i,on})});
  fetchStatus();
}
async function tileRelease(i){
  await api('/api/switch',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({ch:i,release:true})});
  fetchStatus();
}
function showOutTab(i){
  showTab('outputs');
  showOut(i);
}

function showOutList(){
  $('out-list').style.display='';
  $('out-detail').style.display='none';
  curOut=-1;
}
function showOut(i){
  curOut=i;
  $('out-list').style.display='none';
  $('out-detail').style.display='block';
  if(state.outputs) renderOutDetail(state,i);
  fetchStatus();
}
// NEU V3.10: Kanal-Quelle-Dropdown fuer einen Slot (1/2/3) aufbauen. Slot 2/3
// bekommen zusaetzlich die Option "Nicht verwendet" (255) - Konfig1 ist immer
// aktiv, Konfig2/3 sind optional (siehe evalSlotActive() in der .ino: k=255
// matched keinen der Bereiche und ist damit automatisch "nie aktiv").
function kanalOptionsHtml(slot,val){
  const kGrp=[{l:'Einzelkanal',v:Array.from({length:8},(_,n)=>n)},
              {l:'Kanal_L (<800)',v:Array.from({length:16},(_,n)=>20+n)},
              {l:'Kanal_H (>1200)',v:Array.from({length:16},(_,n)=>40+n)},
              {l:'MKan1',v:Array.from({length:8},(_,n)=>80+n)},   // NEU V3.00
              {l:'MKan2',v:Array.from({length:8},(_,n)=>88+n)}];  // NEU V3.00
  let h=`<select id="sel-kanal${slot}" style="width:100%" onchange="saveOutKanal(${slot})">`;
  if(slot>1){
    h+=`<optgroup label="Optionen"><option value="255"${val===255?' selected':''}>Nicht verwendet</option></optgroup>`;
  }
  kGrp.forEach(g=>{
    h+=`<optgroup label="${g.l}">`;
    // NEU V3.00: Nummerierung ueber die Position IN der Gruppe (idx), nicht
    // mehr ueber v%20 - das war nur zufaellig korrekt, solange alle Gruppen
    // bei einem Vielfachen von 20 anfingen (MKan2 beginnt bei 88).
    g.v.forEach((v,idx)=>{
      const n=(idx+1).toString().padStart(2,'0');
      h+=`<option value="${v}"${val===v?' selected':''}>${g.l.split(' ')[0]} ${n}</option>`;
    });
    h+='</optgroup>';
  });
  h+='</select>';
  return h;
}

// NEU V3.10: kompletten Konfig-Block (Kanal/Blink/PWM) fuer einen Slot
// (1/2/3) eines Ausgangs aufbauen - Slot 1 = bisheriges Verhalten (immer
// aktiv), Slot 2/3 = zusaetzliche, optionale Konfigurationen mit eigener
// Kanal-Quelle. Prioritaet bei mehreren aktiven Slots: 1 > 2 > 3 (siehe
// Output() in der .ino).
function buildSlotConfigHtml(slot,i,kanalVal,modeVal,pwmVal,isCrsf){
  const kHtml=kanalOptionsHtml(slot,kanalVal);

  const bv=[[0,'Dauerlicht'],[1,'0,01s'],[2,'0,02s'],[3,'0,03s'],[5,'0,05s'],
    [10,'0,1s'],[25,'0,25s'],[50,'0,5s'],[75,'0,75s'],[100,'1s'],
    [150,'1,5s'],[200,'2s'],[210,'2,1s'],[220,'2,2s'],[230,'2,3s'],[240,'2,4s'],[250,'2,5s']];
  const mInt=modeVal||0;
  const mH=(mInt>>8)&0xFF, mL=mInt&0xFF;
  let mAnHtml=`<select id="sel-mode-an${slot}" style="width:100%" onchange="saveOutMode(${slot})">`;
  bv.forEach(([v,l])=>mAnHtml+=`<option value="${v}"${mH===v?' selected':''}>${v===0?l:'Blinken '+l+' an'}</option>`);
  mAnHtml+='</select>';
  let mAusRow='';
  if(mH>0){
    let h=`<select id="sel-mode-aus${slot}" style="width:100%" onchange="saveOutMode(${slot})">`;
    bv.filter(([v])=>v>0).forEach(([v,l])=>h+=`<option value="${v}"${mL===v?' selected':''}>${'Blinken '+l+' aus'}</option>`);
    h+='</select>';
    mAusRow=`<div class="row"><label>Blink AUS-Zeit</label>${h}</div>`;
  }

  // PWM-Quelle: MWprop nur bei CRSF
  const pwm=pwmVal;
  const isMW=(pwm>=200&&pwm<=207);
  const isFest=!isMW;
  let pHtml=`<select id="sel-pwm-src${slot}" style="width:100%" onchange="saveOutPwmSrc(${slot})">`;
  pHtml+=`<option value="255"${isFest&&pwm===255?' selected':''}>Festwert (0-255)</option>`;
  if(isCrsf){
    pHtml+=`<option value="${200+i}"${isMW?' selected':''}>MWprop (Kanal=${i+1}, auto)</option>`;
  }
  pHtml+='</select>';

  // PWM-Slider (bei Festwert)
  const sv=isFest?pwm:255;
  const slRow=isFest?`<div class="row"><label>PWM-Wert: <b id="pwm-val${slot}">${sv}</b></label>
    <input type="range" min="0" max="255" step="5" value="${sv}" style="width:100%;margin-top:4px"
      oninput="$('pwm-val${slot}').textContent=this.value" onchange="saveOutPwm(${slot},this.value)"></div>`:'';

  return `
    <div class="row"><label>Kanal-Quelle</label>${kHtml}</div>
    <div class="row"><label>Blink AN-Zeit</label>${mAnHtml}</div>
    ${mAusRow}
    <div class="row"><label>PWM-Quelle</label>${pHtml}</div>
    ${slRow}`;
}

function renderOutDetail(d,i){
  const o=d.outputs[i];
  $('out-title').textContent=o.name;
  const sb=$('out-state-big');
  sb.textContent=o.on?'AN':'AUS';
  sb.className='state-big '+(o.on?'state-on':'state-off');
  const isCrsf=(state&&state.rc_system_boot===4);

  // NEU V3.10: bis zu 3 Konfigurationen pro Ausgang, Prioritaet 1>2>3 -
  // Konfig1 = bisheriges kanal/mode/pwm, Konfig2/3 = kanal2/mode2/pwm2 bzw.
  // kanal3/mode3/pwm3 (jeweils optional, Default "Nicht verwendet"=255).
  const slot1Html=buildSlotConfigHtml(1,i,o.kanal,o.mode,o.pwm,isCrsf);
  const slot2Html=buildSlotConfigHtml(2,i,o.kanal2,o.mode2,o.pwm2,isCrsf);
  const slot3Html=buildSlotConfigHtml(3,i,o.kanal3,o.mode3,o.pwm3,isCrsf);

  $('out-cfg').innerHTML=`
    <div class="card-title">Konfig 1 – ${o.name} (h&ouml;chste Priorit&auml;t)</div>
    ${slot1Html}
    <hr class="sep">
    <div class="card-title">Konfig 2 (optional)</div>
    ${slot2Html}
    <hr class="sep">
    <div class="card-title">Konfig 3 (optional, niedrigste Priorit&auml;t)</div>
    ${slot3Html}
    <hr class="sep">
    <div class="row"><label>Override</label><span>${o.override?'<b>Aktiv</b>':'Inaktiv'}</span></div>
    <div class="row" style="margin-top:6px">
      <button class="btn btn-primary" onclick="saveOutName()">Name ändern</button>
    </div>`;
}
// NEU V3.10: slot 1/2/3 -> jeweils eigenes JSON-Feld (kanal/kanal2/kanal3)
function slotField(base,slot){ return slot===1?base:base+slot; }
async function saveOutKanal(slot){
  if(curOut<0)return;
  const body={ch:curOut};
  body[slotField('kanal',slot)]=parseInt($('sel-kanal'+slot).value);
  await api('/api/output',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify(body)});
  fetchStatus();
}
async function saveOutMode(slot){
  if(curOut<0)return;
  const an=parseInt($('sel-mode-an'+slot).value);
  const ausEl=$('sel-mode-aus'+slot);
  const aus=ausEl?parseInt(ausEl.value):an;
  const body={ch:curOut};
  body[slotField('mode',slot)]=(an<<8)|aus;
  await api('/api/output',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify(body)});
  fetchStatus();
}
async function saveOutPwmSrc(slot){
  if(curOut<0)return;
  const body={ch:curOut};
  body[slotField('pwm',slot)]=parseInt($('sel-pwm-src'+slot).value);
  await api('/api/output',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify(body)});
  fetchStatus();
}
async function saveOutPwm(slot,val){
  if(curOut<0)return;
  const body={ch:curOut};
  body[slotField('pwm',slot)]=parseInt(val);
  await api('/api/output',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify(body)});
}
async function saveOutName(){
  if(curOut<0)return;
  const nm=prompt('Neuer Name (max. 16 Zeichen):',state.outputs[curOut].name);
  if(!nm)return;
  await api('/api/output',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({ch:curOut,name:nm.substring(0,16)})});
  fetchStatus();
}

async function switchOut(on){
  if(curOut<0)return;
  await api('/api/switch',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({ch:curOut,on})});
  fetchStatus();
}
async function releaseOut(){
  if(curOut<0)return;
  await api('/api/switch',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({ch:curOut,release:true})});
  fetchStatus();
}
async function releaseAll(){
  for(let i=0;i<8;i++)
    await api('/api/switch',{method:'POST',headers:{'Content-Type':'application/json'},
      body:JSON.stringify({ch:i,release:true})});
  fetchStatus();
}

async function fetchConfig(){
  const d=await api('/api/config');
  if(!d)return;
  $('sel-rc').value=d.rc_system;
  // SBUS: Einkanal-Kanal (crsf_channel wird fuer SBUS als Einkanal-Kanal genutzt)
  // Wenn einkanal_mode==999 -> Deaktiviert im Kanal-Dropdown
  const ek=parseInt(d.einkanal_mode);
  $('sel-sbus-ch').value=(ek===999)?999:parseInt(d.crsf_channel);
  $('sel-ek').value=(ek===999)?0:ek;
  // CRSF: Modul-Adresse
  $('sel-adr').value=d.modul_adress;
  // NEU V3.00: MKan-Gruppen
  $('sel-mkan-adr0').value=d.ek_gadr0;
  $('sel-mkan-adr1').value=d.ek_gadr1;
  $('sel-mkan-ch0').value=d.sbus_gch0;
  $('sel-mkan-mode0').value=d.sbus_gmode0;
  $('sel-mkan-ch1').value=d.sbus_gch1;
  $('sel-mkan-mode1').value=d.sbus_gmode1;
  $('inp-dnam').value=d.dnam;
  $('inp-ssid').value=d.ssid;
  $('inp-pass').value=d.pass;
  $('inp-ip').value=d.ip;
  $('sel-wifi-auto').value=d.wifi_auto?1:0;
  $('inp-wifi-auto-to').value=d.wifi_auto_timeout;
  $('sel-wifi-always').value=d.wifi_always_on?1:0;
  updateWifiAlwaysOnHint();
  // NEU v4.04: UART B/C
  rcFillSelect($('sel-portb'),d.port_roles||RC_ROLES,d.portb_role);
  rcFillSelect($('sel-portc'),d.port_roles||RC_ROLES,d.portc_role);
  fillBaud($('sel-portb-baud'),d.portb_baud);
  fillBaud($('sel-portc-baud'),d.portc_baud);
  window._portRole=[d.portb_role,d.portc_role];
  $('row-portc').style.display=d.has_portc?'':'none';
  $('row-portc-baud').style.display=d.has_portc?'':'none';
  $('portc-dup').style.display=d.portc_dup?'block':'none';
  updateRcRows(d.rc_system, ek);
  window._rcBoot=parseInt(d.rc_system);
}

// Sichtbarkeit der Rows je nach RC-System und Einkanal-Status
// RC-Sound Logik: CRSF -> nur Modul-Adresse; SBUS -> Kanal + (wenn aktiv) Modus
function updateRcRows(rc, ek){
  const isCrsf=(parseInt(rc)===4);
  const ekAktiv=(ek!==999);
  $('row-sbus-ch').style.display=isCrsf?'none':'';
  $('row-ek').style.display=(isCrsf||!ekAktiv)?'none':'';
  $('row-adr').style.display=isCrsf?'':'none';
  // NEU V3.00: MKan-Gruppen - Adresse bei CRSF, Kanal+Modus bei SBUS
  $('row-mkan-adr0').style.display=isCrsf?'':'none';
  $('row-mkan-adr1').style.display=isCrsf?'':'none';
  $('row-mkan-ch0').style.display=isCrsf?'none':'';
  $('row-mkan-mode0').style.display=isCrsf?'none':'';
  $('row-mkan-ch1').style.display=isCrsf?'none':'';
  $('row-mkan-mode1').style.display=isCrsf?'none':'';
}

// RC-System Wechsel: sofort speichern, bei SBUS<->CRSF Wechsel: Neustart
async function onRcChange(){
  const newRc=parseInt($('sel-rc').value);
  const curEk=parseInt($('sel-ek').value)||0;
  updateRcRows(newRc, $('sel-sbus-ch').value==999?999:curEk);
  const oldRc=window._rcBoot||newRc;
  const needsRestart=(newRc===4)!==(oldRc===4);
  await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({rc_system:newRc})});
  const hint=$('rc-hint');
  if(needsRestart){
    api('/api/restart',{method:'POST'});
    let s=5; hint.style.color='var(--yellow)';
    const t=setInterval(()=>{
      hint.textContent='Neustart… Verbinde in '+s+'s';
      if(--s<0){clearInterval(t);window.location.reload();}
    },1000);
  } else {
    hint.textContent='Gespeichert.'; hint.style.color='var(--green)';
    setTimeout(()=>{hint.textContent='';},2000);
  }
}

// SBUS Einkanal-Kanal: speichert crsf_channel; Deaktiviert -> einkanal_mode=999
async function onSbusChChange(){
  const v=parseInt($('sel-sbus-ch').value);
  const deakt=(v===999);
  // Einkanal-Modus Row ein/ausblenden
  $('row-ek').style.display=deakt?'none':'';
  if(deakt){
    await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},
      body:JSON.stringify({einkanal_mode:999,crsf_channel:parseInt($('sel-sbus-ch').value)})});
  } else {
    // Setzt crsf_channel auf gewaehlten SBUS Kanal; einkanal_mode bleibt
    await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},
      body:JSON.stringify({crsf_channel:v,einkanal_mode:parseInt($('sel-ek').value)||0})});
  }
}

// Einkanal-Modus (Normal / WM Adr 0-3)
async function onEkChange(){
  await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({einkanal_mode:parseInt($('sel-ek').value)})});
}

// CRSF Modul-Adresse
// v4.05: Baudraten-Auswahl (Liste aus /rc_common.js mit "Auto", eigener
// Wert bleibt erhalten)
function fillBaud(sel,cur){ rcFillBaud(sel,cur); }
// NEU v4.04: Rolle UART B/C speichern, wirkt nach Neustart. v4.05: neue
// Rolle schlaegt ihre Baudrate vor (Raw/Aus behalten die bisherige).
async function onPortChange(){
  const r=[parseInt($('sel-portb').value),parseInt($('sel-portc').value)];
  [['sel-portb-baud',0],['sel-portc-baud',1]].forEach(([id,i])=>{
    const b=rcRoleDefaultBaud(r[i]);
    if(window._portRole&&r[i]!==window._portRole[i]&&b&&r[i]!==4) fillBaud($(id),b);
  });
  window._portRole=r;
  await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({portb_role:r[0],portc_role:r[1],
      portb_baud:parseInt($('sel-portb-baud').value),
      portc_baud:parseInt($('sel-portc-baud').value)})});
  const h=$('port-hint');
  h.innerHTML='Gespeichert, wirkt nach Neustart. <a href="#" onclick="restartForPorts();return false">Jetzt neu starten</a>';
  h.style.color='var(--green)';
}
function restartForPorts(){
  api('/api/restart',{method:'POST'});
  let s=5; const h=$('port-hint'); h.style.color='var(--yellow)';
  const t=setInterval(()=>{
    h.textContent='Neustart… Verbinde in '+s+'s';
    if(--s<0){clearInterval(t);window.location.reload();}
  },1000);
}
async function onAdrChange(){
  await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({modul_adress:parseInt($('sel-adr').value)})});
}
// NEU V3.00: MKan-Gruppe g (0 oder 1) - Adresse (CRSF) bzw. Kanal+Modus (SBUS)
async function onMkanChange(g){
  const isCrsf=(parseInt($('sel-rc').value)===4);
  const body={};
  if(isCrsf){
    body['ek_gadr'+g]=parseInt($('sel-mkan-adr'+g).value);
  } else {
    body['sbus_gch'+g]=parseInt($('sel-mkan-ch'+g).value);
    body['sbus_gmode'+g]=parseInt($('sel-mkan-mode'+g).value);
  }
  await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify(body)});
}
async function saveRc(){} // nicht mehr per Button benoetigt
async function saveWifi(){
  const ip=$('inp-ip').value.trim();
  await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({dnam:$('inp-dnam').value,ssid:$('inp-ssid').value,pass:$('inp-pass').value,ip:ip})});
  api('/api/restart',{method:'POST'});
  let s=5;
  const t=setInterval(()=>{
    if(--s<0){clearInterval(t);window.location.href='http://'+ip;}
  },1000);
  alert('WiFi gespeichert. Verbinde in 5s mit '+ip+'…');
}
async function doReset(){
  await api('/api/reset',{method:'POST'});
  api('/api/restart',{method:'POST'});
  let s=5;
  const t=setInterval(()=>{ if(--s<0){clearInterval(t);window.location.reload();} },1000);
  alert('Zurückgesetzt. Neustart in 5s…');
}

// ─── WLAN jetzt (NEU v2.00) ────────────────────────────────────────────
// Wirkt sofort, ohne Neustart und ohne die Konfiguration zu speichern
// (siehe handleWifi() in webui.h) - unabhängig vom Auto-Failsafe unten.
async function toggleWifi(enable){
  await api('/api/wifi',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({enable})});
  fetchStatus();
}
async function saveWifiAuto(){
  await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({
      wifi_auto:$('sel-wifi-auto').value==='1',
      wifi_auto_timeout:parseInt($('inp-wifi-auto-to').value)||60
    })});
}

// ─── WLAN dauerhaft aktiv (NEU v2.02) ──────────────────────────────────
// Rein persistierend, wirkt erst ab dem naechsten Neustart (siehe
// webui_init()/wifiFailsafeCheck() in der .ino) - bewusst kein sofortiges
// toggleWifi() hier, analog zum Soundmodul-Projekt seit dessen v7.15.
async function onWifiAlwaysOnChange(){
  await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({wifi_always_on:$('sel-wifi-always').value==='1'})});
  updateWifiAlwaysOnHint();
}
function updateWifiAlwaysOnHint(){
  $('wifi-auto-overridden-hint').style.display=($('sel-wifi-always').value==='1')?'block':'none';
}

// ─── Konfiguration Export/Import (NEU v2.00) ──────────────────────────
// Export: liest /api/config (globale Einstellungen) + /api/status (Ausgänge)
// und bietet das Ergebnis als Datei zum Download an - ganz normaler
// Browser-Download, da der Webserver direkt auf dem Modul läuft. Kein SD-
// Slot nötig/vorhanden - bewusst rein Browser-basiert.
// v4.06: Konfiguration einsammeln, gemeinsam fuer Export als Datei und
// Backup im Flash. null = Geraet nicht erreichbar.

// v1.00 Kombimodul: Sicherungen erkennen und einspielen. Kombimodul und
// Multiswitch haben "outputs", die Sound-Basis "motor"/"sounds".
function isMsData(d){return !!(d&&d.settings&&Array.isArray(d.outputs));}
function isSndData(d){return !!(d&&d.settings&&d.motor&&Array.isArray(d.sounds));}
function isKombiData(d){return isMsData(d)||isSndData(d);}
function sndImportP(d){return new Promise(function(res){snd_importConfigData(d,res);});}
async function importAny(data){
  if(isMsData(data)) return importConfigData(data);
  await rcBackupSnapshot(collectConfig);   // vorher automatisch im Flash sichern
  await sndImportP(data);
  await api('/api/save',{method:'POST'});
  alert('Konfiguration importiert. Seite wird neu geladen…');
  window.location.reload();
}
async function collectConfig(){
  const cfg=await api('/api/config');
  const st=await api('/api/status');
  const snd=await api('/api/sconfig');   // v1.00: Soundteil
  if(!cfg||!st||!snd) return null;
  return {
    device:'RC-Multiswitch-Sound', version:st.version||'', exported:new Date().toISOString(),
    settings:cfg,
    outputs:st.outputs.map((o,i)=>({ch:i,name:o.name,kanal:o.kanal,pwm:o.pwm,mode:o.mode,
      kanal2:o.kanal2,pwm2:o.pwm2,mode2:o.mode2,     // NEU V3.10
      kanal3:o.kanal3,pwm3:o.pwm3,mode3:o.mode3})),  // NEU V3.10
    sound:snd
  };
}
async function exportConfig(){
  const data=await collectConfig();
  if(!data){ alert('Export fehlgeschlagen (Gerät nicht erreichbar).'); return; }
  const cfg=data.settings;
  const text=JSON.stringify(data,null,2);
  let name='ms-sound-config';
  if(cfg.dnam) name+='-'+String(cfg.dnam).replace(/[^A-Za-z0-9_-]+/g,'_');
  name+='.json';
  const blob=new Blob([text],{type:'application/json'});
  const url=URL.createObjectURL(blob);
  const a=document.createElement('a');
  a.href=url; a.download=name;
  document.body.appendChild(a); a.click(); document.body.removeChild(a);
  setTimeout(()=>URL.revokeObjectURL(url),1000);
}

// Import: Datei einlesen, grob validieren, bestätigen lassen (WLAN-
// Zugangsdaten werden überschrieben), dann Schritt für Schritt über die
// bestehenden POST-Endpunkte einspielen und zum Schluss ohne Neustart
// speichern (/api/save).
function importConfigFile(file){
  if(!file) return;
  const reader=new FileReader();
  reader.onload=function(){
    let data;
    try{ data=JSON.parse(reader.result); }catch(e){ alert('Ungültige JSON-Datei.'); return; }
    if(!isKombiData(data)){
      alert('Datei enthält keine gültige Konfiguration (Kombimodul, Multiswitch oder Soundmodul).'); return;
    }
    if(!confirm('Aktuelle Konfiguration (inkl. WLAN-Zugangsdaten) durch die Datei ersetzen?')) return;
    importAny(data);
  };
  reader.onerror=function(){ alert('Datei konnte nicht gelesen werden.'); };
  reader.readAsText(file);
  // Sonst laesst sich dieselbe Datei kein zweites Mal auswaehlen (onchange
  // feuert bei unveraendertem value nicht erneut).
  $('cfg-import-file').value='';
}
async function importConfigData(data){
  // v4.06: bisherigen Stand vorher automatisch im Flash sichern
  await rcBackupSnapshot(collectConfig);
  const s=data.settings||{};
  await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({
      rc_system:s.rc_system, crsf_channel:s.crsf_channel, einkanal_mode:s.einkanal_mode,
      modul_adress:s.modul_adress, dnam:s.dnam, ssid:s.ssid, pass:s.pass, ip:s.ip,
      wifi_auto:s.wifi_auto, wifi_auto_timeout:s.wifi_auto_timeout,
      wifi_always_on:s.wifi_always_on,
      portb_role:s.portb_role, portc_role:s.portc_role,   // NEU v4.04
      portb_baud:s.portb_baud, portc_baud:s.portc_baud,   // v4.05
      ek_gadr0:s.ek_gadr0, ek_gadr1:s.ek_gadr1,           // v1.00: MKan-Gruppen
      sbus_gch0:s.sbus_gch0, sbus_gch1:s.sbus_gch1,
      sbus_gmode0:s.sbus_gmode0, sbus_gmode1:s.sbus_gmode1,
      lua_sounds:s.lua_sounds, led_pin:s.led_pin          // v1.00: Kombimodul
    })});
  for(const o of (data.outputs||[])){
    if(o.ch===undefined) continue;
    await api('/api/output',{method:'POST',headers:{'Content-Type':'application/json'},
      body:JSON.stringify({ch:o.ch,kanal:o.kanal,pwm:o.pwm,mode:o.mode,name:o.name,
        kanal2:o.kanal2,pwm2:o.pwm2,mode2:o.mode2,     // NEU V3.10
        kanal3:o.kanal3,pwm3:o.pwm3,mode3:o.mode3})}); // NEU V3.10
  }
  if(data.sound) await sndImportP(data.sound);   // v1.00: Soundteil
  await api('/api/save',{method:'POST'});
  alert('Konfiguration importiert. Seite wird neu geladen…');
  window.location.reload();
}

// ─── Firmware-Update per WLAN (OTA, NEU v2.00) ─────────────────────────
// Echtes multipart/form-data an /api/otaupdate (RC_Common web_common.cpp) - Fortschritt per XHR-upload.onprogress,
// da fetch() das (noch) nicht zuverlaessig unterstuetzt. Bei Erfolg startet
// das Modul selbst neu; die Antwort trifft trotzdem noch ein, bevor die
// Verbindung tatsaechlich abbricht.
function uploadOtaFile(file){
  // v4.05: gemeinsamer Upload aus /rc_common.js
  rcOtaUpload(file,$('ota-status'),$('ota-file'),function(ok,err){
    if(!ok) alert('Update fehlgeschlagen ('+rcErrText(err)+').');
  },()=>rcBackupSnapshot(collectConfig));   // v4.06: vorher Backup im Flash
}

// ─── Backup im Flash des Moduls (v4.06, RC_Common config_backup) ───────
function fbStatus(t,err){ const e=$('fb-status'); e.textContent=t; e.style.color=err?'var(--red)':'var(--green)'; }
async function flashBackupSave(){
  const data=await collectConfig();
  if(!data){ fbStatus('Gerät nicht erreichbar',1); return; }
  const r=await rcBackupSave(data,false);
  if(r.ok){ fbStatus('Im Modul gesichert'); if($('fb-list').style.display!=='none') flashBackupList(); }
  else fbStatus('Sichern fehlgeschlagen: '+rcErrText(r.error),1);
}
async function flashBackupList(){
  const r=await rcBackupList();
  if(!r.ok){ fbStatus('Fehler: '+rcErrText(r.error),1); return; }
  const n=rcBackupFill($('fb-select'),r.files);
  $('fb-list').style.display=n?'':'none';
  fbStatus(n?n+' Backup(s) im Modul':'Keine Backups im Modul',!n);
}
async function flashBackupRestore(){
  const name=$('fb-select').value;
  if(!name) return;
  const data=await rcBackupGet(name);
  if(!isKombiData(data)){ fbStatus('Backup ist keine gültige Konfiguration',1); return; }
  if(!confirm('Aktuelle Konfiguration (inkl. WLAN-Zugangsdaten) durch dieses Backup ersetzen?\n'+$('fb-select').selectedOptions[0].textContent)) return;
  importAny(data);
}
async function flashBackupDelete(){
  const name=$('fb-select').value;
  if(!name||!confirm('Backup im Modul löschen?\n'+$('fb-select').selectedOptions[0].textContent)) return;
  const r=await rcBackupDelete(name);
  if(r.ok) flashBackupList(); else fbStatus('Löschen fehlgeschlagen: '+rcErrText(r.error),1);
}

function startPoll(){pollTimer=setInterval(pollDebug,800);}
function stopPoll(){clearInterval(pollTimer);}
async function pollDebug(){
  const d=await api('/api/status');
  if(!d)return;
  let html='';
  html+=`BUS_OK: <b>${d.bus_ok?'JA':'NEIN'}</b> | RC: ${d.rc_system_name}<br>`;
  d.outputs.forEach((o,i)=>{
    html+=`Ausgang ${i+1} (${o.name}): ${o.on?'<b>AN</b>':'AUS'}`;
    if(o.override)html+=' 🔒';
    html+='<br>';
  });
  html+=`Einkanal-Data: 0x${d.einkanal_data_hex}<br>`;
  html+=`Kanäle: ${d.channels.map((v,i)=>`ch${i+1}=${v}`).join(' ')}<br>`;
  html+=`WLAN-AP: ${d.wifi_ap_active?'<b>an</b>':'aus'}<br>`;
  $('debug-body').innerHTML=html;

  // CRSF-Diagnose (NEU v2.00) - bei SBUS bleiben die Zaehler auf 0
  if(d.crsf_diag){
    const c=d.crsf_diag;
    let ch='';
    ch+=`Rohe Bytes: ${c.raw_bytes}<br>`;
    ch+=`Gültige Frames: ${c.valid_frames}<br>`;
    ch+=`CRC-Fehler: ${c.crc_errors}<br>`;
    ch+=`DEVICE_PING empfangen: ${c.device_pings}<br>`;
    ch+=`PARAMETER_READ empfangen: ${c.param_reads}<br>`;
    ch+=`PARAMETER_WRITE empfangen: ${c.param_writes}<br>`;
    if(c.fwd_down!==undefined){
      ch+=`Router A&rarr;B/C: ${c.fwd_down}, B/C&rarr;A: ${c.fwd_up}, verworfen: ${c.fwd_dropped}, CRC-Fehler B/C: ${c.down_crc}<br>`;
      if(c.dup_addr) ch+=`<b style="color:#c00">Doppelte Modul-Adresse in der Kette erkannt!</b><br>`;
    }
    $('crsf-diag-body').innerHTML=ch;
  } else {
    $('crsf-diag-body').innerHTML='Keine Daten';
  }

  // NEU v4.04: UART B/C
  if(d.ports){
    const p=d.ports;
    let ph2=`UART B: ${p.b}`+(p.has_c?` | UART C: ${p.c}`:'')+'<br>';
    if(p.c_dup) ph2+='<b style="color:#c00">UART C aus: gleiche Rolle wie UART B</b><br>';
    if(p.sport){
      p.sport.forEach((s,i)=>{
        ph2+=`LiPo ${i+1}: `+(s.online?`${s.cells}S, ${s.total.toFixed(2)} V, min. Zelle ${s.min.toFixed(2)} V`:'keine Antwort')+'<br>';
      });
      ph2+=`S.Port Polls: ${p.sport_polls}, Bytes: ${p.sport_bytes}, Frames: ${p.sport_frames}, CRC-Fehler: ${p.sport_crc}<br>`;
    }
    if(p.gps){
      const g=p.gps;
      ph2+=`GPS: ${g.fix?'Fix':'kein Fix'}, ${g.sats} Sat., ${g.kmh.toFixed(1)} km/h<br>`;
    }
    $('port-diag-body').innerHTML=ph2;
  }

  // MWprop
  let ph='';
  if(d.wm_prop){
    d.wm_prop.forEach((v,i)=>{ ph+=`Prop ${i+1}: ${v} (${Math.round(v/255*100)}%)<br>`; });
  }
  $('prop-body').innerHTML=ph||'Keine Daten';
}

// Init
fetchStatus();
showOutList();
</script>
<script>
var cfg={}, dbg={}, curSnd=1, lipoTmr=0;
var TABS=['motor','sounds','settings','wifi','lipo','debug'];

// ─── Toast ────────────────────────────────────────────────────────────────
var toastTmr=0;
// Fehlercodes der /api/sound*-Endpunkte in Klartext (Begruendung siehe
// handleApiSoundUploadData() im C++-Teil).
function errText(c){
  var m={
    sd_locked:'Datei ist schreibgeschuetzt oder gesperrt - Schreibschutz auf der SD-Karte aufheben (Windows: attrib -r X:\\*.wav)',
    sd_write :'Schreiben auf die SD-Karte fehlgeschlagen (Karte voll, schreibgeschuetzt oder defekt)',
    sd_busy  :'SD-Karte gerade belegt - bitte erneut versuchen',
    bad_name :'Ungueltiger Dateiname',
    aborted  :'Uebertragung abgebrochen'
  };
  return m[c] ? m[c] : c;
}
function toast(msg,err){
  var el=document.getElementById('toast');
  el.textContent=msg; el.className=err?'err show':'show';
  clearTimeout(toastTmr);
  toastTmr=setTimeout(function(){el.className=err?'err':'';},2200);
}

// ─── Tab-Navigation ───────────────────────────────────────────────────────
function snd_showTab(t){
  TABS.forEach(function(n){
    var el=document.getElementById('stab-'+n);
    if(el) el.className=(n===t?'':'hidden');
  });
  document.querySelectorAll('.tab').forEach(function(el,i){
    el.className='tab'+(TABS[i]===t?' active':'');
  });
  // "wifi" ist hier mit aufgenommen, damit die Karte "WLAN jetzt" den
  // aktuellen AP-/RC-Signal-Zustand zeigt (kommt ueber /api/debug, siehe
  // dbg.wifi in loadDebug()/renderWifiLiveStatus()).
  if(t==='debug'||t==='lipo'||t==='wifi') loadDebug();
  clearInterval(lipoTmr); lipoTmr=0;
  if(t==='lipo'||t==='debug'||t==='wifi') lipoTmr=setInterval(loadDebug,2000);
  // Datei-Status (vorhanden/Groesse) beim Betreten von Motor/Sounds aktuell
  // halten - z.B. wenn parallel per SD-Backup/-Karte etwas geaendert wurde.
  if(t==='motor'||t==='sounds') loadSoundFileStatus();
}

// ─── XHR-Helfer ──────────────────────────────────────────────────────────
function xget(url,_,cb){
  var x=new XMLHttpRequest();
  x.open('GET',url,true);
  x.onload=function(){if(cb)cb(x.responseText);};
  x.send();
}
function xpost(url,data,cb){
  var x=new XMLHttpRequest();
  x.open('POST',url,true);
  x.setRequestHeader('Content-Type','application/json');
  x.onload=function(){if(cb)cb(x.responseText);};
  x.onerror=function(){toast('Verbindungsfehler',1);};
  x.send(JSON.stringify(data));
}

// ─── Dropdown-Optionen ───────────────────────────────────────────────────
// Dropdown-Gruppen – aufgeteilt damit V3/V4 filtern kann
var G_BUS_LOW ={l:'BUS Kanal Low', o:function(){var a=[];for(var i=0;i<16;i++)a.push([i,'BUS Kanal Low '+(i<9?'0':'')+(i+1)]);return a;}()};
var G_BUS_HIGH={l:'BUS Kanal High',o:function(){var a=[];for(var i=0;i<16;i++)a.push([20+i,'BUS Kanal High '+(i<9?'0':'')+(i+1)]);return a;}()};
var G_PWM_LOW ={l:'PWM Pin Low',   o:function(){var a=[];for(var i=0;i<6;i++)a.push([40+i,'PWM Pin Low '+(i+1)]);return a;}()};
var G_PWM_HIGH={l:'PWM Pin High',  o:function(){var a=[];for(var i=0;i<6;i++)a.push([50+i,'PWM Pin High '+(i+1)]);return a;}()};
var G_PIN     ={l:'Eingang Pin',   o:function(){var a=[];for(var i=0;i<6;i++)a.push([60+i,'Eingang Pin '+(i+1)]);return a;}()};
var G_EK      ={l:'Einkanal',      o:function(){var a=[];for(var i=0;i<8;i++)a.push([70+i,'Einkanal '+(i<9?'0':'')+(i+1)]);return a;}()};
// "MKan" 80-95 (2 CRSF-Gruppenadressen x 8 Einzelschalter, siehe
// ESP32-RC-Sound.ino) - eine der Quellgruppen fuer Sound 1-24.
var G_MKAN    ={l:'Mehrfachkanal (MKan)', o:function(){var a=[];for(var i=0;i<16;i++)a.push([80+i,'MKan '+(i<9?'0':'')+(i+1)]);return a;}()};
var G_OPT     ={l:'Optionen',      o:[[200,'Dauerbetrieb an'],[999,'Deaktiviert']]};
var G_OPT_D   ={l:'Optionen',      o:[[999,'Deaktiviert']]};
var G_PWM_SPD ={l:'PWM Pin',       o:function(){var a=[];for(var i=0;i<6;i++)a.push([20+i,'PWM Pin '+(i+1)]);return a;}()};
var G_BUS_SPD ={l:'BUS Kanal',     o:function(){var a=[];for(var i=0;i<16;i++)a.push([i,'BUS Kanal '+(i<9?'0':'')+(i+1)]);return a;}()};

// Gruppen je nach HW-Config zusammenstellen
// hw < 2 (V1/V2): alle; hw >= 2 (V3/V4): nur BUS + Einkanal
function srcGroups(hw) {
  var g = [G_BUS_LOW, G_BUS_HIGH];
  if (hw < 2) { g.push(G_PWM_LOW, G_PWM_HIGH, G_PIN); }
  g.push(G_EK);
  g.push(G_MKAN);
  g.push(G_OPT);
  return g;
}
function spdGroups(hw) {
  var g = [G_BUS_SPD];
  if (hw < 2) g.push(G_PWM_SPD);
  g.push(G_OPT_D);
  return g;
}

function fillSel(id,groups){
  var sel=document.getElementById(id); if(!sel)return;
  sel.innerHTML='';
  groups.forEach(function(g){
    var og=document.createElement('optgroup'); og.label=g.l;
    g.o.forEach(function(o){
      var opt=document.createElement('option');
      opt.value=o[0]; opt.textContent=o[1]; og.appendChild(opt);
    });
    sel.appendChild(og);
  });
}
// id-Parameter, damit dieselbe SBUS-Kanalliste (16 Kanaele + "Deaktiviert")
// sowohl fuer "Einkanal Kanal" (ek-ch) als auch fuer die beiden SBUS-
// Gruppenkanaele (sbusgrp-ch0/1) verwendet werden kann.
function fillEkCh(id){
  var sel=document.getElementById(id); sel.innerHTML='';
  var og=document.createElement('optgroup'); og.label='SBUS Kanal';
  for(var i=0;i<16;i++){var o=document.createElement('option');o.value=i;o.textContent='SBUS Kanal '+(i<9?'0':'')+(i+1);og.appendChild(o);}
  sel.appendChild(og);
  var og2=document.createElement('optgroup'); og2.label='Optionen';
  var o2=document.createElement('option');o2.value=999;o2.textContent='Deaktiviert';og2.appendChild(o2);sel.appendChild(og2);
}
// Flache Optionsliste (Index == value), anders als fillSel() oben, das nur
// gruppierte <optgroup>-Daten kennt. Fuer Port-B-Modus genuegt eine flache
// Liste - Reihenfolge/Index kommt direkt vom Geraet (s.portb_options,
// ";"-getrennt, siehe uartRoleOptions() in RC_Common/uart_roles.h),
// damit Web-UI und CRSF/Lua-Menue garantiert dieselbe Liste zeigen.
function fillSelFlat(id,namesStr){
  var sel=document.getElementById(id); if(!sel)return;
  sel.innerHTML='';
  if(!namesStr)return;
  namesStr.split(';').forEach(function(name,i){
    var opt=document.createElement('option');
    opt.value=i; opt.textContent=name; sel.appendChild(opt);
  });
}
function fillModAdr(){
  var sel=document.getElementById('mod-adr'); sel.innerHTML='';
  for(var i=0;i<=20;i++){var o=document.createElement('option');o.value=i;o.textContent=i;sel.appendChild(o);}
}
// Die 2 MKan-Gruppenadressen (config.EK_Gruppen_Adresse[0..1]) - wie
// mod-adr eine WM/CRSF-Adresse 0-20, zusaetzlich "Deaktiviert" (255), damit
// eine ungenutzte Gruppe bewusst abgeschaltet werden kann.
function fillGroupAdr(id){
  var sel=document.getElementById(id); sel.innerHTML='';
  for(var i=0;i<=20;i++){var o=document.createElement('option');o.value=i;o.textContent=i;sel.appendChild(o);}
  var od=document.createElement('option');od.value=255;od.textContent='Deaktiviert';sel.appendChild(od);
}

// ─── Hilfsfunktionen ────────────────────────────────────────────────────
function sv(id,v){var el=document.getElementById(id);if(el)el.value=v;}
function gv(id){var el=document.getElementById(id);return el?el.value:'';}
function gi(id){return parseInt(gv(id))||0;}
function slUpd(sid,lid){var v=gv(sid);document.getElementById(lid).textContent=v;}
function slSet(sid,lid,v){sv(sid,v);if(lid)document.getElementById(lid).textContent=v;}

// ─── RC-System / HW-Config bedingte Felder ──────────────────────────────
function onRcSys(){
  var crsf=(gi('rc-sys')===4);
  document.getElementById('c-ekch').className='card'+(crsf?' hidden':'');
  document.getElementById('c-ekmo').className='card'+(crsf?' hidden':'');
  document.getElementById('c-madr').className='card'+(crsf?'':' hidden');
  document.getElementById('c-mkan').className='card'+(crsf?'':' hidden');
  document.getElementById('c-sbusgrp').className='card'+(crsf?' hidden':'');
}
function onHwCfg(){
  var hw = gi('hw-cfg');
  document.getElementById('c-pwm').className = 'card'+(hw<2?'':' hidden');
  // Port B/C nur auf Hardware V3 sichtbar (V1/V2 brauchen GPIO32/33/27/14
  // fuer die PWM-Einkanal-Eingaenge, siehe config.h).
  var pbc=document.getElementById('c-portbc');
  if(pbc) pbc.className = 'card'+(hw>=2?'':' hidden');
  // Quell-Dropdowns neu befüllen – V3/V4 ohne PWM/Pin-Gruppen
  var curMSrc = gv('m-src');
  var curMSpd = gv('m-spd');
  var curSSrc = gv('s-src');
  fillSel('m-src', srcGroups(hw)); sv('m-src', curMSrc);
  fillSel('m-spd', spdGroups(hw));       sv('m-spd', curMSpd);
  fillSel('s-src', srcGroups(hw)); sv('s-src', curSSrc);
}
// Baudrate beim Wechsel der Port-B-Rolle auf den protokolltypischen Standard
// vorbelegen, bei GPS "Auto". Die Standardwerte kommen vom Geraet
// (s.portb_baud_defaults in /api/config, siehe uartRoleDefaultBaud() in
// RC_Common/uart_roles.h), die Liste aus /rc_common.js (rcFillBaud) -
// loadConfig() befuellt PORTB_BAUD_DEFAULTS unten.
var PORTB_BAUD_DEFAULTS = {};
function onPortBMode(){
  var m = gi('portb-mode');
  if (PORTB_BAUD_DEFAULTS[m] !== undefined && PORTB_BAUD_DEFAULTS[m] !== 0 && m !== 4) rcFillBaud(document.getElementById('portb-baud'), PORTB_BAUD_DEFAULTS[m]);
  var row = document.getElementById('row-portb-baud');
  if (row) row.className = 'row'+(m===0?' hidden':'');
}

// ─── Sound-Tabs ──────────────────────────────────────────────────────────
// NEU: eigener Name je Sound (max. 16 Zeichen), Nummer bleibt davor
function sndName(n){var s=cfg.sounds&&cfg.sounds[n-1];return s&&s.name&&s.name!=='Sound '+n?s.name:'';}
function sndLabel(n){var m=sndName(n);return m?n+' '+m:'Sound '+n;}
function refreshSndTabs(){document.querySelectorAll('.stab').forEach(function(el){el.textContent=sndLabel(parseInt(el.getAttribute('data-s')));});
  var t=document.getElementById('s-name');if(t)t.textContent=sndLabel(curSnd);}
function saveSndName(){
  var s=cfg.sounds&&cfg.sounds[curSnd-1];if(!s)return;
  var nm=prompt('Neuer Name f\u00fcr Sound '+curSnd+' (max. 16 Zeichen, leer = Sound '+curSnd+'):',sndName(curSnd));
  if(nm===null)return; nm=nm.trim().substring(0,16)||('Sound '+curSnd);
  xpost('/api/sound',{id:curSnd,name:nm},function(){s.name=nm;refreshSndTabs();toast('Name gespeichert');});
}
function buildSndTabs(){
  // 24 Sound-Slots.
  var c=document.getElementById('stabs'); c.innerHTML='';
  for(var i=1;i<=24;i++){
    var b=document.createElement('div');
    b.className='stab'+(i===1?' active':'');
    b.textContent=sndLabel(i); b.setAttribute('data-s',i);
    b.onclick=(function(n){return function(){selSnd(n);};})(i);
    c.appendChild(b);
  }
}
function selSnd(n){
  curSnd=n;
  document.querySelectorAll('.stab').forEach(function(el){
    el.className='stab'+(parseInt(el.getAttribute('data-s'))===n?' active':'');
  });
  var lbl=document.getElementById('snd-upload-label'); if(lbl) lbl.textContent=n;
  document.getElementById('snd-upload-progress').className='row hidden';
  updateSoundFileStatusDisplay();
  if(!cfg.sounds)return;
  refreshSndTabs();
  var s=cfg.sounds[n-1];
  sv('s-src',s.source); slSet('s-vol','lv-s-vol',s.vol); sv('s-mode',s.mode);
}

// ─── Config laden ─────────────────────────────────────────────────────────
function loadConfig(){
  xget('/api/sconfig',0,function(resp){
    try{cfg=JSON.parse(resp);}catch(e){toast('Ladefehler',1);return;}
    (document.getElementById('sb-hv')||{}).textContent='v'+cfg.version;
    var m=cfg.motor;
    sv('m-mode',m.mode); sv('m-toggle',m.toggle);
    sv('m-src',m.source); sv('m-spd',m.speed_src);
    slSet('m-vol','lv-m-vol',m.vol);
    slSet('m-rmin','lv-m-rmin',m.rpm_min);
    slSet('m-rmax','lv-m-rmax',m.rpm_max);
    slSet('m-sdly','lv-m-sdly',m.shutdown_s);
    slSet('m-ramp','lv-m-ramp',m.ramp);
    slSet('m-dead','lv-m-dead',m.deadband);
    if(cfg.sounds) selSnd(curSnd);
    var s=cfg.settings;
    sv('rc-sys',s.rc_system); sv('ek-ch',s.ek_channel);
    sv('ek-mode',s.ek_mode); sv('mod-adr',s.modul_adr);
    sv('mkan-adr0',s.mkan_adr0); sv('mkan-adr1',s.mkan_adr1);
    sv('sbusgrp-ch0',s.sbusgrp_ch0); sv('sbusgrp-mode0',s.sbusgrp_mode0);
    sv('sbusgrp-ch1',s.sbusgrp_ch1); sv('sbusgrp-mode1',s.sbusgrp_mode1);
    sv('pwm-min',s.pwm_min); sv('pwm-max',s.pwm_max);
    sv('hw-cfg',s.hw_config_pending);
    var rh=document.getElementById('hw-restart-hint');
    if(rh) rh.className = s.restart_pending ? '' : 'hidden';
    // Port B/C laden: Options-Liste und Baudraten-Standardwerte kommen vom
    // Geraet (s.portb_options/s.portb_baud_defaults) - Dropdown wird deshalb
    // ERST befuellt, dann erst der gespeicherte Wert gesetzt.
    fillSelFlat('portb-mode', s.portb_options);
    PORTB_BAUD_DEFAULTS = s.portb_baud_defaults || {};
    sv('portb-mode', s.portb_role); rcFillBaud(document.getElementById('portb-baud'), s.portb_baud); sv('portc-gps', s.portc_gps);
    onPortBMode();
    onRcSys(); onHwCfg();
    var w=cfg.wifi;
    sv('w-ssid',w.ssid); sv('w-pass',w.pass||'');
    sv('w-ip',w.ip); sv('w-dev',w.device);
    // "WLAN dauerhaft an" (persistiert, siehe c-wifi-always) - Default AN
    var wao=document.getElementById('w-always-on');
    if(wao) wao.checked = (w.always_on===undefined) ? true : !!w.always_on;
    // WLAN-Failsafe-Einstellungen (persistiert, siehe c-wifi-auto)
    var wae=document.getElementById('w-auto-enable');
    if(wae) wae.checked = !!w.auto_enable;
    sv('w-auto-timeout', w.auto_timeout);
    snd_onWifiAlwaysOnChange();
    var sp=cfg.sport;
    sv('pid0',sp.poll_id0); sv('pid1',sp.poll_id1);
  });
}

// ─── Debug / LiPo laden ─────────────────────────────────────────────────
function loadDebug(){
  xget('/api/debug',0,function(resp){
    try{dbg=JSON.parse(resp);}catch(e){return;}
    renderDbg(); renderLipo(); renderWifiLiveStatus();
  });
}
function renderDbg(){
  var ch=document.getElementById('dbg-ch');
  if(ch&&dbg.channels){
    ch.innerHTML='';
    dbg.channels.forEach(function(v,i){
      ch.innerHTML+='<div class="dbg-item"><div class="dbg-k">K'+(i+1)+'</div><div class="dbg-v">'+v+'</div></div>';
    });
  }
  var pw=document.getElementById('dbg-pwm');
  if(pw&&dbg.pwm){
    pw.innerHTML='';
    dbg.pwm.forEach(function(p,i){
      pw.innerHTML+='<div class="row"><span class="lbl">PWM '+(i+1)+'</span><span style="font-size:13px;font-family:var(--mono)">'+p.us+' &micro;s &rarr; '+p.pct+' %</span></div>';
    });
  }
  var wv=document.getElementById('dbg-wav');
  if(wv&&dbg.wav){
    var w=dbg.wav,h='';
    ['loop','shut','start'].forEach(function(n){
      h+='<div class="row"><span class="lbl">'+n+'.wav</span><span class="badge '+(w[n]?'bg-g':'bg-r')+'">'+(w[n]?'OK':'fehlt')+'</span></div>';
    });
    if(w.s)w.s.forEach(function(v,i){
      h+='<div class="row"><span class="lbl">sound'+(i+1)+'.wav'+(sndName(i+1)?' ('+sndName(i+1)+')':'')+'</span><span class="badge '+(v?'bg-g':'bg-r')+'">'+(v?'OK':'fehlt')+'</span></div>';
    });
    wv.innerHTML=h;
  }
  var gp=document.getElementById('dbg-gps');
  if(gp&&dbg.gps){
    var g=dbg.gps;
    if(!g.active && !g.pinConflict){gp.innerHTML='<div class="row"><span class="lbl">Status</span><span class="badge bg-r">GPS ist aus (weder Port B noch Port C)</span></div>';}
    else if(g.pinConflict){gp.innerHTML='<div class="row"><span class="lbl">Status</span><span class="badge bg-r">deaktiviert</span></div>'+
      '<div style="font-size:11px;color:var(--sub);margin-top:8px">GPIO27-Konflikt: Port C (GPS) + SBUS gleichzeitig gewaehlt - GPIO27 wird von SBUS UND GPS beansprucht. GPS bleibt deaktiviert, SBUS hat Vorrang. Fuer GPS bitte Einkanal/RC-System auf CRSF stellen.</div>';}
    else{gp.innerHTML='<div class="row"><span class="lbl">Baudrate</span><span style="font-family:var(--mono)">'+(g.auto?'Auto ':'')+(g.locked?g.baud:'sucht …')+'</span></div>'+
      '<div class="row"><span class="lbl">Fix</span><span class="badge '+(g.fix?'bg-g':'bg-r')+'">'+(g.fix?'OK':'kein Fix')+'</span></div>'+
      '<div class="row"><span class="lbl">Satelliten</span><span style="font-family:var(--mono)">'+g.sats+'</span></div>'+
      '<div class="row"><span class="lbl">Geschwindigkeit</span><span style="font-family:var(--mono)">'+g.speed+' km/h</span></div>';}
  }
  // S.Port Diagnose (Learning aus ESP32-GPS-Telemetry): zeigt nicht nur
  // online/offline, sondern WARUM ein Sensor ggf. nicht online geht.
  var sd=document.getElementById('dbg-sport-diag');
  if(sd&&dbg.sport){
    var s=dbg.sport;
    var h='<div class="row"><span class="lbl">Rohbytes empfangen</span><span style="font-family:var(--mono)">'+s.raw_bytes+'</span></div>'+
      '<div class="row"><span class="lbl">Polls gesendet</span><span style="font-family:var(--mono)">'+s.polls_sent+'</span></div>'+
      '<div class="row"><span class="lbl">G&uuml;ltige Frames</span><span style="font-family:var(--mono)">'+s.valid_frames+'</span></div>'+
      '<div class="row"><span class="lbl">Keine/ung&uuml;ltige Antwort</span><span style="font-family:var(--mono)">'+s.no_response+'</span></div>'+
      '<div class="row"><span class="lbl">CRC-Fehler</span><span style="font-family:var(--mono)">'+s.crc_errors+'</span></div>'+
      '<div class="row"><span class="lbl">Letzte Poll-ID (Echo)</span><span style="font-family:var(--mono)">'+s.last_poll_id+'</span></div>';
    var hint='';
    if(s.raw_bytes==0) hint='Es kommen ueberhaupt keine Bytes an - Verkabelung/Diode/Baudrate (57600, invertiert) pruefen.';
    else if(s.valid_frames==0 && s.no_response>0) hint='Polls werden gesendet, aber es kommt nie ein gueltiges DATA_FRAME zurueck - passt die konfigurierte Physical ID (Seite 12) zu den tatsaechlich angeschlossenen FLVSS/MLVSS-Sensoren?';
    else if(s.crc_errors>s.valid_frames) hint='Viele CRC-Fehler im Verhaeltnis zu gueltigen Frames - deutet auf Stoerungen oder eine wacklige Verbindung hin.';
    if(hint) h+='<div style="font-size:11px;color:var(--sub);margin-top:8px">'+hint+'</div>';
    sd.innerHTML=h;
  }
  // CRSF Diagnose (Learning aus ESP32-GPS-Telemetry)
  var cd=document.getElementById('dbg-crsf-diag');
  if(cd&&dbg.crsf_diag){
    var c=dbg.crsf_diag;
    var h2='<div class="row"><span class="lbl">Rohbytes empfangen</span><span style="font-family:var(--mono)">'+c.raw_bytes+'</span></div>'+
      '<div class="row"><span class="lbl">G&uuml;ltige Frames</span><span style="font-family:var(--mono)">'+c.valid_frames+'</span></div>'+
      '<div class="row"><span class="lbl">CRC-Fehler</span><span style="font-family:var(--mono)">'+c.crc_errors+'</span></div>'+
      '<div class="row"><span class="lbl">Device-Pings erhalten</span><span style="font-family:var(--mono)">'+c.device_pings+'</span></div>'+
      '<div class="row"><span class="lbl">Parameter-Reads erhalten</span><span style="font-family:var(--mono)">'+c.param_reads+'</span></div>'+
      '<div class="row"><span class="lbl">Parameter-Writes erhalten</span><span style="font-family:var(--mono)">'+c.param_writes+'</span></div>';
    var hint2='';
    if(c.raw_bytes==0) hint2='Es kommen ueberhaupt keine Bytes an - TX/RX vertauscht oder RC-System nicht auf CRSF gestellt?';
    else if(c.device_pings==0) hint2='Es kommen Bytes/Frames an, aber noch kein Device-Ping vom Sender - "Sensoren entdecken"/Geraeteliste am Sender einmal ausloesen.';
    if(hint2) h2+='<div style="font-size:11px;color:var(--sub);margin-top:8px">'+hint2+'</div>';
    cd.innerHTML=h2;
  }
  var dc=document.getElementById('dbg-cfg');
  if(dc&&cfg.motor&&cfg.settings){
    var m=cfg.motor,s=cfg.settings;
    dc.innerHTML=
      'HW:'+s.hw_config+' RC-Sys:'+s.rc_system+' EK-Ch:'+s.ek_channel+'<br>'+
      'Motor-Start:'+m.source+' Speed:'+m.speed_src+'<br>'+
      'Rampe:'+m.ramp+' Totband:'+m.deadband+'<br>'+
      'S.Port:0x'+(cfg.sport?cfg.sport.poll_id0:'?')+'/0x'+(cfg.sport?cfg.sport.poll_id1:'?');
  }
}
function renderLipo(){
  var el=document.getElementById('lipo-packs');
  if(!el)return;
  if(!dbg.sport){el.innerHTML='<div style="color:var(--sub);font-size:12px">Keine Daten empfangen</div>';return;}
  if(dbg.portb_role!==undefined&&dbg.portb_role!==2){
    el.innerHTML='<div style="color:var(--yellow);font-size:12px">&#9888; Port B steht nicht auf S.Port-Master &ndash; LiPo-Telemetrie inaktiv.<br>Unter Einstellungen &rarr; Port B/C auf "S.Port-Master" stellen und neu starten.</div>';
    return;
  }
  var sp=dbg.sport,h='';
  sp.packs.forEach(function(p,i){
    h+='<div style="margin-bottom:14px">';
    h+='<div class="row"><span style="font-weight:700">Pack '+(i+1)+'</span>';
    h+='<span class="badge '+(p.online?'bg-g':'bg-s')+'">'+(p.online?'online':'offline')+'</span></div>';
    if(p.online){
      h+='<div class="row"><span class="lbl">Gesamt</span><span style="font-weight:700">'+p.total.toFixed(2)+' V</span></div>';
      h+='<div class="row"><span class="lbl">SoC</span><span style="font-weight:700">'+sp.soc+' %</span></div>';
      h+='<div class="row"><span class="lbl">Min-Zelle</span><span style="font-weight:700">'+p.min.toFixed(3)+' V</span></div>';
      h+='<div class="cell-grid">';
      for(var c=0;c<p.cells;c++){
        var v=p.cv[c];
        var col=v<3.5?'var(--red)':v<3.7?'var(--yellow)':'var(--green)';
        h+='<div class="cell-box"><div class="cell-v" style="color:'+col+'">'+v.toFixed(3)+'</div><div class="cell-l">Z'+(c+1)+'</div></div>';
      }
      h+='</div>';
    }else{
      h+='<div style="color:var(--sub);font-size:12px;padding:6px 0">Kein Signal &ndash; Sensor angeschlossen und Poll-ID korrekt?</div>';
    }
    h+='</div>';
  });
  el.innerHTML=h||'<div style="color:var(--sub)">Keine Sensordaten</div>';
}

// ─── Speichern ────────────────────────────────────────────────────────────
function saveMotor(){
  var m={
    source:gi('m-src'), speed_src:gi('m-spd'),
    vol:gi('m-vol'), mode:gi('m-mode'), toggle:gi('m-toggle'),
    rpm_min:gi('m-rmin'), rpm_max:gi('m-rmax'),
    shutdown_s:gi('m-sdly'), ramp:gi('m-ramp'), deadband:gi('m-dead')
  };
  xpost('/api/sound',{
    id:0, source:m.source, speed_src:m.speed_src,
    vol:m.vol, motor_mode:m.mode, toggle:m.toggle,
    rpm_min:m.rpm_min, rpm_max:m.rpm_max,
    shutdown_s:m.shutdown_s, ramp:m.ramp, deadband:m.deadband
  },function(r){
    try{var d=JSON.parse(r);if(d.ok){
      if(cfg.motor) cfg.motor=Object.assign(cfg.motor,m);
      xget('/save',0,function(){toast('Motor gespeichert');});return;}}catch(e){}
    toast('Fehler',1);
  });
}
function saveSound(){
  var newSrc=gi('s-src'), newVol=gi('s-vol'), newMode=gi('s-mode');
  xpost('/api/sound',{
    id:curSnd,
    source:newSrc, vol:newVol, mode:newMode
  },function(r){
    try{var d=JSON.parse(r);if(d.ok){
      // Lokalen cfg-Cache synchron halten, sonst zeigt ein Tab-Wechsel wieder den
      // alten (veralteten) Wert an, obwohl am Geraet bereits korrekt gespeichert ist.
      if(cfg.sounds&&cfg.sounds[curSnd-1]){
        cfg.sounds[curSnd-1].source=newSrc;
        cfg.sounds[curSnd-1].vol=newVol;
        cfg.sounds[curSnd-1].mode=newMode;
      }
      xget('/save',0,function(){toast('Sound '+curSnd+' gespeichert');});return;}}catch(e){}
    toast('Fehler',1);
  });
}
function testSound(){
  xpost('/api/sound',{id:curSnd,test:true},function(){toast('Sound '+curSnd+' wird gespielt');});
}
// WAV-Datei fuer den aktuell ausgewaehlten Sound per WLAN hochladen. Echtes
// multipart/form-data (FormData) statt einer rohen XHR-Uebertragung, damit
// die Firmware die Datei haeppchenweise verarbeiten kann, ohne sie komplett
// im RAM zu puffern - siehe handleApiSoundUploadData() in
// WebServerManager.cpp.
function uploadSoundFile(file){
  if(!file)return;
  var name='sound'+curSnd+'.wav';
  var prog=document.getElementById('snd-upload-progress');
  var status=document.getElementById('snd-upload-status');
  prog.className='row'; status.textContent='Hochladen … (0%)';

  var fd=new FormData();
  fd.append('file',file,name);

  var x=new XMLHttpRequest();
  x.open('POST','/api/soundupload?name='+encodeURIComponent(name),true);
  x.upload.onprogress=function(e){
    if(e.lengthComputable) status.textContent='Hochladen … ('+Math.round(e.loaded/e.total*100)+'%)';
  };
  x.onload=function(){
    try{
      var d=JSON.parse(x.responseText);
      if(d.ok){
        status.textContent='Fertig: '+d.file+' ('+d.size+' Bytes)';
        toast('Sound '+curSnd+' aktualisiert');
        sndFiles[name]={exists:true,size:d.size};
        updateSoundFileStatusDisplay();
      } else {
        status.textContent='Fehler: '+errText(d.error);
        toast('Upload fehlgeschlagen',1);
      }
    }catch(e){
      status.textContent='Fehler beim Hochladen';
      toast('Upload fehlgeschlagen',1);
    }
  };
  x.onerror=function(){ status.textContent='Verbindungsfehler'; toast('Verbindungsfehler',1); };
  x.send(fd);
  document.getElementById('snd-upload-file').value='';
}
// Datei-Verwaltung fuer Sound-Dateien (Motor: loop/shut/start.wav, Sound
// 1-24: soundN.wav) - Status abfragen, aktuellen Sound-Slot loeschen,
// Motor-Dateien hochladen/loeschen. sndFiles ist ein einfacher Client-Cache
// (name -> {exists,size}), damit nicht nach jeder Kleinigkeit neu vom Geraet
// gelesen werden muss; nach Upload/Loeschen wird er lokal nachgefuehrt.
var sndFiles={};
function loadSoundFileStatus(){
  xget('/api/soundfiles',0,function(r){
    try{ var d=JSON.parse(r); if(d&&d.ok&&d.files){ sndFiles=d.files; updateSoundFileStatusDisplay(); } }
    catch(e){}
  });
}
function fmtFileStatus(name){
  var f=sndFiles[name];
  if(!f) return '-';
  return f.exists ? (Math.round(f.size/1024)+' KB vorhanden') : 'keine Datei auf der SD-Karte';
}
function updateSoundFileStatusDisplay(){
  var name='sound'+curSnd+'.wav';
  var nm=document.getElementById('snd-file-name'); if(nm) nm.textContent=name;
  var st=document.getElementById('snd-file-status'); if(st) st.textContent=fmtFileStatus(name);
  ['loop','shut','start'].forEach(function(k){
    var el=document.getElementById('mf-'+k+'-status');
    if(el) el.textContent=fmtFileStatus(k+'.wav');
  });
}
function deleteSoundSlotFile(){
  var name='sound'+curSnd+'.wav';
  if(!confirm('Datei "'+name+'" von der SD-Karte loeschen?')) return;
  xpost('/api/sounddelete?name='+encodeURIComponent(name),{},function(r){
    try{
      var d=JSON.parse(r);
      if(d.ok){ toast('Sound '+curSnd+' geloescht'); sndFiles[name]={exists:false,size:0}; updateSoundFileStatusDisplay(); }
      else toast('Loeschen fehlgeschlagen: '+errText(d.error),1);
    }catch(e){ toast('Fehler',1); }
  });
}
function uploadMotorFile(name,file){
  if(!file)return;
  var prog=document.getElementById('mf-upload-progress');
  var status=document.getElementById('mf-upload-status');
  prog.className='row'; status.textContent='Hochladen '+name+' … (0%)';

  var fd=new FormData();
  fd.append('file',file,name);

  var x=new XMLHttpRequest();
  x.open('POST','/api/soundupload?name='+encodeURIComponent(name),true);
  x.upload.onprogress=function(e){
    if(e.lengthComputable) status.textContent='Hochladen '+name+' … ('+Math.round(e.loaded/e.total*100)+'%)';
  };
  x.onload=function(){
    try{
      var d=JSON.parse(x.responseText);
      if(d.ok){
        status.textContent='Fertig: '+d.file+' ('+d.size+' Bytes)';
        toast(name+' aktualisiert');
        sndFiles[name]={exists:true,size:d.size};
        updateSoundFileStatusDisplay();
      } else {
        status.textContent='Fehler: '+errText(d.error);
        toast('Upload fehlgeschlagen',1);
      }
    }catch(e){ status.textContent='Fehler beim Hochladen'; toast('Upload fehlgeschlagen',1); }
  };
  x.onerror=function(){ status.textContent='Verbindungsfehler'; toast('Verbindungsfehler',1); };
  x.send(fd);
}
function deleteMotorFile(name){
  if(!confirm('Datei "'+name+'" von der SD-Karte loeschen?')) return;
  xpost('/api/sounddelete?name='+encodeURIComponent(name),{},function(r){
    try{
      var d=JSON.parse(r);
      if(d.ok){ toast(name+' geloescht'); sndFiles[name]={exists:false,size:0}; updateSoundFileStatusDisplay(); }
      else toast('Loeschen fehlgeschlagen: '+errText(d.error),1);
    }catch(e){ toast('Fehler',1); }
  });
}
function saveSettings(){
  var d={
    rc_system:gi('rc-sys'),   ek_channel:gi('ek-ch'),
    ek_mode:gi('ek-mode'),    modul_adr:gi('mod-adr'),
    hw_config_pending:gi('hw-cfg'),
    mkan_adr0:gi('mkan-adr0'), mkan_adr1:gi('mkan-adr1'),
    sbusgrp_ch0:gi('sbusgrp-ch0'), sbusgrp_mode0:gi('sbusgrp-mode0'),
    sbusgrp_ch1:gi('sbusgrp-ch1'), sbusgrp_mode1:gi('sbusgrp-mode1'),
    portb_role:gi('portb-mode'), portb_baud:gi('portb-baud'), portc_gps:gi('portc-gps')
  };
  // PWM nur bei V1/V2 senden (bei V3/V4 nicht vorhanden)
  if(gi('hw-cfg')<2){d.pwm_min=gi('pwm-min');d.pwm_max=gi('pwm-max');}
  xpost('/api/sconfig',d,function(r){
    // Kein zusaetzliches /save noetig - handleApiConfigPost() markiert die
    // Konfiguration serverseitig als "dirty", der eigentliche NVS-Schreib-
    // vorgang folgt automatisch ueber die debounced+cooldown-geschuetzte
    // Pruefung in loop() (seit v7.16, siehe dortigen Kommentar) - genau wie
    // bei /api/sound.
    try{var p=JSON.parse(r);if(p.ok){
      // Komplett neu von /api/config laden statt nur die gerade gesendeten
      // Felder lokal zu uebernehmen: "restart_pending" wird vom Server
      // berechnet (nicht Teil von d) und muss deshalb aktualisiert werden,
      // damit der Neustart-Hinweis sofort stimmt, ohne dass man die Seite
      // neu laden muss.
      loadConfig();
      toast('Einstellungen gespeichert');return;}}catch(e){}
    toast('Fehler',1);
  });
}

// ─── Konfiguration Export/Import ─────────────────────────────────────────
// Export: liest /api/config (dieselbe JSON-Struktur, die auch loadConfig()
// nutzt) und bietet sie als Datei zum Download an - ganz normaler Download
// im echten Browser des Nutzers (dieser Webserver laeuft auf dem Modul
// selbst, keine Sandbox-Einschraenkung wie in einer Vorschau-Umgebung).
function snd_exportConfig(){
  xget('/api/sconfig',0,function(resp){
    var text=resp;
    try{ text=JSON.stringify(JSON.parse(resp),null,2); }catch(e){ /* notfalls Rohtext exportieren */ }
    var name='esp32-rc-sound-config';
    var dev=gv('w-dev');
    if(dev) name+='-'+dev.replace(/[^A-Za-z0-9_-]+/g,'_');
    name+='.json';
    var blob=new Blob([text],{type:'application/json'});
    var url=URL.createObjectURL(blob);
    var a=document.createElement('a');
    a.href=url; a.download=name;
    document.body.appendChild(a); a.click(); document.body.removeChild(a);
    setTimeout(function(){URL.revokeObjectURL(url);},1000);
    toast('Konfiguration exportiert');
  });
}

// Import: Datei einlesen, grob validieren, bestaetigen lassen (WLAN-Zugangs-
// daten werden mit ueberschrieben), dann Schritt fuer Schritt einspielen.
function snd_importConfigFile(file){
  if(!file) return;
  var reader=new FileReader();
  reader.onload=function(){
    var data;
    try{ data=JSON.parse(reader.result); }catch(e){ toast('Ungueltige JSON-Datei',1); return; }
    if(!data||!data.settings||!data.motor||!data.sounds){
      toast('Datei enthaelt keine gueltige Modul-Konfiguration',1); return;
    }
    if(!confirm('Aktuelle Konfiguration (inkl. WLAN-Zugangsdaten) durch die Datei ersetzen?')) return;
    snd_importConfigData(data);
  };
  reader.onerror=function(){ toast('Datei konnte nicht gelesen werden',1); };
  reader.readAsText(file);
  // Sonst laesst sich dieselbe Datei kein zweites Mal auswaehlen (onchange
  // feuert bei unveraendertem value nicht erneut).
  document.getElementById('sb-cfg-import-file').value='';
}

function stripUndef(o){
  Object.keys(o).forEach(function(k){ if(o[k]===undefined) delete o[k]; });
  return o;
}
function runSteps(steps,done){
  var i=0;
  function nextStep(){
    if(i>=steps.length){ done(); return; }
    steps[i++](nextStep);
  }
  nextStep();
}

// /api/config und /api/sound sind bewusst einfache Key/Value-Endpunkte ohne
// Array-Unterstuetzung (siehe handleApiConfigPost()/handleApiSound() in
// WebServerManager.cpp) - der Import laeuft daher als Kette einzelner POSTs:
// einmal /api/config fuer die globalen Einstellungen, einmal /api/sound fuer
// Motor (id 0), dann /api/sound je Sound-Slot, zum Schluss ein einzelnes
// /save (statt 25x einzeln auf den Flash zu schreiben).
function snd_importConfigData(data,done){
  var s=data.settings||{}, w=data.wifi||{}, sp=data.sport||{}, m=data.motor||{};

  var settingsPost=stripUndef({
    rc_system:s.rc_system, ek_channel:s.ek_channel, ek_mode:s.ek_mode, modul_adr:s.modul_adr,
    mkan_adr0:s.mkan_adr0, mkan_adr1:s.mkan_adr1,
    sbusgrp_ch0:s.sbusgrp_ch0, sbusgrp_mode0:s.sbusgrp_mode0,
    sbusgrp_ch1:s.sbusgrp_ch1, sbusgrp_mode1:s.sbusgrp_mode1,
    hw_config_pending:s.hw_config_pending, pwm_min:s.pwm_min, pwm_max:s.pwm_max,
    // portb_mode: Sicherungen bis v7.20 (alte Nummern, Geraet rechnet um)
    portb_role:s.portb_role, portb_mode:(s.portb_role===undefined?s.portb_mode:undefined),
    portb_baud:s.portb_baud, portc_gps:s.portc_gps,
    ssid:w.ssid, pass:w.pass, ip:w.ip, device:w.device,
    poll_id0:sp.poll_id0, poll_id1:sp.poll_id1
  });

  // Feldname-Stolperfalle: /api/config liefert Motor-throttle_mode als
  // "mode" (siehe handleApiConfig()), /api/sound erwartet dafuer aber
  // "motor_mode" - "mode" ist dort bereits fuer Mode_Sound[0] belegt (im
  // Motor-Tab der Weboberflaeche gar nicht angezeigt). Deshalb hier bewusst
  // NICHT 1:1 durchgereicht, sondern umbenannt.
  var motorPost=stripUndef({
    id:0, source:m.source, vol:m.vol, speed_src:m.speed_src,
    motor_mode:m.mode, toggle:m.toggle,
    rpm_min:m.rpm_min, rpm_max:m.rpm_max, shutdown_s:m.shutdown_s,
    ramp:m.ramp, deadband:m.deadband
  });

  var soundPosts=(data.sounds||[]).map(function(sd){
    return stripUndef({ id:sd.id, source:sd.source, vol:sd.vol, mode:sd.mode, name:sd.name });
  });

  var steps=[
    function(next){ xpost('/api/sconfig', settingsPost, function(){ next(); }); },
    function(next){ xpost('/api/sound', motorPost, function(){ next(); }); }
  ];
  soundPosts.forEach(function(p){
    if(p.id===undefined) return;
    steps.push(function(next){ xpost('/api/sound', p, function(){ next(); }); });
  });
  steps.push(function(next){ xget('/save',0,function(){ next(); }); });

  toast('Import laeuft...');
  runSteps(steps,function(){
    if(done){ done(); return; }
    loadConfig();
    toast('Konfiguration importiert');
  });
}

// ─── Konfiguration Export/Import direkt auf/von der SD-Karte im Modul ─────
// Eigene, explizite Buttons zusaetzlich zum Browser-Export/-Import oben.
// Der Import nutzt bewusst dieselbe snd_importConfigData(), die auch der
// Browser-Import benutzt (Datei liegt bereits im /api/config-Format vor).
function sdExportConfig(){
  collectConfig().then(function(data){
    if(!data){ toast('Ger\u00e4t nicht erreichbar',1); return; }
    xpost('/api/sdexport',data,function(r){
      var p; try{ p=JSON.parse(r); }catch(e){ toast('Fehler beim Sichern auf SD',1); return; }
      if(p&&p.ok){ toast('Auf SD gesichert: '+p.file); return; }
      toast('Fehler beim Sichern auf SD'+(p&&p.error?' ('+p.error+')':''),1);
    });
  });
}
function sdListBackups(){
  xget('/api/sdlist',0,function(r){
    var p; try{ p=JSON.parse(r); }catch(e){ toast('Fehler beim Lesen der SD-Karte',1); return; }
    if(!p||!p.ok||!p.files||!p.files.length){ toast('Keine Backups auf der SD-Karte gefunden',1); return; }
    p.files.sort(function(a,b){ return a.name<b.name?1:(a.name>b.name?-1:0); }); // neuestes (hoechste Nummer) zuerst
    var sel=document.getElementById('sd-backup-select');
    sel.innerHTML='';
    p.files.forEach(function(f){
      var o=document.createElement('option');
      o.value=f.name; o.textContent=f.name+' ('+f.size+' Byte)';
      sel.appendChild(o);
    });
    document.getElementById('sd-backup-list').className='';
  });
}
function sdImportSelected(){
  var name=document.getElementById('sd-backup-select').value;
  if(!name) return;
  xget('/api/sdconfig?file='+encodeURIComponent(name),0,function(r){
    var data; try{ data=JSON.parse(r); }catch(e){ toast('Ungueltige Datei auf der SD-Karte',1); return; }
    if(!isKombiData(data)){
      toast('Datei enthaelt keine gueltige Modul-Konfiguration',1); return;
    }
    if(!confirm('Aktuelle Konfiguration (inkl. WLAN-Zugangsdaten) durch "'+name+'" ersetzen?')) return;
    importAny(data);
  });
}
// Ausgewaehltes Config-Backup von der SD-Karte loeschen (Teil der
// Datei-Verwaltung) - danach Liste neu laden, damit sie aktuell bleibt.
function sdDeleteSelected(){
  var name=document.getElementById('sd-backup-select').value;
  if(!name) return;
  if(!confirm('Backup "'+name+'" von der SD-Karte loeschen?')) return;
  xpost('/api/sddelete?file='+encodeURIComponent(name),{},function(r){
    var d; try{ d=JSON.parse(r); }catch(e){ toast('Fehler',1); return; }
    if(d.ok){ toast('Backup geloescht'); sdListBackups(); }
    else{ toast('Loeschen fehlgeschlagen: '+errText(d.error),1); }
  });
}

// Firmware-Update per WLAN (OTA) - echtes multipart/form-data wie beim
// Sound-Upload (uploadSoundFile()), aber an /api/otaupdate; nach Erfolg
// startet das Modul selbst neu (siehe RC_Common/web_common.cpp) - die Verbindung bricht dabei kurz ab, das wird dem
// Nutzer hier erklaert statt als Fehler angezeigt.
function snd_uploadOtaFile(file){
  // v7.21: gemeinsamer Upload aus /rc_common.js (RC_Common)
  var prog=document.getElementById('ota-progress');
  if(prog) prog.className='row';
  rcOtaUpload(file,document.getElementById('sb-ota-status'),document.getElementById('sb-ota-file'),function(ok){
    toast(ok?'Firmware-Update erfolgreich, Modul startet neu':'Update fehlgeschlagen',ok?0:1);
  });
}

// Graut die Auto-Failsafe-Karte aus, solange "WLAN dauerhaft an" aktiv ist -
// diese Einstellungen wirken dann nicht (siehe wifiFailsafeCheck()).
function snd_onWifiAlwaysOnChange(){
  var always = document.getElementById('w-always-on');
  var on = always ? always.checked : false;
  var hint = document.getElementById('sb-wifi-auto-overridden-hint');
  if(hint) hint.className = on ? '' : 'hidden';
  ['w-auto-enable','w-auto-timeout'].forEach(function(id){
    var el=document.getElementById(id);
    if(el){ el.disabled=on; el.style.opacity = on ? '0.5' : '1'; }
  });
}
function snd_saveWifi(){
  var d={ ssid:gv('w-ssid'), pass:gv('w-pass'), ip:gv('w-ip'), device:gv('w-dev'),
          wifi_always_on: document.getElementById('w-always-on').checked?1:0,
          wifi_auto_enable: document.getElementById('w-auto-enable').checked?1:0,
          wifi_auto_timeout: gi('w-auto-timeout') };
  xpost('/api/sconfig',d,function(r){
    try{var p=JSON.parse(r);if(p.ok){
      if(cfg.wifi) cfg.wifi=Object.assign(cfg.wifi,{ssid:d.ssid,pass:d.pass,ip:d.ip,device:d.device,
        always_on:d.wifi_always_on,auto_enable:d.wifi_auto_enable,auto_timeout:d.wifi_auto_timeout});
      toast('WiFi gespeichert');return;}}catch(e){}
    toast('Fehler',1);
  });
}
// Manueller WLAN-Schalter (Karte "WLAN jetzt") - schaltet ueber
// /api/wifi_enable sofort um, ohne die Konfiguration zu beruehren.
function toggleWifiNow(){
  var turnOn = !(dbg.wifi && dbg.wifi.active);
  xpost('/api/wifi_enable',{on:turnOn},function(r){
    try{
      var d=JSON.parse(r);
      dbg.wifi = dbg.wifi || {}; dbg.wifi.active = d.active;
      renderWifiLiveStatus();
      toast(d.active?'WLAN eingeschaltet':'WLAN ausgeschaltet');
    }catch(e){ toast('Fehler',1); }
  });
}
// Zeigt den aktuellen (Laufzeit-)WLAN-Zustand aus dbg.wifi (kommt ueber
// /api/debug, siehe loadDebug() - wird beim Betreten des Tabs "WiFi" und
// danach alle 2s aktualisiert, siehe snd_showTab()).
function renderWifiLiveStatus(){
  var active = !!(dbg.wifi && dbg.wifi.active);
  var st=document.getElementById('wifi-status-now');
  var btn=document.getElementById('wifi-toggle-btn');
  if(st) st.textContent = active ? 'Ein' : 'Aus';
  if(btn) btn.textContent = active ? 'WLAN jetzt ausschalten' : 'WLAN jetzt einschalten';
}
function saveLipo(){
  var d={ poll_id0:gv('pid0'), poll_id1:gv('pid1') };
  xpost('/api/sconfig',d,function(r){
    try{var p=JSON.parse(r);if(p.ok){
      if(cfg.sport) cfg.sport=Object.assign(cfg.sport,d);
      toast('S.Port gespeichert');return;}}catch(e){}
    toast('Fehler',1);
  });
}

// ─── Init ────────────────────────────────────────────────────────────────
// Dropdowns initial mit hw=0 befüllen; nach loadConfig() mit echtem hw-Wert neu befüllen
fillSel('m-src', srcGroups(0, false));
fillSel('m-spd', spdGroups(0));
fillSel('s-src', srcGroups(0));
fillEkCh('ek-ch'); fillModAdr(); fillGroupAdr('mkan-adr0'); fillGroupAdr('mkan-adr1');
fillEkCh('sbusgrp-ch0'); fillEkCh('sbusgrp-ch1'); buildSndTabs();
loadConfig();
loadSoundFileStatus(); // Datei-Status initial laden (Tab "Motor" ist beim Start aktiv)
</script>
<script>
// ── Zusammenfuehrung (nur Pruefstand): Reiter, Karten verschieben ──
(function(){
  var TABS8=['overview','outputs','motor','sounds','rc','wifi','lipo','debug'];
  window.showTab=function(t){
    TABS8.forEach(function(n){var e=document.getElementById('tab-'+n);if(e)e.style.display=(n===t?'block':'none');});
    document.querySelectorAll('nav .tab').forEach(function(el,i){el.classList.toggle('active',TABS8[i]===t);});
    if(t==='debug') startPoll(); else stopPoll();
    if(t==='overview'||t==='outputs'||t==='wifi') fetchStatus();
    if(t==='rc'||t==='wifi') { fetchConfig(); loadKombi(); }
    if(t==='motor'||t==='sounds') loadSoundFileStatus();
    clearInterval(lipoTmr); lipoTmr=0;
    if(t==='lipo'||t==='debug'){ loadDebug(); lipoTmr=setInterval(loadDebug,2000); }
  };
  function card(root,title){
    var r=null;document.querySelectorAll(root+' .card').forEach(function(c){var t=c.querySelector('.card-title');if(t&&t.textContent.indexOf(title)===0&&!r)r=c;});return r;}
  function wrap(c){var d=document.createElement('div');d.className='snd';d.appendChild(c);return d;}
  // Sound-Adresse in den Reiter RC-System
  var madr=document.getElementById('c-madr');
  if(madr){madr.classList.remove('hidden');madr.querySelector('.card-title').textContent='Sound-Adresse (Einzelkanal-Sounds)';
    var b=document.createElement('button');b.className='btn btn-p';b.textContent='Speichern';b.onclick=function(){xpost('/api/sconfig',{modul_adr:gi('mod-adr')},function(){toast('Sound-Adresse gespeichert');});};
    var h=document.createElement('div');h.style.cssText='font-size:12px;color:var(--sub);margin-top:8px';
    h.textContent='Gleiche Zahl wie die Modul-Adresse oben = dieselben Schalter schalten Ausgaenge und Sounds. Am CRSF-Bus meldet sich das Modul mit der Modul-Adresse.';
    madr.appendChild(h);madr.appendChild(b);
    var rc=document.getElementById('tab-rc');var first=rc.querySelector('.card');rc.insertBefore(wrap(madr),first.nextSibling);}
  var after=madr?madr.parentNode:null;
  ['c-ekch','c-ekmo'].forEach(function(id){var c=document.getElementById(id);if(!c||!after)return;
    var t=c.querySelector('.card-title');t.textContent='Sound '+t.textContent+' (SBUS)';
    var w=wrap(c);after.parentNode.insertBefore(w,after.nextSibling);after=w;});
  var ekmo=document.getElementById('c-ekmo');
  if(ekmo){var b2=document.createElement('button');b2.className='btn btn-p';b2.textContent='Speichern';b2.style.marginTop='10px';
    b2.onclick=function(){xpost('/api/sconfig',{ek_channel:gi('ek-ch'),ek_mode:gi('ek-mode')},function(){toast('Sound-Einkanal gespeichert');});};ekmo.appendChild(b2);}
  var rcT=document.getElementById('tab-rc');rcT.appendChild(document.getElementById('c-kombi'));
  // SD-Sicherung in den Reiter WiFi (hinter die Karte Konfiguration sichern)
  var sbBk=document.getElementById('c-cfgbackup');
  if(sbBk){sbBk.querySelector('.card-title').textContent='Sicherung auf der SD-Karte';
    Array.prototype.slice.call(sbBk.children).forEach(function(ch,i){if(i>0&&i<5)ch.remove();});
    var cb=card('#tab-wifi','Konfiguration sichern');var w=wrap(sbBk);if(cb)cb.parentNode.insertBefore(w,cb.nextSibling);else document.getElementById('tab-wifi').appendChild(w);}
  // Sound-Diagnose in den Reiter Debug
  ['BUS Kan','WAV Dateien','S.Port Diagnose'].forEach(function(t){var c=card('#stab-debug',t);if(c)document.getElementById('tab-debug').appendChild(wrap(c));});
  window.loadKombi=function(){fetch('/api/config').then(function(r){return r.json();}).then(function(c){
    document.getElementById('k-luasnd').value=String(c.lua_sounds||8);document.getElementById('k-led').value=String(c.led_pin||48);});};
  window.saveKombi=function(){fetch('/api/config',{method:'POST',body:JSON.stringify({lua_sounds:+document.getElementById('k-luasnd').value,led_pin:+document.getElementById('k-led').value})})
    .then(function(){toast('Gespeichert, wirkt nach Neustart');});};
  loadKombi();
})();
</script>
</body></html>)HTML";
