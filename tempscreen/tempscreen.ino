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
#define MINPRESSURE 200
#define MAXPRESSURE 1000

const int XP=8,XM=A2,YP=A3,YM=9; //240x320 ID=0x9595
const int TS_LEFT = 892,TS_RT = 164,TS_TOP = 108,TS_BOT = 901;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button up_min_btn, down_min_btn, up_max_btn, down_max_btn;

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


int maxTemp = 60;
int minTemp = 50;
int currentTemp = 70;

void setup(void)
{
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    Serial.println("Calibrate for your Touch Panel");
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(1);            //LANDSCAPE
    tft.fillScreen(RED);
    
    tft.setCursor(100, 64);
    tft.setTextColor(WHITE);  
    tft.setTextSize(6);
    tft.print(currentTemp);
    tft.println(" C");

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

    
    up_min_btn.initButton(&tft,  100, 160, 50, 24, WHITE, CYAN, BLACK, "up", 2);
    up_min_btn.drawButton(false);

    down_min_btn.initButton(&tft,  100, 215, 50, 24, WHITE, CYAN, BLACK, "down", 2);
    down_min_btn.drawButton(false);


    up_max_btn.initButton(&tft,  270, 160, 50, 24, WHITE, CYAN, BLACK, "up", 2);
    up_max_btn.drawButton(false);

    down_max_btn.initButton(&tft,  270, 215, 50, 24, WHITE, CYAN, BLACK, "down", 2);
    down_max_btn.drawButton(false);
    
    
}

void loop(void)
{
    if (currentTemp <= maxTemp) {
      if (currentTemp == maxTemp) {
        // BUZZER
      }
      tft.fillScreen(GREEN);
    } else if (currentTemp < minTemp){
      if (currentTemp == minTemp) {
        // BUZZER
      }
      tft.fillScreen(GREEN);
    }
    
    bool down = Touch_getXY();
    up_min_btn.press(down && up_min_btn.contains(pixel_x, pixel_y));
    down_min_btn.press(down && down_min_btn.contains(pixel_x, pixel_y));
    up_max_btn.press(down && up_max_btn.contains(pixel_x, pixel_y));
    down_max_btn.press(down && down_max_btn.contains(pixel_x, pixel_y));

    // UP MIN
    if (up_min_btn.justReleased())
        up_min_btn.drawButton();
    if (up_min_btn.justPressed()) {
        up_min_btn.drawButton(true);
        tft.fillRect(16, 180, 80, 220, RED);

        minTemp++;
        tft.setTextColor(YELLOW);  
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
        tft.setTextColor(RED);  
        tft.setTextSize(2.5);
        tft.setCursor(16, 180);
        tft.print("MIN: ");
        tft.print(minTemp);
        tft.println(" C");

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
        tft.setTextColor(RED);  
        tft.setTextSize(2.5);
        tft.setCursor(190, 180);
        tft.print("MAX: ");
        tft.print(maxTemp);
        tft.println(" C");

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
        tft.setTextColor(RED);  
        tft.setTextSize(2.5);
        tft.setCursor(190, 180);
        tft.print("MAX: ");
        tft.print(maxTemp);
        tft.println(" C");

        maxTemp--;
        tft.setTextColor(YELLOW);  
        tft.setTextSize(2.5);
        tft.setCursor(190, 180);
        tft.print("MAX: ");
        tft.print(maxTemp);
        tft.println(" C");

    }
}
