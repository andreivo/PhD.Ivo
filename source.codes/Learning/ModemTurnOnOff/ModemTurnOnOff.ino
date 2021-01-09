#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM800L
//SoftwareSerial mySerial(4, 2); //SIM800L Tx & Rx is connected to Arduino #3 & #2
HardwareSerial mySerial(2);

bool modenOn = false;

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(115200);
  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(115200);

  delay(1000);
  Serial.println("Modem turn on...");
  pinMode(23, OUTPUT);
  digitalWrite(23, HIGH); //Liga o modem TIP122
  delay(20000);
  
  Serial.println("Initializing...");
  delay(5000);

  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  mySerial.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  updateSerial();
  mySerial.println("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
  updateSerial();
  mySerial.println("AT+CREG?"); //Check whether it has registered in the network
  updateSerial();

  //digitalWrite(23, LOW); //Liga o modem TIP122
}

void loop()
{
  //mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  //Serial.println("send AT");
  delay(1000);
  updateSerial();
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {

    String str = Serial.readString();
    if(str.substring(0) == "on\r\n"){
      digitalWrite(23, HIGH);
      Serial.println("Modem turn on...");
      delay(10000);
      modenOn = true;
    } 

    if(str.substring(0) == "off\r\n"){
      Serial.println("Modem turn off...");
      digitalWrite(23, LOW);
      modenOn = false;
    }

  }
  while(mySerial.available()) 
  {    
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
