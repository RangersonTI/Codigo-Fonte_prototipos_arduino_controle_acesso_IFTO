#include <SPI.h>
#include <MFRC522.h>


#define led1 2 //LED VERMELHO
#define led2 3 //LED AMARELO
#define led3 4 //LED VERDE
int buzzer = 6; //BUZZER
#define RST_PIN 9
#define SS_PIN 10

MFRC522 leitor(SS_PIN, RST_PIN);  // Cria a instancia do MFRC522

void setup() {
  // put your setup code here, to run once: 
  Serial.begin(9600);
  SPI.begin();
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(buzzer, OUTPUT);
  leitor.PCD_Init();    // Inicia o modulo MFRC522
  //Serial.println("Aproxime seu cartão: ");
}

void loop() {
  int tempo = 500;

  if(! leitor.PICC_IsNewCardPresent()){
    digitalWrite(led1, HIGH);
    Serial.println("Nenhum card");
    delay(500);
    return;
  }

  if(! leitor.PICC_ReadCardSerial()){
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    tone(buzzer, 150, 100);
    delay(140);
    tone(buzzer, 150, 100);
    delay(tempo);
    digitalWrite(led2, LOW);
    return;
  }
  
  digitalWrite(led1, LOW);
  digitalWrite(led3, HIGH);
  for (byte i = 0; i < leitor.uid.size; i++)
  {
    Serial.print(leitor.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");
  tone(buzzer, 250);
  delay(100);
  tone(buzzer, 740,100);
  delay(tempo);
  digitalWrite(led3, LOW);
}










