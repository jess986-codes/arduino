#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#include <SPI.h>          // f.k. for Arduino-1.5.2
#include <Adafruit_GFX.h>  // Hardware-specific library
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define MINPRESSURE 200
#define MAXPRESSURE 1000
#define ONE_WIRE_BUS 51

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float celsius=0;

const int XP=8,XM=A2,YP=A3,YM=9; //240x320 ID=0x9595
const int TS_LEFT = 892,TS_RT = 164,TS_TOP = 108,TS_BOT = 901;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button up_min_btn, down_min_btn, up_max_btn, down_max_btn, settings_btn, save_btn;

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.y, TS_TOP, TS_BOT, 0, 320);
        pixel_y = map(p.x, TS_RT, TS_LEFT, 0, 240);
        Serial.println(pixel_x);
        Serial.println(pixel_y);

    }
    return pressed;
}

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


int maxTemp = 21;
int minTemp = 19;
int currentTemp;
int tempTemp;
bool change = false;
bool inSettings = false;

void setup(void)
{
    sensors.begin();
    Serial.begin(9600);
    sensors.requestTemperatures();
    currentTemp=floor(sensors.getTempCByIndex(0));
    tempTemp = currentTemp;
    Serial.println(tempTemp);
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    Serial.println("Calibrate for your Touch Panel");
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(1);            //LANDSCAPE
    tft.fillScreen(RED);

    display();   
}

void display(void) {

    if (change) {
      tft.setTextColor(BLACK); 
    } else {
     tft.setTextColor(WHITE);   
    }
    tft.setCursor(100, 64);
    tft.setTextSize(6);
    tft.print(currentTemp);
    tft.println(" C");

    if (change) {
      tft.setTextColor(BLUE); 
    } else {
     tft.setTextColor(YELLOW);   
    }
    tft.setTextColor(YELLOW);  
    tft.setTextSize(2.5);
    tft.setCursor(16, 180);
    tft.print("MIN: ");
    tft.print(minTemp);
    tft.println(" C");

    tft.setCursor(190, 180);
    tft.print("MAX: ");
    tft.print(maxTemp);
    tft.println(" C");

    settings_btn.initButton(&tft,  250, 20, 112, 28, WHITE, BLACK, WHITE, "settings", 2);
    settings_btn.drawButton(false);

    save_btn.initButton(&tft,  250, 20, 112, 28, WHITE, BLACK, WHITE, "save", 2);
    
    up_min_btn.initButton(&tft,  100, 160, 50, 24, WHITE, CYAN, BLACK, "up", 2);

    down_min_btn.initButton(&tft,  100, 215, 50, 24, WHITE, CYAN, BLACK, "down", 2);

    up_max_btn.initButton(&tft,  270, 160, 50, 24, WHITE, CYAN, BLACK, "up", 2);

    down_max_btn.initButton(&tft,  270, 215, 50, 24, WHITE, CYAN, BLACK, "down", 2);
}

void loop(void)
{       
    bool down = Touch_getXY();
    settings_btn.press(down && settings_btn.contains(pixel_x, pixel_y));
    Serial.println(pixel_x);
    Serial.println(pixel_y);
    
    if (settings_btn.justPressed() && !inSettings) {
      inSettings = true;
      settings_btn.drawButton(true);

      if (change) {
        tft.fillRect(250, 20, 112, 28, GREEN);
      } else {
        tft.fillRect(250, 20, 112, 28, RED);
      }

      save_btn.drawButton(false);
      up_min_btn.drawButton(false);
      down_min_btn.drawButton(false);
      up_max_btn.drawButton(false);
      down_max_btn.drawButton(false);

    }

    if (!inSettings) {
      sensors.requestTemperatures();
      tempTemp = floor(sensors.getTempCByIndex(0));
      Serial.println(tempTemp);
      
      if (tempTemp != currentTemp) {
        Serial.println("I'm in");
        currentTemp = tempTemp;
        tft.setCursor(100, 64);
        if (change) {
          tft.setTextColor(GREEN); 
        } else {
         tft.setTextColor(RED);   
        }
        tft.setTextSize(6);
        tft.print(currentTemp);
        tft.println(" C");

        if (change) {
          tft.setTextColor(BLACK); 
        } else {
         tft.setTextColor(WHITE);   
        }
  
        tft.setCursor(100, 64);
        tft.setTextSize(6);
        tft.print(currentTemp);
        tft.println(" C");
      }
  
      if (currentTemp <= maxTemp && !change) {
        Serial.println(change);
        change = true;
        tft.fillScreen(GREEN);
        display();
      } else if (currentTemp < minTemp && change){
        change = false;
        tft.fillScreen(RED);
        display();
      } else if (currentTemp > maxTemp + 1 && change) {
        change = false;
        tft.fillScreen(RED);
        display();
      }
      
    } else {
      save_btn.press(down && save_btn.contains(pixel_x, pixel_y));
      up_min_btn.press(down && up_min_btn.contains(pixel_x, pixel_y));
      down_min_btn.press(down && down_min_btn.contains(pixel_x, pixel_y));
      up_max_btn.press(down && up_max_btn.contains(pixel_x, pixel_y));
      down_max_btn.press(down && down_max_btn.contains(pixel_x, pixel_y));

      // SAVE
      if (save_btn.justPressed()) {
          inSettings = false;
          save_btn.drawButton(true);
  
          if (change) {
            tft.fillRect(250, 20, 112, 28, GREEN);
            tft.fillRect(75, 148, 50, 24, GREEN);
            tft.fillRect(75, 203, 50, 24, GREEN);
            tft.fillRect(245, 148, 50, 24, GREEN);
            tft.fillRect(245, 203, 50, 24, GREEN);
            
          } else {
            tft.fillRect(250, 20, 112, 28, RED);
            tft.fillRect(75, 148, 50, 24, RED);
            tft.fillRect(75, 203, 50, 24, RED);
            tft.fillRect(245, 148, 50, 24, RED);
            tft.fillRect(245, 203, 50, 24, RED);
          }

          settings_btn.drawButton();

      }

      // UP MIN
      if (up_min_btn.justReleased())
          up_min_btn.drawButton();
      if (up_min_btn.justPressed()) {
          up_min_btn.drawButton(true);
          if (change) {
            tft.setTextColor(GREEN); 
          } else {
           tft.setTextColor(RED);   
          }
          tft.setTextSize(2.5);
          tft.setCursor(16, 180);
          tft.print("MIN: ");
          tft.print(minTemp);
          tft.println(" C");

          if (change) {
            tft.setTextColor(BLUE); 
          } else {
           tft.setTextColor(YELLOW);   
          }
          minTemp++;
          tft.setTextSize(2.5);
          tft.setCursor(16, 180);
          tft.print("MIN: ");
          tft.print(minTemp);
          tft.println(" C");
  
      }
  
      // DOWN MIN
      if (down_min_btn.justReleased())
          down_min_btn.drawButton();
      if (down_min_btn.justPressed()) {
          down_min_btn.drawButton(true);
          if (change) {
            tft.setTextColor(GREEN); 
          } else {
           tft.setTextColor(RED);   
          } 
          tft.setTextSize(2.5);
          tft.setCursor(16, 180);
          tft.print("MIN: ");
          tft.print(minTemp);
          tft.println(" C");

          if (change) {
            tft.setTextColor(BLUE); 
          } else {
           tft.setTextColor(YELLOW);   
          }
          minTemp--;
          tft.setTextColor(YELLOW);  
          tft.setTextSize(2.5);
          tft.setCursor(16, 180);
          tft.print("MIN: ");
          tft.print(minTemp);
          tft.println(" C");
      }
  
      // UP MAX
      if (up_max_btn.justReleased())
          up_max_btn.drawButton();
      if (up_max_btn.justPressed()) {
          up_max_btn.drawButton(true);
          if (change) {
            tft.setTextColor(GREEN); 
          } else {
           tft.setTextColor(RED);   
          }
          tft.setTextSize(2.5);
          tft.setCursor(190, 180);
          tft.print("MAX: ");
          tft.print(maxTemp);
          tft.println(" C");

          if (change) {
            tft.setTextColor(BLUE); 
          } else {
           tft.setTextColor(YELLOW);   
          }
          maxTemp++;
          tft.setTextColor(YELLOW);  
          tft.setTextSize(2.5);
          tft.setCursor(190, 180);
          tft.print("MAX: ");
          tft.print(maxTemp);
          tft.println(" C");
      }
  
      // DOWN MAX
      if (down_max_btn.justReleased())
          down_max_btn.drawButton();
      if (down_max_btn.justPressed()) {
          down_max_btn.drawButton(true);
          if (change) {
            tft.setTextColor(GREEN); 
          } else {
           tft.setTextColor(RED);   
          }
          tft.setTextSize(2.5);
          tft.setCursor(190, 180);
          tft.print("MAX: ");
          tft.print(maxTemp);
          tft.println(" C");

          if (change) {
            tft.setTextColor(BLUE); 
          } else {
           tft.setTextColor(YELLOW);   
          }
          maxTemp--;
          tft.setTextColor(YELLOW);  
          tft.setTextSize(2.5);
          tft.setCursor(190, 180);
          tft.print("MAX: ");
          tft.print(maxTemp);
          tft.println(" C");
      }
      
    }
}
