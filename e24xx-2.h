//=========================================================================//
//-------------------------------------------------------------------------//
// e24xx-2.h -- Header for for E24xx2 class                                //
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

#ifndef _E24XX2_H
#define _E24XX2_H

#include "e24xx.h"

class E24xx2 : public E24xx
{
  public:		//---------------------------------------- public

	E24xx2(e2AppWinInfo *wininfo = 0, BusIO *busp = 0);
	virtual ~E24xx2();

	int Probe(int probe_size = 0);
//	int Read(int probe = 1);
//	int Write(int probe = 1);
//	int Verify();
//	int BankRollOverDetect(int force);

	int SecurityRead(int &blocks);
	int SecurityWrite(int blocks);
	int HighEnduranceRead(int &block_no);
	int HighEnduranceWrite(int block_no);

  protected:	//--------------------------------------- protected

	virtual int bank_out(BYTE const *copy_buf, int bank_no, long size = -1, long idx = 0);
	virtual int bank_in(BYTE *copy_buf, int bank_no, long size = -1, long idx = 0);

  private:		//--------------------------------------- private

	int const SecurityReadCode;
	int const SecurityWriteCode;
	int const HEnduranceReadCode;
	int const HEnduranceWriteCode;

//	int bank_in(BYTE *copy_buf, long size, int bank);
//	int bank_out(BYTE const *copy_buf, long size, int bank);
};
#endif
