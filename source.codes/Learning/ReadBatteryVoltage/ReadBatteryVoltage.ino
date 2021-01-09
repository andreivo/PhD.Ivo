//Analog Input
 
#define ANALOG_PIN_0 34
 
int analog_value = 0;

#define R1 33.0
#define R2 10.0
#define RESISTOR_DIVIDER (R2/(R1+R2))
#define MAX_VOLTAGE_ADC 3.33
#define MAX_VOLTAGE_READ (MAX_VOLTAGE_ADC / RESISTOR_DIVIDER)
#define MAX_ADC_VALUE 4096
#define ADC2VOLTS(adc) ((adc * MAX_VOLTAGE_READ)/MAX_ADC_VALUE)

 
void setup()
{
 Serial.begin(115200);
 delay(1000); // give me time to bring up serial monitor
 Serial.println("ESP32 Analog IN Test");
}
 
void loop()
{
 analog_value = analogRead(ANALOG_PIN_0);
 Serial.print("Voltage: ");
 Serial.print(ADC2VOLTS(analog_value));
 Serial.println("v");
 delay(500); 
}
