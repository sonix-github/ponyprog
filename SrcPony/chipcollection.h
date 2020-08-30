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

#ifndef _CHIPCOLLECTION_H
#define _CHIPCOLLECTION_H

#include "globals.h"
#include "defines.h"

#include <QVector>
#include <QString>
#include <QDebug>
#include <QXmlReader>
#include <QDomNode>

#define EID_INVALID     0


/**
 * @brief BitInfo is the main structure for displaying in the QTreeWidget
 */
struct BitInfo
{
	quint16 bit;        // bit number
	quint16 idx;
	QString ShortDescr; // first column
	QString LongDescr;  // second column
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
	// memory structure
	qint32   prog_sz;  // size of program data (code)
	qint32   data_sz;  // size of memory data
	qint32   wpg_sz;   // page size
	qint32   adr_sz;   // dimensione dello spazio di indirizzamento in numero di banchi
	qint32   boot;     // boot address
};


class cGroupElement
{
  public:
	QString menuName;          // for menu items
	//QString defName;         // for defines
	QVector<quint32> vId;      // pre_types
	QVector <icElement> vChip; // memory inforamtion
	QVector <chipBits> helper; // help information
};


class cChipCollection
{
  public:
	// will be deprecated
	quint32 GetSubType(quint32 type);
	quint32 GetPriType(quint32 type);
	cGroupElement *GetMenuGroupPointer(const QString &menuStr);
	quint32 GetTypeFromSize(quint32 type, int size);
	int GetTypeSize(quint32 type);
	int GetAddrSize(quint32 type);
	int GetTypeSplit(quint32 type);
	int GetTypeWPageSize(quint32 type);
	QString GetTypeString(quint32 type);
	quint32 GetSignatureType(quint32 pri_type, quint16 sign);
	quint32 GetFirstFromPritype(quint32 pritype);

	/**
	 * @brief search chip name in vectors
	 *
	 */
	quint32 GetTypeFromString(const QString &name);
	bool ReadConfigFromXml(const QString &filename);
	chipBits *GetTypeFuses(quint32 type);

  private:
	bool addGroup(cGroupElement *g);
	icElement *getChipPointer(quint32 type);

	bool parseNode(const QDomNode &nd, QVector<BitInfo> &b, QVector<MaskDescr> &d);

	int convertSize(const QString &s);

  public:
	QVector <cGroupElement *> icGroups;
};

#endif // _CHIPCOLLECTION_H
