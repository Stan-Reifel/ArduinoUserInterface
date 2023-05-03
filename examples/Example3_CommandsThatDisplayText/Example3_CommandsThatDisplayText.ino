
//      ******************************************************************
//      *                                                                *
//      *       Example shows how to display information on the LCD      *
//      *                                                                *
//      *            S. Reifel & Co.                7/11/2018            *
//      *                                                                *
//      ******************************************************************

// This sketch shows how to use the "Arduino User Interface" to create a 
// Commands that display information.  
//
// It is assumed that you've already run Example1 which sets the LCD's contrast. 
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
// for each menu, create a forward declaration with "extern"
//
extern MENU_ITEM mainMenu[];


//
// the main menu
//
MENU_ITEM mainMenu[] = {
  {MENU_ITEM_TYPE_MAIN_MENU_HEADER,  "",                NULL,                       mainMenu},
  {MENU_ITEM_TYPE_COMMAND,           "Show status",     menuCommandShowStatus,      NULL},
  {MENU_ITEM_TYPE_COMMAND,           "Free Ram",        menuCommandShowFreeRam,     NULL},
  {MENU_ITEM_TYPE_COMMAND,           "About",           menuCommandShowAboutBox,    NULL},
  {MENU_ITEM_TYPE_COMMAND,           "Countdown 100",   menuCommandCountdown100,    NULL},
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
//                          Commands executed from the menu
// ---------------------------------------------------------------------------------

//
// menu command to: display some status information
//
void menuCommandShowStatus(void)
{
  char stringBuffer[10];
  
  //
  // clear the screen
  //
  ui.lcdClearDisplay();

  //
  // start on the top, line 0
  //
  ui.lcdSetCursorXY(0, 0);                           // move the cursor to column 0, line 0
  ui.lcdPrintString("Temp:");                        // draw "Temp" left justified
  ui.lcdSetCursorXY(LCD_LAST_COLUMN_X, 0);           // move cursor to the extreme right pixel
  int fakeTemp = 123;
  ui.lcdPrintIntRightJustified(fakeTemp, 0);

  //
  // now draw on the second line, line 1
  //
  ui.lcdSetCursorXY(0, 1);                           // move the cursor to column 0, line 1
  ui.lcdPrintString("Power:");                       // draw "Power" left justified
  ui.lcdSetCursorXY(LCD_LAST_COLUMN_X, 1);           // move cursor to the extreme right pixel
  int fakePowerUsage = -86;
  ui.lcdPrintIntRightJustified(fakePowerUsage, 0);   // print the INT, right justified

  ui.lcdSetCursorXY(0, 2);                           // move the cursor to column 0, line 2
  ui.lcdPrintString("Voltage:");
  ui.lcdSetCursorXY(LCD_LAST_COLUMN_X, 2);
  float fakeVoltage = 4.6;
  dtostrf(fakeVoltage, 1, 2, stringBuffer);          // convert a floating point value to string
  ui.lcdPrintStringRightJustified(stringBuffer, 0);  // display the string, right justified

  ui.lcdSetCursorXY(0, 3);
  ui.lcdPrintString("Flux cap:");
  ui.lcdSetCursorXY(LCD_LAST_COLUMN_X, 3);
  ui.lcdPrintStringRightJustified("On", 0);


  //
  // draw the button bar on the LCD's bottom row, showing "OK" over the   
  // "Select" button, and nothing over the "Back" button
  //
  ui.drawButtonBar("OK", "");

  //
  // read button events until the "Select" button is pressed
  //
  while(ui.getButtonEvent() != BUTTON_ID_SELECT + BUTTON_PUSHED_EVENT)
    ;
}



//
// menu command to: display the amount of unused RAM
//
void menuCommandShowFreeRam(void)
{
  //
  // clear the screen
  //
  ui.lcdClearDisplay();

  //
  // print "Free RAM" center on line #1
  //
  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 1);
  ui.lcdPrintStringCentered("Free RAM", 0);

  //
  // print the number of bytes of free RAM center on line #2
  //
  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 2);
  ui.lcdPrintIntCentered(freeRam(), 0);

  //
  // draw the button bar on the LCD's bottom row, showing "OK" over the   
  // "Select" button, and nothing over the "Back" button
  //
  ui.drawButtonBar("OK", "");

  //
  // read button events until the "Select" button is pressed
  //
  while(ui.getButtonEvent() != BUTTON_ID_SELECT + BUTTON_PUSHED_EVENT)
    ;
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
  // display several lines of centered text
  //
  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 0);
  ui.lcdPrintStringCentered("Arduino UI", 0);

  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 1);
  ui.lcdPrintStringCentered("Version 0.0", 0);

  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 4);
  ui.lcdPrintStringCentered("(c) 2018", 0);

  ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 5);
  ui.lcdPrintStringCentered("S. Reifel & Co", 0);

  //
  // instead of waiting for a button, display the "About box" for 4 seconds
  //
  delay(4000);
}



//
// menu command to: count from 100 to 0
//
void menuCommandCountdown100(void)
{
  int countValue = 100;

  
  //
  // clear the display
  //
  ui.lcdClearDisplay();
  

  //
  // draw the button bar on the LCD's bottom row, showing "OK" over the   
  // "Select" button, and "Cancel" over the "Back" button
  //
  ui.drawButtonBar("OK", "Cancel");


  //
  // count down from 100
  //
  while(true)
  {
    //
    // display the count
    //
    ui.lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 2);    // show number centered on line 2

                                                    // the "3" pads printing the number with 
                                                    // spaces so it always prints 3 characters 
    ui.lcdPrintIntCentered(countValue, 3);          // wide, try setting to 0 to see the problem


    //
    // decrement the number, return when counted down to zero
    //
    countValue--;
    if (countValue == 0)
      return;


    //
    // read the buttons
    //
    int buttonEvent = ui.getButtonEvent();


    //
    // check if the "OK" button has been pressed
    //
    if (buttonEvent == BUTTON_ID_SELECT + BUTTON_PUSHED_EVENT)
    {
        //
        // do you stuff needed when the OK button is pressed, then return
        //
        return;
    }


    //
    // check if the "Cancel" button has been pressed
    //
    if (buttonEvent == BUTTON_ID_BACK + BUTTON_PUSHED_EVENT)
    {
        return;                         // just return
    }
    

    //
    // delay a little before show the next number in the countdown
    //
    delay(80);
  }
}


// ---------------------------------------------------------------------------------
//                                    Misc functions
// ---------------------------------------------------------------------------------

//
// return the number of bytes of free RAM
//
int freeRam() 
{
  extern int __heap_start, *__brkval;
  int v;
  
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
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

