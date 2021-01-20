#include <WiFi.h>
#include "SD.h"
#include "SdFat.h"
#include <SPI.h>
#include <Update.h>

WiFiClient client;

long contentLength = 0;
bool isValidContentType = false;


const int MICROSD_PIN_CHIP_SELECT = 21; // Pino serial
const int MICROSD_PIN_MOSI = 19; // Pino serial
const int MICROSD_PIN_MISO = 18; // Pino serial
const int MICROSD_PIN_SCK = 22; // Clock pin

// SdFat software SPI template
SoftSpiDriver<MICROSD_PIN_MISO, MICROSD_PIN_MOSI, MICROSD_PIN_SCK> softSpi;
// Speed argument is ignored for software SPI.
#define SD_CONFIG SdSpiConfig(MICROSD_PIN_CHIP_SELECT, DEDICATED_SPI, SD_SCK_MHZ(0), &softSpi)

SdFat32 sd;

void setup() {
  Serial.begin(115200);
  delay(500);
  
  connectWiFi();
       
  if (!sd.begin(SD_CONFIG)) {   
    Serial.println("Card Mount Failed");
    return;
  }
    
  Serial.println("Initializing SD card...");
  //if (!SD.begin(MICROSD_PIN_CHIP_SELECT)) {
  if (!sd.begin(SD_CONFIG)) {    
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }

  Serial.println("SD card initialization SUCSSES!");

  
  printDirectory("update");
  downloadFile();
  printDirectory("update");
  startUpdate("update/cicadaDCP.bin");

}
void loop() {

//    delay(5000); //Wait for 5 seconds before writing the next data 
}

void printDirectory(String path) {
    int attempts = 0;
    File32 dir;
    File32 myFile;

    // Open root directory
    if (!dir.open(path.c_str())) {
        Serial.println(F("Error dir.open failed"));
    }
    dir.rewindDirectory();

    while (myFile.openNext(&dir, O_RDONLY)) {
        char fileName[40];
        int len = 15;
        myFile.getName(fileName, sizeof (fileName));
        if (myFile.isDir()) {
            // Indicate a directory.
            Serial.write('/');
            len = 14;
        }

        Serial.print(fileName);
        Serial.print(" ");
        myFile.printModifyDateTime(&Serial);
        Serial.print(" ");
        myFile.printFileSize(&Serial);
        Serial.println(" bytes");

        myFile.close();
    }

    if (dir.getError()) {
        Serial.println(F("OpenNext failed"));
    }
}

boolean startUpdate(String filename) {  
  uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
  if (!Update.begin(maxSketchSpace, U_FLASH)) { //start with max available size
    Update.printError(Serial);
    Serial.println("ERROR");
  }
  
  //String result = "";
  File32 myFile;
  if (myFile.open(filename.c_str(), O_RDONLY)) {
      // read from the file until there's nothing else in it:
      while (myFile.available()) {
          //result = result + (char) myFile.read();
          uint8_t ibuffer[128];
          myFile.read((uint8_t *)ibuffer, 128);
          //Serial.println((char *)ibuffer);
          Update.write(ibuffer, sizeof(ibuffer));                    
      }
      Serial.print(Update.end(true));
      // close the file:
      myFile.close();
      ESP.restart();  
      
  } else {
      // if the file didn't open, print an error:
      Serial.print(F("Error opening: "));
      Serial.println(filename);
  }
  return true;        
}

void connectWiFi()
{
    Serial.println("Connecting");

    //Troque pelo nome e senha da sua rede WiFi
    WiFi.begin("IVO", "FELIPE1V0");
    
    //Espera enquanto não estiver conectado
    while(WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println();
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
}

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}

// OTA Logic 
void downloadFile() {
  
  String host = "www.cemaden.gov.br";
  int port = 80;
  String bin = "/arquivos/cigarra/CicadaOTA.bin";

  
  Serial.println("Connecting to: " + String(host));
  // Connect to S3
  if (client.connect(host.c_str(), port)) {
    // Connection Succeed.
    // Fecthing the bin
    Serial.println("Fetching Bin: " + String(bin));

    // Get the contents of the bin file
    client.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    // Check what is being sent
        Serial.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Cache-Control: no-cache\r\n" +
                     "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 50000) {
        Serial.println("Client Timeout !");
        client.stop();
        return;
      }
    }
    // Once the response is available,
    // check stuff

    /*
       Response Structure
        HTTP/1.1 200 OK
        x-amz-id-2: NVKxnU1aIQMmpGKhSwpCBh8y2JPbak18QLIfE+OiUDOos+7UftZKjtCFqrwsGOZRN5Zee0jpTd0=
        x-amz-request-id: 2D56B47560B764EC
        Date: Wed, 14 Jun 2017 03:33:59 GMT
        Last-Modified: Fri, 02 Jun 2017 14:50:11 GMT
        ETag: "d2afebbaaebc38cd669ce36727152af9"
        Accept-Ranges: bytes
        Content-Type: application/octet-stream
        Content-Length: 357280
        Server: AmazonS3
                                   
        {{BIN FILE CONTENTS}}

    */
    while (client.available()) {
      // read line till /n
      String line = client.readStringUntil('\n');
      // remove space, to check if the line is end of headers
      line.trim();

      // if the the line is empty,
      // this is end of headers
      // break the while and feed the
      // remaining `client` to the
      // Update.writeStream();
      if (!line.length()) {
        //headers ended
        break; // and get the OTA started
      }

      // Check if the HTTP Response is 200
      // else break and Exit Update
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
          Serial.println(line);
          break;
        }
      }

      // extract headers here
      // Start with content length
      if (line.startsWith("Content-Length: ")) {
        contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
        Serial.println("Got " + String(contentLength) + " bytes from server");
      }

      // Next, the content type
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        Serial.println("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }
  } else {
    // Connect to S3 failed
    // May be try?
    // Probably a choppy network?
    Serial.println("Connection to " + String(host) + " failed. Please check your setup");
    // retry??
    // execOTA();
  }

  // Check what is the contentLength and if content type is `application/octet-stream`
  Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

  // check contentLength and content type
  if (contentLength && isValidContentType) {
    // create buffer for read
    uint8_t buff[128] = { 0 };
    File32 myFile;
    if (myFile.open("update/test.bin", O_WRONLY | O_CREAT)) {
      // read all data from server
      while(contentLength > 0 || contentLength == -1) {
          // get available data size
          size_t size = client.available();
          if(size) {
              // read up to 128 byte
              int c = client.readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

              // write it to Serial
              myFile.write(buff, c);

              if(contentLength > 0) {
                  contentLength -= c;
              }
          }
          delay(1);
      }
      myFile.close();  
      Serial.println("finish");
    }
    
  } else {
    Serial.println("There was no content in the response");
    client.flush();
  }
}
