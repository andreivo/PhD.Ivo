#define TINY_GSM_MODEM_SIM800 //Tipo de modem que estamos usando
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <base64.h>
#include "Security.h"

//MQTT Port
#define MQTT_PORT 1883 

//Interval
#define INTERVAL 500000

//Tempo em que o último envio/refresh foi feito
uint32_t lastTime = 0;

//Canal serial que vamos usar para comunicarmos com o modem. Utilize sempre 1
HardwareSerial SerialGSM(1);
TinyGsm modemGSM(SerialGSM);
TinyGsmClient gsmClient(modemGSM);

//Cliente MQTT, passamos a url do server, a porta
//e o cliente GSM
PubSubClient client(MQTT_SERVER, MQTT_PORT, gsmClient);


void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Modem turn on...");
    pinMode(23, OUTPUT);
    digitalWrite(23, HIGH); //Liga o modem TIP122
    delay(10000);

    pinMode(14, OUTPUT);
    digitalWrite(14, LOW);
  
    setupGSM(); //Inicializa e configura o modem GSM
    connectMQTTServer(); //Conectamos ao mqtt server
    //modemGSM.sleepEnableImpl(true);
    //delay(5000);
}

void loop() {
  //Se desconectou do server MQTT
  if(!client.connected())
  {
    //Mandamos conectar
    connectMQTTServer();
    //digitalWrite(14, LOW);
   // delay(100);
    //modemGSM.sleepEnableImpl(false);
    //delay(5000);
  }

   //Tempo decorrido desde o boot em milissegundos
  unsigned long now = millis();

  //Se passou o intervalo de envio
  if(now - lastTime > INTERVAL)
  {
    //Publicamos para o server mqtt
    publishMQTT();
    //Atualizamos o tempo em que foi feito o último envio
    lastTime = now;
  }
}

void setupGSM()
{
  Serial.println("Setup GSM...");
  //Inicializamos a serial onde está o modem
  SerialGSM.begin(9600, SERIAL_8N1, 16, 17, false);
  delay(3000);

  //Mostra informação sobre o modem
  Serial.println(modemGSM.getModemInfo());

  //Inicializa o modem
  if (!modemGSM.restart())
  {
    Serial.println("Restarting GSM Modem failed");
    delay(10000);
    ESP.restart();
    return;
  }

  //Espera pela rede
  if (!modemGSM.waitForNetwork()) 
  {
    Serial.println("Failed to connect to network");
    delay(10000);
    ESP.restart();
    return;
  }

  //Conecta à rede gprs (APN, usuário, senha)
  if (!modemGSM.gprsConnect("claro.com.br", "claro", "claro")) {
    Serial.println("GPRS Connection Failed");
    delay(10000);
    ESP.restart();
    return;
  }

  Serial.println("Setup GSM Success");
}

void connectMQTTServer() {
  Serial.println("Connecting to MQTT Server...");
  //Se conecta ao device que definimos
  if (client.connect(DEVICE_ID, MQTT_USER, MQTT_PWD)) {
    //Se a conexão foi bem sucedida
    Serial.println("Connected");
  } else {
    //Se ocorreu algum erro
    Serial.print("error = ");
    Serial.println(client.state());
    delay(10000);
    ESP.restart();
  }
}


void publishMQTT()
{
//  //Cria o json que iremos enviar para o server MQTT
//  String msg = createJsonString1();
//  Serial.print("Publish message 1: ");
//  Serial.println(msg);
//  //Publicamos no tópico
//  int status = client.publish(TOPIC, msg.c_str());
//  Serial.println("Status msg 1: " + String(status));//Status 1 se sucesso ou 0 se deu erro
//
//  msg = createJsonString2();
//  Serial.print("Publish message 2: ");
//  Serial.println(msg);
//  //Publicamos no tópico
//  status = client.publish(TOPIC, msg.c_str());
//  Serial.println("Status msg 2: " + String(status));//Status 1 se sucesso ou 0 se deu erro
}

String createJsonString1() 
{
  String payload = "{\"tknDCP\":\"354990401F\",\"pwdDCP\":\"PWDDCP\",\"sntDT\":\"2020-12-06 18:10:24\",\"measures\":[";
  payload = payload+"{\"snsEC\":20,\"dtT\":\"pluvio\",\"colDT\":\"2020-12-06 18:10:24\",\"val\":\"24\"},";
  payload = payload+"{\"snsEC\":21,\"dtT\":\"battery\",\"colDT\":\"2020-12-06 18:10:24\",\"val\":\"143.99977\"}]}";
  return payload;
}

String createJsonString2() 
{
  String payload = "{\"tknDCP\":\"354990401F\",\"pwdDCP\":\"PWDDCP\",\"sntDT\":\"2020-12-06 18:10:24\",\"measures\":[";
  payload = payload+"{\"snsEC\":22,\"dtT\":\"hum\",\"colDT\":\"2020-12-06 18:10:24\",\"val\":\"30\"},";
  payload = payload+"{\"snsEC\":23,\"dtT\":\"temp\",\"colDT\":\"2020-12-06 18:10:24\",\"val\":\"24\"}]}";
  return payload;
}
