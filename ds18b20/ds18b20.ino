#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 8

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

float celsius=0;
int red = 7;
int green = 4;
 
void setup(void)
{
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  sensors.begin();
  Serial.begin(9600);
}

void loop(void)
{ 
  sensors.requestTemperatures(); 
  celsius=sensors.getTempCByIndex(0);
  Serial.print(" C  ");
  Serial.print(celsius);
  
  if (celsius > 60) {
      RGB_color(255, 0);  
  } 
  else if (celsius > 50) {
      RGB_color(0, 255);
  } 
  else {
      RGB_color(255, 0);
  }
  
}

void RGB_color(int red_light_value, int green_light_value) {
  analogWrite(red, red_light_value);
  analogWrite(green, green_light_value);
}
