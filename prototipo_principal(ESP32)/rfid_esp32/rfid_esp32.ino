// ESP32 ->   Wi-Fi A/B/G/N

#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h> // Biblioteca para conexao do com RFID-RC522
#include <HTTPClient.h> 
#include <ArduinoJson.h> // Biblioteca para deserealizaação de JSON para arduino
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
JsonDocument docJson

const String ssid = "JUMENTO BRANCO";
const String password = "banana3338";

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

  String tag_rfid_value;
  String status_retornado;
  delay(1000);

  if (!leitor.PICC_IsNewCardPresent()){
    //Serial.print("\nNenhum Card");
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
    tag_rfid_value += String(leitor.uid.uidByte[i], HEX);

    if(i != 3){
      tag_rfid_value += " ";
    }
  }
  tag_rfid_value.toUpperCase();
  Serial.print(tag_rfid_value);

  status_retornado = VerificarStatusRetornado(resultado_json);
  resultado_json = ValidarAcesso(tag_rfid_value);
  status_retornado = deserializeJson(docJson, resultado_json)

  digitalWrite(ledVerde, HIGH);
  tone(buzzer, 250, 100);
  delay(100);
  tone(buzzer, 740, 100);
  delay(700);
  digitalWrite(ledVerde, LOW);
 
}

String ValidarAcesso(String tag_rfid_value){
  String data_json;

  http.begin("http://192.168.1.106:7000/leitor/prototipo_esp32/validarAcesso/");
  http.addHeader("Content-Type","application/json");
  data_json = "{\"tag_rfid_value\": \""+tag_rfid_value+"\", \"cod_esp32\": \"control_acess_ifto_permission_true\"}";
  Serial.println(data_json);
  
  int CodhttpResponse = http.POST(data_json);

  if(CodhttpResponse >0){
    String resultado_json = http.getString();
    Serial.println(CodhttpResponse);
    Serial.println(resultado);
    return resultado_json;
  }
  else{
    print(""+CodhttpResponse+" - Erro durante requisição");
  }

  http.end();
}

String VerificarStatusRetornado(String status_json)

void somAtencao(){
  digitalWrite(ledAmarelo, HIGH);
  tone(buzzer, 150, 100);
  delay(140);
  tone(buzzer, 150, 100);
  delay(140);
  digitalWrite(ledAmarelo, LOW);
}