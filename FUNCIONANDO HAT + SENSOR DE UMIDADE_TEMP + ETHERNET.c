### FUNCIONANDO HAT + SENSOR DE UMIDADE/TEMP + ETHERNET


#include <Ethernet.h>
#include "DHT.h"
#include <SPI.h>

#define DHTPIN 3
#define DHTTYPE DHT22
#define PORTA_PIN 4

DHT dht(DHTPIN, DHTTYPE);

int ultimoEstadoPorta = HIGH;

// Endereço MAC fictício (pode ser qualquer valor único na sua rede)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 };
// IP fixo 192.168.1.3 e máscara /24
IPAddress ip(192, 168, 0, 9);
//IPAddress gateway(192, 168, 1, 2);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando sensores e rede...");

  // Inicia sensor DHT
  dht.begin();

  // Configura botão/contato seco
  pinMode(PORTA_PIN, INPUT_PULLUP);

  // Inicializa Ethernet com IP fixo
  //Ethernet.begin(mac, ip, gateway, gateway, subnet);
  Ethernet.begin(mac, ip, subnet);
  Serial.print("IP do módulo Ethernet: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // --- Leitura DHT ---
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("Falha na leitura do DHT22!");
  } else {
    Serial.print("Umidade: ");
    Serial.print(umidade);
    Serial.print(" %  |  Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" *C");
  }

  // --- Leitura porta ---
  int estadoPorta = digitalRead(PORTA_PIN);
  if (estadoPorta != ultimoEstadoPorta) {
    if (estadoPorta == LOW) {
      Serial.println("🚪 Porta ABERTA");
    } else {
      Serial.println("🚪 Porta FECHADA");
    }
    ultimoEstadoPorta = estadoPorta;
  }

  delay(500);
}
