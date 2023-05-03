
//      ******************************************************************
//      *                                                                *
//      *            First step is to set your LCD's contrast            *
//      *                                                                *
//      *            S. Reifel & Co.                7/11/2018            *
//      *                                                                *
//      ******************************************************************


// This sketch is used to set the LCD's contrast.  If the contrast is too low 
// the display will show nothing, if too high you will see a large dark rectangle.  
// Run this sketch, press and hold the Up and Down buttons to find the optimal
// value, then press "Set".
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
// configure the hardware
//
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



//
// This example is just to set the LCD's contrast, a necessary first step.
// The other example are better for demonstrating how to work with this 
// library.
//
void loop() 
{  
  int minValue = 1;
  int maxValue = 127;
  int step = 1;
  int initialValue = getContrastConfigValue();
  
  ui.displaySlider(minValue, maxValue, step, initialValue, "Set Contrast", contrastCommandCallback);

  ui.lcdClearDisplay();
  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 2);
  ui.lcdPrintStringCentered("Done!", 0);
  delay(4000);
}



//
// "set contrast" callback function
//  Enter:  operation = instruction from slider
//          value = value from slider
//
void contrastCommandCallback(byte operation, int value)
{
  switch(operation)
  {
    case SLIDER_DISPLAY_VALUE_CHANGED:
    {
      ui.lcdSetContrast(value);
      break;
    }
    
    case SLIDER_DISPLAY_VALUE_SET:
    {
      saveContrastConfigValue(value);
      break;
    }
    
    case SLIDER_DISPLAY_CANCELED:
    {
      ui.lcdSetContrast(getContrastConfigValue());
      break;
    }
  }
}


// ---------------------------------------------------------------------------------
//                         Read / write EEPROM configuration values
// ---------------------------------------------------------------------------------

//
// indexes into the EEPROM for configuation data, NOTE: saving a byte uses 2 
// bytes of EEPROM, saving an int uses 3 bytes
//
const int EEPROM_CONTRAST_BYTE_IDX = 0;
const int EEPROM_NEXT_FREE_IDX = EEPROM_CONTRAST_BYTE_IDX + 2;



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


