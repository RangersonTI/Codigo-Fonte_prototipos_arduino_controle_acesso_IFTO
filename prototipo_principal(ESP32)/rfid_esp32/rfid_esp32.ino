// ESP32 ->   Wi-Fi A/B/G/N

#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h> // Biblioteca para conexao do com RFID-RC522
#include <HTTPClient.h>
/*
Conexoes pino RFID-RC522

VSPI RST - RST -> 4 (BRANCO)
VSPI MOSI - MOSI -> 23 (ROXO)
VSPI MISO - MISO -> 19 (CINZA)
VSPI SCK - SCK -> 18 (AZUL)
VSPI SS - SS (SDA) -> 5 (VERDE)
VCC -> 3V (VERMELHO
GND -> GND (PRETO)

*/
#define ledAmarelo 17
#define ledVerde 21
#define ledVermelho 16
#define buzzer 2
#define RST_PIN 4
#define SS_PIN 5

MFRC522 leitor(SS_PIN, RST_PIN);
WiFiClient client;
HTTPClient http;

const String ssid = "JUMENTO BRANCO";
const String password = "banana3338";

IPAddress addr_server(192,168,1,105);

void setup() {
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  Serial.begin(9600);
  SPI.begin();
  leitor.PCD_Init(); //Inicia o módulo MFRC522
}

void loop() {
  digitalWrite(ledVermelho, HIGH);

  if(WiFi.status() != WL_CONNECTED){
    digitalWrite(ledVermelho, LOW);
    VerificarConexao();
  }

  VerificarCard();
}

void VerificarConexao(){
  while (WiFi.status() != WL_CONNECTED){
    Serial.print("Estabilizando conexao");
    WiFi.begin(ssid,password);
    digitalWrite(ledVermelho,HIGH);
    delay(200);
    digitalWrite(ledVermelho,LOW);
    delay(200);
    digitalWrite(ledVermelho,HIGH);
    delay(200);
    digitalWrite(ledVermelho,LOW);
    delay(200);
    digitalWrite(ledVermelho,HIGH);
    delay(200);
    digitalWrite(ledVermelho,LOW);
    delay(200);
    digitalWrite(ledVermelho,HIGH);
    delay(200);
    digitalWrite(ledVermelho,LOW);
    delay(1000);

    if (WiFi.status() == WL_CONNECTED){
      digitalWrite(ledVerde,HIGH);
      delay(1000);
      digitalWrite(ledVerde,LOW);
      delay(50);
      Serial.println("Conexao estabilizada - ");
      Serial.print(WiFi.localIP());
    }
  }
}

// Método para verificar se há card sendo inserido

void VerificarCard(){

  String rfid_data;
  delay(1000);

  if (!leitor.PICC_IsNewCardPresent()){
    Serial.print("\nNenhum Card");
    return;
  }

  if(!leitor.PICC_ReadCardSerial()){
    Serial.println("Erro de leitura");
    digitalWrite(ledVermelho, LOW);
    somAtencao();
    return;
  }

  digitalWrite(ledVermelho, LOW);
  Serial.print("\n");
  for (byte i = 0; i < leitor.uid.size; i++){
    rfid_data += String(leitor.uid.uidByte[i], HEX);

    if(i != 3){
      rfid_data += " ";
    }
  }
  rfid_data.toUpperCase();
  Serial.print(rfid_data);
  digitalWrite(ledVerde, HIGH);
  tone(buzzer, 250, 100);
  delay(100);
  tone(buzzer, 740, 100);
  delay(700);
  digitalWrite(ledVerde, LOW);
 
}

void ValidarAcesso(){
  String data_json;

  http.begin("https://192.168.1.105/prototipo_esp32/validarAcesso/");
  http.addHeader("Content-Type","application/json");
  data_json = ""

}

void somAtencao(){
  digitalWrite(ledAmarelo, HIGH);
  tone(buzzer, 150, 100);
  delay(140);
  tone(buzzer, 150, 100);
  delay(140);
  digitalWrite(ledAmarelo, LOW);
}