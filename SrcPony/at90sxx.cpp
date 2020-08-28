//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2020   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
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
// along with this program (see LICENSE);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//=========================================================================//

#include "types.h"
#include "globals.h"
#include "at90sxx.h"            // Header file
#include "errcode.h"

#include <QDebug>

#include "e2awinfo.h"
#include "e2cmdw.h"

At90sxx::At90sxx(e2AppWinInfo *wininfo, BusIO *busp)
	:       Device(wininfo, busp, 1 /*BANK_SIZE*/)
{
	qDebug() << Q_FUNC_INFO;
}

At90sxx::~At90sxx()
{
	qDebug() << Q_FUNC_INFO;
}

int At90sxx::SecurityRead(quint32 &bits)
{
	int rv = Probe();               //No size probe needed, just probe for presence

	if (rv > 0)
	{
		bits = GetBus()->ReadLockBits(GetAWInfo()->GetId());
		rv = OK;
	}

	return rv;
}

int At90sxx::SecurityWrite(quint32 bits)
{
	int rv = Probe();               //No size probe needed, just probe for presence

	if (rv > 0)     //Try to write even with AutoXXX device setted
	{
		rv = GetBus()->WriteLockBits(bits, GetAWInfo()->GetId());
	}

	return rv;
}

int At90sxx::FusesRead(quint32 &bits)
{
	int rv = Probe();               //No size probe needed, just probe for presence

	if (rv > 0)
	{
		bits = GetBus()->ReadFuseBits(GetAWInfo()->GetId());
		rv = OK;
	}

	return rv;
}

int At90sxx::FusesWrite(quint32 bits)
{
	int rv = Probe();               //No size probe needed, just probe for presence

	if (rv > 0)
	{
		rv = GetBus()->WriteFuseBits(bits, GetAWInfo()->GetId());
	}

	return rv;
}

/**
 * code[0]: Vendor Code
 *                      0x1E (manufactured by Atmel)
 * code[1]: Part Family and Flash Size
 *                      0x90 AVR with 1K flash
 *                      0x91 AVR with 2K flash
 *                      0x92 AVR with 4K flash
 *                      0x93 AVR with 8K flash
 *                      0x94 AVR with 16K flash
 *                      0x95 AVR with 32K flash
 *                      0x96 AVR with 64K flash
 *                      0x97 AVR with 128K flash
 * code[2]: Part Number
 *
 * Part number identification table
 * [ 1 ]        [ 2 ]
 * 0x90         0x01            AT90S1200 (1K + 64)
 * 0x91         0x01            AT90S2313 (2K + 128)
 * 0x92         0x01            AT90S4414 (4K + 256)
 * 0x93         0x01            AT90S8515 (8K + 512)
 *
 * 0x06         0x01            ATmega603 (64K + 2K) first few pieces
 * 0x01         0x01            ATmega103 (128K + 4K) first few pieces
 * 0x96         0x01            ATmega603 (64K + 2K)
 * 0x97         0x01            ATmega103 (128K + 4K)
 *
 * 0x91         0x02            AT90S2323 (2K + 128)
 * 0x91         0x03            AT90S2343 (2K + 128)
 *
 * 0x92         0x03            AT90S4434 (4K + 256)
 * 0x93         0x03            AT90S8535 (8K + 512)
 *
 * 0x91         0x05            AT90S2333 (2K + 128)
 * 0x92         0x02            AT90S4433 (4K + 256)
 *
 * 0x93         0x04            AT90C8534 (8K + 512)
 *
 * 0x93         0x07            ATmega8 (8K + 512)
 * 0x94         0x03            ATmega16 (16K + 512)
 *
 * 0x94         0x01            ATmega161 (16K + 512)
 * 0x94         0x02            ATmega163 (16K + 512)
 *
 * 0x95         0x01            ATmega323 (32K + 1K)
 * 0x97         0x02            ATmega128 (128K + 4K)
 * 0x96     0x02        ATmega64 (64K + 2K)
 *
 * 0x90         0x03            ATtiny10 (1K)
 * 0x90         0x04            ATtiny11 (1K)
 * 0x90         0x05            ATtiny12 (1K + 64)
 * 0x90         0x06            ATtiny15 (1K + 64)
 *
 * 0x91         0x06            ATtiny22 (2K + 128)
 * 0x91         0x07            ATtiny28 (2K)
 **/


int At90sxx::QueryType(quint32 &type)
{
	int rv = DEVICE_UNKNOWN;

	int code[3];

	code[0] = GetBus()->ReadDeviceCode(0);
	code[1] = GetBus()->ReadDeviceCode(1);
	code[2] = GetBus()->ReadDeviceCode(2);

	qDebug() << "At90sxx::ParseID() *** " << (hex) << code[0] << " - " << code[1] << " - " << code[2] << (dec);

	detected_type = type = 0;
	detected_signature = "";

	//
	if (code[0] == 0x00 && code[1] == 0x01 && code[2] == 0x02)
	{
		//device is locked
		rv = DEVICE_LOCKED;
	}
	else if (code[0] == 0x1E)
	{
// 		quint16 pri_type = AT90SXX; // ATtiny , ATmega
		quint16 sign = (code[1] << 8) + code[2];

		quint32 pri[] = {AT90SXX, ATtiny, ATmega, 0};

		for (int i = 0; pri[i] != 0; i++)
		{
			type = GetAWInfo()->GetSignatureType(pri[i], sign);

			if (type != EID_INVALID)
			{
				detected_type = type;
				detected_signature.sprintf("%02X-%02X-%02X", code[0], code[1], code[2]);
				rv = OK;
				return rv;
			}
		}
	}
	else
	{
		//Preproduction device or device missing
		rv = DEVICE_UNKNOWN;
	}

	return rv;
}

// Probe() can change the bus according to the device detected.
// Note that the Verify() operation don't call probe() before
// to read the device, so it use the current bus. In the case
// of AutoXXX selected the bus setted is the AT1200S.
// So it works because of the Read routine is the same just for
// AT1200Bus, AT90SXXBus and ATmegaBus.

// Detect the size of the device. Note that if the
// device is locked, the probe don't work and returns the
// error DEVICE_LOCKED.
// If the device is missing return 0.
int At90sxx::Probe(int probe_size)
{
	int rv;

	qDebug() << "At90sxx::Probe(" << probe_size << ") IN";

	if (cmdWin->GetIgnoreFlag())
	{
		rv = GetSize();
	}
	else
	{
		quint32 type;
		rv = QueryType(type);
// 		quint32 subtype = GetAWInfo()->GetE2PSubType(type);

		if (rv == OK)
		{
			if (GetAWInfo()->GetId() == type)
			{
				rv = GetSize();
			}
			else
			{
				rv = DEVICE_BADTYPE;
			}
		}
	}

	qDebug() << "At90sxx::Probe() = " << rv << " **  OUT";

	return rv;
}

int At90sxx::Erase(int probe, int type)
{
	int rv = OK;

	if ((type & PROG_TYPE) && (type & DATA_TYPE))
	{
		GetBus()->Erase();
		rv = Probe(probe || GetNoOfBank() == 0);
	}
	else
	{
		rv = NOTSUPPORTED;
	}

	return rv;
}

int At90sxx::Read(int probe, int type)
{
	int rv = Probe(probe || GetNoOfBank() == 0);

	if (rv > 0)
	{
		if (GetSize() >= GetSplitted())
		{
			if (type & PROG_TYPE)
			{
				rv = ReadProg();
			}

			if (rv > 0 && GetSize() > GetSplitted())        //Check for DATA size
			{
				if (type & DATA_TYPE)
				{
					rv = ReadData();
				}
			}

			if (rv > 0)
			{
				if (type & CONFIG_TYPE)
				{
					// read the fuses
					quint32 id = GetAWInfo()->GetId();
					quint32 f = GetBus()->ReadFuseBits(id);
					GetAWInfo()->SetFuseBits(f);

					f = GetBus()->ReadLockBits(id);
					GetAWInfo()->SetLockBits(f);
				}
			}
		}
	}

	return rv;
}

int At90sxx::Write(int probe, int type)
{
	//      if ( (type & PROG_TYPE) && (type & DATA_TYPE) )
	if ((type & PROG_TYPE))                 //Because to write the flash we must erase ALL the device (a msg may alert that doing so the DATA may be erased too)
	{
		GetBus()->Erase();
	}

	int rv = Probe(probe || GetNoOfBank() == 0);

	if (rv > 0)
	{
		if (GetSize() >= GetSplitted())
		{
			if (type & PROG_TYPE)
			{
				rv = WriteProg();
			}

			if (rv > 0 && GetSize() > GetSplitted())        //check for DATA size
			{
				if (type & DATA_TYPE)
				{
					rv = WriteData();
				}
			}

			if (rv > 0)
			{
				if (type & CONFIG_TYPE)
				{
					quint32 id = GetAWInfo()->GetId();
					if (id == AT90S4433 || id == AT90S2333)
					{
						//write the locks
						quint32 f = GetAWInfo()->GetLockBits();
						GetBus()->WriteLockBits(f, id);

						GetBus()->WaitMsec(100);

						//write the fuses
						f = GetAWInfo()->GetFuseBits();
						GetBus()->WriteFuseBits(f, id);
					}
					else
					{
						//write the fuses
						quint32 f = GetAWInfo()->GetFuseBits();
						GetBus()->WriteFuseBits(f, id);

						//write the locks
						f = GetAWInfo()->GetLockBits();
						GetBus()->WriteLockBits(f, id);
					}
				}
			}
		}
	}

	return rv;
}

int At90sxx::Verify(int type)
{
	if (GetSize() == 0)
	{
		return BADPARAM;
	}

	int rval = -1;

	if (GetSize() >= GetSplitted())
	{
		unsigned char *localbuf = new unsigned char[GetSize()];

		int v_data = OK, v_prog = OK, v_config = OK;

		if (type & PROG_TYPE)
		{
			v_prog = VerifyProg(localbuf);
		}

		if (type & DATA_TYPE)
		{
			v_data = VerifyData(localbuf);
		}

		if (type & CONFIG_TYPE)
		{
			// read the fuses & locks
			quint32 f = GetBus()->ReadFuseBits(GetAWInfo()->GetId());
			quint32 l = GetBus()->ReadLockBits(GetAWInfo()->GetId());

			if (GetAWInfo()->GetLockBits() == l && GetAWInfo()->GetFuseBits() == f)
			{
				v_config = OK;
			}
			else
			{
				v_config = 1;
			}
		}

		rval = (v_prog == OK && v_data == OK && v_config == OK) ? 1 : 0;

		delete[] localbuf;
	}

	return rval;
}
