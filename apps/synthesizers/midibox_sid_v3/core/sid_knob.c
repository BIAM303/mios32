// $Id$
/*
 * MBSID Patch Routines
 *
 * ==========================================================================
 *
 *  Copyright (C) 2009 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <mios32.h>
#include <string.h>

#include "sid_knob.h"


/////////////////////////////////////////////////////////////////////////////
// for optional debugging messages via DEBUG_MSG (defined in mios32_config.h)
// should be at least 1 for sending error messages
/////////////////////////////////////////////////////////////////////////////
#define DEBUG_VERBOSE_LEVEL 1


/////////////////////////////////////////////////////////////////////////////
// Global Variables
/////////////////////////////////////////////////////////////////////////////

sid_knob_t sid_knob[SID_NUM][SID_KNOB_NUM];


/////////////////////////////////////////////////////////////////////////////
// Local definitions
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// Type definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Local prototypes
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Initialisation
/////////////////////////////////////////////////////////////////////////////
s32 SID_KNOB_Init(u32 mode)
{
  int sid, i;

  for(sid=0; sid<SID_NUM; ++i)
    for(i=0; i<SID_KNOB_NUM; ++i)
      SID_KNOB_KnobInit((sid_knob_t *)&sid_knob[sid][i]);

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// Initialises a Knob
/////////////////////////////////////////////////////////////////////////////
s32 SID_KNOB_KnobInit(sid_knob_t *knob)
{
  // clear complete structure
  memset(knob, 0, sizeof(sid_knob_t));

  knob->max = 0xff;

  return 0; // no error
}