#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h" // Biblioteca para o sensor DHT
#include <ArduinoJson.h> 

const char* ssid = "FIAP-IOT";
const char* password = "F!@p25.IOT";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_client_id = "ESP32_DHT_Test"; // ID Único
const char* mqtt_topic = "skillquest/bemestar/ambiente";

// Intervalo de publicação (5 segundos)
const long publishingInterval = 5000;
unsigned long lastMsg = 0;

// Definições para o sensor
#define DHTPIN 4    // Pino GPIO conectado ao pino Data do DHT22
#define DHTTYPE DHT11
#define LDR_PIN 34
#define MIC_PIN 35  // Pino GPIO 35 para o Microfone (Entrada Analógica)

// NOVAS DEFINIÇÕES DE LIMITE DE LUMINOSIDADE
#define LUX_LIMITE_ESCURO 800  // Abaixo deste valor, a luz é muito forte
#define LUX_LIMITE_NOCIVO 2000 // Acima deste valor, a luz é insuficiente

// NOVAS DEFINIÇÕES DE LIMITE DE RUÍDO (Escala 0-100)
#define NOISE_LIMITE_ALTO 70 // Acima deste valor, o ruído atrapalha
#define NOISE_LIMITE_BAIXO 30 // Abaixo deste valor, o ruído é silencioso

WiFiClient espClient;
PubSubClient client(espClient); 

// Inicializa o objeto DHT
DHT dht(DHTPIN, DHTTYPE);


float t; // Variável para temperatura
float h;
int lux; // Variável para umidade (RAW LDR)
int noise; // Variável para o nível de ruído (processado 0-100)
String lightStatus; // Variável para armazenar o status da luz
String noiseStatus; // NOVO: Variável para armazenar o status do ruído

void setup_wifi() {
// ... (setup_wifi mantida) ...
  delay(10);
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
// ... (reconnect mantida) ...
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("conectado!");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void publish_ambient_data() {
  if (!client.connected()) {
    reconnect();
  }

  // Verifica se alguma leitura falhou e trata o erro
  if (isnan(h) || isnan(t)) {
    Serial.println("Falha ao ler o sensor DHT!");
    return;
  }

  // 2. Formatação JSON: Aumentar a capacidade para 5 campos + buffer
  const size_t CAPACITY = JSON_OBJECT_SIZE(5) + 200; // Ajustado o buffer para strings longas
  StaticJsonDocument<CAPACITY> doc;

  // Corrigindo para um tamanho de buffer seguro, como 250
  char payload[250]; 

  doc["temp"] = t;
  doc["hum"] = h;
  doc["lux"] = lux;
  doc["noise"] = noise;
  doc["light_status"] = lightStatus; 
  doc["noise_status"] = noiseStatus; // NOVO CAMPO ADICIONADO

  serializeJson(doc, payload);

  // 3. Publicação MQTT
  Serial.print("Publicando dados: ");
  Serial.println(payload);
  client.publish(mqtt_topic, payload);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  client.loop();
  
  // 1. LEITURA DOS SENSORES
  h = dht.readHumidity(); 
  t = dht.readTemperature();
  lux = analogRead(LDR_PIN);

  // --- 🎙️ LEITURA E PROCESSAMENTO DO RUÍDO (Mantida) ---
  long startMillis = millis();
  int peakToPeak = 0; 
  int sample;
  int signalMax = 0;
  int signalMin = 4095; 

  // Amostra por 50 milissegundos
  while (millis() - startMillis < 50) {
    sample = analogRead(MIC_PIN);
    if (sample < 4095) { 
      if (sample > signalMax) {
        signalMax = sample;
      } else if (sample < signalMin) {
        signalMin = sample;
      }
    }
  }
  peakToPeak = signalMax - signalMin; 
  
  // Mapeia o valor de Pico-a-Pico (ex: de 0 a 1000) para um nível de Ruído mais legível (ex: de 0 a 100)
  noise = peakToPeak / 10; 
  if (noise > 100) noise = 100;

  // --- 💡 LÓGICA DE CLASSIFICAÇÃO DA LUZ (Mantida) ---
  if (lux >= LUX_LIMITE_NOCIVO) {
    lightStatus = "MUITO FORTE (NOCIVA)"; // Simplificando a string para evitar erro de buffer
  } else if (lux >= LUX_LIMITE_ESCURO) {
    lightStatus = "CONFORTÁVEL";
  } else {
    lightStatus = "INSUFICIENTE (ESCURO)";
  }

  // --- 📢 NOVO: LÓGICA DE CLASSIFICAÇÃO DO RUÍDO ---
  if (noise > NOISE_LIMITE_ALTO) {
    noiseStatus = "ALTO - RECOMENDADO ABAIXAR RUÍDO (ATRAPALHA)";
  } else if (noise >= NOISE_LIMITE_BAIXO) {
    noiseStatus = "MODERADO - NÍVEL ACEITÁVEL";
  } else {
    noiseStatus = "BAIXO - SILÊNCIO IDEAL";
  }


  // 3. TRATAMENTO DE ERRO E SAÍDA SERIAL
  if (isnan(h) || isnan(t)) {
      Serial.println("Falha ao ler o sensor DHT para serial!");
  } else {
      Serial.println("--- DADOS AMBIENTAIS ---");
      Serial.print(" Temperatura: ");
      Serial.print(t);
      Serial.print(" °C |  Umidade: ");
      Serial.print(h);
      Serial.println(" %");
  }
  
  // Exibição do Status da Luminosidade
  Serial.print(" Luminosidade (RAW): ");
  Serial.print(lux);
  Serial.print(" -> ");
  Serial.println(lightStatus); 

  // Exibição do Status do Ruído
  Serial.print(" Nível de Ruído (0-100): ");
  Serial.print(noise);
  Serial.print(" -> ");
  Serial.println(noiseStatus); // Imprime a classificação
  Serial.println("------------------------");


  // 4. LÓGICA DE PUBLICAÇÃO
  unsigned long now = millis();
  if (now - lastMsg > publishingInterval) {
    lastMsg = now;
    publish_ambient_data();
  }
}