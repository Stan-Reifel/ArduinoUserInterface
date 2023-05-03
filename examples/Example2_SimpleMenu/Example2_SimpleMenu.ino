
//      ******************************************************************
//      *                                                                *
//      *          Example to shows how to create a simple menu          *
//      *                                                                *
//      *            S. Reifel & Co.                7/11/2018            *
//      *                                                                *
//      ******************************************************************

// This sketch shows how to use the "Arduino User Interface" to create a 
// simple menu with 5 commands.  It is assumed that you've already run 
// Example1 which sets the LCD's contrast, and there's an LED on pin 13. 
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
// Notes on building the menu table:
//
// The first line in the table always defines what type of menu it is, either a 
// Main Menu, or a Sub Menu.  The table's last line marks the end of the table.  
// In between are menu items, each line represents one menu choice the user will
// see displayed.
//
// There are three types of menu items: Commands, Toggles, and Sub Menus. In this
// sketch we are only going to explore "Commands".
//
// A MENU_ITEM_TYPE_COMMAND entry indicates that a function will be executed when 
// the menu item is selected by the user. In the second field is the text 
// displayed in the menu.  The third field is a pointer to a function that is 
// executed when this menu item is chosen by the user.  The last field should 
// always be NULL.


//
// for each menu, create a forward declaration with "extern"
//
extern MENU_ITEM mainMenu[];


//
// the main menu
//
MENU_ITEM mainMenu[] = {
  {MENU_ITEM_TYPE_MAIN_MENU_HEADER,  "",                NULL,                       mainMenu},
  {MENU_ITEM_TYPE_COMMAND,           "Blink slow",      menuCommandBlinkSlow,       NULL},
  {MENU_ITEM_TYPE_COMMAND,           "Blink fast",      menuCommandBlinkFast,       NULL},
  {MENU_ITEM_TYPE_COMMAND,           "Blink heart",     menuCommandBlinkHeartbeat,  NULL},
  {MENU_ITEM_TYPE_COMMAND,           "Blink SOS",       menuCommandBlinkSOS,        NULL},
  {MENU_ITEM_TYPE_COMMAND,           "Blink random",    menuCommandBlinkRandom,     NULL},
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
//                            Commands executed from the menu
// ---------------------------------------------------------------------------------

//
// menu command to: blink slowly
//
void menuCommandBlinkSlow(void)
{
  ui.lcdClearDisplay();              // erase the menu while running the command
  
  for(int i = 0; i < 3; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(800);
    digitalWrite(LED_PIN, LOW);
    delay(800);
  }
}



//
// menu command to: blink fast
//
void menuCommandBlinkFast(void)
{
   ui.lcdClearDisplay();              // erase the menu while running the command
  
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



//
// menu command to: blink random
//
void menuCommandBlinkRandom(void)
{
  ui.lcdClearDisplay();              // erase the menu while running the command

  for(int i = 0; i < 15; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(random(10, 200));
    digitalWrite(LED_PIN, LOW);
    delay(random(10, 200));
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

