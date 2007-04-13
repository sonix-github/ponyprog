//===============================================================
// vnotice.cxx - vNoticeDialog class functions - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vnotice.h>		// our header
#include <v/vicon.h>		// our header

// Define static data of the class

#define bang_width 40
#define bang_height 32
static unsigned char bang_bits[] = {
   0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00,
   0x7e, 0x00, 0x00, 0x00, 0x00, 0xe7, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x00,
   0x00, 0x00, 0x80, 0x83, 0x01, 0x00, 0x00, 0xc0, 0x99, 0x03, 0x00, 0x00,
   0xc0, 0x34, 0x03, 0x00, 0x00, 0xe0, 0x6a, 0x07, 0x00, 0x00, 0x70, 0x76,
   0x0e, 0x00, 0x00, 0x70, 0x6a, 0x0e, 0x00, 0x00, 0x38, 0x76, 0x1c, 0x00,
   0x00, 0x18, 0x6a, 0x18, 0x00, 0x00, 0x1c, 0x76, 0x38, 0x00, 0x00, 0x0e,
   0x6a, 0x70, 0x00, 0x00, 0x06, 0x76, 0x60, 0x00, 0x00, 0x07, 0x6a, 0xe0,
   0x00, 0x80, 0x03, 0x76, 0xc0, 0x01, 0x80, 0x03, 0x6a, 0xc0, 0x01, 0xc0,
   0x01, 0x76, 0x80, 0x03, 0xc0, 0x00, 0x6a, 0x00, 0x03, 0xe0, 0x00, 0x34,
   0x00, 0x07, 0x70, 0x00, 0x18, 0x00, 0x0e, 0x70, 0x00, 0x00, 0x00, 0x0e,
   0x38, 0x00, 0x3c, 0x00, 0x1c, 0x1c, 0x00, 0x76, 0x00, 0x38, 0x1c, 0x00,
   0x6a, 0x00, 0x38, 0x0e, 0x00, 0x76, 0x00, 0x70, 0x0f, 0x00, 0x3c, 0x00,
   0xf0, 0x1f, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfc,
   0xff, 0xff, 0xff, 0x3f};

    static vIcon bang(&bang_bits[0], bang_height, bang_width);

    static CommandObject NoticeDialog[] =
      {
	{C_Icon, 1, 0, "Notice", (void*)&bang, CA_None,isSens,NoFrame, 0, 0},

	{C_Text, 2, 0, "UserMessage",
		 NoList, CA_NoBorder | CA_MainMsg, isSens, NoFrame, 1,0},

	{C_Button, M_OK, M_OK, " OK ", NoList,CA_DefaultButton,isSens,NoFrame, 0, 1},
	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
      };

//======================>>> vNoticeDialog::Notice <<<=======================
  void vNoticeDialog::Notice(VCONST char* msg)
  {
    //	Show a message, and OK
    //	no important return

    int dummy;

    if (!_cmdsAdded)				// Only once
      {
	AddDialogCmds(NoticeDialog);		// Set up standard dialog
	_cmdsAdded = 1;
      }
    (void) ShowModalDialog(msg,dummy);		// show and wait
  }

//====================>>> vNoticeDialog::DialogCommand <<<=======================
  void vNoticeDialog::DialogCommand(ItemVal id, ItemVal val, CmdType ctype)
  {
    vModalDialog::DialogCommand(id,val,ctype);
//    if (id == M_OK)
//	CloseDialog();
  }

// --------------------------------------------------------------------- 
