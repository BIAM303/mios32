// $Id$
/*
 * Header file for UART functions
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _MIOS32_UART_H
#define _MIOS32_UART_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

// number of UART interfaces (0..2)
#ifndef MIOS32_UART_NUM
#define MIOS32_UART_NUM 2
#endif

// Tx buffer size (1..256)
#ifndef MIOS32_UART_TX_BUFFER_SIZE
#define MIOS32_UART_TX_BUFFER_SIZE 5
#endif

// Rx buffer size (1..256)
#ifndef MIOS32_UART_RX_BUFFER_SIZE
#define MIOS32_UART_RX_BUFFER_SIZE 5
#endif

// Baudrate of UART first interface
#ifndef MIOS32_UART0_BAUDRATE
#define MIOS32_UART0_BAUDRATE 31250
#endif

// Baudrate of UART second interface
#ifndef MIOS32_UART1_BAUDRATE
#define MIOS32_UART1_BAUDRATE 31250
#endif


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern s32 MIOS32_UART_Init(u32 mode);

extern s32 MIOS32_UART_RxBufferFree(u8 uart);
extern s32 MIOS32_UART_RxBufferUsed(u8 uart);
extern s32 MIOS32_UART_RxBufferGet(u8 uart);
extern s32 MIOS32_UART_RxBufferPeek(u8 uart);
extern s32 MIOS32_UART_RxBufferPut(u8 uart, u8 b);
extern s32 MIOS32_UART_TxBufferFree(u8 uart);
extern s32 MIOS32_UART_TxBufferUsed(u8 uart);
extern s32 MIOS32_UART_TxBufferGet(u8 uart);
extern s32 MIOS32_UART_TxBufferPut(u8 uart, u8 b);
extern s32 MIOS32_UART_TxBufferPutMore(u8 uart, u8 *buffer, u16 len);


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////



#endif /* _MIOS32_UART_H */