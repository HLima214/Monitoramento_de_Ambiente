# Futuro do Trabalho - IOT

## 💡 1. Visão Geral do Projeto

### **1.1. Descrição do Problema**
A **produtividade** e o **bem-estar** em ambientes de trabalho (seja home office ou escritório) são diretamente impactados por fatores ambientais. Ruído excessivo, luminosidade inadequada e desconforto térmico podem levar à fadiga, distração e redução do desempenho cognitivo. O desafio é obter dados em tempo real sobre essas condições para automatizar ajustes ou fornecer recomendações acionáveis.

### **1.2. Solução Proposta**
O projeto utiliza um microcontrolador ESP32 como um gateway IoT para coletar dados de múltiplos sensores e publicar classificações de bem-estar em um broker MQTT. O sistema classifica as condições em tempo real (ex: Ruído Alto, Luz Confortável) e as envia para a nuvem para monitoramento e análise.

### **1.3. Componentes Monitorados**
O sistema monitora e classifica três parâmetros essenciais:

1. 🌡️ Temperatura e Umidade: Medidos pelo sensor DHT11.

2. 💡 Luminosidade: Medida pelo sensor LDR, classificando o ambiente em Nociva, Confortável ou Escuro.

3. 🔊 Nível de Ruído: Medido por um Sensor de Microfone Analógico, classificando o ambiente em Alto, Moderado ou Baixo.



## ⚙️ 2. Instruções de Uso e Configuração
### **2.1. Dependências de Software (Bibliotecas)**
O projeto requer as seguintes bibliotecas instaladas no seu Arduino IDE ou PlatformIO:

| Biblioteca | Propósito | 
| --------   | -----     |
| WiFi.h (Inclusa no ESP32 Core) | Gerencia a conexão Wi-Fi.                                      |
| PubSubClient.h                 | Implementa o protocolo MQTT para comunicação com o broker.     |
| DHT.h                          | Interface com o sensor de temperatura e umidade DHT11.         |
| ArduinoJson.h                  | Usada para serializar os dados dos sensores no formato JSON para publicação.                                                                                       |


### **2.2. Configuração do Código (.ino)**

| Constante | Descrição |
|---------  |-----------|
|const char* ssid|Nome da sua rede Wi-Fi.|
|const char* password|Senha da sua rede Wi-Fi.|
|const char* mqtt_server|Endereço do broker MQTT (pode ser HiveMQ, Mosquitto, etc.).|
|const char* mqtt_client_id|ID único para identificação do cliente no broker.|
|#define LDR_PIN|Pino ADC do ESP32 conectado ao LDR.|
|#define MIC_PIN|Pino ADC do ESP32 conectado ao Microfone.|
|#define DHTPIN|Pino GPIO conectado ao pino de dados do DHT11.|


### **2.3. Diagrama de Conexão (Físico)**
O ESP32 deve ser conectado aos sensores da seguinte forma:
|Componente|Pino do Sensor|Pino do ESP32|Tipo de Pino|
|---------|--------------|------------|--------------|
|DHT11|Data|GPIO 4|Digital|
|LDR (Divisor de Tensão)|Ponto Central|GPIO 34|Analógico (ADC)|
|Microfone (Saída Analógica)|A0 / AO|GPIO 35|Analógico (ADC)|


## 📡 3. Comunicação e Tópicos MQTT
O projeto utiliza o protocolo MQTT (Message Queuing Telemetry Transport) para enviar os dados de forma leve e eficiente para a nuvem.

### Tópico de Publicação (Publish)
|Chave|Valor|Uso|
|----|-----|-----|
|Tópico|<crie seu tópico>|Canal onde os dados são publicados|


### Estrutura do Payload JSON
Os dados são serializados no formato JSON (utilizando ArduinoJson) com as classificações de status já definidas.
```
{
  "temp": 28.7,
  "hum": 77.0,
  "lux": 946,
  "noise": 85,
  "light_status": "MUITO FORTE (NOCIVA)",
  "noise_status": "ALTO - RECOMENDADO ABAIXAR RUÍDO (ATRAPALHA)"
}
```

### Chaves de dados
|Chave JSON|Tipo|Descrição|
|---------|-----|---------|
|temp|Float|Temperatura em graus Celsius (°C).|
|hum|Float|Umidade relativa do ar (%).|
|lux|Integer|Valor RAW (0-4095) do ADC do LDR.|
|noise|Integer|Nível de ruído processado (Escala 0-100).|
|light_status|String|Classificação do LDR (Ex: CONFORVÁVEL).|
|noise_status|String|Classificação do ruído (Ex: ALTO).|


## 🚀 4. Resultados e Impacto
O impacto deste projeto reside na capacidade de transformar dados brutos de sensores em informação acionável para o bem-estar e a produtividade.
- Automação: As plataformas que consomem o tópico MQTT podem ser configuradas para ajustes automáticos (ex: acionar um ar-condicionado se a temperatura ou umidade estiverem fora do limite, ou fechar persianas se light_status for "NOCIVA").
- Monitoramento: Permite que gestores de escritórios ou usuários de home office monitorem as condições remotamente através de um dashboard.
- Melhoria Contínua: Fornece insights baseados em dados para otimizar a acústica, iluminação e ventilação do ambiente de trabalho.

Para ver o vídeo sobre o projeto:

[Clique aqui](https://youtu.be/Q3ORTyTgAx0)
