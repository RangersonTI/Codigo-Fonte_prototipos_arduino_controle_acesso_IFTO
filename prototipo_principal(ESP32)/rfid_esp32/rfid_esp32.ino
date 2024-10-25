// ESP32 ->   Wi-Fi A/B/G/N

#include <WiFi.h>
#include <time.h>
#include <SPI.h>
#include <MFRC522.h> // Biblioteca para conexao do com RFID-RC522
#include <HTTPClient.h> 
#include <ArduinoJson.h> // Biblioteca para deserealização de JSON para arduino
#include <LiquidCrystal_I2C.h> // Biblioteca para utilização do LCD
/*

 * BIBLIOTECAS E PLACAS UTILIZADAS
    * esp32 (by Espressif v3.0.5) -> placa
    * MFRC522 (by GithubCommunity v1.4.11) -> biblioteca
    * ArduinoJson (Benoi Blanchot v7.1.0) -> biblioteca
    * LiquidCrystal_I2C (Frank de Brabander v1.1.2) -> biblioteca

 * CONEXOES (PINAGEM) RFID-RC522

   * VSPI RST - RST -> 4 (BRANCO)
   * VSPI MOSI - MOSI -> 23 (ROXO)
   * VSPI MISO - MISO -> 19 (CINZA)
   * VSPI SCK - SCK -> 18 (AZUL)
   * VSPI SS - SS (SDA) -> 5 (VERDE)
   * VCC -> 3V (VERMELHO
   * GND -> GND (PRETO)

*/
#define ledAmarelo 17
#define ledVerde 0
#define ledVermelho 16
#define buzzer 2
#define RST_PIN 4
#define SS_PIN 5

MFRC522 leitor(SS_PIN, RST_PIN); // INSTANCIA O OBJETO PARA UTILIZAÇÃO DO mfrc522 (leitor do RFID)
WiFiClient client;
HTTPClient http;
JsonDocument docJson;
LiquidCrystal_I2C lcd(0x27,20,4); /* INSTANCIA O OBJETO PARA O USO DO LCD AO ESP32, PASSANDO OS SEGUINTE 
                                   PARAMETROS DE CONFIGURAÇÃO: endereco do lcd, qtd_linhas e qtd_colunas*/

const String ssid = "ssid";
const String password = "password";
long timezone = -3; // Fuso horario BMT corresdente ao país (BRASIL)
byte daysavetime = 1;

void setup() {
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  Serial.begin(9600);
  SPI.begin();
  leitor.PCD_Init(); //INICIA O MODULO * MFRC522 *
  lcd.init(); // INICIA
  lcd.backlight(); // ACIONA A LUZ DE FUNDO DO LCD
  lcd.display(); // ACIONA O FUNDO DE TEXTO DO LCD
  configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
}

void loop() {
  digitalWrite(ledVermelho, HIGH);

  if(WiFi.status() != WL_CONNECTED){
    digitalWrite(ledVermelho, LOW);
    lcd.print("Estabelecendo");
    lcd.setCursor(0, 1);
    lcd.print("conexao ...");
    VerificarConexao();
    lcdMsgBoasVindas();
  }

  VerificarCard();
}

// METODO PARA VERIFICAR E CONECTAR O PROTOTIPO COM A INTERNET

void VerificarConexao(){
  while (WiFi.status() != WL_CONNECTED){
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
    delay(200);
    digitalWrite(ledVermelho,HIGH);
    delay(200);
    digitalWrite(ledVermelho,LOW);
    delay(1500);

    if (WiFi.status() == WL_CONNECTED){
      digitalWrite(ledVerde,HIGH);
      lcd.clear();
      lcd.print("Conexao");
      lcd.setCursor(0,1);
      lcd.print("estabelecida - :)");
      delay(3000);
      digitalWrite(ledVerde,LOW);
      Serial.print(WiFi.localIP());
    }
  }
}

// METODO PARA VERIFICAR SE H CARD SENDO INSERIDO

void VerificarCard(){

  String tag_rfid_value;
  String status_retornado;
  delay(700);

  if (!leitor.PICC_IsNewCardPresent()){
    //Serial.print("\nNenhum Card");
    return;
  }

  if(!leitor.PICC_ReadCardSerial()){
    Serial.println("Erro de leitura");
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
  Serial.println(tag_rfid_value);

  String resultado_json = ValidarAcesso(tag_rfid_value);
  
  VerificarStatusRetornado(resultado_json); 
}


// METODO RESPONSAVEL POR COMUNICAR COM SERVIDOR PARA VALIDAR O ACESSO

String ValidarAcesso(String tag_rfid_value){
  String data_json;

  http.begin("http://192.168.1.110:7000/leitor/prototipo_esp32/validarAcesso/"); // Aqui deverá ser alterado para o ip/dns de produção da aplicação
  http.addHeader("Content-Type","application/json");
  data_json = "{\"tag_rfid_value\": \""+tag_rfid_value+"\", \"cod_esp32\": \"control_acess_ifto_permission_true\"}";

  int CodhttpResponse = http.POST(data_json);

  if(CodhttpResponse >0){
    String resultado_json = http.getString();
    Serial.println(CodhttpResponse);
    Serial.println(resultado_json);
    http.end();
    return resultado_json;
  }
  else{
    http.end();
    String erro = "Erro durante requisição";
    return erro;
  }
}


// METODO PARA VERIFICAR O STATUS (cod) RETORNADO PELO SERVIDOR (conversao de JSON)

void VerificarStatusRetornado(String status){
/*
  Codigos de Status para a Validacao e Cadastro de Acesso
    
    save_acess : Historico de acesso salvo... Liberado :)
    erro_to_save_acess : Erro ao registrar o acesso
    rfid_unidentified: RFID nao vinculado
    rfid_not_found : RFID invalido
*/
    lcd.clear();
    deserializeJson(docJson, status);
    String status_retornado = ((docJson["Status"]).as<String>());

    if(status_retornado == "save_acess"){
      String date = AtualizarData();
      String hour = AtualizarHora();

      lcd.setCursor(2, 0);
      lcd.print(date);
      lcd.setCursor(14, 0);
      lcd.print(hour);
      lcd.setCursor(2, 2);
      lcd.print("Acesso Liberado...");
      lcd.setCursor(10, 3);
      lcd.print(":)");
      acessoLiberado();
      lcdMsgBoasVindas();
    }
    else if(status_retornado == "erro_to_save_acess"){
      String date = AtualizarData();
      String hour = AtualizarHora();

      lcd.setCursor(2, 0);
      lcd.print(date);
      lcd.setCursor(14, 0);
      lcd.print(hour);
      lcd.setCursor(3, 2);
      lcd.print("Erro ao regis-"); // Erro da parte do servidor
      lcd.setCursor(3, 2);
      lcd.print("trar o acesso.");
      somAtencao();
      lcdMsgBoasVindas();
    }
    else if(status_retornado == "rfid_unidentified"){

      String date = AtualizarData();
      String hour = AtualizarHora();

      lcd.setCursor(2, 0);
      lcd.print(date);
      lcd.setCursor(14, 0);
      lcd.print(hour);
      lcd.setCursor(1, 2);
      lcd.print("RFID nao vinculado.");
      somAtencao();
      lcdMsgBoasVindas();
    }
    else if(status_retornado == "rfid_not_found"){
      String date = AtualizarData();
      String hour = AtualizarHora();

      lcd.setCursor(2, 0);
      lcd.print(date);
      lcd.setCursor(14, 0);
      lcd.print(hour);
      lcd.setCursor(3, 2);
      lcd.print("RFID invalido.");
      somAtencao();
      lcdMsgBoasVindas();
    }
    else{
      String date = AtualizarData();
      String hour = AtualizarHora();

      lcd.setCursor(2, 0);
      lcd.print(date);
      lcd.setCursor(14, 0);
      lcd.print(hour);
      lcd.setCursor(0, 2);
      lcd.print(status);
      somAtencao();
      lcdMsgBoasVindas();
    }
}

void somAtencao(){
  digitalWrite(ledVermelho, LOW);
  digitalWrite(ledAmarelo, HIGH);
  tone(buzzer, 150, 110);
  delay(150);
  tone(buzzer, 150, 110);
  delay(2000);
  digitalWrite(ledAmarelo, LOW);
}

void acessoLiberado(){
  digitalWrite(ledVermelho, LOW);
  digitalWrite(ledVerde, HIGH);
  tone(buzzer, 250, 120);
  delay(120);
  tone(buzzer, 740, 150);
  delay(2000);
  digitalWrite(ledVerde, LOW);
}

void lcdMsgBoasVindas(){
  String date = AtualizarData();
  String hour = AtualizarHora();
  
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Bem-vindo ao IFTO");
  lcd.setCursor(5, 1);
  lcd.print(date);
  lcd.setCursor(8, 2);
  lcd.print(hour);
}

String AtualizarData(){
  
  struct tm tmstruct ;

  tmstruct.tm_year = 0;
  getLocalTime(&tmstruct);

  if ((String(tmstruct.tm_mday)).length() == 1){
    if (String(( tmstruct.tm_mon) + 1).length() == 1){
      return ("0" + String(tmstruct.tm_mday) + "-0" + String(( tmstruct.tm_mon) + 1) + "-" + String((tmstruct.tm_year) + 1900) );
    }

    return ("0" + String(tmstruct.tm_mday) + "-" + String(( tmstruct.tm_mon) + 1) + "-" + String((tmstruct.tm_year) + 1900) );
  }
  else{
    return (String(tmstruct.tm_mday) + "-" + String(( tmstruct.tm_mon) + 1) + "-" + String((tmstruct.tm_year) + 1900) );
  }
}

String AtualizarHora(){
  
  struct tm tmstruct ;
  getLocalTime(&tmstruct);

  if ((String(tmstruct.tm_min)).length() == 1){
    return (String(tmstruct.tm_hour-1) + ":0" + (String(tmstruct.tm_min)));
  }
  else{
    return (String(tmstruct.tm_hour-1) + ":" + (String(tmstruct.tm_min)));
  }
}