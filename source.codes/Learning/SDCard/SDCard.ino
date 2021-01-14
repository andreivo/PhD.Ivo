/*
  SD card basic file example
 
 This example shows how to create and destroy an SD card file   
 The circuit:
 * SD card attached to SPI bus as follows:
 ** UNO:  MOSI - pin 11, MISO - pin 12, CLK - pin 13, CS - pin 4 (CS pin can be changed)
  and pin #10 (SS) must be an output
 ** Mega:  MOSI - pin 51, MISO - pin 50, CLK - pin 52, CS - pin 4 (CS pin can be changed)
  and pin #52 (SS) must be an output
 ** Leonardo: Connect to hardware SPI via the ICSP header

 created   Nov 2010 by David A. Mellis
 modified 9 Apr 2012 by Tom Igoe
 modified 13 June 2012 by Limor Fried
 
 This example code is in the public domain.
   
 */
#include <SPI.h>
#include <mySD.h>

FileSD root;

// change this to match your SD shield or module;
//     Arduino Ethernet shield: pin 4
//     Adafruit SD shields and modules: pin 10
//     Sparkfun SD shield: pin 8
const int chipSelect = 4;

const int MICROSD_PIN_CHIP_SELECT = 21; // Pino serial
const int MICROSD_PIN_MOSI = 19; // Pino serial
const int MICROSD_PIN_MISO = 18; // Pino serial
const int MICROSD_PIN_SCK = 22; // Clock pin

FileSD myFile;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  delay(500);



  Serial.println("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on Arduino Uno boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
  pinMode(MICROSD_PIN_CHIP_SELECT, OUTPUT);
  pinMode(MICROSD_PIN_MOSI,INPUT_PULLUP);
  
  if (!SD.begin(MICROSD_PIN_CHIP_SELECT, MICROSD_PIN_MOSI, MICROSD_PIN_MISO, MICROSD_PIN_SCK)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  root = SD.open("/");
  
  printDirectory(root, 0);
  
  Serial.println("done!");

  //writeFile();
  //Serial.println("done!");
}

void loop()
{
  // nothing happens after setup finishes.
}

void printDirectory(FileSD dir, int numTabs) {
  // Begin at the start of the directory
  dir.rewindDirectory();
  
  while(true) {
     FileSD entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');   // we'll have a nice indentation
     }
     // Print the 8.3 name
     Serial.print(entry.name());
     // Recurse for directories, otherwise print the file size
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

void writeFile(){
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("xxxx.dht", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
  // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening");
  }
  
  // re-open the file for reading:
  myFile = SD.open("xxxx.DHT");
  if (myFile) {
    Serial.println("xxxx.DHT:");
    String result = "";
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      result = result + (char) myFile.read();
      //char x = (char) myFile.read();
      //if(x=='\n'){
//      Serial.println("");
//      }
//      Serial.print(x);
    }
    // close the file:
    myFile.close();
    Serial.println(result);
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  
}

String getFirstFile(String path) {
  FileSD dir = SD.open(path.c_str());
  // Begin at the start of the directory
  dir.rewindDirectory();
  
  FileSD entry = dir.openNextFile();
  if (!entry) {
      entry.close();
      return "";
  }
  String filename = entry.name();
  entry.close();
  return filename;
       
}
