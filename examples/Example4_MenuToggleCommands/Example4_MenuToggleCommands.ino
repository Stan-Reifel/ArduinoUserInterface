
//      ******************************************************************
//      *                                                                *
//      *        Example shows how to add "Toggles" to your menu         *
//      *                                                                *
//      *            S. Reifel & Co.                7/11/2018            *
//      *                                                                *
//      ******************************************************************

// Toggles are used somewhat like Radio Buttons in a dialog box.  They allow 
// the user to select one of a fixed number of choices (such as On / Off, or   
// Red / Green / Blue).  Each time the user clicks on a toggle menu item, it 
// alternates the selection (i.e. toggles between On and Off, or rotates 
// between Red, Green and Blue).
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
static byte ledState = false;
static byte colorState = 0;



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
// Earlier examples have menus filled with just "Commands".  Here we show a  
// menu with "Commands" and "Toggles".  Toggles let the user select one of a 
// fixed number of choices (such as On / Off,  or  Red / Green / Blue).  Each 
// time the user clicks a toggle menu item, it alternates the selection.
//
// In the second field of a A MENU_ITEM_TYPE_TOGGLE entry is the text displayed
// in the menu.  The third field is a pointer to a callback function that you 
// write to alternates the value. The last field should always be NULL.


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
  {MENU_ITEM_TYPE_TOGGLE,            "Color",           menuToggleColorCallback,    NULL},
  {MENU_ITEM_TYPE_COMMAND,           "About",           menuCommandShowAboutBox,    NULL},
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
  // check if menu is requesting that the state be changed (can have more than 2 states)
  //
  if (ui.toggleMenuChangeStateFlag)
  {
    //
    // select the next state
    //
    ledState = !ledState;
  }
  
  //
  // turn the LED on or off as indicated
  //
  if (ledState)
    digitalWrite(LED_PIN, HIGH);
  else
    digitalWrite(LED_PIN, LOW);
 
  //
  // send back an indication of the current state
  //
  if(ledState)
    ui.toggleMenuStateText = "On";
  else
    ui.toggleMenuStateText = "Off";
}



//
// menu command to: change the selected color
//
void menuToggleColorCallback(void)
{
  //
  // check if menu is requesting that the state be changed (can have more than 2 states)
  //
  if (ui.toggleMenuChangeStateFlag)
  {
    //
    // select the next state
    //
    colorState++;
    if (colorState >= 3) colorState = 0;
  }
  
  
  //
  // here is where do you something because the user changed the color
  //
  if (colorState == 0)
    ;
  else if(colorState == 1)
    ;
  else
    ;  

 
  //
  // send back an indication of the current state
  //
  if (colorState == 0)
    ui.toggleMenuStateText = "Brown";
  else if(colorState == 1)
    ui.toggleMenuStateText = "Black";
  else
    ui.toggleMenuStateText = "White";
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

