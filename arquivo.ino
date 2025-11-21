#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

// =====================================================================
//  CONFIGURAÇÕES DE REDE / MQTT / FIWARE
// =====================================================================

// Wi-Fi do Wokwi
const char* SSID = "Wokwi-GUEST";
const char* PASSWORD = "";

// IP do BROKER MQTT (IoT Agent MQTT rodando na VM FIWARE)
const char* BROKER_MQTT = "20.164.0.231";
const int   BROKER_PORT = 1883;

// Tópicos FIWARE (DEVEM SER EXATAMENTE ESTES)
const char* TOPICO_PUBLISH   = "/TEF/noisemonitor001/attrs";   // envio de dados
const char* TOPICO_SUBSCRIBE = "/TEF/noisemonitor001/cmd";     // comandos
const char* ID_MQTT          = "noisemonitor001";              // ID do dispositivo no IoT Agent

// Sensor (potenciômetro simulando ruído)
const int POT_PIN = 34;

// LED onboard e buzzer para alertas
const int LED_PIN = 2;
const int BUZZER_PIN = 27;

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Objetos MQTT
WiFiClient espClient;
PubSubClient MQTT(espClient);

// Controle de frequência das publicações
unsigned long lastPublish = 0;
const unsigned long PUBLISH_INTERVAL = 5000; // envia a cada 5s

// =====================================================================
//  FUNÇÕES DE INICIALIZAÇÃO
// =====================================================================

// Monitor Serial
void initSerial() {
  Serial.begin(115200);
  Serial.println("Inicializando ESP32...");
}

// Conexão Wi-Fi
void initWiFi() {
  Serial.print("Conectando ao WiFi ");
  Serial.print(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }

  Serial.println("\nWiFi conectado!");
}

// Configuração do cliente MQTT
void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);

  Serial.println("MQTT configurado.");
}

// =====================================================================
//  CALLBACK DE COMANDOS (ON/OFF) ENVIADOS PELO FIWARE
// =====================================================================

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  Serial.print("Comando recebido: ");
  Serial.println(msg);

  // O IoT Agent envia comandos no formato:
  // noisemonitor001@on|
  // noisemonitor001@off|

  if (msg == "noisemonitor001@on|") {
    digitalWrite(LED_PIN, HIGH);
  }
  else if (msg == "noisemonitor001@off|") {
    digitalWrite(LED_PIN, LOW);
  }
}

// =====================================================================
//  RECONEXÃO MQTT
// =====================================================================

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.println("Conectando ao MQTT...");

    if (MQTT.connect(ID_MQTT)) {
      Serial.println("MQTT conectado.");
      MQTT.subscribe(TOPICO_SUBSCRIBE);
    } else {
      Serial.println("Falha na conexão. Tentando novamente...");
      delay(2000);
    }
  }
}

// =====================================================================
//  PUBLICAÇÃO DE DADOS PARA O FIWARE
// =====================================================================

// ENVIO EXIGIDO PELO IoT AGENT:
//
// object_id = "noise"
// ENTÃO ENVIAR:
//
//   "noise|<valor>"
//
void publishNoise(int noise) {
  String payload = "noise|" + String(noise);

  MQTT.publish(TOPICO_PUBLISH, payload.c_str());
  Serial.println("Publicado -> " + payload);
}

// =====================================================================
//  ALERTA NO BUZZER
// =====================================================================

void beep(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(120);
    digitalWrite(BUZZER_PIN, LOW);
    delay(120);
  }
}

// =====================================================================
//  LCD
// =====================================================================

void updateLCD(int noise, String status) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Noise: ");
  lcd.print(noise);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Status: ");
  lcd.print(status);
}

// =====================================================================
//  SETUP
// =====================================================================

void setup() {
  initSerial();

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();

  initWiFi();
  initMQTT();
}

// =====================================================================
//  LOOP PRINCIPAL
// =====================================================================

void loop() {
  if (!MQTT.connected()) reconnectMQTT();
  MQTT.loop();

  unsigned long now = millis();
  if (now - lastPublish >= PUBLISH_INTERVAL) {
    lastPublish = now;

    // Lê o potenciômetro
    int raw = analogRead(POT_PIN);
    int noise = map(raw, 0, 4095, 0, 100);

    // Determina status do ambiente
    String status;
    if (noise <= 40)      status = "IDEAL";
    else if (noise <= 70) status = "MODERADO";
    else                  status = "ALTO";

    // Envia para FIWARE
    publishNoise(noise);

    // Atualiza LCD
    updateLCD(noise, status);

    // Alerta (LED + buzzer)
    if (status == "ALTO") {
      digitalWrite(LED_PIN, HIGH);
      beep(1);
      digitalWrite(LED_PIN, LOW);
    }
  }
}
