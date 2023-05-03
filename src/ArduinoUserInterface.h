
//      ******************************************************************
//      *                                                                *
//      *            Header file for ArduinoUserInterface.cpp            *
//      *                                                                *
//      *              Copyright (c) S. Reifel & Co,  2014               *
//      *                                                                *
//      ******************************************************************


// MIT License
// 
// Copyright (c) 2014 Stanley Reifel & Co.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#ifndef ArduinoUserInterface_h
#define ArduinoUserInterface_h

#include "Arduino.h"
#include <avr/pgmspace.h>


//
// definition of an entry in the menu table
//
typedef struct _MENU_ITEM
{
  byte MenuItemType;
  char *MenuItemText;
  void (*MenuItemFunction)();
  _MENU_ITEM *MenuItemSubMenu;
} MENU_ITEM;


//
// menu item types
//
const byte MENU_ITEM_TYPE_MAIN_MENU_HEADER = 0;
const byte MENU_ITEM_TYPE_SUB_MENU_HEADER  = 1;
const byte MENU_ITEM_TYPE_SUB_MENU         = 2;
const byte MENU_ITEM_TYPE_COMMAND          = 3;
const byte MENU_ITEM_TYPE_TOGGLE           = 4;
const byte MENU_ITEM_TYPE_END_OF_MENU      = 5;


//
// menu misc constants
//
const byte MAX_MENU_ITEMS_TO_DISPLAY = 4;


//
// ID values for the buttons
//
const byte BUTTON_ID_NONE        = 0;
const byte BUTTON_ID_SELECT      = 1;	      // the button below the LCD on the left
const byte BUTTON_ID_BACK        = 2;	      // the button below the LCD on the right
const byte BUTTON_ID_UP          = 3;	      // the top button right of the LCD
const byte BUTTON_ID_DOWN        = 4;	      // the bottom button right of the LCD


//
// types of button events
//
const byte BUTTON_NO_EVENT        = 0x00;   // the button has not changed
const byte BUTTON_PUSHED_EVENT    = 0x40;   // the button was pressed down
const byte BUTTON_RELEASED_EVENT  = 0x80;   // the button was released
const byte BUTTON_REPEAT_EVENT    = 0xc0;   // button is held down & periodically repeats


//
// Slider Display callback actions
//
const byte SLIDER_DISPLAY_VALUE_CHANGED = 1;
const byte SLIDER_DISPLAY_VALUE_SET     = 2;
const byte SLIDER_DISPLAY_CANCELED      = 3;


//
// LCD size constants
//
const byte LCD_WIDTH_IN_PIXELS = 84;
const byte LCD_LAST_COLUMN_X = LCD_WIDTH_IN_PIXELS-1;
const byte LCD_HEIGHT_IN_PIXELS = 48;
const byte LCD_HEIGHT_IN_LINES = 6;


//
// the ArduinoUserInterface class
//
class ArduinoUserInterface
{
  public:
    //
    // public member variables
    //
    byte toggleMenuChangeStateFlag;
    char *toggleMenuStateText;


    //
    // public functions
    //
    ArduinoUserInterface(void);
    void connectToPins(byte _lcdClockPin, byte _lcdDataInPin, byte _lcdDataControlPin, byte _lcdChipEnablePin, byte _buttonAnalogPin);
    void displayAndExecuteMenu(MENU_ITEM *menu);
    void displaySlider(int minValue, int maxValue, int step, int initialValue, char *label, void (*callbackFunc)(byte, int));
    void displayFloatSlider(float minValue, float maxValue, float step, float initialValue, char *label, byte digitsRightOfDecimal, void (*callbackFunc)(byte, float));
    void clearDisplaySpace(void);
    byte getButtonEvent(void);
    boolean intInRange(int value, int lowerValue, int upperValue);
    void drawButtonBar(char *leftButtonText, char *rightButtonText);
    void lcdPrintInt(int n);
    void lcdPrintIntLeftJustified(int n, int padToNumberOfDigits);
    void lcdPrintIntRightJustified(int n, int padToNumberOfDigits);
    void lcdPrintIntCentered(int n, int padToNumberOfDigits);
    void lcdPrintString(char *s);
    void lcdPrintStringLeftJustified(char *s, int padToNumberOfCharacters);
    void lcdPrintStringRightJustified(char *s, int padToNumberOfCharacters);
    void lcdPrintStringCentered(char *s, int padToNumberOfCharacters);
    void lcdPrintCenteredStringReverse(char *s, byte X, byte padToWidth);
    void lcdPrintStringReverse(char *s);
    void lcdPrintCharacter(byte character);
    void lcdPrintCharacterReverse(byte character);
    byte lcdStringWidthInPixels(char *s);
    void lcdClearDisplay(void);
    void lcdFillToColumnX(int X, byte byteOfPixels);
    void lcdFillToEndOfLine(byte byteOfPixels);
    void lcdDrawRowOfPixels(int X1, int X2, int lineNumber, byte byteOfPixels);
    void lcdSetCursorXY(int column, int lineNumber);
    void lcdSetContrast(int contrastValue);
    void writeConfigurationByte(int EEPromAddress, byte value);
    byte readConfigurationByte(int EEPromAddress, byte defaultValue);
    void writeConfigurationInt(int EEPromAddress, int value);
    int readConfigurationInt(int EEPromAddress, int defaultValue);
    void writeConfigurationLong(int EEPromAddress, long value);
    long readConfigurationLong(int EEPromAddress, long defaultValue);


  private:
    //
    // private member variables
    //
    byte lcdClockPin;
    byte lcdDataInPin;
    byte lcdDataControlPin;
    byte lcdChipEnablePin;
    byte lcdCursorColumnX;
    byte lcdCursorRowY;

    byte buttonAnalogPin;
    byte buttonState;
    byte buttonID;
    unsigned long buttonEventStartTime;

    MENU_ITEM *currentMenuTable;
    byte currentMenuItemIdx;
    byte currentMenuTopLineItemIdx;

    int sliderDisplayMinValue;
    int sliderDisplayMaxValue;
    int sliderDisplayStep;
    int sliderDisplayValue;
    void (*sliderDisplayCallbackFunc)(byte, int);
    unsigned int sliderDisplayRepeatCount;
    
    float sliderDisplayFloatMinValue;
    float sliderDisplayFloatMaxValue;
    float sliderDisplayFloatStep;
    float sliderDisplayFloatValue;
    byte sliderDisplayDigitsRightOfDecimal;
    void (*sliderDisplayFloatCallbackFunc)(byte, float);

    
    //
    // private functions
    //
    void selectAndDrawMenu(MENU_ITEM *menu);
    void drawMenu(void);
    void drawMenuItem(byte menuIdx, byte menuLine);
    void drawSlider(void);
    void drawFloatSlider(void);
    void executeMenuItem();
    void buttonsInitialize(void);
    byte readButtonsToGetButtonID(void);
    void lcdInitialise(void);
    void lcdWriteCommand(byte command);
    void lcdWriteData(byte data);
    void lcdSerialOut(byte data);
};

// ------------------------------------ End ---------------------------------
#endif


