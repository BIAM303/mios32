// $Id$
/*
 * Universal LCD driver which supports various character and graphical LCDs
 *
 * See mios32_lcd_type_t in mios32_lcd.h for list of LCDs
 *
 * The LCD type will be fetched from the Bootloader Info section after reset.
 * It can be changed with the bootloader update application.
 *
 * Optionally it's also possible to change the type during runtime with
 * MIOS32_LCD_TypeSet (e.g. after it has been loaded from a config file from
 * SD Card)
 *
 * This driver has to be selected with MIOS32_LCD environment variable set
 * to "universal"
 *
 * Please only add drivers which are not resource hungry (e.g. they shouldn't
 * allocate RAM for buffering) - otherwise all applications would be affected!
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <mios32.h>
#include <glcd_font.h>

#include "app_lcd.h"


/////////////////////////////////////////////////////////////////////////////
// temporary - will be removed soon once TK updates his OLED board!
/////////////////////////////////////////////////////////////////////////////

#ifndef APP_LCD_USE_J10_FOR_CS
#define APP_LCD_USE_J10_FOR_CS 1
#endif


/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

static u32 display_available = 0;


/////////////////////////////////////////////////////////////////////////////
// Initializes application specific LCD driver
// IN: <mode>: optional configuration
// OUT: returns < 0 if initialisation failed
/////////////////////////////////////////////////////////////////////////////
s32 APP_LCD_Init(u32 mode)
{
  // currently only mode 0 supported
  if( mode != 0 )
    return -1; // unsupported mode

  // enable display by default
  display_available |= (1 << mios32_lcd_device);

  switch( mios32_lcd_parameters.lcd_type ) {
  case MIOS32_LCD_TYPE_GLCD_KS0108:
  case MIOS32_LCD_TYPE_GLCD_KS0108_INVCS: {

  } break;

  case MIOS32_LCD_TYPE_GLCD_DOG: {
    // DOGM128 works at 3.3V, level shifting (and open drain mode) not required
    if( MIOS32_BOARD_J15_PortInit(0) < 0 )
      return -2; // failed to initialize J15

    // initialize LCD
    MIOS32_DELAY_Wait_uS(50000); // exact 50 mS delay

    // initialisation sequence based on EA-DOGL/M datasheet
  
    APP_LCD_Cmd(0x40); //2 - Display start line = 0
    APP_LCD_Cmd(0xA1); //8 - ADC Normal mode = 0 
    APP_LCD_Cmd(0xC0); //15 - COMS normal = 1/65  duty
    APP_LCD_Cmd(0xA6); //9 - Display  = normal  
    APP_LCD_Cmd(0xA2); //11 - 1/65 duty 1/9 bias for 65x132 display
    APP_LCD_Cmd(0x2F); //16  - Power control set = B.,R,F all ON
    APP_LCD_Cmd(0xF8); //20-1 - select Booster ratio set
    APP_LCD_Cmd(0x00); //20-2 - Booster ratio register (must be preceeded by 20-1)
    APP_LCD_Cmd(0x27); //17 - VO volt reg set 
    APP_LCD_Cmd(0x81); //18-1 - Elect vol control - contrast
#if 0
    APP_LCD_Cmd(0x16); //18-2 - Contrast level dec 22	
#else
    APP_LCD_Cmd(0x10); //18-2 - Contrast level dec 16
#endif
    APP_LCD_Cmd(0xAC); //19-1 - Static Indicator - set off
    APP_LCD_Cmd(0x00); //19-2 - No Indicator
    APP_LCD_Cmd(0xAF); //20 - Display ON

  } break;

  case MIOS32_LCD_TYPE_GLCD_SSD1306: {
    // the OLED works at 3.3V, level shifting (and open drain mode) not required
    if( MIOS32_BOARD_J15_PortInit(0) < 0 )
      return -2; // failed to initialize J15

#if APP_LCD_USE_J10_FOR_CS
    int pin;
    for(pin=0; pin<8; ++pin)
      MIOS32_BOARD_J10_PinInit(pin, MIOS32_BOARD_PIN_MODE_OUTPUT_PP);
#endif

    // initialize LCD
    APP_LCD_Cmd(0xa8); // Set MUX Ratio
    APP_LCD_Cmd(0x3f);

    APP_LCD_Cmd(0xd3); // Set Display Offset
    APP_LCD_Cmd(0x00);

    APP_LCD_Cmd(0x40); // Set Display Start Line

    APP_LCD_Cmd(0xa0); // Set Segment re-map

    APP_LCD_Cmd(0xc0); // Set COM Output Scan Direction

    APP_LCD_Cmd(0xda); // Set COM Pins hardware configuration
    APP_LCD_Cmd(0x12);

    APP_LCD_Cmd(0x81); // Set Contrast Control
    APP_LCD_Cmd(0x7f); // middle

    APP_LCD_Cmd(0xa4); // Disable Entiere Display On

    APP_LCD_Cmd(0xa6); // Set Normal Display

    APP_LCD_Cmd(0xd5); // Set OSC Frequency
    APP_LCD_Cmd(0x80);

    APP_LCD_Cmd(0x8d); // Enable charge pump regulator
    APP_LCD_Cmd(0x14);

    APP_LCD_Cmd(0xaf); // Display On

    APP_LCD_Cmd(0x20); // Enable Page mode
    APP_LCD_Cmd(0x02);
  } break;

  case MIOS32_LCD_TYPE_CLCD:
  case MIOS32_LCD_TYPE_CLCD_DOG:
  default: {
    // 0: J15 pins are configured in Push Pull Mode (3.3V)
    // 1: J15 pins are configured in Open Drain mode (perfect for 3.3V->5V levelshifting)

    if( mios32_lcd_parameters.lcd_type == MIOS32_LCD_TYPE_CLCD_DOG ) {
      // DOG CLCD works at 3.3V, level shifting (and open drain mode) not required
      if( MIOS32_BOARD_J15_PortInit(0) < 0 )
	return -2; // failed to initialize J15
    } else {
      if( MIOS32_BOARD_J15_PortInit(1) < 0 )
	return -2; // failed to initialize J15
    }

    // initialize LCD
    MIOS32_BOARD_J15_DataSet(0x38);
    MIOS32_BOARD_J15_RS_Set(0);
    MIOS32_BOARD_J15_E_Set(mios32_lcd_device, 1);
    MIOS32_BOARD_J15_E_Set(mios32_lcd_device, 0);
    MIOS32_BOARD_J15_RW_Set(0);
    MIOS32_DELAY_Wait_uS(50000);

    MIOS32_BOARD_J15_E_Set(mios32_lcd_device, 1);
    MIOS32_BOARD_J15_E_Set(mios32_lcd_device, 0);
    MIOS32_DELAY_Wait_uS(50000);

    MIOS32_BOARD_J15_E_Set(mios32_lcd_device, 1);
    MIOS32_BOARD_J15_E_Set(mios32_lcd_device, 0);
    MIOS32_DELAY_Wait_uS(50000);

    APP_LCD_Cmd(0x08); // Display Off
    APP_LCD_Cmd(0x0c); // Display On
    APP_LCD_Cmd(0x06); // Entry Mode
    APP_LCD_Cmd(0x01); // Clear Display
    MIOS32_DELAY_Wait_uS(50000);

    // for DOG displays: perform additional display initialisation
    if( mios32_lcd_parameters.lcd_type == MIOS32_LCD_TYPE_CLCD_DOG ) {
      APP_LCD_Cmd(0x39); // 8bit interface, switch to instruction table 1
      APP_LCD_Cmd(0x1d); // BS: 1/4, 3 line LCD
      APP_LCD_Cmd(0x50); // Booster off, set contrast C5/C4
      APP_LCD_Cmd(0x6c); // set Voltage follower and amplifier
      APP_LCD_Cmd(0x7c); // set contrast C3/C2/C1
      //  APP_LCD_Cmd(0x38); // back to instruction table 0
      // (will be done below)

      // modify cursor mapping, so that it complies with 3-line dog displays
      u8 cursor_map[] = {0x00, 0x10, 0x20, 0x30}; // offset line 0/1/2/3
      MIOS32_LCD_CursorMapSet(cursor_map);
    }

    APP_LCD_Cmd(0x38); // experience from PIC based MIOS: without these lines
    APP_LCD_Cmd(0x0c); // the LCD won't work correctly after a second APP_LCD_Init
  }
  }

  return (display_available & (1 << mios32_lcd_device)) ? 0 : -1; // return -1 if display not available
}


/////////////////////////////////////////////////////////////////////////////
// Sends data byte to LCD
// IN: data byte in <data>
// OUT: returns < 0 if display not available or timed out
/////////////////////////////////////////////////////////////////////////////
s32 APP_LCD_Data(u8 data)
{
  // check if if display already has been disabled
  if( !(display_available & (1 << mios32_lcd_device)) )
    return -1;

  switch( mios32_lcd_parameters.lcd_type ) {
  case MIOS32_LCD_TYPE_GLCD_KS0108:
  case MIOS32_LCD_TYPE_GLCD_KS0108_INVCS: {
  } break;

  case MIOS32_LCD_TYPE_GLCD_DOG: {
    // select LCD depending on current cursor position
    u8 line = 0;
    if( mios32_lcd_y >= 3*mios32_lcd_parameters.height )
      line = 3;
    else if( mios32_lcd_y >= 2*mios32_lcd_parameters.height )
      line = 2;
    else if( mios32_lcd_y >= 1*mios32_lcd_parameters.height )
      line = 1;

    u8 row = 0;
    if( mios32_lcd_x >= 1*mios32_lcd_parameters.height )
      row = 1;

    u8 cs = 2*line + row;

    if( cs >= 8 )
      return -1; // invalid CS line

    // chip select and DC
    MIOS32_BOARD_J15_DataSet(~(1 << cs));
    MIOS32_BOARD_J15_RS_Set(1); // RS pin used to control DC

    // send data
    MIOS32_BOARD_J15_SerDataShift(data);

    // increment graphical cursor
    ++mios32_lcd_x;
    // if end of display segment reached: set X position of all segments to 0
    if( (mios32_lcd_x % mios32_lcd_parameters.width) == 0 ) {
      APP_LCD_Cmd(0x10); // Set upper nibble to 0
      return APP_LCD_Cmd(0x00); // Set lower nibble to 0
    }

    return 0; // no error
  } break;

  case MIOS32_LCD_TYPE_GLCD_SSD1306: {
    // select LCD depending on current cursor position
    // THIS PART COULD BE CHANGED TO ARRANGE THE 8 DISPLAYS ON ANOTHER WAY
    u8 line = mios32_lcd_y / mios32_lcd_parameters.height;
    u8 row = (mios32_lcd_x % (2*mios32_lcd_parameters.width)) / mios32_lcd_parameters.width;

    u8 cs = 2*line + row;

    if( cs >= 8 )
      return -1; // invalid CS line

    // chip select and DC
#if APP_LCD_USE_J10_FOR_CS
    MIOS32_BOARD_J10_Set(~(1 << cs));
#else
    MIOS32_BOARD_J15_DataSet(~(1 << cs));
#endif
    MIOS32_BOARD_J15_RS_Set(1); // RS pin used to control DC

    // send data
    MIOS32_BOARD_J15_SerDataShift(data);

    // increment graphical cursor
    ++mios32_lcd_x;

    // if end of display segment reached: set X position of all segments to 0
    if( (mios32_lcd_x % mios32_lcd_parameters.width) == 0 ) {
      APP_LCD_Cmd(0x00); // set X=0
      APP_LCD_Cmd(0x10);
    }

    return 0; // no error
  } break;

  case MIOS32_LCD_TYPE_CLCD:
  case MIOS32_LCD_TYPE_CLCD_DOG:
  default: {
    // wait until LCD unbusy, exit on error (timeout)
    if( MIOS32_BOARD_J15_PollUnbusy(mios32_lcd_device, 2500) < 0 ) {
      // disable display
      display_available &= ~(1 << mios32_lcd_device);
      return -2; // timeout
    }

    // write data
    MIOS32_BOARD_J15_DataSet(data);
    MIOS32_BOARD_J15_RS_Set(1);
    MIOS32_BOARD_J15_E_Set(mios32_lcd_device, 1);
    MIOS32_BOARD_J15_E_Set(mios32_lcd_device, 0);

    return 0; // no error
  }
  }

  return -3; // not supported
}


/////////////////////////////////////////////////////////////////////////////
// Sends command byte to LCD
// IN: command byte in <cmd>
// OUT: returns < 0 if display not available or timed out
/////////////////////////////////////////////////////////////////////////////
s32 APP_LCD_Cmd(u8 cmd)
{
  // check if if display already has been disabled
  if( !(display_available & (1 << mios32_lcd_device)) )
    return -1;

  switch( mios32_lcd_parameters.lcd_type ) {
  case MIOS32_LCD_TYPE_GLCD_KS0108:
  case MIOS32_LCD_TYPE_GLCD_KS0108_INVCS: {
  } break;

  case MIOS32_LCD_TYPE_GLCD_DOG: {
    // select all LCDs
    MIOS32_BOARD_J15_DataSet(0x00);
    MIOS32_BOARD_J15_RS_Set(0); // RS pin used to control DC

    // send command
    MIOS32_BOARD_J15_SerDataShift(cmd);

    return 0; // no error
  } break;

  case MIOS32_LCD_TYPE_GLCD_SSD1306: {
    // select all LCDs
#if APP_LCD_USE_J10_FOR_CS
    MIOS32_BOARD_J10_Set(0x00);
#else
    MIOS32_BOARD_J15_DataSet(0x00);
#endif
    MIOS32_BOARD_J15_RS_Set(0); // RS pin used to control DC

    MIOS32_BOARD_J15_SerDataShift(cmd);
  } break;

  case MIOS32_LCD_TYPE_CLCD:
  case MIOS32_LCD_TYPE_CLCD_DOG:
  default: {
    // wait until LCD unbusy, exit on error (timeout)
    if( MIOS32_BOARD_J15_PollUnbusy(mios32_lcd_device, 2500) < 0 ) {
      // disable display
      display_available &= ~(1 << mios32_lcd_device);
      return -2; // timeout
    }

    // write command
    MIOS32_BOARD_J15_DataSet(cmd);
    MIOS32_BOARD_J15_RS_Set(0);
    MIOS32_BOARD_J15_E_Set(mios32_lcd_device, 1);
    MIOS32_BOARD_J15_E_Set(mios32_lcd_device, 0);

    return 0; // no error
  }
  }

  return -3; // not supported
}


/////////////////////////////////////////////////////////////////////////////
// Clear Screen
// IN: -
// OUT: returns < 0 on errors
/////////////////////////////////////////////////////////////////////////////
s32 APP_LCD_Clear(void)
{
  switch( mios32_lcd_parameters.lcd_type ) {
  case MIOS32_LCD_TYPE_GLCD_KS0108:
  case MIOS32_LCD_TYPE_GLCD_KS0108_INVCS: {
  } break;

  case MIOS32_LCD_TYPE_GLCD_DOG: {
    s32 error = 0;
    u8 x, y;

    // use default font
    MIOS32_LCD_FontInit((u8 *)GLCD_FONT_NORMAL);

    // send data
    for(y=0; y<(mios32_lcd_parameters.height/8); ++y) {
      error |= MIOS32_LCD_CursorSet(0, y);

      // select all LCDs
      MIOS32_BOARD_J15_DataSet(0x00);
      MIOS32_BOARD_J15_RS_Set(1); // RS pin used to control DC

      for(x=0; x<mios32_lcd_parameters.width; ++x)
	MIOS32_BOARD_J15_SerDataShift(0x00);
    }

    // set X=0, Y=0
    error |= MIOS32_LCD_CursorSet(0, 0);

    return error;
  } break;

  case MIOS32_LCD_TYPE_GLCD_SSD1306: {
    s32 error = 0;
    u8 x, y;

    // use default font
    MIOS32_LCD_FontInit((u8 *)GLCD_FONT_NORMAL);

    // send data
    for(y=0; y<mios32_lcd_parameters.height/8; ++y) {
      error |= MIOS32_LCD_CursorSet(0, y);

      // select all LCDs
#if APP_LCD_USE_J10_FOR_CS
      MIOS32_BOARD_J10_Set(0x00);
#else
      MIOS32_BOARD_J15_DataSet(0x00);
#endif
      MIOS32_BOARD_J15_RS_Set(1); // RS pin used to control DC

      for(x=0; x<mios32_lcd_parameters.width; ++x)
	MIOS32_BOARD_J15_SerDataShift(0x00);
    }

    // set X=0, Y=0
    error |= MIOS32_LCD_CursorSet(0, 0);

    return error;
  } break;

  case MIOS32_LCD_TYPE_CLCD:
  case MIOS32_LCD_TYPE_CLCD_DOG:
  default:
    // -> send clear command
    return APP_LCD_Cmd(0x01);
  }

  return -3; // not supported
}


/////////////////////////////////////////////////////////////////////////////
// Sets cursor to given position
// IN: <column> and <line>
// OUT: returns < 0 on errors
/////////////////////////////////////////////////////////////////////////////
s32 APP_LCD_CursorSet(u16 column, u16 line)
{
  switch( mios32_lcd_parameters.lcd_type ) {
  case MIOS32_LCD_TYPE_GLCD_KS0108:
  case MIOS32_LCD_TYPE_GLCD_KS0108_INVCS: {
  } break;

  case MIOS32_LCD_TYPE_GLCD_DOG: {
    // mios32_lcd_x/y set by MIOS32_LCD_CursorSet() function
    return APP_LCD_GCursorSet(mios32_lcd_x, mios32_lcd_y);
  } break;

  case MIOS32_LCD_TYPE_GLCD_SSD1306: {
    // mios32_lcd_x/y set by MIOS32_LCD_CursorSet() function
    return APP_LCD_GCursorSet(mios32_lcd_x, mios32_lcd_y);
  } break;

  case MIOS32_LCD_TYPE_CLCD:
  case MIOS32_LCD_TYPE_CLCD_DOG:
  default: {
    // exit with error if line is not in allowed range
    if( line >= MIOS32_LCD_MAX_MAP_LINES )
      return -1;

    // -> set cursor address
    return APP_LCD_Cmd(0x80 | (mios32_lcd_cursor_map[line] + column));
  }
  }

  return -3; // not supported
}


/////////////////////////////////////////////////////////////////////////////
// Sets graphical cursor to given position
// IN: <x> and <y>
// OUT: returns < 0 on errors
/////////////////////////////////////////////////////////////////////////////
s32 APP_LCD_GCursorSet(u16 x, u16 y)
{
  switch( mios32_lcd_parameters.lcd_type ) {
  case MIOS32_LCD_TYPE_GLCD_KS0108:
  case MIOS32_LCD_TYPE_GLCD_KS0108_INVCS: {
  } break;

  case MIOS32_LCD_TYPE_GLCD_DOG: {
    s32 error = 0;
  
    // set X position
    error |= APP_LCD_Cmd(0x10 | (((x % mios32_lcd_parameters.width) >> 4) & 0x0f));   // First send MSB nibble
    error |= APP_LCD_Cmd(0x00 | ((x % mios32_lcd_parameters.width) & 0x0f)); // Then send LSB nibble

    // set Y position
    error |= APP_LCD_Cmd(0xb0 | ((y>>3) % (mios32_lcd_parameters.height/8)));

    return error;
  } break;

  case MIOS32_LCD_TYPE_GLCD_SSD1306: {
    s32 error = 0;

    // set X position
    error |= APP_LCD_Cmd(0x00 | (x & 0xf));
    error |= APP_LCD_Cmd(0x10 | ((x>>4) & 0xf));

    // set Y position
    error |= APP_LCD_Cmd(0xb0 | ((y>>3) & 7));

    return error;
  } break;
  }

  return -3; // not supported
}


/////////////////////////////////////////////////////////////////////////////
// Initializes a single special character
// IN: character number (0-7) in <num>, pattern in <table[8]>
// OUT: returns < 0 on errors
/////////////////////////////////////////////////////////////////////////////
s32 APP_LCD_SpecialCharInit(u8 num, u8 table[8])
{
  switch( mios32_lcd_parameters.lcd_type ) {
  case MIOS32_LCD_TYPE_GLCD_KS0108:
  case MIOS32_LCD_TYPE_GLCD_KS0108_INVCS: {
  } break;

  case MIOS32_LCD_TYPE_GLCD_DOG: {
  } break;

  case MIOS32_LCD_TYPE_GLCD_SSD1306: {
  } break;

  case MIOS32_LCD_TYPE_CLCD:
  case MIOS32_LCD_TYPE_CLCD_DOG:
  default: {
    s32 i;

    // send character number
    APP_LCD_Cmd(((num&7)<<3) | 0x40);

    // send 8 data bytes
    for(i=0; i<8; ++i)
      if( APP_LCD_Data(table[i]) < 0 )
	return -1; // error during sending character

    // set cursor to original position
    return APP_LCD_CursorSet(mios32_lcd_column, mios32_lcd_line);
  }
  }

  return -3; // not supported
}


/////////////////////////////////////////////////////////////////////////////
// Sets the background colour
// Only relevant for colour GLCDs
// IN: r/g/b value
// OUT: returns < 0 on errors
/////////////////////////////////////////////////////////////////////////////
s32 APP_LCD_BColourSet(u32 rgb)
{
  switch( mios32_lcd_parameters.lcd_type ) {
  case MIOS32_LCD_TYPE_GLCD_KS0108:
  case MIOS32_LCD_TYPE_GLCD_KS0108_INVCS: {
  } break;

  case MIOS32_LCD_TYPE_GLCD_DOG: {
  } break;

  case MIOS32_LCD_TYPE_GLCD_SSD1306: {
  } break;
  }

  return -3; // not supported
}


/////////////////////////////////////////////////////////////////////////////
// Sets the foreground colour
// Only relevant for colour GLCDs
// IN: r/g/b value
// OUT: returns < 0 on errors
/////////////////////////////////////////////////////////////////////////////
s32 APP_LCD_FColourSet(u32 rgb)
{
  switch( mios32_lcd_parameters.lcd_type ) {
  case MIOS32_LCD_TYPE_GLCD_KS0108:
  case MIOS32_LCD_TYPE_GLCD_KS0108_INVCS: {
  } break;

  case MIOS32_LCD_TYPE_GLCD_DOG: {
  } break;

  case MIOS32_LCD_TYPE_GLCD_SSD1306: {
  } break;
  }

  return -3; // not supported
}



/////////////////////////////////////////////////////////////////////////////
// Sets a pixel in the bitmap
// IN: bitmap, x/y position and colour value (value range depends on APP_LCD_COLOUR_DEPTH)
// OUT: returns < 0 on errors
/////////////////////////////////////////////////////////////////////////////
s32 APP_LCD_BitmapPixelSet(mios32_lcd_bitmap_t bitmap, u16 x, u16 y, u32 colour)
{
  switch( mios32_lcd_parameters.lcd_type ) {
  case MIOS32_LCD_TYPE_GLCD_KS0108:
  case MIOS32_LCD_TYPE_GLCD_KS0108_INVCS: {
  } break;

  case MIOS32_LCD_TYPE_GLCD_DOG: {
    if( x >= bitmap.width || y >= bitmap.height )
      return -1; // pixel is outside bitmap

    u8 *pixel = (u8 *)&bitmap.memory[bitmap.line_offset*(y / 8) + x];
    u8 mask = 1 << (y % 8);

    *pixel &= ~mask;
    if( colour )
      *pixel |= mask;

    return 0; // no error
  } break;

  case MIOS32_LCD_TYPE_GLCD_SSD1306: {
    if( x >= bitmap.width || y >= bitmap.height )
      return -1; // pixel is outside bitmap

    u8 *pixel = (u8 *)&bitmap.memory[bitmap.line_offset*(y / 8) + x];
    u8 mask = 1 << (y % 8);

    *pixel &= ~mask;
    if( colour )
      *pixel |= mask;

    return 0; // no error
  } break;
  }

  return -3; // not supported
}



/////////////////////////////////////////////////////////////////////////////
// Transfers a Bitmap within given boundaries to the LCD
// IN: bitmap
// OUT: returns < 0 on errors
/////////////////////////////////////////////////////////////////////////////
s32 APP_LCD_BitmapPrint(mios32_lcd_bitmap_t bitmap)
{
  switch( mios32_lcd_parameters.lcd_type ) {
  case MIOS32_LCD_TYPE_GLCD_KS0108:
  case MIOS32_LCD_TYPE_GLCD_KS0108_INVCS: {
  } break;

  case MIOS32_LCD_TYPE_GLCD_DOG: {
    int line;
    int y_lines = (bitmap.height >> 3);

    for(line=0; line<y_lines; ++line) {

      // calculate pointer to bitmap line
      u8 *memory_ptr = bitmap.memory + line * bitmap.line_offset;

      // set graphical cursor after second line has reached
      if( line > 0 ) {
	mios32_lcd_x -= bitmap.width;
	mios32_lcd_y += 8;
	APP_LCD_GCursorSet(mios32_lcd_x, mios32_lcd_y);
      }

      // transfer character
      int x;
      for(x=0; x<bitmap.width; ++x)
	APP_LCD_Data(*memory_ptr++);
    }

    // fix graphical cursor if more than one line has been print
    if( y_lines >= 1 ) {
      mios32_lcd_y = mios32_lcd_y - (bitmap.height-8);
      APP_LCD_GCursorSet(mios32_lcd_x, mios32_lcd_y);
    }

    return 0; // no error
  } break;

  case MIOS32_LCD_TYPE_GLCD_SSD1306: {
    int line;
    int y_lines = (bitmap.height >> 3);

    for(line=0; line<y_lines; ++line) {

      // calculate pointer to bitmap line
      u8 *memory_ptr = bitmap.memory + line * bitmap.line_offset;

      // set graphical cursor after second line has reached
      if( line > 0 ) {
	mios32_lcd_x -= bitmap.width;
	mios32_lcd_y += 8;
	APP_LCD_GCursorSet(mios32_lcd_x, mios32_lcd_y);
      }

      // transfer bitmap
      int x;
      for(x=0; x<bitmap.width; ++x)
	APP_LCD_Data(*memory_ptr++);
    }

    // fix graphical cursor if more than one line has been print
    if( y_lines >= 1 ) {
      mios32_lcd_y = mios32_lcd_y - (bitmap.height-8);
      APP_LCD_GCursorSet(mios32_lcd_x, mios32_lcd_y);
    }

    return 0; // no error
  } break;
  }

  return -3; // not supported
}
