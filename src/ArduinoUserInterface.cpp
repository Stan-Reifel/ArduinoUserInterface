
//      ******************************************************************
//      *                                                                *
//      *                     Arduino User Interface                     *
//      *                                                                *
//      *            Stan Reifel                     8/10/2014           *
//      *               Copyright (c) S. Reifel & Co, 2014               *
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


//
// This user interface works with Arduino applications having a Nokia 5110 LCD 
// display and 4 push buttons.  The display is divided into two sections, each 
// section is the full width of the screen.  
//
// The top section of the display is the "DisplaySpace".  Here is where menus,
// message boxes, configuration screens, along with the application's main 
// display is shown.
//
// Along the bottom of the screen is the "ButtonBar".  Mounted below the LCD
// display are two of the push buttons.  The function of these buttons change, 
// so the ButtonBar is used to label what the buttons do.  Typically the left 
// button will be labeled "Select" and the right "Back".
//
// Mounted along the right side of the LCD display are the other two buttons, one 
// to go up, the other down.
//
// displayAndExecuteMenu() presents a menu in the DisplaySpace.  This function is 
// called with a pointer to a table that defines the contents of the menu.  Each 
// menu item includes the menu text, along with what to do when the menu item is 
// selected by the user.  There are three types of menu items: Commands, Toggles, 
// and Sub Menus.
//
// A typical main menu table might look like this:
//
//  MENU_ITEM mainMenu[] = {
//   {MENU_ITEM_TYPE_MAIN_MENU_HEADER, "",         NULL,            mainMenu},
//   {MENU_ITEM_TYPE_COMMAND,          "Contrast", contrastCommand, NULL},
//   {MENU_ITEM_TYPE_TOGGLE,           "Sound",    soundCallback,   NULL},
//   {MENU_ITEM_TYPE_SUB_MENU,         "Settings", NULL,            settingsMenu},
//   {MENU_ITEM_TYPE_END_OF_MENU,      "",         NULL,            NULL}
//  };
//
//
// The first line in the table always defines what type of menu it is, either a 
// Main Menu, or a Sub Menu.  The table's last line marks the end of the table.  
// In between are menu items.
//
// A MENU_ITEM_TYPE_COMMAND entry indicates that a function will be executed when 
// this menu item is selected.  A pointer to the function is third field in the
// entry.
//
// A MENU_ITEM_TYPE_TOGGLE is used somewhat like a Radio Button in a dialog 
// box.  It allows the user to select one of a fixed number of choices (such as  
// On / Off,   or   Red / Green / Blue).  Each time the user clicks on this type
// of menu item, it alternates the selection (i.e. toggles between On and Off, or 
// rotates between Red, Green and Blue).  The third field in this entry points 
// to a callback function that alternates the value.
//
// A MENU_ITEM_TYPE_SUB_MENU entry is used to select a different menu.  For 
// example, a main menu might reference a "Settings" sub menu. The fourth field  
// in this entry points to the new menu table that defines the sub menu.
//
// As described above, the first line in a menu table indicates the type of menu:
//
// A MENU_ITEM_TYPE_MAIN_MENU_HEADER in the first line of the table specifies 
// that the table is the main menu.  In this case, the fourth field can have one 
// of two values.  If the fourth field set to NULL, then the "Back" button will 
// be displayed. Pressing "Back" will return from the menu system, resuming the  
// main application.  Alternately, if the fourth field is set to a pointer back 
// to the main menu table, then the "Back" button will not be displayed.  This 
// would be used for the case when the main menu is the main application.
//
// A MENU_ITEM_TYPE_SUB_MENU_HEADER in the first line of the table specifies that
// the table is for a sub menu.  Sub menus that are menus that are called from a 
// main menu, or another sub menu.  The fourth field is a pointer back to the 
// parent menu table (typically this would be the main menu).  This is used to 
// reselect the parent menu when the user presses the "Back" button, indicating 
// that they are done with the sub menu.
//
// In addition to menus, there are other things that can be shown in the 
// DisplaySpace.  A Slider allows the users to select a numeric value (such as 
// 0 to 255,  or  -1000 to 1000).  In addition to printing the number on the LCD, 
// a Slider shows a graphical representation of the value. The user sets the 
// number by pressing the Up and Down buttons.  Sliders are displayed by calling 
// displaySlider().
//

// ---------------------------------------------------------------------------------

#include <avr/pgmspace.h>
#include <EEPROM.h>
#include "ArduinoUserInterface.h"



// ---------------------------------------------------------------------------------
//                                     Setup functions 
// ---------------------------------------------------------------------------------

//
// constructor for the ArduinoArduinoUserInterface class
//
ArduinoUserInterface::ArduinoUserInterface(void)
{
}



//
// connect the IO pins for the LCD and buttons, then clear the display
//  Enter:  _lcdClockPin = digital pin that connects to the LCD's clock pin (CLK)
//          _lcdDataInPin = digital pin that connects to the LCD's data pin (DIN)
//          _lcdDataControlPin = digital pin that connects to LCD's control pin (DC)
//          _lcdChipEnablePin = digital pin that connects to the LCD's chip enable pin (CE)
//          _buttonAnalogPin = analog pin that connects to the buttons
//
void ArduinoUserInterface::connectToPins(byte _lcdClockPin, byte _lcdDataInPin, 
                              byte _lcdDataControlPin, byte _lcdChipEnablePin,
                              byte _buttonAnalogPin)
{
  //
  // assign IO pin numbers
  //
  lcdClockPin = _lcdClockPin;
  lcdDataInPin = _lcdDataInPin;
  lcdDataControlPin = _lcdDataControlPin;
  lcdChipEnablePin = _lcdChipEnablePin;
  buttonAnalogPin = _buttonAnalogPin;
  

  //
  // initialize the hardware and clear the display
  //
  lcdInitialise();
  lcdClearDisplay();
  buttonsInitialize();
}


// ---------------------------------------------------------------------------------
//                                     Menu display  
// ---------------------------------------------------------------------------------

//
// display the top leven menu, wait for button presses and execute commands 
// connected to the menu
//  Enter:  menu -> the menu to display
//
void ArduinoUserInterface::displayAndExecuteMenu(MENU_ITEM *menu)
{
  byte buttonEvent;
  byte newMenuItemIdx;
  byte menuItemType;
  MENU_ITEM *parentMenu;
  
  //
  // display the top lever menu, selecting the first item in the menu
  //
  selectAndDrawMenu(menu);
  
  //
  // check for and execute button pushes and menu commands
  //
  while(true)
  {
    //
    // check for a new button event
    //
    buttonEvent = getButtonEvent();
    switch(buttonEvent)
    {
      //
      // check if the "down" button has been pressed
      //
      case BUTTON_ID_DOWN + BUTTON_PUSHED_EVENT:
      case BUTTON_ID_DOWN + BUTTON_REPEAT_EVENT:
      {
        //
        // move the selected menu entry down one, if not already at the end of 
        // the table
        //
        newMenuItemIdx = currentMenuItemIdx + 1;
        if (currentMenuTable[newMenuItemIdx].MenuItemType == MENU_ITEM_TYPE_END_OF_MENU)
          break;
        currentMenuItemIdx = newMenuItemIdx;
        
        //
        // check if new menu item is off the end of the screen
        //
        if(currentMenuItemIdx - currentMenuTopLineItemIdx >= MAX_MENU_ITEMS_TO_DISPLAY)
          currentMenuTopLineItemIdx++;
        
        //
        // redraw the menu
        //
        drawMenu();
        break;
      }

      //
      // check if the "up" button has been pressed
      //
      case BUTTON_ID_UP + BUTTON_PUSHED_EVENT:
      case BUTTON_ID_UP + BUTTON_REPEAT_EVENT:
      {
        //
        // move the selected menu entry up one, if not already at the beginning 
        // of the table
        //
        newMenuItemIdx = currentMenuItemIdx - 1;
        if (newMenuItemIdx == 0)
          break;
        currentMenuItemIdx = newMenuItemIdx;
        
        //
        // check if new menu item is off the beginning of the screen
        //
        if(currentMenuItemIdx - currentMenuTopLineItemIdx < 0)
          currentMenuTopLineItemIdx--;
          
        //
        // redraw the menu
        //
        drawMenu();
        break;
      }

      //
      // check if the "select" button has been pressed
      //
      case BUTTON_ID_SELECT + BUTTON_PUSHED_EVENT:
      {
        executeMenuItem();
        break;
      }


      //
      // check if the "back" button has been pressed
      //
      case BUTTON_ID_BACK + BUTTON_PUSHED_EVENT:
      {
        //
        // get the type of the currently displayed menu
        //
        menuItemType = currentMenuTable[0].MenuItemType;
        
        //
        // check if this is a sub menu, if so go back to its parent
        //
        if (menuItemType == MENU_ITEM_TYPE_SUB_MENU_HEADER)
        {
          parentMenu = currentMenuTable[0].MenuItemSubMenu;
          selectAndDrawMenu(parentMenu);
          break;
        }
        
        //
        // check if this is the main menu, if so optionally return from this 
        // subroutine call
        //
        if (menuItemType == MENU_ITEM_TYPE_MAIN_MENU_HEADER)
        {
          //
          // if no menu is attached to the main menu, then just return to the 
          // caller of this function
          //
          parentMenu = currentMenuTable[0].MenuItemSubMenu;
          if (parentMenu == NULL)
            return;
          break;
        }
      }
    }
  }
}



//
// select and display a menu or submenu
//  Enter:  menu -> the menu to display
//
void ArduinoUserInterface::selectAndDrawMenu(MENU_ITEM *menu)
{ 
  //
  // remember this menu and select the first item in the menu
  //
  currentMenuTable = menu;
  currentMenuItemIdx = 1;
  currentMenuTopLineItemIdx = 1;

  //
  // clear the display space and move the cursor to the top line
  //
  clearDisplaySpace();
  
  //
  // draw all lines of the menu, showing one of the items as selected
  //
  drawMenu();

  //
  // draw the button bar, don't include the back button if this is the main menu
  // and it links back to itself
  //
  if ((currentMenuTable[0].MenuItemType == MENU_ITEM_TYPE_MAIN_MENU_HEADER) &&
    (currentMenuTable[0].MenuItemSubMenu != NULL))
  {
    drawButtonBar("Select", "");
  }
  else
  {
    drawButtonBar("Select", "Back");
  }
}



//
// select and display a menu or submenu
//  Enter:  currentMenuTable -> the menu to display
//          currentMenuItemIdx = index in that menu of the line to select
//
void ArduinoUserInterface::drawMenu(void)
{ 
  byte menuIdx;
  byte menuLine;

  //
  // draw each of the menu items
  //
  menuIdx = currentMenuTopLineItemIdx;
  for(menuLine = 0; menuLine < MAX_MENU_ITEMS_TO_DISPLAY; menuLine++)
  {
    if(currentMenuTable[menuIdx].MenuItemType == MENU_ITEM_TYPE_END_OF_MENU)
      break;
    
    drawMenuItem(menuIdx, menuLine);
    menuIdx++;
  }
}



//
// draw one entry of a menu, show it in reverse video if it is selected
//  Enter:  menuIdx = the index into the currently selected menu to draw
//          menuLine = line number on the LCD to draw the item, 0 = top line
//
void ArduinoUserInterface::drawMenuItem(byte menuIdx, byte menuLine)
{
  byte menuItemSelected;
  byte fillCharacter;
  char *menuItemText;
  byte widthOfToggleMenuStateTextInPixels;
  
  //
  // move the cursor the the correct line number on the LCD
  //
  lcdSetCursorXY(0, menuLine);

  //
  // get the text displayed on the menu line
  //
  menuItemText = currentMenuTable[menuIdx].MenuItemText;
  
  //
  // determine if this menu item is selected
  //
  if (menuIdx == currentMenuItemIdx)
  {
    fillCharacter = 0xff;
    menuItemSelected = true;
  }
  else
  {
    fillCharacter = 0x00;
    menuItemSelected = false;
  }
  
  //
  // determine the menu entry type and draw it
  //
  switch(currentMenuTable[menuIdx].MenuItemType)
  {    
    //
    // display the text for "sub menu" type menu entry
    //    
    case MENU_ITEM_TYPE_SUB_MENU:
    {
      lcdFillToColumnX(3, fillCharacter);
      
      if(menuItemSelected)
      {
        lcdPrintStringReverse(menuItemText);
        lcdFillToColumnX(LCD_WIDTH_IN_PIXELS - 6, fillCharacter);
        lcdPrintCharacterReverse(0x80);
      }
      else
      {
        lcdPrintString(menuItemText);
        lcdFillToColumnX(LCD_WIDTH_IN_PIXELS - 6, fillCharacter);
        lcdPrintCharacter(0x80);
      }
      break;
    }

    //
    // display the text for "command" type menu entry
    //
    case MENU_ITEM_TYPE_COMMAND:
    {
      lcdFillToColumnX(3, fillCharacter);
      
      if(menuItemSelected)
        lcdPrintStringReverse(menuItemText);
      else
        lcdPrintString(menuItemText);

      lcdFillToEndOfLine(fillCharacter);
      break;
    }

    //
    // display the text for "toggle" type menu entry
    //    
    case MENU_ITEM_TYPE_TOGGLE:
    {
      //
      // execute the callback fuction to get the text for the toggle
      //
      toggleMenuChangeStateFlag = false;
      (currentMenuTable[menuIdx].MenuItemFunction)();
      widthOfToggleMenuStateTextInPixels = lcdStringWidthInPixels(toggleMenuStateText);

      //
      // draw the menu item
      //      
      lcdFillToColumnX(3, fillCharacter);
      
      if(menuItemSelected)
      {
        lcdPrintStringReverse(menuItemText);
        lcdFillToColumnX(LCD_WIDTH_IN_PIXELS - widthOfToggleMenuStateTextInPixels, fillCharacter);
        lcdPrintStringReverse(toggleMenuStateText);
      }
      else
      {
        lcdPrintString(menuItemText);
        lcdFillToColumnX(LCD_WIDTH_IN_PIXELS - widthOfToggleMenuStateTextInPixels, fillCharacter);
        lcdPrintString(toggleMenuStateText);
      }
      break;
    }
  }
}



//
// execute the currently selected menu item
//
void ArduinoUserInterface::executeMenuItem()
{
  MENU_ITEM *subMenu;

  //
  // determine the type of menu entry, then execute it
  //
  switch(currentMenuTable[currentMenuItemIdx].MenuItemType)
  {    
    //
    // switch the menu to the selected sub menu
    //
    case MENU_ITEM_TYPE_SUB_MENU:
    {
      subMenu = currentMenuTable[currentMenuItemIdx].MenuItemSubMenu;
      selectAndDrawMenu(subMenu);
      break;
    }
    
    //
    // execute the menu item's function
    //
    case MENU_ITEM_TYPE_COMMAND:
    {
      //
      // execute the menu item's function
      //
      (currentMenuTable[currentMenuItemIdx].MenuItemFunction)();
      
      //
      // display the menu again
      //
      selectAndDrawMenu(currentMenuTable);
      break;
    }
    
    //
    // toggle the menu item, then redisplay
    //
    case MENU_ITEM_TYPE_TOGGLE:
    {
      toggleMenuChangeStateFlag = true;
      (currentMenuTable[currentMenuItemIdx].MenuItemFunction)();
      drawMenuItem(currentMenuItemIdx, currentMenuItemIdx - currentMenuTopLineItemIdx);
    }
  }
}


// ---------------------------------------------------------------------------------
//                                    Slider display  
// ---------------------------------------------------------------------------------

//
// the Slider Display
//	Enter:  minValue = the minimum value for the slider
//		      maxValue = the max value for the slider
//		      step = amount added/subtracted to the value with each button press
//		      initialValue = initial value to display on the slider
//		      label -> string printed above the slider
//		      callbackFunc -> the callback function
//
void ArduinoUserInterface::displaySlider(int minValue, int maxValue, int step, 
                int initialValue, char *label, void (*callbackFunc)(byte, int))
{
  byte buttonEvent;
  int stepSize;
  
  //
  // remember the slider values
  //
  sliderDisplayMinValue = minValue;
  sliderDisplayMaxValue = maxValue;
  sliderDisplayValue = initialValue;
  sliderDisplayStep = step;
  sliderDisplayCallbackFunc = callbackFunc;


  //
  // clear the display space and move the cursor to the top line
  //
  clearDisplaySpace();
  
  
  //
  // draw the title of the slider, centered
  //
  lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 0); 
  lcdPrintStringCentered(label, 0);

  //
  // draw all lines of the menu, showing one of the items as selected
  //
  drawSlider();

  //
  // draw the button bar
  //
  drawButtonBar("Set", "Cancel");

  //
  // check for and execute button pushes
  //
  while(true)
  {
    int newSliderValue;
    
    //
    // check for a new button event
    //
    buttonEvent = getButtonEvent();
    switch(buttonEvent)
    {
      //
      // check if the "down" button has been pressed
      //
      case BUTTON_ID_DOWN + BUTTON_PUSHED_EVENT:
        sliderDisplayRepeatCount = 0;
      case BUTTON_ID_DOWN + BUTTON_REPEAT_EVENT:
      {
        sliderDisplayRepeatCount++;
        stepSize = sliderDisplayStep * ((sliderDisplayRepeatCount / 16) + 1);
        newSliderValue = sliderDisplayValue - stepSize;
 
        if (newSliderValue < sliderDisplayMinValue)
          newSliderValue = sliderDisplayMinValue;
          
        if (newSliderValue == sliderDisplayValue)
          break;
 
        sliderDisplayValue = newSliderValue; 
        (*sliderDisplayCallbackFunc)(SLIDER_DISPLAY_VALUE_CHANGED, sliderDisplayValue);
        drawSlider();
        break;
      }

      //
      // check if the "up" button has been pressed
      //
      case BUTTON_ID_UP + BUTTON_PUSHED_EVENT:
        sliderDisplayRepeatCount = 0;
      case BUTTON_ID_UP + BUTTON_REPEAT_EVENT:
      {
        sliderDisplayRepeatCount++;
        stepSize = sliderDisplayStep * ((sliderDisplayRepeatCount / 16) + 1);
        newSliderValue = sliderDisplayValue + stepSize;
 
        if (newSliderValue > sliderDisplayMaxValue)
          newSliderValue = sliderDisplayMaxValue;
          
        if (newSliderValue == sliderDisplayValue)
          break;
 
        sliderDisplayValue = newSliderValue; 
        (*sliderDisplayCallbackFunc)(SLIDER_DISPLAY_VALUE_CHANGED, sliderDisplayValue);
        drawSlider();
        break;
      }

      //
      // check if the "set" button has been pressed
      //
      case BUTTON_ID_SELECT + BUTTON_PUSHED_EVENT:
      {
        (*sliderDisplayCallbackFunc)(SLIDER_DISPLAY_VALUE_SET, sliderDisplayValue);
        return;
      }


      //
      // check if the "cancel" button has been pressed
      //
      case BUTTON_ID_BACK + BUTTON_PUSHED_EVENT:
      {
        (*sliderDisplayCallbackFunc)(SLIDER_DISPLAY_CANCELED, 0);
        return;
      }
    }
  }
}


const byte SLIDER_FRAME_LINE_NUMBER = 2;
const byte SLIDER_FRAME_LEFT_X = 0;
const byte SLIDER_FRAME_RIGHT_X = LCD_LAST_COLUMN_X;
const byte SLIDER_NEEDLE_PADDING_FROM_FRAME = 3;
const byte SLIDER_NEEDLE_LEFT = SLIDER_FRAME_LEFT_X + SLIDER_NEEDLE_PADDING_FROM_FRAME;
const long SLIDER_NEEDLE_WIDTH = (SLIDER_FRAME_RIGHT_X - SLIDER_FRAME_LEFT_X) - (2 * SLIDER_NEEDLE_PADDING_FROM_FRAME);

//
// draw the slider
//
void ArduinoUserInterface::drawSlider(void)
{
  byte needleLength;
//  char stringBuffer[7];
  
  //
  // draw the left side of the frame
  //
  lcdDrawRowOfPixels(SLIDER_FRAME_LEFT_X, SLIDER_FRAME_LEFT_X, SLIDER_FRAME_LINE_NUMBER, 0x7f);
  
  //
  // draw right side of frame up to the beginning of the needle
  //
  lcdDrawRowOfPixels(SLIDER_FRAME_LEFT_X+1, SLIDER_NEEDLE_LEFT-1, SLIDER_FRAME_LINE_NUMBER, 0x41);
  
  //
  // draw the needle
  //
  needleLength = (byte) ((((long)sliderDisplayValue - (long)sliderDisplayMinValue) * SLIDER_NEEDLE_WIDTH) / 
    (long)(sliderDisplayMaxValue - (sliderDisplayMinValue)));

  lcdDrawRowOfPixels(SLIDER_NEEDLE_LEFT, SLIDER_NEEDLE_LEFT + needleLength, SLIDER_FRAME_LINE_NUMBER, 0x5D);
  
  //
  // draw the frame after needle
  //
  lcdDrawRowOfPixels(SLIDER_NEEDLE_LEFT + needleLength + 1, SLIDER_FRAME_RIGHT_X - 1, SLIDER_FRAME_LINE_NUMBER, 0x41);

  //
  // draw the right side of the frame
  //
  lcdDrawRowOfPixels(SLIDER_FRAME_RIGHT_X, SLIDER_FRAME_RIGHT_X, SLIDER_FRAME_LINE_NUMBER, 0x7f);
  
  //
  // draw the numberic value under the slider
  //
  lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, SLIDER_FRAME_LINE_NUMBER+1);
  lcdPrintIntCentered(sliderDisplayValue, 6);
}



//
// the Float Slider Display
//  Enter:  minValue = the minimum value for the slider
//	        maxValue = the max value for the slider
//	        step = amount added/subtracted to the value with each button press
//	        initialValue = initial value to display on the slider
//	        label -> string printed above the slider
//          digitsRightOfDecimal = number of digits right of the decimal point 
//            that are displayed
//	        callbackFunc -> the callback function
//
void ArduinoUserInterface::displayFloatSlider(float minValue, float maxValue, 
            float step, float initialValue, char *label, byte digitsRightOfDecimal, 
            void (*callbackFunc)(byte, float))
{
  byte buttonEvent;
  float stepSize;

  //
  // remember the slider values
  //
  sliderDisplayFloatMinValue = minValue;
  sliderDisplayFloatMaxValue = maxValue;
  sliderDisplayFloatValue = initialValue;
  sliderDisplayFloatStep = step;
  sliderDisplayDigitsRightOfDecimal = digitsRightOfDecimal;
  sliderDisplayFloatCallbackFunc = callbackFunc;


  //
  // clear the display space and move the cursor to the top line
  //
  clearDisplaySpace();
  
  
  //
  // draw the title of the slider, centered
  //
  lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, 0); 
  lcdPrintStringCentered(label, 0);

  //
  // draw all lines of the menu, showing one of the items as selected
  //
  drawFloatSlider();

  //
  // draw the button bar
  //
  drawButtonBar("Set", "Cancel");

  //
  // check for and execute button pushes
  //
  while(true)
  {
    float newSliderFloatValue;
    
    //
    // check for a new button event
    //
    buttonEvent = getButtonEvent();
    switch(buttonEvent)
    {
      //
      // check if the "down" button has been pressed
      //
      case BUTTON_ID_DOWN + BUTTON_PUSHED_EVENT:
        sliderDisplayRepeatCount = 0;
      case BUTTON_ID_DOWN + BUTTON_REPEAT_EVENT:
      {
        sliderDisplayRepeatCount++;
        stepSize = sliderDisplayFloatStep * ((sliderDisplayRepeatCount / 16) + 1);
        newSliderFloatValue = sliderDisplayFloatValue - stepSize;
 
        if (newSliderFloatValue < sliderDisplayFloatMinValue)
          newSliderFloatValue = sliderDisplayFloatMinValue;
          
        if (newSliderFloatValue == sliderDisplayFloatValue)
          break;
 
        sliderDisplayFloatValue = newSliderFloatValue; 
        (*sliderDisplayFloatCallbackFunc)(SLIDER_DISPLAY_VALUE_CHANGED, sliderDisplayFloatValue);
        drawFloatSlider();
        break;
      }

      //
      // check if the "up" button has been pressed
      //
      case BUTTON_ID_UP + BUTTON_PUSHED_EVENT:
        sliderDisplayRepeatCount = 0;
      case BUTTON_ID_UP + BUTTON_REPEAT_EVENT:
      {
        sliderDisplayRepeatCount++;
        stepSize = sliderDisplayFloatStep * ((sliderDisplayRepeatCount / 16) + 1);
        newSliderFloatValue = sliderDisplayFloatValue + stepSize;
 
        if (newSliderFloatValue > sliderDisplayFloatMaxValue)
          newSliderFloatValue = sliderDisplayFloatMaxValue;
          
        if (newSliderFloatValue == sliderDisplayFloatValue)
          break;
 
        sliderDisplayFloatValue = newSliderFloatValue; 
        (*sliderDisplayFloatCallbackFunc)(SLIDER_DISPLAY_VALUE_CHANGED, sliderDisplayFloatValue);
        drawFloatSlider();
        break;
      }

      //
      // check if the "set" button has been pressed
      //
      case BUTTON_ID_SELECT + BUTTON_PUSHED_EVENT:
      {
        (*sliderDisplayFloatCallbackFunc)(SLIDER_DISPLAY_VALUE_SET, sliderDisplayFloatValue);
        return;
      }


      //
      // check if the "cancel" button has been pressed
      //
      case BUTTON_ID_BACK + BUTTON_PUSHED_EVENT:
      {
        (*sliderDisplayFloatCallbackFunc)(SLIDER_DISPLAY_CANCELED, 0.0);
        return;
      }
    }
  }
}



//
// draw the slider
//
void ArduinoUserInterface::drawFloatSlider(void)
{
  byte needleLength;
  char stringBuffer[10];
  
  //
  // draw the left side of the frame
  //
  lcdDrawRowOfPixels(SLIDER_FRAME_LEFT_X, SLIDER_FRAME_LEFT_X, SLIDER_FRAME_LINE_NUMBER, 0x7f);
  
  //
  // draw right side of frame up to the beginning of the needle
  //
  lcdDrawRowOfPixels(SLIDER_FRAME_LEFT_X+1, SLIDER_NEEDLE_LEFT-1, SLIDER_FRAME_LINE_NUMBER, 0x41);
  
  //
  // draw the needle
  //
  needleLength = (byte) ((((sliderDisplayFloatValue - sliderDisplayFloatMinValue) * (float)SLIDER_NEEDLE_WIDTH) / 
    (sliderDisplayFloatMaxValue - sliderDisplayFloatMinValue)) + 0.5);

  lcdDrawRowOfPixels(SLIDER_NEEDLE_LEFT, SLIDER_NEEDLE_LEFT + needleLength, SLIDER_FRAME_LINE_NUMBER, 0x5D);
  
  //
  // draw the frame after needle
  //
  lcdDrawRowOfPixels(SLIDER_NEEDLE_LEFT + needleLength + 1, SLIDER_FRAME_RIGHT_X - 1, SLIDER_FRAME_LINE_NUMBER, 0x41);

  //
  // draw the right side of the frame
  //
  lcdDrawRowOfPixels(SLIDER_FRAME_RIGHT_X, SLIDER_FRAME_RIGHT_X, SLIDER_FRAME_LINE_NUMBER, 0x7f);
  
  //
  // draw the numberic value under the slider
  //
  lcdSetCursorXY(LCD_WIDTH_IN_PIXELS/2, SLIDER_FRAME_LINE_NUMBER+1);
  dtostrf(sliderDisplayFloatValue, 1, sliderDisplayDigitsRightOfDecimal, stringBuffer);
  lcdPrintStringCentered(stringBuffer, 12);
}


// ---------------------------------------------------------------------------------
//                            General display space functions  
// ---------------------------------------------------------------------------------

//
// clear the LCD "display space" the 5 lines above the button bar
//
void ArduinoUserInterface::clearDisplaySpace(void)
{
  int characterColumn;
 
  //
  // write enough blank pixels to clear the display
  //
  lcdSetCursorXY(0, 0);          
  for (characterColumn = 0; characterColumn < LCD_WIDTH_IN_PIXELS * (LCD_HEIGHT_IN_LINES - 1); characterColumn++)
    lcdWriteData(0x00);
  
  //
  // move cursor to the beginning
  //
  lcdSetCursorXY(0, 0);          
}


// ---------------------------------------------------------------------------------
//                                  ButtonBar functions  
// ---------------------------------------------------------------------------------

//
// values for: buttonsState
//
const byte WAITING_FOR_BUTTON_DOWN_STATE                 = 0;
const byte CONFIRM_BUTTON_DOWN_STATE                     = 1;
const byte WAITING_FOR_BUTTON_UP_STATE                   = 2;
const byte WAITING_FOR_BUTTON_UP_AFTER_AUTO_REPEAT_STATE = 3;
const byte WAITING_FOR_ALL_BUTTONS_UP_STATE              = 4;
const byte CONFIRM_ALL_BUTTONS_UP_STATE                  = 5;


//
// delay periods for dealing with buttons in milliseconds
//
const long BUTTON_DEBOUNCE_PERIOD = 30;
const long BUTTON_AUTO_REPEAT_DELAY = 800;
const long BUTTON_AUTO_REPEAT_RATE = 130;


//
// size and position constants
//
const byte BUTTON_WIDTH = 39;
const byte LEFT_BUTTON_CENTER_X = 20;
const byte RIGHT_BUTTON_CENTER_X = 65;


// ---------------------------------------------------------------------------------


//
// initialize the buttons
//
void ArduinoUserInterface::buttonsInitialize(void)
{
  buttonState = WAITING_FOR_BUTTON_DOWN_STATE;
}



//
// check for an event from any of the push buttons
//   Exit:  event value returned (sum of the button ID + the button event)
//           BUTTON_NO_EVENT returned if no event
//
byte ArduinoUserInterface::getButtonEvent(void)
{
  byte currentButtonID;
  unsigned long currentTime;


  //
  // check if no button is press now and no button has been pressed for a while
  //
  currentButtonID = readButtonsToGetButtonID();
  if ((buttonState == WAITING_FOR_BUTTON_DOWN_STATE) && (currentButtonID == BUTTON_ID_NONE))
    return(BUTTON_NO_EVENT);                       // nothing pressed, return no event


  //
  // something is going on, read the time
  //
  currentTime = millis();   

  
  //
  // check the state that the button was in last
  //
  switch(buttonState)
  {
    //
    // check if waiting for button to go down
    //
    case WAITING_FOR_BUTTON_DOWN_STATE:
    {
      //
      // a button has been pressed, remember it
      //
      buttonID = currentButtonID;
      buttonState = CONFIRM_BUTTON_DOWN_STATE;
      buttonEventStartTime = currentTime;
      break;                                          // return no event now
     } 


    //
    // we think a button has been press, let's double check
    //
    case CONFIRM_BUTTON_DOWN_STATE:
    {
      if (currentTime < (buttonEventStartTime + BUTTON_DEBOUNCE_PERIOD))
        break;                                        // wait until debounce period complete
  
      //
      // make sure the same button is pressed, as was pressed before
      //
      if (currentButtonID != buttonID)
      {
        buttonState = WAITING_FOR_BUTTON_DOWN_STATE;  // not the same button, start over
        break;                                        // return no event
      }

      //
      // a button has been pressed
      //
      buttonEventStartTime = currentTime;             // start auto repeat timer
      buttonState = WAITING_FOR_BUTTON_UP_STATE;  
      return(BUTTON_PUSHED_EVENT + buttonID);         // return button "pressed" event
    }


    //
    // check if waiting for button to go back up
    //      
    case WAITING_FOR_BUTTON_UP_STATE:
    {
      //
      // button has been down, check if it is still down
      //
      if (currentButtonID != buttonID)
      {
        //
        // nothing pressed now or not the same button pressed, go wait for all buttons up
        //
        buttonState = WAITING_FOR_ALL_BUTTONS_UP_STATE;
        break;                                   // return no event
      }
      
      //
      // the button is still down, check if time to auto repeat
      //
      if (currentTime < (buttonEventStartTime + BUTTON_AUTO_REPEAT_DELAY))
        break;                                   // no, return no event
                                           
      buttonEventStartTime = currentTime;        // yes auto repeat, reset auto repeat timer
      buttonState = WAITING_FOR_BUTTON_UP_AFTER_AUTO_REPEAT_STATE;
      return(BUTTON_REPEAT_EVENT + buttonID);    // return button "auto repeat" event
    }
     

    //
    // check if waiting for button to repeat
    //
    case WAITING_FOR_BUTTON_UP_AFTER_AUTO_REPEAT_STATE:
    {
      //
      // button has been down & auto repeat started, check if it is still down
      //
      if (currentButtonID != buttonID)
      {
        //
        // nothing pressed now or not the same button pressed, go wait for all buttons up
        //
        buttonState = WAITING_FOR_ALL_BUTTONS_UP_STATE;
        break;
      }
 
      //
      // button is still down, check if time to auto repeat
      //
      if (currentTime < (buttonEventStartTime + BUTTON_AUTO_REPEAT_RATE))
        break;                                        // return no event
                                           
      buttonEventStartTime = currentTime;             // yes, auto repeat, reset auto repeat timer
      return(BUTTON_REPEAT_EVENT + buttonID);         // return button "auto repeat" event
    }
    

    //
    // check if all the buttons are up now
    //
    case WAITING_FOR_ALL_BUTTONS_UP_STATE:
    {
      //
      // check if the button is up
      //
      if (currentButtonID != BUTTON_ID_NONE)
        break;                                         // all buttons not up, return no event
      
      //
      // the button is up
      //
      buttonState = CONFIRM_ALL_BUTTONS_UP_STATE;
      buttonEventStartTime = currentTime;
      break;                                           // return no event now
    }


    //
    // verify all buttons still up
    //
    case CONFIRM_ALL_BUTTONS_UP_STATE:
    {
      if (currentTime < (buttonEventStartTime + BUTTON_DEBOUNCE_PERIOD))
        break;                                           // delay period not up, return no event
  
      //
      // make sure no buttons are pressed
      //
      if (currentButtonID != BUTTON_ID_NONE)
      {
        buttonState = WAITING_FOR_ALL_BUTTONS_UP_STATE;  // something IS pressed so start over
        break;                                           // return no event
      }

      //
      // button has been released now
      //
      buttonState = WAITING_FOR_BUTTON_DOWN_STATE;       // cycle of button events complete
      return(BUTTON_RELEASED_EVENT + buttonID);          // return button "Up" event
    }
  }

  return(BUTTON_NO_EVENT);
}



//
// check if any buttons are pushed and return its button number
//  Exit: PUSH_BUTTON_DOWN or PUSH_BUTTON_UP
//          or PUSH_BUTTON_BACK or PUSH_BUTTON_NONE
//
byte ArduinoUserInterface::readButtonsToGetButtonID(void)
{
  int buttonAnalogValue;

  //
  // determine analog voltage readings based on the resistor ladder
  //
  const int DOWN_BUTTON_VALUE = 1024.0 * 0.658;
  const int UP_BUTTON_VALUE = 1024.0 * 0.476;
  const int BACK_BUTTON_VALUE = 1024.0 * 0.312;
  const int SELECT_BUTTON_VALUE = 1024.0 * 0.0;
  
  //
  // read the button voltage, then determine which button it is
  //
  buttonAnalogValue = analogRead(buttonAnalogPin);
  
  if (intInRange(buttonAnalogValue, DOWN_BUTTON_VALUE - 40, DOWN_BUTTON_VALUE + 40))
    return(BUTTON_ID_DOWN);

  if (intInRange(buttonAnalogValue, UP_BUTTON_VALUE - 40, UP_BUTTON_VALUE + 40))
    return(BUTTON_ID_UP);

  if (intInRange(buttonAnalogValue, BACK_BUTTON_VALUE - 40, BACK_BUTTON_VALUE + 40))
    return(BUTTON_ID_BACK);

  if (intInRange(buttonAnalogValue, SELECT_BUTTON_VALUE - 0, SELECT_BUTTON_VALUE + 80))
    return(BUTTON_ID_SELECT);

  return(BUTTON_ID_NONE);      
}



//
// test if a number is in the given range
//  Enter:  value = number to test
//          lowerValue = bottom of range (inclusive)
//          upperValue = top of range (inclusive)
//  Exit:   true return if value is in range
//
boolean ArduinoUserInterface::intInRange(int value, int lowerValue, int upperValue)
{
  if ((value <= upperValue)  && (value >= lowerValue))
    return(true);
  else
    return(false);
}



//
// draw the button bar
//  Enter:  leftButtonText -> text for left button, empty string will draw no 
//            button
//          rightButtonText -> text for right button, empty string will draw no 
//            button
//
void ArduinoUserInterface::drawButtonBar(char *leftButtonText, char *rightButtonText)
{
  byte leftButtonLeftX = 0;
  byte leftButtonRightX = leftButtonLeftX + BUTTON_WIDTH;
  byte rightButtonLeftX = RIGHT_BUTTON_CENTER_X - BUTTON_WIDTH/2;
  byte rightButtonRightX = rightButtonLeftX + BUTTON_WIDTH - 1;
  
  //
  // move cursor to the button bar line
  //
  lcdSetCursorXY(leftButtonLeftX, LCD_HEIGHT_IN_LINES - 1);

  //
  // draw the left button if not blank
  //
  if (leftButtonText[0] != 0)
    lcdPrintCenteredStringReverse(leftButtonText, LEFT_BUTTON_CENTER_X, BUTTON_WIDTH);
  
  //
  // draw white between the buttons
  //
  lcdFillToColumnX(rightButtonLeftX - 1, 0);

  //
  // draw the right button if not blank
  //
  if (rightButtonText[0] != 0)
    lcdPrintCenteredStringReverse(rightButtonText, RIGHT_BUTTON_CENTER_X, BUTTON_WIDTH);
  else
    lcdFillToEndOfLine(0);

  //
  // draw a line above the left button so upper case letters are covered
  //
  lcdSetCursorXY(leftButtonLeftX, LCD_HEIGHT_IN_LINES - 2);
  if (leftButtonText[0] != 0)
    lcdFillToColumnX(leftButtonRightX, 0x80);

  //
  // draw white space between buttons on line above
  //
  lcdFillToColumnX(rightButtonLeftX-1, 0);


  //
  // draw a line above the left button so upper case letters are covered
  //
  if (rightButtonText[0] != 0)
    lcdFillToColumnX(rightButtonRightX, 0x80);
  else
    lcdFillToEndOfLine(0);
}


// ---------------------------------------------------------------------------------
//                                    LCD functions  
// ---------------------------------------------------------------------------------


//
// constants for the LCD display
//
const int LCD_COMMAND_BYTE = LOW;
const int LCD_DATA_BYTE = HIGH;


//
// ASCII font, 5 x 8 pixels, this font is stored in program memory rather than RAM
//

const byte Font[][5] PROGMEM = 
  {
   {0x00, 0x00, 0x00, 0x00, 0x00},     // 20  
   {0x00, 0x00, 0x5f, 0x00, 0x00},     // 21 !
   {0x00, 0x07, 0x00, 0x07, 0x00},     // 22 "
   {0x14, 0x7f, 0x14, 0x7f, 0x14},     // 23 #
   {0x24, 0x2a, 0x7f, 0x2a, 0x12},     // 24 $
   {0x23, 0x13, 0x08, 0x64, 0x62},     // 25 %
   {0x36, 0x49, 0x55, 0x22, 0x50},     // 26 &
   {0x00, 0x05, 0x03, 0x00, 0x00},     // 27 '
   {0x00, 0x1c, 0x22, 0x41, 0x00},     // 28 (
   {0x00, 0x41, 0x22, 0x1c, 0x00},     // 29 )
   {0x14, 0x08, 0x3e, 0x08, 0x14},     // 2a *
   {0x08, 0x08, 0x3e, 0x08, 0x08},     // 2b +
   {0x00, 0x50, 0x30, 0x00, 0x00},     // 2c ,
   {0x08, 0x08, 0x08, 0x08, 0x08},     // 2d -
   {0x00, 0x60, 0x60, 0x00, 0x00},     // 2e .
   {0x20, 0x10, 0x08, 0x04, 0x02},     // 2f /
   {0x3e, 0x51, 0x49, 0x45, 0x3e},     // 30 0
   {0x00, 0x42, 0x7f, 0x40, 0x00},     // 31 1
   {0x42, 0x61, 0x51, 0x49, 0x46},     // 32 2
   {0x21, 0x41, 0x45, 0x4b, 0x31},     // 33 3
   {0x18, 0x14, 0x12, 0x7f, 0x10},     // 34 4
   {0x27, 0x45, 0x45, 0x45, 0x39},     // 35 5
   {0x3c, 0x4a, 0x49, 0x49, 0x30},     // 36 6
   {0x01, 0x71, 0x09, 0x05, 0x03},     // 37 7
   {0x36, 0x49, 0x49, 0x49, 0x36},     // 38 8
   {0x06, 0x49, 0x49, 0x29, 0x1e},     // 39 9
   {0x00, 0x36, 0x36, 0x00, 0x00},     // 3a :
   {0x00, 0x56, 0x36, 0x00, 0x00},     // 3b ;
   {0x08, 0x14, 0x22, 0x41, 0x00},     // 3c <
   {0x14, 0x14, 0x14, 0x14, 0x14},     // 3d =
   {0x00, 0x41, 0x22, 0x14, 0x08},     // 3e >
   {0x02, 0x01, 0x51, 0x09, 0x06},     // 3f ?
   {0x32, 0x49, 0x79, 0x41, 0x3e},     // 40 @
   {0x7e, 0x11, 0x11, 0x11, 0x7e},     // 41 A
   {0x7f, 0x49, 0x49, 0x49, 0x36},     // 42 B
   {0x3e, 0x41, 0x41, 0x41, 0x22},     // 43 C
   {0x7f, 0x41, 0x41, 0x22, 0x1c},     // 44 D
   {0x7f, 0x49, 0x49, 0x49, 0x41},     // 45 E
   {0x7f, 0x09, 0x09, 0x09, 0x01},     // 46 F
   {0x3e, 0x41, 0x49, 0x49, 0x7a},     // 47 G
   {0x7f, 0x08, 0x08, 0x08, 0x7f},     // 48 H
   {0x00, 0x41, 0x7f, 0x41, 0x00},     // 49 I
   {0x20, 0x40, 0x41, 0x3f, 0x01},     // 4a J
   {0x7f, 0x08, 0x14, 0x22, 0x41},     // 4b K
   {0x7f, 0x40, 0x40, 0x40, 0x40},     // 4c L
   {0x7f, 0x02, 0x0c, 0x02, 0x7f},     // 4d M
   {0x7f, 0x04, 0x08, 0x10, 0x7f},     // 4e N
   {0x3e, 0x41, 0x41, 0x41, 0x3e},     // 4f O
   {0x7f, 0x09, 0x09, 0x09, 0x06},     // 50 P
   {0x3e, 0x41, 0x51, 0x21, 0x5e},     // 51 Q
   {0x7f, 0x09, 0x19, 0x29, 0x46},     // 52 R
   {0x46, 0x49, 0x49, 0x49, 0x31},     // 53 S
   {0x01, 0x01, 0x7f, 0x01, 0x01},     // 54 T
   {0x3f, 0x40, 0x40, 0x40, 0x3f},     // 55 U
   {0x1f, 0x20, 0x40, 0x20, 0x1f},     // 56 V
   {0x3f, 0x40, 0x38, 0x40, 0x3f},     // 57 W
   {0x63, 0x14, 0x08, 0x14, 0x63},     // 58 X
   {0x07, 0x08, 0x70, 0x08, 0x07},     // 59 Y
   {0x61, 0x51, 0x49, 0x45, 0x43},     // 5a Z
   {0x00, 0x7f, 0x41, 0x41, 0x00},     // 5b [
   {0x02, 0x04, 0x08, 0x10, 0x20},     // 5c 
   {0x00, 0x41, 0x41, 0x7f, 0x00},     // 5d ]
   {0x04, 0x02, 0x01, 0x02, 0x04},     // 5e ^
   {0x40, 0x40, 0x40, 0x40, 0x40},     // 5f _
   {0x00, 0x01, 0x02, 0x04, 0x00},     // 60 `
   {0x20, 0x54, 0x54, 0x54, 0x78},     // 61 a
   {0x7f, 0x48, 0x44, 0x44, 0x38},     // 62 b
   {0x38, 0x44, 0x44, 0x44, 0x20},     // 63 c
   {0x38, 0x44, 0x44, 0x48, 0x7f},     // 64 d
   {0x38, 0x54, 0x54, 0x54, 0x18},     // 65 e
   {0x08, 0x7e, 0x09, 0x01, 0x02},     // 66 f
   {0x0c, 0x52, 0x52, 0x52, 0x3e},     // 67 g
   {0x7f, 0x08, 0x04, 0x04, 0x78},     // 68 h
   {0x00, 0x44, 0x7d, 0x40, 0x00},     // 69 i
   {0x20, 0x40, 0x44, 0x3d, 0x00},     // 6a j 
   {0x7f, 0x10, 0x28, 0x44, 0x00},     // 6b k
   {0x00, 0x41, 0x7f, 0x40, 0x00},     // 6c l
   {0x7c, 0x04, 0x18, 0x04, 0x78},     // 6d m
   {0x7c, 0x08, 0x04, 0x04, 0x78},     // 6e n
   {0x38, 0x44, 0x44, 0x44, 0x38},     // 6f o
   {0x7c, 0x14, 0x14, 0x14, 0x08},     // 70 p
   {0x08, 0x14, 0x14, 0x18, 0x7c},     // 71 q
   {0x7c, 0x08, 0x04, 0x04, 0x08},     // 72 r
   {0x48, 0x54, 0x54, 0x54, 0x20},     // 73 s
   {0x04, 0x3f, 0x44, 0x40, 0x20},     // 74 t
   {0x3c, 0x40, 0x40, 0x20, 0x7c},     // 75 u
   {0x1c, 0x20, 0x40, 0x20, 0x1c},     // 76 v
   {0x3c, 0x40, 0x30, 0x40, 0x3c},     // 77 w
   {0x44, 0x28, 0x10, 0x28, 0x44},     // 78 x
   {0x0c, 0x50, 0x50, 0x50, 0x3c},     // 79 y
   {0x44, 0x64, 0x54, 0x4c, 0x44},     // 7a z
   {0x00, 0x08, 0x36, 0x41, 0x00},     // 7b {
   {0x00, 0x00, 0x7f, 0x00, 0x00},     // 7c |
   {0x00, 0x41, 0x36, 0x08, 0x00},     // 7d }
   {0x10, 0x08, 0x08, 0x10, 0x08},     // 7e ~
   {0x78, 0x46, 0x41, 0x46, 0x78},     // 7f 
   {0x3c, 0x3c, 0x18, 0x18, 0x00}      // 80 right pointing arrow 
  };


// ---------------------------------------------------------------------------------

//
// initialize the LCD display
//
void ArduinoUserInterface::lcdInitialise(void)
{
  //
  // setup the IO pins
  //
  digitalWrite(lcdChipEnablePin, HIGH);
  pinMode(lcdChipEnablePin, OUTPUT);
  digitalWrite(lcdChipEnablePin, HIGH);

  pinMode(lcdDataControlPin, OUTPUT);
  pinMode(lcdDataInPin, OUTPUT);

  pinMode(lcdClockPin, OUTPUT);
  digitalWrite(lcdClockPin, LOW);

  
  //
  // configure the display
  //
  lcdWriteCommand(0x00);      // NOP
  lcdWriteCommand(0x21);      // set H bit to program config registers
  lcdWriteCommand(0xC0);      // set LCD Vop (Contrast) (0x80 - 0xff)
  lcdWriteCommand(0x06);      // set LCD temp coefficent (0x04 - 0x07)
  lcdWriteCommand(0x14);      // set LCD bias mode to 1:48 (0x10 - 0x17)
  lcdWriteCommand(0x20);      // clear H bit to access X & Y registers
  lcdWriteCommand(0x0C);      // set "normal mode"

  lcdCursorColumnX = 0;       // remember the LCD cursor's current position
  lcdCursorRowY = 0;  
}



//
// print a signed int at location of the cursor
//  Enter:  n = signed number to print 
//
void ArduinoUserInterface::lcdPrintInt(int n)
{
  char stringBuffer[7];
  byte numberOfDigits;
  
  itoa(n, stringBuffer, 10);
  lcdPrintString(stringBuffer);
}



//
// print a signed int at location of the cursor, left justified, optional white 
// space will be added after the digits to pad to the given width
//  Enter:  n = signed number to print 
//          padToNumberOfDigits = total width of the number (in characters)  
//            including white space after the digits (0 to 6)
//
void ArduinoUserInterface::lcdPrintIntLeftJustified(int n, int padToNumberOfDigits)
{
  char stringBuffer[7];
  
  //
  // print the number
  //
  itoa(n, stringBuffer, 10);
  lcdPrintStringLeftJustified(stringBuffer, padToNumberOfDigits);
}



//
// print a signed int at location of the cursor, right justified, optional white 
// space will be added before the digits to pad to the given width
//  Enter:  n = signed number to print 
//          padToNumberOfDigits = total width of the number (in characters)  
//            including white space after the digits (0 to 6)
//
void ArduinoUserInterface::lcdPrintIntRightJustified(int n, int padToNumberOfDigits)
{
  char stringBuffer[7];
  
  //
  // conver the number to a string
  //
  itoa(n, stringBuffer, 10);
  lcdPrintStringRightJustified(stringBuffer, padToNumberOfDigits);
}



//
// print a signed int at location of the cursor, center, optional white 
// space will be added before and after the digits to pad to the given width
//  Enter:  n = signed number to print 
//          padToNumberOfDigits = total width of the number (in characters)  
//            including white space after the digits (0 to 6)
//
void ArduinoUserInterface::lcdPrintIntCentered(int n, int padToNumberOfDigits)
{
  char stringBuffer[7];
  
  //
  // conver the number to a string
  //
  itoa(n, stringBuffer, 10);
  lcdPrintStringCentered(stringBuffer, padToNumberOfDigits);
}



//
// print a string to the LCD display
//  Enter:  s -> a null terminated string 
//
void ArduinoUserInterface::lcdPrintString(char *s)
{
  //
  // loop, writing one character at a time until the end of the string is reached
  //
  while (*s)
  {
    lcdPrintCharacter(*s++);
  }
}



//
// print a string at location of the cursor, left justified, optional white 
// space will be added after the string to pad to the given character width
//  Enter:  s -> string to print 
//          padToNumberOfCharacters = total width of printed string (in characters)  
//            including white space after the string (0 to 13)
//
void ArduinoUserInterface::lcdPrintStringLeftJustified(char *s, 
                            int padToNumberOfCharacters)
{
  byte numberOfCharacters;
  
  //
  // print the string
  //
  lcdPrintString(s);
  
  //
  // add optional padding after the string
  //
  numberOfCharacters = strlen(s);
  if (padToNumberOfCharacters > numberOfCharacters)
    lcdFillToColumnX(lcdCursorColumnX + ((padToNumberOfCharacters - numberOfCharacters) * 6), 0x00);
}



//
// print a string at location of the cursor, right justified, optional white 
// space will be added before the string to pad to the given width
//  Enter:  s -> string to print 
//          padToNumberOfCharacters = total width of printed string (in characters)  
//            including white space after the string (0 to 13)
//
void ArduinoUserInterface::lcdPrintStringRightJustified(char *s, 
                              int padToNumberOfCharacters)
{
  byte numberOfCharacters;
  int newCursorColumnX;
  
  numberOfCharacters = strlen(s);
  
  if (padToNumberOfCharacters > numberOfCharacters)
  {
    newCursorColumnX = lcdCursorColumnX - (padToNumberOfCharacters * 6);
    
    if (newCursorColumnX < 0)
      newCursorColumnX = 0;
      
    lcdSetCursorXY(newCursorColumnX, lcdCursorRowY);
      
    lcdFillToColumnX(lcdCursorColumnX + ((padToNumberOfCharacters - numberOfCharacters) * 6), 0x00);
    
    lcdPrintString(s);
  }
  
  else
  {
    newCursorColumnX = lcdCursorColumnX - (numberOfCharacters * 6);
    
    if (newCursorColumnX < 0)
      newCursorColumnX = 0;
      
    lcdSetCursorXY(newCursorColumnX, lcdCursorRowY);
    
    lcdPrintString(s);    
  }
}



//
// print a string at location of the cursor, center, optional white 
// space will be added before and after the string to pad to the given width
//  Enter:  s -> string to print 
//          padToNumberOfCharacters = total width of printed string (in   
//            characters) including white space after the string (0 to 13)
//
void ArduinoUserInterface::lcdPrintStringCentered(char *s, int padToNumberOfCharacters)
{
  byte numberOfCharacters;
  int newCursorColumnX;
  byte pixelsOfPadding;
  
  numberOfCharacters = strlen(s);
  
  if (padToNumberOfCharacters > numberOfCharacters)
  {
    newCursorColumnX = lcdCursorColumnX - (padToNumberOfCharacters * 3);
    
    if (newCursorColumnX < 0)
      newCursorColumnX = 0;
      
    lcdSetCursorXY(newCursorColumnX, lcdCursorRowY);
    
    pixelsOfPadding = (padToNumberOfCharacters - numberOfCharacters) * 3;
    lcdFillToColumnX(lcdCursorColumnX + pixelsOfPadding, 0x00);
    
    lcdPrintString(s);

    lcdFillToColumnX(lcdCursorColumnX + pixelsOfPadding, 0x00);
  }
  
  else
  {
    newCursorColumnX = lcdCursorColumnX - (numberOfCharacters * 3);
    
    if (newCursorColumnX < 0)
      newCursorColumnX = 0;
      
    lcdSetCursorXY(newCursorColumnX, lcdCursorRowY);
    
    lcdPrintString(s);    
  }
}



//
// print a string in reverse video to the LCD with centering on the current line
//  Enter:  s -> a null terminated string 
//          X = X coord to the center position to print (0 - 83)
//          padToWidth = desired with of reverse video string, black padding will
//            be added on each side of the string if needed, set to zero to add no 
//            padding
//
void ArduinoUserInterface::lcdPrintCenteredStringReverse(char *s, byte X, 
                               byte padToWidth)
{
  int startingX;
  byte stringWidthInPixels;
  
  stringWidthInPixels = lcdStringWidthInPixels(s);

  //
  // determine padding needed on right and left sides of string
  //
  if (padToWidth > stringWidthInPixels)
  {
    //
    // move the cursor to the beginning of the printing
    //
    startingX = X - ((padToWidth / 2) + (padToWidth % 2));
    lcdSetCursorXY(startingX, lcdCursorRowY);  
    
    //
    // add black padding up to the beginning of the string
    //
    lcdFillToColumnX(X - (stringWidthInPixels / 2), 0xff);

    //
    // print the string
    //
    lcdPrintStringReverse(s);

    //
    // add black padding at the end of the string
    //
    lcdFillToColumnX(startingX + padToWidth, 0xff);
  }
  
  else
  {
    //
    // move the cursor to the beginning of the printing
    //
    lcdSetCursorXY(X - (stringWidthInPixels/2), lcdCursorRowY);  

    //
    // print the string
    //
    lcdPrintStringReverse(s);
  }
}



//
// print a string to the LCD display in reverse video
//  Enter:  s -> a null terminated string 
//
void ArduinoUserInterface::lcdPrintStringReverse(char *s)
{
  //
  // loop, writing one character at a time until the end of the string is reached
  //
  while (*s)
  {
    lcdPrintCharacterReverse(*s++);
  }
}



//
// print one ASCII charater to the display
//  Enter:  c = character to display
//
void ArduinoUserInterface::lcdPrintCharacter(byte character)
{
  int pixelColumn;
  
  //
  // make sure character is in range of the font table
  //
  if ((character < 0x20) || (character > 0x80))
    character = 0x20;
  
  //
  // from the character, get the index into the font table
  //
  character -= 0x20;
  
  //
  // write all 5 columns of the character
  //
  for (pixelColumn = 0; pixelColumn < 5; pixelColumn++)
  {
    lcdWriteData(pgm_read_byte(&Font[character][pixelColumn]));   
  }
  
  //
  // write a column of blank pixels after the character
  //
  lcdWriteData(0x00);
  
  //
  // move the cursor forward by one character
  //
  lcdCursorColumnX += 6;
}



//
// print one ASCII charater to the display in reverse video
//  Enter:  c = character to display
//
void ArduinoUserInterface::lcdPrintCharacterReverse(byte character)
{
  int pixelColumn;
  byte lineOfPixels;
  
  //
  // make sure character is in range of the font table
  //
  if ((character < 0x20) || (character > 0x80))
    character = 0x20;
  
  //
  // from the character, get the index into the font table
  //
  character -= 0x20;
  
  //
  // write all 5 columns of the character
  //
  for (pixelColumn = 0; pixelColumn < 5; pixelColumn++)
  {
    lineOfPixels = pgm_read_byte(&Font[character][pixelColumn]);
    lineOfPixels = ~lineOfPixels;
    lcdWriteData(lineOfPixels);   
  }
    
  //
  // write a column of blank pixels after the character
  //
  lcdWriteData(0xff);
  
  //
  // move the cursor forward by one character
  //
  lcdCursorColumnX += 6;
}



//
// determine the width of a string in pixels
//
byte ArduinoUserInterface::lcdStringWidthInPixels(char *s)
{
  return(strlen(s) * 6);
}



//
// clear the LCD display by writing blank pixels
//
void ArduinoUserInterface::lcdClearDisplay(void)
{
  int characterColumn;
  int lastCharacterColumn;  
  
  //
  // move cursor to the beginning
  //
  lcdSetCursorXY(0, 0);          

  //
  // determine how many bytes must be written to clear the display
  //
  lastCharacterColumn = LCD_WIDTH_IN_PIXELS * LCD_HEIGHT_IN_PIXELS / 8;
  
  //
  // write enough blank pixels to clear the display
  //
  for (characterColumn = 0; characterColumn < lastCharacterColumn; characterColumn++)
  {
    lcdWriteData(0x00);
  }
  
  //
  // move cursor to the beginning
  //
  lcdSetCursorXY(0, 0);          
}



//
// fill current line, beginning at the cursor location with a repeating column 
// of 8 pixels up to the X column
//          X = fill up to, but not including this column, the cursor will 
//              then be at this column
//          byteOfPixels = byte of pixels to repeat
//
void ArduinoUserInterface::lcdFillToColumnX(int X, byte byteOfPixels)
{
  X--;
  if(X < 0)
    return;
    
  lcdDrawRowOfPixels(lcdCursorColumnX, X, lcdCursorRowY, byteOfPixels);
}



//
// fill current line, beginning at the cursor location to the end of the line, 
// with a repeating column of 8 pixels
//          byteOfPixels = byte of pixels to repeat
//
void ArduinoUserInterface::lcdFillToEndOfLine(byte byteOfPixels)
{
  lcdDrawRowOfPixels(lcdCursorColumnX, LCD_LAST_COLUMN_X, lcdCursorRowY, byteOfPixels);
}



//
// draw a row of pixels, repeating the same column of 8 pixels across
//  Enter:  X1 = starting pixel column (0 - 83, 0 = left most column)
//          X2 = ending pixel column (0 - 83)
//          lineNumber = character line (0 - 5, 0 = top row)
//          byteOfPixels = byte of pixels to repeat
//
void ArduinoUserInterface::lcdDrawRowOfPixels(int X1, int X2, int lineNumber, 
                                              byte byteOfPixels)
{
  int pixelCounter;
  int pixelCount;

  //
  // move to the first pixel
  //
  lcdSetCursorXY(X1, lineNumber);


  //
  // write the pixels
  //
  pixelCount = X2 - X1 + 1;
  if(pixelCount <= 0)
    return;
  
  for (pixelCounter = 0; pixelCounter < pixelCount; pixelCounter++)
  {
    lcdWriteData(byteOfPixels);   
  }

  //
  // remember the cursor location
  //
  lcdCursorColumnX += pixelCount;
}



//
// set the coords where the next character will be written to the LCD display
//  Enter:  column = pixel column (0 - 83, 0 = left most column)
//          lineNumber = character line (0 - 5, 0 = top row)
//
void ArduinoUserInterface::lcdSetCursorXY(int column, int lineNumber)
{
  lcdWriteCommand(0x80 | column);
  lcdWriteCommand(0x40 | lineNumber);
  
  lcdCursorColumnX = column;        // remember the LCD's cursor position
  lcdCursorRowY = lineNumber; 
}



//
// set the LCD screen contrast value 
//  Enter:  contrastValue = value to set for the screen's contrast (0 - 127)
//
void ArduinoUserInterface::lcdSetContrast(int contrastValue)
{
  lcdWriteCommand(0x21);                      // set H bit to program config registers
  lcdWriteCommand(contrastValue | 0x80);      // set LCD Vop (Contrast) (0x80 - 0xff)
  lcdWriteCommand(0x20);                      // clear H bit to access X & Y registers
}



//
// write a single command byte to the LCD display
//
void ArduinoUserInterface::lcdWriteCommand(byte command)
{
  digitalWrite(lcdDataControlPin, LCD_COMMAND_BYTE);
  digitalWrite(lcdChipEnablePin, LOW);
  lcdSerialOut(command);
  digitalWrite(lcdChipEnablePin, HIGH);
}



//
// write a single data byte to the LCD display
//
void ArduinoUserInterface::lcdWriteData(byte data)
{
  digitalWrite(lcdDataControlPin, LCD_DATA_BYTE);
  digitalWrite(lcdChipEnablePin, LOW);
  lcdSerialOut(data);
  digitalWrite(lcdChipEnablePin, HIGH);
}



//
// serially transmit the data byte
//
void ArduinoUserInterface::lcdSerialOut(byte data)
{
  byte bitCount = 8;

  //
  // transmit 8 bits
  //
  do
  {
    //
    // write the MSB bit
    //
    if (data & 0x80)
      digitalWrite(lcdDataInPin, HIGH);
    else
      digitalWrite(lcdDataInPin, LOW);

    //
    // set the clock high
    //
    digitalWrite(lcdClockPin, HIGH);

    //
    // move the next bit into the MSB position
    //
    data = data << 1;
    bitCount--;

    //
    // set the clock low
    //
    digitalWrite(lcdClockPin, LOW);
  } while(bitCount != 0);
}


// ---------------------------------------------------------------------------------
//                                   EEPROM functions
// ---------------------------------------------------------------------------------

//
// write a configuration byte to the EEPROM
//  Enter:  EEPromAddress = address in EEPROM to write 
//          value = 8 bit value to write to EEPROM
//          note: 2 bytes of EEPROM space are used 
//
void ArduinoUserInterface::writeConfigurationByte(int EEPromAddress, byte value)
{
  if (EEPROM.read(EEPromAddress) == 0xff)
    EEPROM.write(EEPromAddress, 0);
	
  EEPROM.write(EEPromAddress + 1, value);
}



//
// read a configuration byte from the EEPROM
//  Enter:  EEPromAddress = address in EEPROM to read from 
//          defaultValue = default value to return if value has never been 
//            written to the EEPROM
//          note: 2 bytes of EEPROM space are used 
//  Exit:   byte value from EEPROM (or default value) returned
//
byte ArduinoUserInterface::readConfigurationByte(int EEPromAddress, byte defaultValue)
{
  if (EEPROM.read(EEPromAddress) == 0xff)
    return(defaultValue);
	
    return(EEPROM.read(EEPromAddress + 1));
}



//
// write a configuration int to the EEPROM
//  Enter:  EEPromAddress = address in EEPROM to write 
//          value = 16 bit value to write to EEPROM
//          note: 3 bytes of EEPROM space are used 
//
void ArduinoUserInterface::writeConfigurationInt(int EEPromAddress, int value)
{
  if (EEPROM.read(EEPromAddress) == 0xff)
    EEPROM.write(EEPromAddress, 0);
	
  EEPROM.write(EEPromAddress + 1, (byte)((unsigned int)value & 0xFF));
  EEPROM.write(EEPromAddress + 2, (byte)((unsigned int)value >> 8));
}



//
// read a configuration int from the EEPROM
//  Enter:  EEPromAddress = address in EEPROM to read from 
//          defaultValue = default value to return if value has never been 
//            written to the EEPROM
//          note: 3 bytes of EEPROM space are used 
//  Exit:   int value from EEPROM (or default value) returned
//
int ArduinoUserInterface::readConfigurationInt(int EEPromAddress, int defaultValue)
{
  unsigned int i;
	
  if (EEPROM.read(EEPromAddress) == 0xff)
    return(defaultValue);
		
  i = EEPROM.read(EEPromAddress + 1);
  i += ((unsigned int) (EEPROM.read(EEPromAddress + 2)) << 8);
	
  return((int) i);
}



//
// write a configuration long to the EEPROM
//  Enter:  EEPromAddress = address in EEPROM to write 
//          value = 16 bit value to write to EEPROM
//          note: 5 bytes of EEPROM space are used 
//
void ArduinoUserInterface::writeConfigurationLong(int EEPromAddress, long value)
{
  if (EEPROM.read(EEPromAddress) == 0xff)
    EEPROM.write(EEPromAddress, 0);
	
  EEPROM.write(EEPromAddress + 1, (byte)((unsigned long)value & 0xFF));
  
  value = value >> 8;
  EEPROM.write(EEPromAddress + 2, (byte)((unsigned long)value & 0xFF));
  
  value = value >> 8;
  EEPROM.write(EEPromAddress + 3, (byte)((unsigned long)value & 0xFF));
  
  value = value >> 8;
  EEPROM.write(EEPromAddress + 4, (byte)((unsigned long)value & 0xFF));
}



//
// read a configuration long from the EEPROM
//  Enter:  EEPromAddress = address in EEPROM to read from 
//          defaultValue = default value to return if value has never been 
//            written to the EEPROM
//          note: 5 bytes of EEPROM space are used 
//  Exit:   long value from EEPROM (or default value) returned
//
long ArduinoUserInterface::readConfigurationLong(int EEPromAddress, long defaultValue)
{
  unsigned long i;
	
  if (EEPROM.read(EEPromAddress) == 0xff)
    return(defaultValue);
		
  i = EEPROM.read(EEPromAddress + 1);
  i += ((unsigned long) (EEPROM.read(EEPromAddress + 2)) << 8);
  i += ((unsigned long) (EEPROM.read(EEPromAddress + 3)) << 16);
  i += ((unsigned long) (EEPROM.read(EEPromAddress + 4)) << 24);
	
  return((long) i);
}


// -------------------------------------- End --------------------------------------


