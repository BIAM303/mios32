// $Id$
/*
 * MIDIbox SEQ V4
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
#include <blm8x8.h>

#include <seq_midi_out.h>
#include <seq_bpm.h>

#include "tasks.h"

#include "app.h"

#include "seq_core.h"
#include "seq_par.h"
#include "seq_trg.h"
#include "seq_led.h"
#include "seq_ui.h"
#include "seq_pattern.h"

#include "seq_midply.h"

#include "seq_midi_in.h"

#include "seq_file.h"
#include "seq_file_b.h"



/////////////////////////////////////////////////////////////////////////////
// Encoder Configuration
/////////////////////////////////////////////////////////////////////////////
#define NUM_ENCODERS 17
const mios32_enc_config_t encoders[NUM_ENCODERS] = {
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 6, .cfg.pos=2 }, // Data Wheel

  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 1, .cfg.pos=6 }, // V-Pot 1
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 1, .cfg.pos=4 }, // V-Pot 2
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 1, .cfg.pos=2 }, // V-Pot 3
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 1, .cfg.pos=0 }, // V-Pot 4
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 3, .cfg.pos=6 }, // V-Pot 5
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 3, .cfg.pos=4 }, // V-Pot 6
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 3, .cfg.pos=2 }, // V-Pot 7
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 3, .cfg.pos=0 }, // V-Pot 8
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 4, .cfg.pos=6 }, // V-Pot 9
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 4, .cfg.pos=4 }, // V-Pot 10
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 4, .cfg.pos=2 }, // V-Pot 11
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 4, .cfg.pos=0 }, // V-Pot 12
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 5, .cfg.pos=6 }, // V-Pot 13
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 5, .cfg.pos=4 }, // V-Pot 14
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 5, .cfg.pos=2 }, // V-Pot 15
  { .cfg.type=DETENTED2, .cfg.speed=NORMAL, .cfg.speed_par=0, .cfg.sr= 5, .cfg.pos=0 }, // V-Pot 16
};


/////////////////////////////////////////////////////////////////////////////
// Local prototypes
/////////////////////////////////////////////////////////////////////////////
static void NOTIFY_MIDI_Rx(mios32_midi_port_t port, u8 byte);
static void NOTIFY_MIDI_Tx(mios32_midi_port_t port, u8 byte);


/////////////////////////////////////////////////////////////////////////////
// This hook is called after startup to initialize the application
/////////////////////////////////////////////////////////////////////////////
void APP_Init(void)
{
  s32 i;

  // initialize all LEDs
  MIOS32_BOARD_LED_Init(0xffffffff);

#if 1
  // initialize encoders
  for(i=0; i<NUM_ENCODERS; ++i)
    MIOS32_ENC_ConfigSet(i, encoders[i]);
#endif

  // initialize second CLCD
  MIOS32_LCD_DeviceSet(1);
  MIOS32_LCD_Init(0);
  MIOS32_LCD_DeviceSet(0);

#if DEFAULT_SRM_ENABLED
  // init BLM
  BLM8X8_Init(0);
#endif

  // initialize MIDI handlers
  SEQ_MIDI_IN_Init(0);
  SEQ_MIDI_OUT_Init(0);

  // initialize parameter/trigger layers
  SEQ_PAR_Init(0);
  SEQ_TRG_Init(0);

  // init sequencer core
  SEQ_CORE_Init(0);

#if MID_PLAYER_TEST
  // init MIDI file player
  SEQ_MIDPLY_Init(0);
#endif

  // init user interface
  SEQ_LED_Init(0);
  SEQ_UI_Init(0);

  // initial load of filesystem
  SEQ_FILE_Init(0); // TODO: periodically check if SD card connected/disconnected during runtime
  SEQ_FILE_B_Init(0);

  // start tasks (differs between MIOS32 and MacOS)
  TASKS_Init(0);

  // install MIDI Rx/Tx callback functions
  MIOS32_MIDI_DirectRxTxCallback_Init(NOTIFY_MIDI_Rx, NOTIFY_MIDI_Tx);
}


/////////////////////////////////////////////////////////////////////////////
// This task is running endless in background
/////////////////////////////////////////////////////////////////////////////
void APP_Background(void)
{
  // toggle the state of all LEDs (allows to measure the execution speed with a scope)
#if 0
  MIOS32_BOARD_LED_Set(0xffffffff, ~MIOS32_BOARD_LED_Get());
#endif

  // call LCD Handler
  SEQ_UI_LCD_Handler();

  // update LEDs
  SEQ_UI_LED_Handler();
}


/////////////////////////////////////////////////////////////////////////////
//  This hook is called when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void APP_NotifyReceivedEvent(u8 port, mios32_midi_package_t midi_package)
{
  SEQ_MIDI_IN_Receive(port, midi_package);
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called when a SysEx byte has been received
/////////////////////////////////////////////////////////////////////////////
void APP_NotifyReceivedSysEx(u8 port, u8 sysex_byte)
{
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called when a byte has been received via COM interface
/////////////////////////////////////////////////////////////////////////////
void APP_NotifyReceivedCOM(mios32_com_port_t port, u8 byte)
{
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called before the shift register chain is scanned
/////////////////////////////////////////////////////////////////////////////
void APP_SRIO_ServicePrepare(void)
{
#if DEFAULT_SRM_ENABLED
  // prepare DOUT registers of 8x8 BLM to drive the column
  BLM8X8_PrepareCol();
#endif
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called after the shift register chain has been scanned
/////////////////////////////////////////////////////////////////////////////
void APP_SRIO_ServiceFinish(void)
{
#if DEFAULT_SRM_ENABLED
  // call the BLM8X8_GetRow function after scan is finished to capture the read DIN values
  BLM8X8_GetRow();
#endif
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called when a button has been toggled
// pin_value is 1 when button released, and 0 when button pressed
/////////////////////////////////////////////////////////////////////////////
void APP_DIN_NotifyToggle(u32 pin, u32 pin_value)
{
#if COM_DEBUG == 1
  // for debugging
  printf("Pin %3d (SR%d:D%d) = %d\n\r", pin, (pin>>3)+1, pin&7, pin_value);
#endif

  // forward to UI button handler
  SEQ_UI_Button_Handler(pin, pin_value);
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called when a BLM8x8 button has been toggled
// (see also SEQ_TASK_Period1mS)
// pin_value is 1 when button released, and 0 when button pressed
/////////////////////////////////////////////////////////////////////////////
#if DEFAULT_SRM_ENABLED
void APP_BLM8X8_NotifyToggle(u32 pin, u32 pin_value)
{
#if COM_DEBUG == 1
  // for debugging
  printf("BLM Pin %3d (SR%d:D%d) = %d\n\r", pin, (pin>>3)+1, pin&7, pin_value);
#endif

  // forward to UI button handler
  SEQ_UI_Button_Handler(pin + 128, pin_value);
}
#endif


/////////////////////////////////////////////////////////////////////////////
// This hook is called when an encoder has been moved
// incrementer is positive when encoder has been turned clockwise, else
// it is negative
/////////////////////////////////////////////////////////////////////////////
void APP_ENC_NotifyChange(u32 encoder, s32 incrementer)
{
#if COM_DEBUG == 1
  // for debugging
  printf("Enc %2d = %d\n\r", encoder, incrementer);
#endif

  // forward to UI encoder handler
  SEQ_UI_Encoder_Handler(encoder, incrementer);
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void APP_AIN_NotifyChange(u32 pin, u32 pin_value)
{
}

/////////////////////////////////////////////////////////////////////////////
// This task is called periodically each mS
/////////////////////////////////////////////////////////////////////////////
void SEQ_TASK_Period1mS(void)
{
  // update high-prio LED functions
  SEQ_UI_LED_Handler_Periodic();

  // for menu handling (e.g. flashing cursor, doubleclick counter, etc...)
  SEQ_UI_MENU_Handler_Periodic();

#if DEFAULT_SRM_ENABLED
  // check for BLM8x8 pin changes, call button handler of sequencer on each toggled pin
  BLM8X8_ButtonHandler(APP_BLM8X8_NotifyToggle);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// This task is called periodically each mS as well
// it handles sequencer and MIDI events
/////////////////////////////////////////////////////////////////////////////
void SEQ_TASK_MIDI(void)
{
  // execute sequencer handler
#if MID_PLAYER_TEST
  SEQ_MIDPLY_Handler();
#else
  SEQ_CORE_Handler();
#endif

  // send timestamped MIDI events
  SEQ_MIDI_OUT_Handler();
}


/////////////////////////////////////////////////////////////////////////////
// This task is triggered whenever a pattern has to be loaded
/////////////////////////////////////////////////////////////////////////////
void SEQ_TASK_Pattern(void)
{
  SEQ_PATTERN_Handler();
}


/////////////////////////////////////////////////////////////////////////////
// Installed via MIOS32_MIDI_DirectRxTxCallback_Init
// Executed immediately on each incoming MIDI byte, partly from interrupt
// handlers!
// These function should be executed so fast as possible. They can be used
// to trigger MIDI Rx/Tx LEDs or to trigger on MIDI clock events. In order to
// avoid MIDI buffer overruns, the max. recommented execution time is 100 uS!
/////////////////////////////////////////////////////////////////////////////
static void NOTIFY_MIDI_Rx(mios32_midi_port_t port, u8 midi_byte)
{
  // TODO: here we could filter a certain port
  SEQ_BPM_NotifyMIDIRx(midi_byte);
}

static void NOTIFY_MIDI_Tx(mios32_midi_port_t port, u8 midi_byte)
{
}

