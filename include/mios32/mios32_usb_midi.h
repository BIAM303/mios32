// $Id$
/*
 * Header file for USB MIDI Driver
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _MIOS32_USB_MIDI_H
#define _MIOS32_USB_MIDI_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

// 1 to stay compatible to USB MIDI spec, 0 as workaround for some windows versions...
#ifndef MIOS32_USB_MIDI_USE_AC_INTERFACE
#define MIOS32_USB_MIDI_USE_AC_INTERFACE 1
#endif

// allowed numbers: 1..8
#ifndef MIOS32_USB_MIDI_NUM_PORTS
#define MIOS32_USB_MIDI_NUM_PORTS 1
#endif

// buffer size (should be at least >= MIOS32_USB_MIDI_DESC_DATA_*_SIZE/4)
#ifndef MIOS32_USB_MIDI_RX_BUFFER_SIZE
#define MIOS32_USB_MIDI_RX_BUFFER_SIZE   64 // packages
#endif

#ifndef MIOS32_USB_MIDI_TX_BUFFER_SIZE
#define MIOS32_USB_MIDI_TX_BUFFER_SIZE   64 // packages
#endif


// size of IN/OUT pipe
#ifndef MIOS32_USB_MIDI_DATA_IN_SIZE
#define MIOS32_USB_MIDI_DATA_IN_SIZE           64
#endif
#ifndef MIOS32_USB_MIDI_DATA_OUT_SIZE
#define MIOS32_USB_MIDI_DATA_OUT_SIZE          64
#endif


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern s32 MIOS32_USB_MIDI_Init(u32 mode);

extern s32 MIOS32_USB_MIDI_ChangeConnectionState(u8 connected);
extern void MIOS32_USB_MIDI_EP1_IN_Callback(void);
extern void MIOS32_USB_MIDI_EP1_OUT_Callback(void);

extern s32 MIOS32_USB_MIDI_CheckAvailable(void);

extern s32 MIOS32_USB_MIDI_MIDIPackageSend_NonBlocking(mios32_midi_package_t package);
extern s32 MIOS32_USB_MIDI_MIDIPackageSend(mios32_midi_package_t package);
extern s32 MIOS32_USB_MIDI_MIDIPackageReceive(mios32_midi_package_t *package);

extern s32 MIOS32_USB_MIDI_Handler(void);


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////


#endif /* _MIOS32_USB_MIDI_H */