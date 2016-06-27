//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2016   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id$
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
// Linux SysFS IO (useful on raspberry and embedded PC)

#include "linuxsysfsint.h"
#include "errcode.h"
#include "e2app.h"

#define GPIO_OUT			true
#define GPIO_IN				false

#ifdef	_LINUX_
# include <stdio.h>
# include <stdlib.h>
# include <errno.h>
# include <unistd.h>
# include <fcntl.h>
#else
# ifdef	__BORLANDC__
#   define	__inline__
# else // MICROSOFT VC++
#   define	__inline__ __inline
#   define _export
# endif
#endif

LinuxSysFsInterface::LinuxSysFsInterface()
{
	UserDebug(Constructor, "LinuxSysFsInterface::LinuxSysFsInterface() Constructor\n");

	Install(0);
	old_portno = 0;
    fd_ctrl = fd_clock = fd_datain = fd_dataout = -1;
}

LinuxSysFsInterface::~LinuxSysFsInterface()
{
	Close();
}

#ifdef	_LINUX_

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

static int gpio_open(unsigned int gpio, bool out_dir)
{
	char buf[MAX_BUF];
	int rval;

	//trying with gpio command (you need wiringPi installed)
	snprintf(buf, sizeof(buf), "gpio export %u %s", gpio, out_dir ? "out" : "in");
	rval = system(buf);
	if (rval != 0)
	{
		int fd;

		fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
		if (fd < 0)
		{
			fprintf(stderr, "Unable to open GPIO export interface: %s\n", strerror(errno));
			rval = -1;
		}
		else
		{
			int ret, len;

			len = snprintf(buf, sizeof(buf), "%d", gpio);
			ret = write(fd, buf, len);
			close(fd);
			rval = (ret == len) ? 0 : -1;
		}

		if (rval == 0)
		{
			snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);
			fd = open(buf, O_WRONLY);
			if (fd < 0)
			{
				fprintf(stderr, "Unable to open GPIO direction interface: %s\n", strerror(errno));
				rval = -1;
			}
			else
			{
				int ret, len;

				if (out_dir)
					len = snprintf(buf, sizeof(buf), "out");
				else
					len = snprintf(buf, sizeof(buf), "in");
				ret = write(fd, buf, len);
				close(fd);
				rval = (ret == len) ? 0 : -1;
			}
		}
	}

	//open the value interace
	if (rval == 0)
	{
		int fd;

		snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
		fd = open(buf, O_WRONLY);
		if (fd < 0)
		{
			fprintf(stderr, "Unable to open GPIO set-value interface: %s\n", strerror(errno));
			rval = -1;
		}
		else
		{
			rval = fd;
		}
	}
	UserDebug3(UserApp2, "gpio_open(%u, %s) rval = %d\n", gpio, out_dir ? "out" : "in", rval);

	return rval;
}

static int gpio_close(unsigned int gpio, int fd)
{
	char buf[MAX_BUF];
	int rval = 0;

	//close value interface
	if (fd > 0)
		close(fd);

	//trying with gpio command (you need wiringPi installed)
	snprintf(buf, sizeof(buf), "gpio unexport %u", gpio);
	rval = system(buf);
	if (rval != 0)
	{
		fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
		if (fd < 0)
		{
			fprintf (stderr, "Unable to open GPIO unexport interface: %s\n", strerror(errno));
			rval = -1;
		}
		else
		{
			int ret, len;

			len = snprintf(buf, sizeof(buf), "%d", gpio);
			ret = write(fd, buf, len);
			close(fd);
			rval = (ret == len) ? 0 : -1;
		}
	}
	UserDebug2(UserApp2, "gpio_close(%u) rval = %d\n", gpio, rval);

	return rval;
}
#endif

int LinuxSysFsInterface::SetPower(int onoff)
{
	UserDebug1(UserApp3, "LinuxSysFsInterface::SetPower(%d)\n", onoff);
	return OK;
}

int LinuxSysFsInterface::InitPins()
{
	pin_ctrl = THEAPP->GetGpioPinCtrl();
	pin_datain = THEAPP->GetGpioPinDataIn();
	pin_dataout = THEAPP->GetGpioPinDataOut();
	pin_clock = THEAPP->GetGpioPinClock();

	UserDebug2(UserApp2, "LinuxSysFsInterface::InitPins Ctrl=%d, Clock=%d ", pin_ctrl, pin_clock);
	UserDebug2(UserApp2, "DataIn=%d, DataOut=%d\n", pin_datain, pin_dataout);

#ifdef	_LINUX_
	fd_ctrl = gpio_open(pin_ctrl, GPIO_OUT);
	fd_clock = gpio_open(pin_clock, GPIO_OUT);
	fd_datain = gpio_open(pin_datain, GPIO_IN);
	fd_dataout = gpio_open(pin_dataout, GPIO_OUT);
	if (fd_ctrl < 0 || fd_clock < 0 || fd_datain < 0 || fd_dataout < 0)
	{
		DeInitPins();
		return E2ERR_OPENFAILED;
	}
#endif
	return OK;
}

void LinuxSysFsInterface::DeInitPins()
{
#ifdef	_LINUX_
	gpio_close(pin_ctrl, fd_ctrl);
	gpio_close(pin_clock, fd_clock);
	gpio_close(pin_datain, fd_datain);
	gpio_close(pin_dataout, fd_dataout);
	fd_ctrl = fd_clock = fd_datain = fd_dataout = -1;
#endif
}

int LinuxSysFsInterface::Open(int com_no)
{
	UserDebug1(UserApp1, "LinuxSysFsInterface::Open(%d) IN\n", com_no);

	int ret_val = OK;

	if (IsInstalled() != com_no)
	{
		if ((ret_val = InitPins()) == OK)
			Install(com_no);
	}

	UserDebug1(UserApp2, "LinuxSysFsInterface::Open() = %d OUT\n", ret_val);

	return ret_val;
}

void LinuxSysFsInterface::Close()
{
	UserDebug(UserApp1, "LinuxSysFsInterface::Close() IN\n");

	if (IsInstalled())
	{
		SetPower(0);
		DeInitPins();
		Install(0);
	}

	UserDebug(UserApp2, "LinuxSysFsInterface::Close() OUT\n");
}

// Per l'AVR e` la linea di RESET
void LinuxSysFsInterface::SetControlLine(int res)
{
	UserDebug2(UserApp3, "LinuxSysFsInterface::SetControlLine(%d) *** Inst=%d\n", res, IsInstalled());

	if (IsInstalled())
	{
		if (THEAPP->GetPolarity() & RESETINV)
			res = !res;

#ifdef	_LINUX_
		if (res)
            write(fd_ctrl, "1", 2);
        else
            write(fd_ctrl, "0", 2);
#endif
	}
}

void LinuxSysFsInterface::SetDataOut(int sda)
{
	UserDebug2(UserApp3, "LinuxSysFsInterface::SetDataOut(%d) *** Inst=%d\n", sda, IsInstalled());

	if (IsInstalled())
	{
		if ( (THEAPP->GetPolarity() & DOUTINV) )
			sda = !sda;

#ifdef	_LINUX_
		if (sda)
            write(fd_dataout, "1", 2);
        else
            write(fd_dataout, "0", 2);
#endif
	}
}

void LinuxSysFsInterface::SetClock(int scl)
{
	UserDebug2(UserApp3, "LinuxSysFsInterface::SetClock(%d) *** Inst=%d\n", scl, IsInstalled());

	if (IsInstalled())
	{
		if ( (THEAPP->GetPolarity() & CLOCKINV) )
			scl = !scl;

#ifdef	_LINUX_
		if (scl)
            write(fd_clock, "1", 2);
        else
            write(fd_clock, "0", 2);
#endif
	}
}

void LinuxSysFsInterface::SetClockData()
{
	UserDebug1(UserApp3, "LinuxSysFsInterface::SetClockData() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		SetClock(1);
		SetDataOut(1);
	}
}


void LinuxSysFsInterface::ClearClockData()
{
	UserDebug1(UserApp3, "LinuxSysFsInterface::ClearClockData() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		SetClock(0);
		SetDataOut(0);
	}
}

int LinuxSysFsInterface::GetDataIn()
{
	UserDebug1(UserApp3, "LinuxSysFsInterface::GetDataIn() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		unsigned int val = 0;
#ifdef	_LINUX_
        char ch;

        read(fd_datain, &ch, 1);
        val = (ch == '0') ? 0 : 1;
#endif
		if (THEAPP->GetPolarity() & DININV)
			return !val;
		else
			return val;
	}
	else
		return E2ERR_NOTINSTALLED;
}

int LinuxSysFsInterface::GetClock()
{
	return 1;
}

int LinuxSysFsInterface::IsClockDataUP()
{
	UserDebug1(UserApp3, "LinuxSysFsInterface::IsClockDataUP() *** Inst=%d\n", IsInstalled());

	return GetDataIn();
}

int LinuxSysFsInterface::IsClockDataDOWN()
{
	UserDebug1(UserApp3, "LinuxSysFsInterface::IsClockDataDOWN() *** Inst=%d\n", IsInstalled());

	return !GetDataIn();
}

/**
int LinuxSysFsInterface::TestPort(int com_no)
{
	UserDebug1(UserApp1, "LinuxSysFsInterface::TestPort(%d) IN\n", com_no);
	int ret_val = TestSave(com_no);

	if (ret_val == OK)
	{
		int a,b;
		Wait w;

		ret_val = E2ERR_OPENFAILED;

		SetClockData();
		w.WaitMsec(50);
		a = (GetCPWReg() & WF_SCL) ? 1 : 0;
		b = GetPresence() ? 1 : 0;

		if (a == b)
		{
			ClearClockData();
			w.WaitMsec(50);
			a = (GetCPWReg() & WF_SCL) ? 1 : 0;
			b = GetPresence() ? 1 : 0;

			if (a == b)
			{
				SetClockData();
				w.WaitMsec(50);
				a = (GetCPWReg() & WF_SCL) ? 1 : 0;
				b = GetPresence() ? 1 : 0;

				if (a == b)
					ret_val = OK;
			}
		}
	}
	TestRestore();
	UserDebug1(UserApp2, "LinuxSysFsInterface::TestPort() = %d OUT\n", ret_val);

	return ret_val;
}
**/
