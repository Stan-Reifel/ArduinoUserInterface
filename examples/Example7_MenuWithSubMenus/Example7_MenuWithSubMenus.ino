
//      ******************************************************************
//      *                                                                *
//      *                  Creating menus with sub menus                 *
//      *                                                                *
//      *            S. Reifel & Co.                7/11/2018            *
//      *                                                                *
//      ******************************************************************

// Often it is useful to group related commands into their own menu, this 
// is what Sub-menus are for.  This example show how to create a Main-menu 
// that has one or more sub-menus. 
//
//
// Hardware:
// 
// The "Arduino User Interface" library is for projects having a Nokia 5110 LCD 
// display and 4 push buttons.  The library works well with the "Arduino UI Shield".  
// Unfortunately this shield is not commercially available, so you will need to 
// build your own.  PCB files and a schematic can be found at the GitHub link 
// below. 
// 
// The library needs to be told which Arduino pins are connected to the display.  
// The default values used here assume you have an "Arduino UI Shield" and it's 
// plugged into a Uno, Mega or Nano.  If you have different hardware, be sure the 
// call to connectToPins() is setup properly.
// 
// Documentation for the "Arduino User Interface" library and "Arduino UI Shield"  
// can be found at:
//    https://github.com/Stan-Reifel/ArduinoUserInterface



// ***********************************************************************

#include <ArduinoUserInterface.h>


//
// assign IO pin numbers for the LCD display and buttons
//
#ifdef ARDUINO_AVR_NANO
  //
  // use these pin numbers if the display is connected to an Arduino NANO
  //
  const byte LCD_CLOCK_PIN = A0;
  const byte LCD_DATA_IN_PIN = A1;
  const byte LCD_DATA_CONTROL_PIN = A2;
  const byte LCD_CHIP_ENABLE_PIN = A3;
  const byte BUTTONS_ANALOG_PIN = A6;
#else
  //
  // use these pin numbers if the display is connected to an Uno or Mega
  //
  const byte LCD_CLOCK_PIN = A0;
  const byte LCD_DATA_IN_PIN = A1;
  const byte LCD_DATA_CONTROL_PIN = A2;
  const byte LCD_CHIP_ENABLE_PIN = 4;
  const byte BUTTONS_ANALOG_PIN = A3;
#endif


//
// other pin assignments
//
const int LED_PIN = 13;


//
// create the user interface object
//
ArduinoUserInterface ui;



// ---------------------------------------------------------------------------------
//                                 Setup the hardware
// ---------------------------------------------------------------------------------

void setup() 
{
  //
  // setup the LED pin and enable print statements
  //
  pinMode(LED_PIN, OUTPUT);   
  Serial.begin(9600);


  //
  // assign pin numbers to the LCD display and buttons
  //
  ui.connectToPins(LCD_CLOCK_PIN, LCD_DATA_IN_PIN, LCD_DATA_CONTROL_PIN, LCD_CHIP_ENABLE_PIN, BUTTONS_ANALOG_PIN);


  //
  // read the contrast configuration value previously set in EEPROM,
  // then set the LCD's contrast
  //
  byte contrastValue = getContrastConfigValue();
  ui.lcdSetContrast(contrastValue);
}


// ---------------------------------------------------------------------------------
//           Define the menus and top level loop, place menus after setup()
// ---------------------------------------------------------------------------------

//
// Notes on building the main menu:
//
// A MENU_ITEM_TYPE_SUB_MENU entry is used to select a different menu.  For 
// example, a main menu might reference a "Settings" sub menu. The fourth field  
// in this entry points to the menu table that defines the sub menu.
//


//
// for each menu, create a forward declaration with "extern"
//
extern MENU_ITEM mainMenu[];
extern MENU_ITEM blinkMenu[];
extern MENU_ITEM blinkRepeatMenu[];


//
// the main menu
//
MENU_ITEM mainMenu[] = {
  {MENU_ITEM_TYPE_MAIN_MENU_HEADER,  "",                NULL,                       mainMenu},
  {MENU_ITEM_TYPE_SUB_MENU,          "Blink",           NULL,                       blinkMenu},
  {MENU_ITEM_TYPE_COMMAND,           "About",           menuCommandShowAboutBox,    NULL},
  {MENU_ITEM_TYPE_END_OF_MENU,       "",                NULL,                       NULL}
};


//
// Notes on building the sub menu:
//
// Sub-menus are menus called from the main-menu, or another sub-menu. The
// Sub-menu table is built just like the Main-menu, except the first entry
// must be MENU_ITEM_TYPE_SUB_MENU_HEADER. In this entry's fourth field is a 
// pointer back to the parent menu table (typically the main menu).  This is 
// used to reselect the parent menu when the user presses the "Back" button, 
// indicating they are done with the sub menu.


//
// the "Blink" submenu
//
MENU_ITEM blinkMenu[] = {
  {MENU_ITEM_TYPE_SUB_MENU_HEADER,   "",                NULL,                       mainMenu},
  {MENU_ITEM_TYPE_COMMAND,           "Fast",            menuCommandBlinkFast,       NULL},
  {MENU_ITEM_TYPE_COMMAND,           "Heartbeat",       menuCommandBlinkHeartbeat,  NULL},
  {MENU_ITEM_TYPE_COMMAND,           "SOS",             menuCommandBlinkSOS,        NULL},
  {MENU_ITEM_TYPE_SUB_MENU,          "Repeat",          NULL,                       blinkRepeatMenu},
  {MENU_ITEM_TYPE_END_OF_MENU,       "",                NULL,                       NULL}
};


//
// the "Blink/Repeat" submenu
//
MENU_ITEM blinkRepeatMenu[] = {
  {MENU_ITEM_TYPE_SUB_MENU_HEADER,   "",                NULL,                       blinkMenu},
  {MENU_ITEM_TYPE_COMMAND,           "2 times",         menuCommandBlink2Times,     NULL},
  {MENU_ITEM_TYPE_COMMAND,           "4 times",         menuCommandBlink4Times,     NULL},
  {MENU_ITEM_TYPE_COMMAND,           "6 times",         menuCommandBlink6Times,     NULL},
  {MENU_ITEM_TYPE_END_OF_MENU,       "",                NULL,                       NULL}
};



//
// display the menu, then execute commands selected by the user
//
void loop() 
{  
  ui.displayAndExecuteMenu(mainMenu);
}


// ---------------------------------------------------------------------------------
//                         Commands executed from the Main menu
// ---------------------------------------------------------------------------------


//
// menu command to: display an "About box"
//
void menuCommandShowAboutBox(void)
{
  //
  // clear the display
  //
  ui.lcdClearDisplay();

  //
  // display several lines of centered text
  //
  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 1);
  ui.lcdPrintStringCentered("Arduino UI", 0);

  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 2);
  ui.lcdPrintStringCentered("Version 0.0", 0);

  //
  // draw the button bar, then wait for the user to press OK
  //
  ui.drawButtonBar("OK", "");
  while(ui.getButtonEvent() != BUTTON_ID_SELECT + BUTTON_PUSHED_EVENT)
    ;
}



// ---------------------------------------------------------------------------------
//                       Commands executed from the Blink menu
// ---------------------------------------------------------------------------------

//
// menu command to: blink fast
//
void menuCommandBlinkFast(void)
{
  ui.lcdClearDisplay();              // erase the menu while running the command
  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 2);
  ui.lcdPrintStringCentered("Blink fast", 0);

  
  for(int i = 0; i <12; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(70);
    digitalWrite(LED_PIN, LOW);
    delay(70);
  } 
}



//
// menu command to: blink in a Heartbeat pattern
//
void menuCommandBlinkHeartbeat(void)
{
  ui.lcdClearDisplay();              // erase the menu while running the command
  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 2);
  ui.lcdPrintStringCentered("Heartbeat", 0);
  
  for(int i = 0; i <4; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(140);

    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(450);
  } 
}



//
// menu command to: blink SOS
//
void menuCommandBlinkSOS(void)
{
  ui.lcdClearDisplay();              // erase the menu while running the command
  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 2);
  ui.lcdPrintStringCentered("Blink SOS", 0);

  const int dotPeriod = 80;
  
  for(int i = 0; i < 3; i++)          // . . .
  {
    digitalWrite(LED_PIN, HIGH);
    delay(dotPeriod);
    digitalWrite(LED_PIN, LOW);
    delay(dotPeriod * 3);
  }
  delay(dotPeriod * 3);

  for(int i = 0; i < 3; i++)          // -  -  - 
  {
    digitalWrite(LED_PIN, HIGH);
    delay(dotPeriod * 3);
    digitalWrite(LED_PIN, LOW);
    delay(dotPeriod * 3);
  }
  delay(dotPeriod * 3);
  
  for(int i = 0; i < 3; i++)          // . . .
  {
    digitalWrite(LED_PIN, HIGH);
    delay(dotPeriod);
    digitalWrite(LED_PIN, LOW);
    delay(dotPeriod * 3);
  }
}



// ---------------------------------------------------------------------------------
//                  Commands executed from the Blink/Repeat menu
// ---------------------------------------------------------------------------------

//
// menu command to: blink 2 times
//
void menuCommandBlink2Times(void)
{
  ui.lcdClearDisplay();              // erase the menu while running the command
  
  for(int i = 0; i <2; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  } 
}



//
// menu command to: blink 4 times
//
void menuCommandBlink4Times(void)
{
  ui.lcdClearDisplay();              // erase the menu while running the command
  
  for(int i = 0; i <4; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  } 
}



//
// menu command to: blink 6 times
//
void menuCommandBlink6Times(void)
{
  ui.lcdClearDisplay();              // erase the menu while running the command
  
  for(int i = 0; i <6; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  } 
}



// ---------------------------------------------------------------------------------
//                       Read / write EEPROM configuration values
// ---------------------------------------------------------------------------------

//
// indexes into the EEPROM for configuation data, NOTE: saving a byte uses 2 
// bytes of EEPROM, saving an int uses 3 bytes
//
const int EEPROM_CONTRAST_BYTE_IDX = 0;
const byte CONFIG_CONTRAST_DEFAULT = 80;    // better to default too dark, than too light



//
// get/set contrast configuration value
//

byte getContrastConfigValue(void) 
{
  int value;
  
  value = ui.readConfigurationByte(EEPROM_CONTRAST_BYTE_IDX, CONFIG_CONTRAST_DEFAULT);
  if (value > 127)
    value = CONFIG_CONTRAST_DEFAULT;
    
  return(value);
}

