
//      ******************************************************************
//      *                                                                *
//      *        Saving configuration settings made by the user          *
//      *                                                                *
//      *            S. Reifel & Co.                7/11/2018            *
//      *                                                                *
//      ******************************************************************

// Toggles and Slider are often used to configure your project at runtime.  
// These settings can be saved in the Arduino's EEPROM so the project 
// defaults to the configured values when powered up.
//
// The "Arduino User Interface" library has these functions for saving/reading
// configuration values:
//      writeConfigurationByte()   and   readConfigurationByte()
//      writeConfigurationInt()    and   readConfigurationInt()
//      writeConfigurationLong()   and   readConfigurationLong()
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
  // read the Contrast configuration value previously set in EEPROM,  
  // then intialize the LCD 
  //
  byte contrastValue = getContrastConfigValue();
  ui.lcdSetContrast(contrastValue);


  //
  // read the LED configuration value previously set in EEPROM,  
  // then intialize the LED 
  //
  if (getLEDConfigValue())
    digitalWrite(LED_PIN, HIGH);
  else
    digitalWrite(LED_PIN, LOW);    
}


// ---------------------------------------------------------------------------------
//           Define the menus and top level loop, place menus after setup()
// ---------------------------------------------------------------------------------

//
// for each menu, create a forward declaration with "extern"
//
extern MENU_ITEM mainMenu[];


//
// the main menu
//
MENU_ITEM mainMenu[] = {
  {MENU_ITEM_TYPE_MAIN_MENU_HEADER,  "",                NULL,                       mainMenu},
  {MENU_ITEM_TYPE_TOGGLE,            "LED",             menuToggleLEDCallback,      NULL},
  {MENU_ITEM_TYPE_COMMAND,           "Set contrast",    menuCommandSetContrast,     NULL},
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
//                       Commands & Toggles executed from the menu
// ---------------------------------------------------------------------------------


//
// menu command to: turn on / off the LED
//
void menuToggleLEDCallback(void)
{
  //
  // read from EEPROM the current LED state
  //
  boolean currentLEDState = (boolean) getLEDConfigValue();
  
  //
  // check if menu is requesting that the state be changed (can have more than 2 states)
  //
  if (ui.toggleMenuChangeStateFlag)
  {
    //
    // select the next state, then save it in EEPROM
    //
    currentLEDState = !currentLEDState;
    saveLEDConfigValue(currentLEDState);
  }
  
  //
  // turn the LED on or off as indicated
  //
  if (currentLEDState)
    digitalWrite(LED_PIN, HIGH);
  else
    digitalWrite(LED_PIN, LOW);
 
  //
  // send back an indication of the current state
  //
  if(currentLEDState)
    ui.toggleMenuStateText = "On";
  else
    ui.toggleMenuStateText = "Off";
}



//
// menu command to: set the LCD's contrast
//
void menuCommandSetContrast(void)
{
  int minValue = 1;
  int maxValue = 127;
  int step = 1;
  int initialValue = getContrastConfigValue();      // read current contrast value from EEPROM
  
  ui.displaySlider(minValue, maxValue, step, initialValue, "Set Contrast", setContrastCallback);
}



//
// call back function for: set contrast
//  Enter:  operation = instruction from slider
//          value = value from slider
//
void setContrastCallback(byte operation, int value)
{
  switch(operation)
  {
    case SLIDER_DISPLAY_VALUE_CHANGED:
    {
      //
      // as the user changes the value up and down, display the contrast
      // change in realtime
      //
      ui.lcdSetContrast(value);
      break;
    }
    
    case SLIDER_DISPLAY_VALUE_SET:
    {
      //
      // the user has selected the value then pressed "OK", so set this 
      // as the contrast level
      //
      saveContrastConfigValue(value);
      break;
    }
    
    case SLIDER_DISPLAY_CANCELED:
    {
      //
      // the user has pressed "Cancel", so restore the original contrast value
      //
      ui.lcdSetContrast(getContrastConfigValue());
      break;
    }
  }
}


// ---------------------------------------------------------------------------------
//                       Read / write EEPROM configuration values
// ---------------------------------------------------------------------------------

//
// indexes into the EEPROM for configuation data, NOTE: saving a byte uses 2 
// bytes of EEPROM, saving an int uses 3 bytes, a long uses 5 bytes
//
const int EEPROM_CONTRAST_BYTE_IDX = 0;
const int EEPROM_LED_IDX = EEPROM_CONTRAST_BYTE_IDX + 2;
const int EEPROM_NEXT_FREE_IDX = EEPROM_LED_IDX + 2;


//
// get/set contrast configuration value
//
const byte CONFIG_CONTRAST_DEFAULT = 80;    // better to default too dark, than too light

void saveContrastConfigValue(byte value) 
{
  ui.writeConfigurationByte(EEPROM_CONTRAST_BYTE_IDX, value);
}

byte getContrastConfigValue(void) 
{
  int value;
  
  value = ui.readConfigurationByte(EEPROM_CONTRAST_BYTE_IDX, CONFIG_CONTRAST_DEFAULT);
  if (value > 127)
    value = CONFIG_CONTRAST_DEFAULT;
    
  return(value);
}



//
// get/set LED configuration value
//
const byte CONFIG_LED_DEFAULT = false;

void saveLEDConfigValue(byte value) 
{
  ui.writeConfigurationByte(EEPROM_LED_IDX, value);
}

byte getLEDConfigValue(void) 
{
  int value;
  
  value = ui.readConfigurationByte(EEPROM_LED_IDX, CONFIG_LED_DEFAULT);    
  return(value);
}

