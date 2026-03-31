// --- Bibliotecas
#include <WiFi.h>
#include <WebServer.h>
#include <HardwareSerial.h>
#include <ESPmDNS.h> //Serviço DNS para propagar hostnme

// --- Credenciais da rede Wi-Fi ---
const char* ssid = "ssid-da-rede-wifi";
const char* password = "senha-da-rede-wifi";

const int trigPin = 5; // GPIO 5 no ESP32
const int echoPin = 18; // GPIO 18 no ESP32

// --- Armazena a última distância medida ---
float lastDistance = 0;
float distanciaAnterior = 0;

// --- Cria o servidor web na porta 80 ---
WebServer server(80);

// --- Função para a página WEB ---
void handleRoot() {

  // --- Mapeamento da distancia em litros
  const float distanciaMax = 100.0;    // cm → caixa vazia
  const float distanciaMin = 25.0;    // cm → caixa cheia
  const float capacidadeTotal = 1000; // Capacidade teórica em litros

  // garante que a distância esteja dentro do intervalo válido
  if (lastDistance > distanciaMax) lastDistance = distanciaMax;
  if (lastDistance < distanciaMin) lastDistance = distanciaMin;

  // cálculo do nível em litros
  float nivel_percentual = (distanciaMax - lastDistance) * 100.0 / distanciaMax;
  float nivel_litros = (nivel_percentual / 100.0) * capacidadeTotal;

  if (nivel_percentual < 0) nivel_percentual = 0;
  if (nivel_percentual > 100) nivel_percentual = 100;

  // cor dinâmica
  String cor = "#22c55e"; // verde
  if (nivel_percentual < 30) cor = "#ef4444"; // vermelho
  else if (nivel_percentual < 60) cor = "#facc15"; // amarelo

 // Monta o conteúdo HTML da página
  String html;
  if (lastDistance == 0) {
    html = "<h1>Sem eco (timeout). Aguardando dados...</h1>";
  } else {
    html = "<h1>Distancia atual: " + String(lastDistance, 2) + " cm</h1>";
  }

  html = R"rawliteral(
  <html>
  <head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Dashboard Caixa d'Água</title>

  <style>

  body{
    margin:0;
    font-family:Arial;
    background:#0f172a;
    color:white;
    text-align:center;
  }

  .header{
    font-size:26px;
    padding:20px;
    font-weight:bold;
  }

  .container{
    display:flex;
    flex-direction:column;
    align-items:center;
  }

  .tank{
    width:160px;
    height:280px;
    border:4px solid #94a3b8;
    border-radius:12px;
    margin-top:20px;
    position:relative;
    overflow:hidden;
    background:#1e293b;
  }

  .water{
    position:absolute;
    bottom:0;
    width:100%;
    transition:height 1s;
  }

  .percent{
    font-size:42px;
    font-weight:bold;
    margin-top:10px;
  }

  .cards{
    display:flex;
    gap:15px;
    margin-top:30px;
    flex-wrap:wrap;
    justify-content:center;
  }

  .card{
    background:#1e293b;
    padding:15px;
    border-radius:10px;
    width:140px;
  }

  .label{
    font-size:13px;
    color:#94a3b8;
  }

  .value{
    font-size:22px;
    margin-top:5px;
  }

  .footer{
    margin-top:25px;
    font-size:12px;
    color:#94a3b8;
  }

  button{
    margin-top:15px;
    padding:10px 20px;
    border:none;
    border-radius:6px;
    background:#38bdf8;
    color:white;
    font-size:14px;
  }

  </style>

  <script>
  setTimeout(function(){
  location.reload();
  },5000);
  </script>

  </head>

  <body>

  <div class="header">Caixa d'Água</div>

  <div class="container">
  )rawliteral";

  html += "<div class='tank'>";
  html += "<div class='water' style='height:" + String(nivel_percentual,1) + "%; background:" + cor + ";'></div>";
  html += "</div>";

  html += "<div class='percent'>" + String(nivel_percentual,0) + "%</div>";

  html += "<div class='cards'>";
  html += "<div class='card'>";
  html += "<div class='label'>Distância</div>";

  html += "<div class='value'>" + String(lastDistance,1) + " cm</div>";
  html += "</div>";

  html += "<div class='card'>";
  html += "<div class='label'>Volume</div>";
  html += "<div class='value'>" + String(nivel_litros,0) + " L</div>";
  html += "</div>";

  html += "<div class='card'>";
  html += "<div class='label'>Capacidade</div>";
  html += "<div class='value'>" + String(capacidadeTotal,0) + " L</div>";
  html += "</div>";
  html += "</div>";

  html += R"rawliteral(
    <button onclick="location.reload()">Atualizar</button>

    <div class="footer">
    ESP32 • Monitor de nível IoT
  </div>

  </div>

  </body>
  </html>
   )rawliteral";

  server.send(200, "text/html", html);
}

void reconnectWiFi() { 
  WiFi.disconnect(); 
  delay(1000); 
  WiFi.begin(ssid, password); 
  Serial.println("Reconectando WiFi..."); 
  int tentativas = 0;

  while (WiFi.status() != WL_CONNECTED && tentativas < 20) { 
    delay(500); 
    Serial.print("."); 
    tentativas++; 
    } 
    if (WiFi.status() == WL_CONNECTED) { 
      Serial.println("Reconectado!"); 
    } else { 
      Serial.println("Falha WiFi - reiniciando ESP"); 
      ESP.restart();
      }
    }

void setup() {
  WiFi.setSleep(false); // evita entrar em modo economia e perder conexão
  Serial.println("Iniciando ESP32 e conectando ao wi-fi...");
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // --- Configura LED interno (geralmente pino 2)
  #define LED_BUILTIN 2
  // Configura o pino do LED como saída
  pinMode(LED_BUILTIN, OUTPUT);
  // Deixa o LED desligado até conectar
  digitalWrite(LED_BUILTIN, LOW);

  // --- Conectando ao Wi-Fi
  Serial.print("Conectando à rede: ");
  Serial.println(ssid);
  WiFi.setHostname("sensor-nivel");
  WiFi.begin(ssid, password);

  // Tempo máximo de espera (10 segundos)
  unsigned long startAttemptTime = millis();
  // Aguarda até conectar
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    Serial.print(".");
    delay(500);
    }

  // ---- Verifica se conectou e configura o servidor Web ---
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ Wi-Fi conectado com sucesso!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Hostname: ");
    Serial.println(WiFi.getHostname());
    digitalWrite(LED_BUILTIN, HIGH); // sinaliza sucesso

    // Configuração do servidor Web
    server.on("/", handleRoot);   // rota principal "/"
    server.begin();                // inicia o servidor
    Serial.println("Servidor web iniciado.");

  } else {
    Serial.println();
    Serial.println("❌ Falha ao conectar ao Wi-Fi!");
    //Serial.println("Verifique o SSID e a senha, ou reinicie o roteador.");
    digitalWrite(LED_BUILTIN, LOW);  // mantém LED apagado
  }

  //--- Verifica se há servico de mDNS rodando
  if (!MDNS.begin("sensor-nivel")) {
  Serial.println("Erro ao iniciar mDNS");
  } else {
  Serial.println("mDNS iniciado: http://sensor-nivel.local");
  }
}

void loop() {
  unsigned long lastReconnectAttempt = 0;
  if (WiFi.status() == WL_CONNECTED) {

  // Acende o LED
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500); // espera 0.5 segundo

  // Apaga o LED
  digitalWrite(LED_BUILTIN, LOW);
  delay(500); // espera 0.5 segundo

  // mantém o servidor respondendo
  server.handleClient(); 

  } else {
      Serial.println("Wi-Fi desconectado!");
      //WiFi.reconnect(); // tenta reconectar automaticamente
      if (millis() - lastReconnectAttempt > 10000) { // tenta a cada 10s
      reconnectWiFi();
      lastReconnectAttempt = millis();
      }
    }

  // --- INICIO --- 
  // Mede a distância no Serial Monitor)
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms
  
  temp = lastDistance; // escopo local para atualizar a distanciaAnterior
  if (duration != 0) {
    lastDistance = duration * 0.0343 / 2;
    distanciaAnterior = temp;
    
    Serial.print("Distance: ");
    Serial.print(lastDistance);
    Serial.println(" cm");
  }
  delay(500);
}