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

#ifndef _EEPTYPES_H
#define _EEPTYPES_H

#include "globals.h"
#include "defines.h"

#include <QVector>
#include <QString>

#define EID_INVALID     0

#define NO_OF_EEPTYPE   0x12


// EK 2017
// common structure for chips
struct chipMap
{
	int  prog_sz;  // size of program data (code)
	int  data_sz;  // size of memory data
	int  wpg_sz;   // page size
	int  adr_sz;   // dimensione dello spazio di indirizzamento in numero di banchi
	int  boot;     // boot address
};


/**
 * @brief BitInfo is the main structure for displaying in the QTreeWidget
 */
struct BitInfo
{
	quint16 bit;        // bit number
	quint16 idx;
	QString ShortDescr; // first column
	QString LongDescr;  // second column
	//const QString ExtDescr;   // for additional infos
};


/**
 * @brief MaskDescr is the structure for QComboBoxes with help information
 */
struct MaskDescr
{
	QString mask;      // in mask are coded name of mask and bits
	QString LongDescr; //
	QString ExtDescr;  // for additional infos
};


struct chipBits
{
	QStringList chNames; // chip names for same descriptions of chip bits
	QVector<BitInfo> fuse;
	QVector<MaskDescr> fuseDescr;
	QVector<BitInfo> lock;
	QVector<MaskDescr> lockDescr;
};


struct icElement
{
	QString  name;     // for menu items
// 	QString  defName;  // for defines
	quint32  id;       // groupid | subtype
	quint16  sign;     // signature for detecting
	quint16  reserv;
	chipMap  chMap;    // memory structure
	chipBits helper;   // for the popup help window
};


struct groupElement
{
	QString menuName;     // for menu items
	//QString defName;  // for defines
	QVector<quint32> vId; // pre_types
	QVector <icElement> vChip;
};



#endif
