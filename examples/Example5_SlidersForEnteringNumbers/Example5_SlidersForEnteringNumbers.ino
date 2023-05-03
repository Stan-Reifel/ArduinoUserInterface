
//      ******************************************************************
//      *                                                                *
//      *       Example shows how to use "Sliders" in your sketch        *
//      *                                                                *
//      *            S. Reifel & Co.                7/11/2018            *
//      *                                                                *
//      ******************************************************************


// A Slider allows the users to select a numeric value (such as 0 to 255, or
// -1000 to 1000,  or  0.0 to 15.5).  There are two types of slides, one for
// INTs and one for FLOATs.
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


//
// local state variables
//
static int stateFakePowerLevel = 500;
static float stateFakeMultiplier = 0.5;


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
// for each menu, create a forward declaration with "extern"
//
extern MENU_ITEM mainMenu[];


//
// the main menu
//
MENU_ITEM mainMenu[] = {
  {MENU_ITEM_TYPE_MAIN_MENU_HEADER,  "",              NULL,                           mainMenu},
  {MENU_ITEM_TYPE_COMMAND,           "Power Level",   menuCommandSetFakePowerLevel,   NULL},
  {MENU_ITEM_TYPE_COMMAND,           "Multiplier",    menuCommandSetFakeMultiplier,   NULL},
  {MENU_ITEM_TYPE_END_OF_MENU,       "",              NULL,                           NULL}
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
// menu command to: set the fake power level value
//
void menuCommandSetFakePowerLevel(void)
{
  //
  // setup values used by the slider
  //
  int minValue = 0;
  int maxValue = 1000;
  int step = 5;
  int initialValue = stateFakePowerLevel;
 

  //
  // display the slider and let the user pick the number
  //
  ui.displaySlider(minValue, maxValue, step, initialValue, "Power Level", setPowerLevelCallback);
}


//
// call back function for: set the "fake power level"
//  Enter:  operation = instruction from slider
//          value = value from slider
//
void setPowerLevelCallback(byte operation, int value)
{
  switch(operation)
  {
    case SLIDER_DISPLAY_VALUE_CHANGED:
    {
      //
      // as the user changes the value up and down, we can optionally add
      // code that updates the hardware here
      //
      break;
    }
    
    case SLIDER_DISPLAY_VALUE_SET:
    { 
      //
      // the user has selected the value then pressed "OK", so set this 
      // as the fake power level
      //
      stateFakePowerLevel = value;
      break;
    }
    
    case SLIDER_DISPLAY_CANCELED:
    {
      //
      // the user has pressed "Cancel"
      //
      break;
    }
  }
}



//
// menu command to: set the fake multiplier value
//
void menuCommandSetFakeMultiplier(void)
{
  //
  // setup values used by the slider
  //
  float minValue = 0.0;
  float maxValue = 1.0;
  float step = 0.05;
  float initialValue = stateFakeMultiplier;
  byte digitsRightOfDecimal = 2;
 

  //
  // display the slider and let the user pick the number
  //
  ui.displayFloatSlider(minValue, maxValue, step, initialValue, "Multiplier", digitsRightOfDecimal, setMultiplierCallback);
}


//
// call back function for: set the "fake Multiplier"
//  Enter:  operation = instruction from slider
//          value = value from slider
//
void setMultiplierCallback(byte operation, float value)
{
  switch(operation)
  {
    case SLIDER_DISPLAY_VALUE_CHANGED:
    {
      //
      // as the user changes the value up and down, we can optionally add
      // code that updates the hardware here
      //
      break;
    }
    
    case SLIDER_DISPLAY_VALUE_SET:
    { 
      //
      // the user has selected the value then pressed "OK", so set this 
      // as the fake multiplier
      //
      stateFakeMultiplier = value;
      break;
    }
    
    case SLIDER_DISPLAY_CANCELED:
    {
      //
      // the user has pressed "Cancel"
      //
      break;
    }
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

