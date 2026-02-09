// --- Bibliotecas
#include <WiFi.h>
#include <WebServer.h>
#include <HardwareSerial.h>

// --- Credenciais da rede Wi-Fi ---
const char* ssid = "ssid-da-rede-wifi";
const char* password = "senha-da-rede-wifi";

const int trigPin = 5; // GPIO 5 no ESP32
const int echoPin = 18; // GPIO 18 no ESP32

// --- Armazena a última distância medida ---
float lastDistance = 0;

// --- Cria o servidor web na porta 80 ---
WebServer server(80);

// --- Função para a página WEB ---
void handleRoot() {
  String html;

  if (lastDistance == 0) {
    html = "<h1>Sem eco (timeout). Aguardando dados...</h1>";
  } else {
    html = "<h1>Distancia atual: " + String(lastDistance, 2) + " cm</h1>";
  }

  // Monta o conteúdo HTML da página
  html += R"rawliteral(
    <hr>
    <p>Servidor Web ESP32 funcionando!</p>
    <button onclick="location.reload()">Atualizar</button>
    <!--
    <script>
      setTimeout(() => { location.reload(); }, 2000);
    </script>
    -->
  )rawliteral";

  server.send(200, "text/html", html);
}

void setup() {
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
    digitalWrite(LED_BUILTIN, HIGH); // sinaliza sucesso

    // Configuração do servidor Web
    server.on("/", handleRoot);   // rota principal "/"
    server.begin();                // inicia o servidor
    Serial.println("Servidor web iniciado.");

  } else {
    Serial.println();
    Serial.println("❌ Falha ao conectar ao Wi-Fi!");
    Serial.println("Verifique o SSID e a senha, ou reinicie o roteador.");
    digitalWrite(LED_BUILTIN, LOW);  // mantém LED apagado
  }
}

void loop() {
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
      Serial.println("Wi-Fi desconectado! Tentando reconectar...");
      WiFi.reconnect(); // tenta reconectar automaticamente
    }

  // Mede a distância no Serial Monitor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms
  if (duration != 0) {
    lastDistance = duration * 0.0343 / 2;
  
    Serial.print("Distance: ");
    Serial.print(lastDistance);
    Serial.println(" cm");
  }
  delay(500);
}
