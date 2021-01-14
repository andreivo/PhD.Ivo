#include <WiFi.h>
#include "SD.h"
#include "SdFat.h"
#include <SPI.h>
#include <Update.h>
#include <ESP32_FTPClient.h>

char ftp_server[] = "files.000webhost.com";
char ftp_user[]   = "cicadadcp";
char ftp_pass[]   = "cicada123";

ESP32_FTPClient ftp (ftp_server,ftp_user,ftp_pass);


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
  
  //startUpdate("update/testeOTA.ino.esp32.bin");

}
void loop() {
  downloadFile();
  //printDirectory("update");

  while (1){
    Serial.println("wait");
    delay(1000); //Wait for 5 seconds before writing the next data 
  }
  
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


void downloadFile(){
  ftp.OpenConnection();

  //Change directory
  ftp.ChangeWorkDir("/public_html/update/all");

  // Get the file size
  const char * fileName = "testeOTA.bin";
  size_t       fileSize = 0;
  String       list[128];

  // Get the directory content in order to allocate buffer
  // my server response is type=file;size=18;modify=20190731140703;unix.mode=0644;unix.uid=10183013;unix.gid=10183013;unique=809g7c8e92e4; helloworld.txt
  ftp.InitFile("Type A");
  ftp.ContentList("", list);
  for( uint8_t i = 0; i < sizeof(list); i++)
  {
    uint8_t indexSize = 0;
    uint8_t indexMod  = 0;

    if(list[i].length() > 0)
    {
      //list[i].toLowerCase();
      
      if( list[i].indexOf(fileName) > -1 )
      {
        indexSize = list[i].indexOf("size") + 5;
        indexMod  = list[i].indexOf("modify") - 1;

        fileSize = list[i].substring(indexSize, indexMod).toInt();
      }

      // Print the directory details
      Serial.println(list[i]);
    }
    else
      break;
  }

  // Print file size
  Serial.println("\nFile size is: " + String(fileSize));

  //Dynammically alocate buffer
  unsigned char * downloaded_file = (unsigned char *) malloc(fileSize);

  // And download the file
Serial.println("0");  
  ftp.InitFile("Type I");
Serial.println("1");  
  ftp.DownloadFile(fileName, downloaded_file, fileSize, false);
Serial.println("2");
  printBuffered(downloaded_file, fileSize);
Serial.println("3");
//  File32 myFile;
//  if (myFile.open("update/test.bin", O_WRITE | O_CREAT)) {
//      myFile.write(&downloaded_file, fileSize);
//      //myFile.println(downloaded_file);
//      // close the file:
//      myFile.close();
//  } else {
//      // if the file didn't open, print an error:
//      Serial.print(F("Error opening "));
//      Serial.println("update/test.bin");
//  }
  Serial.println("finish");
}

void printBuffered( unsigned char * data, int dataLength) {
  unsigned char clientBuf[dataLength];
  size_t clientCount = 0;
  
  for(int i = 0; i < dataLength;i++){
    Serial.write(data[i]);
//    clientBuf[clientCount] = data[i];
//    //client.write(data[i])
//    clientCount++;
//    if (clientCount > bufferSize-1) {
//        cli->write(clientBuf, bufferSize); 
//      clientCount = 0;
//    }
  }
  
//  if (clientCount > 0){
//      cli->write(clientBuf, clientCount); 
//  }
}
