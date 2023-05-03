
//      ******************************************************************
//      *                                                                *
//      *                            Stopwatch                           *
//      *                                                                *
//      *            S. Reifel & Co.                7/13/2018            *
//      *                                                                *
//      ******************************************************************

// The previous examples show sketches that first run by displaying a menu.  
// In some cases you want to start your application with its display, then 
// press a button to pull up its menu.  This sketch runs a Stopwatch.  The 
// stopwatch is displayed immediately when powered up.  At anytime the user  
// can press the "Menu" button to get a list of options.  
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
// values for stopwatchMode
//
const byte STOPWATCH_READY = 0;
const byte STOPWATCH_RUNNING = 1;
const byte STOPWATCH_STOPPED = 2;



//
// local vars
//
static byte displayDigits;
static byte stopwatchMode;
static long stopwatchStartTime;
static long stopwatchCurrentTime;



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


  //
  // read the "display digits" configuration value previously set in EEPROM
  //
  displayDigits = getDisplayDigitsConfigValue();
}


// ---------------------------------------------------------------------------------
//                                 Define the menus
// ---------------------------------------------------------------------------------

//
// Notes on building the Main-menu:
//
// In this app, the first screen the user sees is not a menu, but the stopwatch.
// The user calls up the menu by pressing the "Menu" button.  They will return 
// to the stopwatch from the menu by pressing the "Back" button.
//
// Normally the Main-menu does not show a "Back" button.  To enable the "Back" 
// button, the fourth field in the MENU_ITEM_TYPE_MAIN_MENU_HEADER line is set 
// to NULL.
// 


//
// forward declarations for the sub menus
//
extern MENU_ITEM mainMenu[];


//
// the main menu
//
MENU_ITEM mainMenu[] = {
  {MENU_ITEM_TYPE_MAIN_MENU_HEADER,  "",                NULL,                       NULL},
  {MENU_ITEM_TYPE_COMMAND,           "Set contrast",    menuCommandSetContrast,     NULL},
  {MENU_ITEM_TYPE_TOGGLE,            "Digits",          menuToggleDigitsCallback,   NULL},
  {MENU_ITEM_TYPE_COMMAND,           "About",           menuCommandShowAboutBox,    NULL},
  {MENU_ITEM_TYPE_END_OF_MENU,       "",                NULL,                       NULL}
};


// ---------------------------------------------------------------------------------
//                             Stopwatch application
// ---------------------------------------------------------------------------------

//
// example of an application that's displayed first upon power up, then
// menus can be called from it
//
void loop()
{
  stopWatch();
}



//
// stopwatch application
//
void stopWatch(void)
{
  byte buttonEvent;
  long timeNow;
  
  //
  // initialize the stopwatch
  //
  ui.lcdClearDisplay();
  stopwatchStartTime = 0;
  stopwatchCurrentTime = 0;
  stopwatchMode = STOPWATCH_READY;
  drawStopwatch();

  //
  // draw the buttons on the button bar
  //
  ui.drawButtonBar("Start", "Menu");  
  
  //
  // check for and execute button pushes, starting and stoppng the stopwatch
  //
  while(true)
  {
    //
    // check for a new button event
    //
    buttonEvent = ui.getButtonEvent();
    switch(buttonEvent)
    {
      //
      // check if the start/stop/reset button has been pressed
      //
      case BUTTON_ID_SELECT + BUTTON_PUSHED_EVENT:
      {
        //
        // check the current stopwatch mode, then select the next mode
        //
        switch(stopwatchMode)
        {
          case STOPWATCH_READY:
          {
            //
            // start the stopwatch
            //
            stopwatchStartTime = millis();
            stopwatchMode = STOPWATCH_RUNNING;
            drawStopwatch();
            ui.drawButtonBar("Stop", "Menu");
            break;
          }

          case STOPWATCH_RUNNING:
          {
            //
            // stop the stopwatch
            //
            stopwatchCurrentTime = millis();
            stopwatchMode = STOPWATCH_STOPPED;
            drawStopwatch();
            ui.drawButtonBar("Reset", "Menu");
            break;
          }

          case STOPWATCH_STOPPED:
          {
            //
            // reset the stopwatch
            //
            stopwatchStartTime = 0;
            stopwatchCurrentTime = 0;
            stopwatchMode = STOPWATCH_READY;
            drawStopwatch();
            ui.drawButtonBar("Start", "Menu");
            break;
          }
        }
        break;
      }


      //
      // check if the "menu" button has been pressed
      //
      case BUTTON_ID_BACK + BUTTON_PUSHED_EVENT:
      {
        //
        // show the menu
        //
        ui.displayAndExecuteMenu(mainMenu);
        
        //
        // return to the stop watch
        //
        ui.lcdClearDisplay();
        stopwatchStartTime = 0;
        stopwatchCurrentTime = 0;
        stopwatchMode = STOPWATCH_READY;
        drawStopwatch();
        ui.drawButtonBar("Start", "Menu");
        break;
      }
    }
    
    //
    // update the time shown on the stopwatch
    //
    if (stopwatchMode == STOPWATCH_RUNNING)
    {
      timeNow = millis();
      if (timeNow > stopwatchCurrentTime + 49)
      {
        stopwatchCurrentTime = timeNow;
        drawStopwatch();
      }
    }
  }
}


//
// draw the time on the stopwatch
//
void drawStopwatch()
{
  byte hours;
  byte minutes;
  byte seconds;
  byte hundredths;
  long milliSeconds;
  char sBuf[12];
  char *sbufPntr;
  
  milliSeconds = stopwatchCurrentTime - stopwatchStartTime;
  
  hours = milliSeconds / (60L * 60L * 1000L);
  milliSeconds -= (hours * (60L * 60L * 1000L));
  
  minutes = milliSeconds / (60L * 1000L);
  milliSeconds -= (minutes * (60L * 1000L));
  
  seconds = milliSeconds / (1000L);
  milliSeconds -= (seconds * 1000L);
  
  hundredths = milliSeconds / 10L;
  
  sbufPntr = sBuf;
  
  if (displayDigits == 8)
  {
    sbufPntr = timeDigits(sbufPntr, hours);
    *sbufPntr++ = ':';
  }
  
  sbufPntr = timeDigits(sbufPntr, minutes);
  *sbufPntr++ = ':';
  
  sbufPntr = timeDigits(sbufPntr, seconds);
  *sbufPntr++ = ':';
 
  sbufPntr = timeDigits(sbufPntr, hundredths);

  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 2);
  ui.lcdPrintStringCentered(sBuf, 0);
}



//
// convert a number into a two digit string, pad with leading zeros
//
char* timeDigits(char *sBufPntr, byte n)
{
  *sBufPntr++ = (n / 10) + '0';
  *sBufPntr++ = (n % 10) + '0';
  *sBufPntr = 0;
  return(sBufPntr);
}


// ---------------------------------------------------------------------------------
//                          Commands executed from the menu
// ---------------------------------------------------------------------------------


//
// menu command to: set stopwatch's number of digits to either 6 or 8
//
void menuToggleDigitsCallback(void)
{
  //
  // check if menu is requesting that the state be changed (can have more than 2 states)
  //
  if (ui.toggleMenuChangeStateFlag)
  {
    //
    // select the next state, then save it in EEPROM
    //
    if (displayDigits == 8)
      displayDigits = 6;
    else
      displayDigits = 8;

    saveDisplayDigitsConfigValue(displayDigits);
  }
 
  //
  // send back an indication of the current state
  //
  if (displayDigits == 8)
    ui.toggleMenuStateText = "8";
  else
    ui.toggleMenuStateText = "6";
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
  // display this applications "About" info
  //
  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 0);
  ui.lcdPrintStringCentered("Arduino", 0);

  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 1);
  ui.lcdPrintStringCentered("Stopwatch", 0);

  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 3);
  ui.lcdPrintStringCentered("Version 1.0", 0);

  //
  // draw the button bar, then wait for the user to press OK
  //
  ui.drawButtonBar("OK", "");
  while(ui.getButtonEvent() != BUTTON_ID_SELECT + BUTTON_PUSHED_EVENT)
    ;
}



// ---------------------------------------------------------------------------------
//                       Read / write EEPROM configuration values
// ---------------------------------------------------------------------------------

//
// indexes into the EEPROM for configuation data, NOTE: saving a byte uses 2 
// bytes of EEPROM, saving an int uses 3 bytes
//
const int EEPROM_CONTRAST_BYTE_IDX = 0;
const int EEPROM_DISPLAY_DIGITS_IDX = EEPROM_CONTRAST_BYTE_IDX + 2;
const int EEPROM_NEXT_FREE_IDX = EEPROM_DISPLAY_DIGITS_IDX + 2;


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
// get/set "Display digits" configuration value
//
const byte CONFIG_DISPLAY_DIGITS_DEFAULT = 8;

void saveDisplayDigitsConfigValue(byte value) 
{
  ui.writeConfigurationByte(EEPROM_DISPLAY_DIGITS_IDX, value);
}

byte getDisplayDigitsConfigValue(void) 
{
  int value;
  
  value = ui.readConfigurationByte(EEPROM_DISPLAY_DIGITS_IDX, CONFIG_DISPLAY_DIGITS_DEFAULT);    
  return(value);
}

