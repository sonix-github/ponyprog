//=========================================================================//
//-------------------------------------------------------------------------//
// busio.h -- Header for for BusIO class                             //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997, 1998  Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lanconel@cs.unibo.it                                           //
//  http://www.cs.unibo.it/~lanconel                                       //
//                                                                         //
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

#ifndef	_BUSIO_H
#define	_BUSIO_H

#include "types.h"
#include "wait.h"
#include "businter.h"

//Formato di bus generico seriale
class BusIO : public Wait
{
 public:		//------------------------------- public
	BusIO(BusInterface *p = 0);
//	virtual ~BusIO() { }

	virtual int Open(int port)
		{ return (errno = busI->Open(port)); }
	virtual void Close()
		{ busI->Close(); }
	virtual int Error();

	virtual int TestPort(int port)
		{ return OK; }

	virtual int Reset() = 0;

	virtual long Read(int addr, UBYTE *data, long length) = 0;
	virtual long Write(int addr,UBYTE const *data, long length) = 0;

	int GetErrNo()
		{ return errno; }
	int GetLastAddr() const		//useful in I2C Bus transaction,
		{ return last_addr; }	// tell the slave address no ack

	void SetBusInterface(BusInterface *ptr)
		{ if (ptr) busI = ptr; }

	int CheckAbort(int progress = 0);

	virtual void SetDelay();
	virtual void SetDelay(int delay);
	int GetDelay() const
		{ return shot_delay; }

 protected:		//------------------------------- protected

	int	errno;			//eventuale codice di errore
	int	last_addr;

	int shot_delay;		//delay unit to perform bus timing

	BusInterface *busI;

 private:		//------------------------------- private

	int old_progress;
};

#endif
