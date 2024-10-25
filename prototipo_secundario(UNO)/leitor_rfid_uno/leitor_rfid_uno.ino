#include <SPI.h>
#include <MFRC522.h>

#define led_vermelho 2 //LED VERMELHO
#define led_verde 3 //LED VERDE
#define RST_PIN 9 //PINO DE RESET
#define SS_PIN 10 //PINO SDA

MFRC522 leitor(SS_PIN, RST_PIN);  // Cria a instancia do MFRC522

void setup() {
  // put your setup code here, to run once: 
  Serial.begin(9600);
  SPI.begin();
  pinMode(led_vermelho, OUTPUT);
  pinMode(led_verde, OUTPUT);
  leitor.PCD_Init();    // Inicia o modulo MFRC522
}

void loop() {
  int tempo = 600;

  if(! leitor.PICC_IsNewCardPresent()){
    digitalWrite(led_vermelho, HIGH);
    Serial.println("Nenhum card");
    delay(tempo);
    return;
  }

  if(! leitor.PICC_ReadCardSerial()){
    digitalWrite(led_vermelho, LOW);
    digitalWrite(led_verde, HIGH);
    delay(tempo);
    digitalWrite(led_verde, LOW);
    return;
  }

  digitalWrite(led_vermelho, LOW);
  digitalWrite(led_verde, HIGH);
  for (byte i = 0; i < leitor.uid.size; i++)
  {
    Serial.print(leitor.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");
  delay(tempo);
  digitalWrite(led_verde, LOW);
}










