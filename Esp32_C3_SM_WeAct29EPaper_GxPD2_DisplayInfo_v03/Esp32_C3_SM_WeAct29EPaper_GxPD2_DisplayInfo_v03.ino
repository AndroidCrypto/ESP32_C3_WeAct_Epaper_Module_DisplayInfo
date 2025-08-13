/*
  The sketch is showing how to connect a WeAct 2.9-inch EPaper Module to an ESP32-C3 Supermini
  microcontroller.

  It will display 6 lines of text with technical details of the display and a rectangle at the
  edges of the display.

  This sketch can work with the "Black & White" and "Red, Black & White" ("RBW") variants of the display.
  If your display is of RBW type simply uncomment the compiler directive '//#define SCREEN_IS_RBW'

  Documentation of the Epaper display: https://github.com/WeActStudio/WeActStudio.EpaperModule
*/

/*
Version Management
12.08.2025 V03 Tutorial version
11.08.2025 V02 code improvements
02.08.2025 V01 Initial programming
*/

#include <SPI.h>
#define ENABLE_GxEPD2_display 0
#include <GxEPD2_BW.h>  // https://github.com/ZinggJM/GxEPD2
#include <GxEPD2_3C.h>
#include <U8g2_for_Adafruit_GFX.h>  // https://github.com/olikraus/U8g2_for_Adafruit_GFX

#define SCREEN_WIDTH 296
#define SCREEN_HEIGHT 128

#define SCREEN_IS_RBW // Red Black White color

// Connections for ESP32-C3 Supermini, trying to have most ADC pins left available
static const uint8_t EPD_BUSY = 21; // to EPD BUSY
static const uint8_t EPD_CS = 10;   // to EPD CS
static const uint8_t EPD_RST = 6;   // to EPD RST
static const uint8_t EPD_DC =  7;   // to EPD DC

static const uint8_t EPD_SCK = 20;  // to EPD CLK
static const uint8_t EPD_MISO = -1; // Master-In Slave-Out not used, as no data from display
static const uint8_t EPD_MOSI =  5; // to EPD DIN / SDA

/*
Wiring of the display to the ESP32-C3 Supermini device. Please double-check the pins on the PCB
and colors of your cable to avoid any damage of the display..
Nr name  color   ESP32-C3 pin
1  BUSY  violet  21 
2  RES   orange   6
3  D/C   white    7
4  CS    blue    10
5  SCL   green   20 
6  SDA   yellow   5 ('SDA' terminal is known as 'MOSI')
7  GND   black   GND
8  VCC   red     VCC is 3.3 volt, not 5 volt !

On the board use 4-Lines SPI (upper solder pads closed)
*/

/*
  Pins to avoid on an ESP32-C3 Supermini board:
  GPIO  8 : Onboard LED
  GPIO  9 : Boot button
*/

/*
  Pins to avoid on an ESP32-C3 Supermini Plus board:
  GPIO  8 : Onboard LED + RGB LED (pin sharing)
  GPIO  9 : Boot button
*/

/*
  Source: https://www.waveshare.com/wiki/2.9inch_e-Paper_Module_Manual
  For e-Paper displays that support partial refresh, please note that you cannot refresh them with 
  the partial refresh mode all the time. After refreshing partially several times, you need to fully 
  refresh EPD once. Otherwise, the display effect will be abnormal, which cannot be repaired!
  
  Note that the screen cannot be powered on for a long time. When the screen is not refreshed, please 
  set the screen to sleep mode or power off it. Otherwise, the screen will remain in a high voltage 
  state for a long time, which will damage the e-Paper and cannot be repaired!
  
  When using the e-Paper display, it is recommended that the refresh interval is at least 180s, and 
  refresh at least once every 24 hours. If the e-Paper is not used for a long time, you should use the 
  program to clear the screen before storing it. (Refer to the datasheet for specific storage 
  environment requirements.)
*/

// BW
#ifndef SCREEN_IS_RBW
  GxEPD2_BW<GxEPD2_290_BS, GxEPD2_290_BS::HEIGHT> display(GxEPD2_290_BS(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));  // DEPG0290BS 128x296, SSD1680
// RBW
#else
  GxEPD2_3C<GxEPD2_290_C90c, GxEPD2_290_C90c::HEIGHT> display(GxEPD2_290_C90c(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY)); // GDEM029C90 128x296, SSD1680
#endif

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts; // Select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall

// -------------------------------------------------------------------------------
// CPU frequency
#define CPU_FREQUENCY_MHZ 10 // 160 | 80 | 40 | 20 | 10 MHz

void InitialiseDisplay() {
  display.init(115200, true, 2, false);
  //display.init(115200);  //for older Waveshare HAT's
  SPI.end();
  SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);
  display.setRotation(1);                     // Use 1 or 3 for landscape modes
  u8g2Fonts.begin(display);                   // connect u8g2 procedures to Adafruit GFX
  u8g2Fonts.setFontMode(1);                   // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);              // left to right (this is default)
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);  // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);  // apply Adafruit GFX color
  u8g2Fonts.setFont(u8g2_font_helvB10_tf);    // Explore u8g2 fonts from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  display.fillScreen(GxEPD_WHITE);
  display.setFullWindow();
  Serial.println("Initialise Display done");
}

void drawString(int x, int y, String text) {
  int16_t  x1, y1; //the bounds of x,y and w and h of the variable 'text' in pixels.
  uint16_t w, h;
  display.setTextWrap(false);
  display.getTextBounds(text, x, y, &x1, &y1, &w, &h);
  u8g2Fonts.setCursor(x, y + h);
  u8g2Fonts.print(text);
}

void setup() {

  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("Esp32-C3 Supermini WeAct 2.9-inch EPaper Module GxPD2 library DisplayInfo v03");

  setCpuFrequencyMhz(CPU_FREQUENCY_MHZ);
  Serial.printf("The processor is running with %d MHz\n", getCpuFrequencyMhz());

  InitialiseDisplay();

  u8g2Fonts.setFont(u8g2_font_helvB14_tf);
  uint8_t distance = 20;
  uint8_t counter = 0;
  const uint8_t startX = 10;
  const uint8_t startY = 10;
  const char *DISPLAY_TYPE = "Type: WeAct Epaper Module";
  const char *DISPLAY_INTERFACE = "Interface: SPI | Chip: SSD1680";
#ifndef SCREEN_IS_RBW
  const char *DISPLAY_DRIVER = "Driver: GxEPD2_290_BS";
  const char *DISPLAY_SIZE = "Size: 2.9-inch 128 x 296 pixels";
  const char *DISPLAY_COLORS = "Colors: Black and White";
#else
  const char *DISPLAY_DRIVER = "Driver: GxEPD2_290_C90c";
  const char *DISPLAY_SIZE = "Size: 2.9-inch 128 x 296 pixels";
  const char *DISPLAY_COLORS = "Colors: Red, Black and White";
#endif
  const char *PROGRAM_AUTHOR = "Author: AndroidCrypto";

  drawString(startX, startY + 0 * distance, DISPLAY_TYPE);
  drawString(startX, startY + 1 * distance, DISPLAY_INTERFACE);
  drawString(startX, startY + 2 * distance, DISPLAY_DRIVER);
  drawString(startX, startY + 3 * distance, DISPLAY_SIZE);
#ifndef SCREEN_IS_RBW
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  drawString(startX, startY + 4 * distance, DISPLAY_COLORS);
#else
  u8g2Fonts.setForegroundColor(GxEPD_RED);
  drawString(startX, startY + 4 * distance, DISPLAY_COLORS);
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
#endif
  drawString(startX, startY + 5 * distance, PROGRAM_AUTHOR);
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GxEPD_BLACK);

  display.display(false); // Full screen update mode

  // Important use bring the display in one of the two modes to
  // avoid a damage of the display that cannot get repaired.
  //display.powerOff();
  display.hibernate();
}

void loop() {
}
