//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2017   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: lptinterf.cpp,v 1.5 2009/11/16 22:29:18 lancos Exp $
//-------------------------------------------------------------------------//
//                                                                         //
// This program is free software; you can redistribute it and/or           //
// modify it under the terms of the GNU  General Public License            //
// as published by the Free Software Foundation; either version2 of        //
// the License, or (at your option) any later version.                     //
//                                                                         //
// This program is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of          //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
// General Public License for more details.                                //
//                                                                         //
// You should have received a copy of the GNU  General Public License      //
// along with this program (see COPYING);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//-------------------------------------------------------------------------//
//=========================================================================//

#include "types.h"
#include "errcode.h"

#include <QDebug>

#ifdef  _WINDOWS
#include <windows.h>
#else

//#include <linux/parport.h>
//If you use latest kernel (2.4.xx) you should use standard ppdev.o module and include ppdev.h
//otherwise install the ppuser module, and include the file ppuser.h
#define USE_K2_4_PPDEV  0

// #include <sys/types.h>
// #include <linux/hdreg.h>
// #include <sys/ioctl.h>
// #include <fcntl.h>

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#if USE_K2_4_PPDEV == 1
# include "linux/ppdev.h"
# define PARPORTDEVNAME "/dev/parport%d"
//N.B. be sure to issue a "chmod 666 /dev/parport?"
#else
# include "ppuser.h"
# define PARPORTDEVNAME "/dev/ppuser%d0"
//N.B. be sure to issue a "chmod 666 /dev/ppuser?"
#endif


#endif



#include "globals.h"
#include "lptinterf.h"

#define MAX_LPTPORTS    3

LPTInterface::LPTInterface()
{
	qDebug() << "LPTInterface::LPTInterface()";

	last_ctrl = last_data = 0;
#ifdef  _WINDOWS
	lpt_control.LPPort = 1;         //by default use LPT1

	hLPTCONVxD = CreateFile("\\\\.\\LPTCON.VXD", 0, 0, NULL, 0,
	                        FILE_FLAG_DELETE_ON_CLOSE | FILE_FLAG_OVERLAPPED,
	                        NULL);
#else
	lpt_control.LPPort = 0;         //no used port

	hLpt = INVALID_HANDLE_VALUE;
	/**
	        char name[16];
	        int val;
	        sprintf(name, PARPORTDEVNAME, lpt_control.LPPort-1);
	        hLpt = open(name, O_RDWR);
	        if (hLpt != INVALID_HANDLE_VALUE)
	        {
	                val = ioctl(hLpt, PPCLAIM, 0);
	                if ( val )
	                {
	                        qDebug() << "LPTInterface::LPTInterface() ** ppclaim = "<< val;
	                        close(hLpt);
	                        hLpt = INVALID_HANDLE_VALUE;
	                }
	        }
	**/
#endif
}

LPTInterface::~LPTInterface()
{
	qDebug() <<  "LPTInterface::~LPTInterface()";

#ifdef  _WINDOWS
	CloseHandle(hLPTCONVxD);
	hLPTCONVxD = INVALID_HANDLE_VALUE;
#else
	Close();
#endif
}

void LPTInterface::Close()
{
#ifndef _WINDOWS
	qDebug() << "LPTInterface::Close() I ** hLpt = " << hLpt;

	if (hLpt != INVALID_HANDLE_VALUE)
	{
		ioctl(hLpt, PPRELEASE, 0);
		close(hLpt);
		hLpt = INVALID_HANDLE_VALUE;
		lpt_control.LPPort = 0;
	}

	qDebug() << "LPTInterface::Close() O ** hLpt = " << hLpt;
#endif
}

void LPTInterface::SetPort(int port_no)
{
	qDebug() << "LPTInterface::SetPort(" << port_no << ")";

	if ( port_no >= 1 && port_no <= MAX_LPTPORTS )
	{
		lpt_control.LPPort = port_no;

#ifdef __linux__

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			ioctl(hLpt, PPRELEASE, 0);
			close(hLpt);
		}

		//              char name[MAXPATH];
		QString name = QString().sprintf(PARPORTDEVNAME, lpt_control.LPPort - 1);
		//              sprintf(name, PARPORTDEVNAME, lpt_control.LPPort - 1);
		hLpt = open(name.toLatin1(), O_RDWR);

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			if ( ioctl(hLpt, PPCLAIM, 0) )
			{
				close(hLpt);
				hLpt = INVALID_HANDLE_VALUE;
			}
		}

		qDebug() << "LPTInterface::SetPort() ** hLpt = " << hLpt;
#endif
	}
}

//   Bit           Bit
//    0 - Unused    4 - Pin 13
//    1 - Unused    5 - Pin 12
//    2 - Unused    6 - Pin 10
//    3 - Pin 15    7 - Pin 11 (Inverted)
//
/////////////////////////////////////////////////////////////////

int LPTInterface::InDataPort(int port_no)
{
	qDebug() << "LPTInterface::InDataPort(" << port_no << ") ** lp=" << lpt_control.LPPort;

	int ret_val = E2ERR_NOTINSTALLED;
#ifdef  _WINDOWS

	if ( hLPTCONVxD != INVALID_HANDLE_VALUE )
	{
		if ( port_no >= 1 && port_no <= MAX_LPTPORTS )
		{
			lpt_control.LPPort = port_no;
		}

		uint8_t value;

		if ( !DeviceIoControl(hLPTCONVxD, LPTCON_READ,
		                      &lpt_control, sizeof(LPTCONDATA),
		                      &value, sizeof(value), NULL, 0) )
		{
			ret_val = E2ERR_OPENFAILED;
		}
		else
		{
			ret_val = value;
		}
	}

#else

	if ( port_no >= 1 && port_no <= MAX_LPTPORTS
	                && port_no != lpt_control.LPPort )
	{
		lpt_control.LPPort = port_no;

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			ioctl(hLpt, PPRELEASE, 0);
			close(hLpt);
		}

		//              char name[MAXPATH];
		QString name = QString().sprintf(PARPORTDEVNAME, lpt_control.LPPort - 1);
		//              sprintf(name, PARPORTDEVNAME, lpt_control.LPPort - 1);
		hLpt = open(name.toLatin1(), O_RDWR);

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			if ( ioctl(hLpt, PPCLAIM, 0) )
			{
				close(hLpt);
				hLpt = INVALID_HANDLE_VALUE;
			}
		}

		qDebug() << "LPTInterface::InDataPort() ** hLpt=" << hLpt << ", " << name;
	}

	if ( hLpt != INVALID_HANDLE_VALUE )
	{
		if ( ioctl(hLpt, PPRSTATUS, &lpt_control.LPByte) )
		{
			ret_val = E2ERR_ACCESSDENIED;
		}
		else
		{
			ret_val = lpt_control.LPByte;
		}
	}

#endif
	qDebug() << "LPTInterface::InDataPort() = " << ret_val << " (" << lpt_control.LPByte << ")";

	return ret_val;
}


//
//   Bit           Bit
//    0 - Pin 2    4 - Pin 6
//    1 - Pin 3    5 - Pin 7
//    2 - Pin 4    6 - Pin 8
//    3 - Pin 5    7 - Pin 9
//
/////////////////////////////////////////////////////////////////

int LPTInterface::OutDataPort(int val, int port_no)
{
	qDebug() << "LPTInterface::OutDataPort(" << val << ", " << port_no << ")";

	int ret_val = E2ERR_NOTINSTALLED;

#ifdef  _WINDOWS

	if ( hLPTCONVxD != INVALID_HANDLE_VALUE )
	{
		if ( port_no >= 1 && port_no <= MAX_LPTPORTS )
		{
			lpt_control.LPPort = port_no;
		}

		lpt_control.LPByte = last_data = (uint8_t)val;

		ret_val = DeviceIoControl(hLPTCONVxD, LPTCON_WRITE_DATA,
		                          &lpt_control, sizeof(LPTCONDATA),
		                          NULL, 0, NULL, 0) ? OK : E2ERR_NOTINSTALLED;
	}

#else

	if ( port_no >= 1 && port_no <= MAX_LPTPORTS
	                && port_no != lpt_control.LPPort )
	{
		lpt_control.LPPort = port_no;

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			ioctl(hLpt, PPRELEASE, 0);
			close(hLpt);
		}

		//              char name[MAXPATH];
		QString name = QString().sprintf(PARPORTDEVNAME, lpt_control.LPPort - 1);
		//              sprintf(name, PARPORTDEVNAME, lpt_control.LPPort - 1);
		hLpt = open(name.toLatin1(), O_RDWR);

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			if ( ioctl(hLpt, PPCLAIM, 0) )
			{
				close(hLpt);
				hLpt = INVALID_HANDLE_VALUE;
			}
		}
	}

	if ( hLpt != INVALID_HANDLE_VALUE )
	{
		lpt_control.LPByte = last_data = (uint8_t)val;

		if ( ioctl(hLpt, PPWDATA, &lpt_control.LPByte) )
		{
			ret_val = E2ERR_ACCESSDENIED;
		}
		else
		{
			ret_val = OK;
		}
	}

#endif
	qDebug() << "LPTInterface::OutDataPort() = " << ret_val;

	return ret_val;
}




//
//   Bit
//    0 - Pin 1
//    1 - Pin 14
//    2 - Pin 16
//    3 - Pin 17      NOTE: Bits 5-7 are not used.
//
/////////////////////////////////////////////////////////////////

int LPTInterface::OutControlPort(int val, int port_no)
{
	qDebug() << "LPTInterface::OutControlPort(" << val << ", " << port_no << ")";

	int ret_val = E2ERR_NOTINSTALLED;

#ifdef  _WINDOWS

	if ( hLPTCONVxD != INVALID_HANDLE_VALUE )
	{
		if ( port_no >= 1 && port_no <= MAX_LPTPORTS )
		{
			lpt_control.LPPort = port_no;
		}

		lpt_control.LPByte = last_ctrl = (uint8_t)val & 0x0F;

		ret_val = DeviceIoControl(hLPTCONVxD, LPTCON_WRITE_CONTROL,
		                          &lpt_control, sizeof(LPTCONDATA),
		                          NULL, 0, NULL, 0) ? OK : E2ERR_NOTINSTALLED;
	}

#else

	if ( port_no >= 1 && port_no <= MAX_LPTPORTS
	                && port_no != lpt_control.LPPort )
	{
		lpt_control.LPPort = port_no;

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			ioctl(hLpt, PPRELEASE, 0);
			close(hLpt);
		}

		//              char name[MAXPATH];
		QString name = QString().sprintf(PARPORTDEVNAME, lpt_control.LPPort - 1);
		//              sprintf(name, PARPORTDEVNAME, lpt_control.LPPort - 1);
		hLpt = open(name.toLatin1(), O_RDWR);

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			if ( ioctl(hLpt, PPCLAIM, 0) )
			{
				close(hLpt);
				hLpt = INVALID_HANDLE_VALUE;
			}
		}
	}

	if ( hLpt != INVALID_HANDLE_VALUE )
	{
		lpt_control.LPByte = last_ctrl = (uint8_t)val & 0x0F;

		if ( ioctl(hLpt, PPWCONTROL, &lpt_control.LPByte) )
		{
			ret_val = E2ERR_ACCESSDENIED;
		}
		else
		{
			ret_val = OK;
		}
	}

#endif
	qDebug() << "LPTInterface::OutControlPort() = " << ret_val;

	return ret_val;
}

int LPTInterface::OutDataMask(int mask, int val)
{
	int ret_val = E2ERR_NOTINSTALLED;

#ifdef  _WINDOWS

	if ( hLPTCONVxD != INVALID_HANDLE_VALUE )
#else
	if ( hLpt != INVALID_HANDLE_VALUE )
#endif
	{
		if (val == 0)
		{
			last_data &= ~mask;
		}
		else if (val == 1)
		{
			last_data |= mask;
		}
		else
		{
			last_data ^= mask;
		}

		ret_val = OutDataPort(last_data);
		/***
		                lpt_control.LPByte = last_data;

		                ret_val = DeviceIoControl(hLPTCONVxD, LPTCON_WRITE_DATA,
		                                        &lpt_control, sizeof(LPTCONDATA),
		                                        NULL, 0, NULL, 0) ? OK : E2ERR_NOTINSTALLED;
		****/
	}

	return ret_val;
}

int LPTInterface::OutControlMask(int mask, int val)
{
	int ret_val = E2ERR_NOTINSTALLED;

#ifdef  _WINDOWS

	if ( hLPTCONVxD != INVALID_HANDLE_VALUE )
#else
	if ( hLpt != INVALID_HANDLE_VALUE )
#endif
	{
		if (val == 0)
		{
			last_ctrl &= ~mask;
		}
		else if (val == 1)
		{
			last_ctrl |= mask;
		}
		else
		{
			last_ctrl ^= mask;
		}

		ret_val = OutControlPort(last_ctrl);
		/****
		                lpt_control.LPByte = last_ctrl;

		                ret_val = DeviceIoControl(hLPTCONVxD, LPTCON_WRITE_CONTROL,
		                                        &lpt_control, sizeof(LPTCONDATA),
		                                        NULL, 0, NULL, 0) ? OK : E2ERR_NOTINSTALLED;
		****/
	}

	return ret_val;
}

