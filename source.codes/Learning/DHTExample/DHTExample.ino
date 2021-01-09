#include <SimpleDHT.h>


SimpleDHT22 dht;

#define DHT_PIN 27

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  Serial.println("ler: ");
  // put your main code here, to run repeatedly:
  readDHT();
  delay(500);
}

void readDHT()
{
  float t, h;  
  if (dht.read2(DHT_PIN, &t, &h, NULL) == SimpleDHTErrSuccess)
  {
    
    Serial.print("temperature = ");
    Serial.println(t);
    Serial.print("humidity = ");
    Serial.println(h);
  }else{
    Serial.println("Erro ao ler!");
    }
}
