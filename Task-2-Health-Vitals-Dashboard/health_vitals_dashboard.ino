/*
  Task 2: ESP32-hosted health vitals dashboard demonstration.
  IMPORTANT: BPM and SpO2 are simulated, not measured by a sensor.
  This is not a medical device.
*/

#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

WebServer server(80);

String makePage() {
  String page = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Health Vitals Demo</title>
<style>
body{font-family:Arial,sans-serif;background:#101827;color:#f4f7fb;text-align:center;margin:0;padding:24px}
h1{font-size:26px}.note{color:#ffd166;font-size:14px}
.cards{display:flex;gap:14px;justify-content:center;flex-wrap:wrap;margin:24px auto}
.card{background:#1d2a40;border-radius:14px;padding:20px;min-width:145px}
.value{font-size:36px;font-weight:bold;color:#63e6be}
canvas{width:100%;max-width:650px;background:#1d2a40;border-radius:12px}
</style>
</head>
<body>
<h1>Health Vitals Dashboard</h1>
<p class="note">DEMO ONLY — all displayed readings are simulated, not real measurements.</p>
<div class="cards">
<div class="card"><p>Heart Rate</p><div class="value"><span id="bpm">--</span> BPM</div></div>
<div class="card"><p>SpO2</p><div class="value"><span id="spo2">--</span>%</div></div>
</div>
<canvas id="chart" width="650" height="240"></canvas>
<script>
const canvas=document.getElementById('chart'),ctx=canvas.getContext('2d');
let values=[];
function update(){
 const bpm=68+Math.floor(Math.random()*17);
 const spo2=97+Math.floor(Math.random()*3);
 document.getElementById('bpm').textContent=bpm;
 document.getElementById('spo2').textContent=spo2;
 values.push(bpm); if(values.length>35) values.shift();
 ctx.clearRect(0,0,canvas.width,canvas.height);
 ctx.strokeStyle='#63e6be';ctx.lineWidth=3;ctx.beginPath();
 values.forEach((v,i)=>{const x=i*(canvas.width/34),y=canvas.height-((v-60)/30)*canvas.height;
 if(i===0)ctx.moveTo(x,y);else ctx.lineTo(x,y);});ctx.stroke();
}
update();setInterval(update,2000);
</script>
</body>
</html>
)HTML";
  return page;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected.");
  Serial.print("Open this address in a browser on the same Wi-Fi: http://");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", makePage());
  });
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();
}
