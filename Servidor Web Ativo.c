#include <Ethernet.h>
#include <SPI.h>        // Biblioteca permite comunicação do Shield ENC28J60 com a placa Arduino
#include <DHT.h>        // Inclusão da biblioteca DHT para leitura do sensor

// Configuração de rede
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 };
IPAddress ip(192, 168, 0, 9);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server(80); // Porta de conexão do servidor WEB

// Configuração do sensor DHT22
#define DHTPIN 3
#define DHTTYPE DHT22     // <- ALTERADO AQUI para DHT22
DHT dht(DHTPIN, DHTTYPE);

// Configuração do contato seco
#define PORTA_PIN 4
int ultimoEstadoPorta = HIGH; // Assume porta fechada no início

void setup() {
    Ethernet.begin(mac, ip, subnet);
    server.begin();
    dht.begin();

    pinMode(PORTA_PIN, INPUT_PULLUP); // Configura contato seco
    Serial.begin(9600); // Para depuração no monitor serial
}

void loop() {
    delay(2000); // Tempo de espera entre medições

    // Leitura da temperatura e umidade
    float temp = dht.readTemperature();
    float umid = dht.readHumidity();

    // Verificação de leitura válida
    if (isnan(temp) || isnan(umid)) {
        Serial.println("❌ Falha na leitura do sensor DHT22");
        return;
    }

    // Leitura do estado da porta
    int estadoPorta = digitalRead(PORTA_PIN);
    String statusPorta = (estadoPorta == LOW) ? "ABERTA" : "FECHADA";

    // Log no serial
    if (estadoPorta != ultimoEstadoPorta) {
        Serial.print("🚪 Porta ");
        Serial.println(statusPorta);
        ultimoEstadoPorta = estadoPorta;
    }

    Serial.print("🌡️ Temperatura lida: ");
    Serial.print(temp);
    Serial.print(" ºC | 💧 Umidade lida: ");
    Serial.print(umid);
    Serial.println(" %");

    // Conexão com cliente
    EthernetClient client = server.available();
    if (client) {
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                if (c == '\n') {
                    // Cabeçalho e conteúdo da página HTML
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println();
                    client.println("<!DOCTYPE html>");
                    client.println("<html>");
                    client.println("<head>");
                    client.println("<meta charset='utf-8'/>");
                    client.println("<title>Servidor Arduino</title>");
                    client.println("</head>");
                    client.println("<body style='background-color:#ADF2E3'>");
                    client.println("<center><font color='blue'><h1>ETICE - SISTEMA DE MONITORAMENTO</h1></font></center>");
                    client.println("<center><font color='blue'><h1>DWDM DE _TESTE_ </h1></font></center>");
                    client.println("<h1><center>MONITORAMENTO DE TEMPERATURA, UMIDADE E PORTA</center></h1>");
                    client.println("<br/>");
                    client.println("<hr />");

                    // Temperatura
                    client.print("<center><font size='6'>Temperatura: ");
                    client.print(String(temp, 1));  // Uma casa decimal
                    client.println(" ºC</font></center>");
                    client.println("<hr />");

                    // Umidade
                    client.print("<center><font size='6'>Umidade: ");
                    client.print(String(umid, 1));  // Uma casa decimal
                    client.println(" %</font></center>");
                    client.println("<hr />");

                    // Estado da porta
                    client.print("<center><font size='6'>Estado da Porta: ");
                    client.print(statusPorta);
                    client.println("</font></center>");
                    client.println("<hr />");

                    client.println("</body></html>");

                    client.stop();
                }
            }
        }
    }
}

