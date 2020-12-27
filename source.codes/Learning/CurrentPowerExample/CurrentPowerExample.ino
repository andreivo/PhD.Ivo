//Programa: Medidor de corrente com sensor ACS712
//Autor: Arduino e Cia

//int valor = 0;
String str;
int tamanho;

const int analogIn = 13;
int mVperAmp = 66; 
int RawValue = 0;
int ACSoffset = 1673;
double Voltage = 0;
double Amps = 0;

void draw()
{
  //Comandos graficos para o display devem ser colocados aqui
  Serial.print("Corrente (mA)");
  Serial.println(Amps*1000);  
  Serial.print("Voltage (V)");
  Serial.println(Voltage/1000);  
}

void setup(void)
{
  Serial.begin(115200);
  delay(500);
  Serial.println("Sensor de Corrente ACS712"); 
  Serial.println("");
  Serial.println("");
}

void loop(void)
{
  Calcula_corrente();
  draw();
  delay(1000);
}

void Calcula_corrente()
{
  RawValue = analogRead(analogIn);
  Voltage = (RawValue * 1660) / 1970; // Gets you mV 
  Amps = ((Voltage - ACSoffset) / mVperAmp);
  delay(2000);
}
