//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2017   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: infomdlg.cpp,v 1.7 2009/11/16 23:40:43 lancos Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <QDebug>
#include <QString>

#include "string_table.h"
#include "globals.h"
#include "infomdlg.h"

using namespace Translator;


//======================>>> infoModalDialog::infoModalDialog <<<==================
e24xx_infoModalDialog::e24xx_infoModalDialog(QWidget* bw, int rlv, uint16_t crc, long size, const QString title) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(title);

	if (rlv == 1)
	{
		txiFrom->setText(STR_MSGYES);// SetCommandLabel(txtRllBnk, STR_MSGYES, e24xxCmds);
	}
	else if (rlv == 2)
	{
		txiFrom->setText(STR_MSGNO);//SetCommandLabel(txtRllBnk, STR_MSGNO, e24xxCmds);
	}
	else
	{
		txiFrom->setText(STR_MSGUNKNOWN);//SetCommandLabel(txtRllBnk, STR_MSGUNKNOWN, e24xxCmds);
	}

	lblFrom->setText(STR_MSGCRC); // crc label
	lblTo->setText(STR_MSGEEPSIZE); // size label
	lblVal->setText(STR_MSGFLASHSIZE); // flash size label

	QString str;
	str = QString().sprintf("%04Xh", crc);
	txiTo->setText(str);
	txiTo->setReadOnly(true);

	str = QString().sprintf("%ld Byte", size);
	txiVal->setText(str);
	txiVal->setReadOnly(true);

	lblVal->setHidden(true);
	txiVal->setHidden(true);

	pushOk->setText(STR_BTNCLOSE);
	pushCancel->setHidden(true);

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	//      connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


//===================>>> infoModalDialog::~infoModalDialog <<<====================
e24xx_infoModalDialog::~e24xx_infoModalDialog()
{
	qDebug() << "infoModalDialog::~infoModalDialog()";
}


void e24xx_infoModalDialog::onOk()
{
	accept();
}


//======================>>> infoModalDialog::infoModalDialog <<<==================
other_infoModalDialog::other_infoModalDialog(QWidget* bw, long fsize, long esize, uint16_t crc, const QString title) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(title);

	QString str;

	lblFrom->setText(STR_MSGCRC); // crc label
	lblTo->setText(STR_MSGEEPSIZE); // size label
	lblVal->setText(STR_MSGFLASHSIZE); // flash size label

	str = QString().sprintf( "%04Xh", crc);
	txiFrom->setText(str);
	txiFrom->setReadOnly(true);

	str = QString().sprintf( "%ld Byte", fsize);
	txiTo->setText(str);
	txiTo->setReadOnly(true);

	str = QString().sprintf( "%ld Byte", esize);
	txiVal->setText(str);
	txiVal->setReadOnly(true);

	pushOk->setText(STR_BTNCLOSE);
	pushCancel->setHidden(true);

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	//      connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


//===================>>> infoModalDialog::~infoModalDialog <<<====================
other_infoModalDialog::~other_infoModalDialog()
{
	qDebug() << "infoModalDialog::~infoModalDialog()";
}


void other_infoModalDialog::onOk()
{
	accept();
}


notesModalDialog::notesModalDialog(QWidget* bw, QString &id, QString &cm, const QString title) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(title);

	qDebug() << "notesModalDialog::notesModalDialog()";

	id_ptr = &id;
	cm_ptr = &cm;

	lblStrID->setText(STR_MSGCHIPID);
	lblCommnt->setText(STR_MSGNOTE);

	if (id.length() && cm.length())
	{
		id_txt = id;
		txiStrID->setText(id_txt);
		cmt_txt = cm;
		txiCommnt->setText(cmt_txt);
	}

	pushOk->setText(STR_BTNOK);
	pushCancel->setText(STR_BTNCANC);

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


notesModalDialog::~notesModalDialog()
{
	qDebug() << "notesModalDialog::~notesModalDialog()";
}


void notesModalDialog::onOk()
{
	cmt_txt = txiCommnt->text();
	id_txt = txiStrID->text();

	*id_ptr = id_txt;
	*cm_ptr = cmt_txt;

	accept();
}


editModalDialog::editModalDialog(QWidget* bw, int curval, const QString title) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(title);

	qDebug() << "editModalDialog::editModalDialog()";

	if (curval < 0)
	{
		curval = 0;
	}

	lblFrom->setText(STR_MSGHEX);
	lblTo->setText(STR_MSGDECIMAL);
	lblVal->setText(STR_MSGCHAR);

	oldval = curval;

	QString str;
	str = QString().sprintf("%02X", curval);
	txiFrom->setText(str);

	connect(txiFrom, SIGNAL(changed()), this, SLOT(onEdit()));

	str = QString().sprintf("%d", curval);
	txiTo->setText(str);
	connect(txiTo, SIGNAL(changed()), this, SLOT(onEdit()));

	str = QString().sprintf("%c", curval);
	txiVal->setText(str);
	connect(txiVal, SIGNAL(changed()), this, SLOT(onEdit()));

	pushOk->setText(STR_BTNOK);
	pushCancel->setText(STR_BTNCANC);

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


editModalDialog::~editModalDialog()
{
	qDebug() << "editModalDialog::~editModalDialog()";
}


void editModalDialog::onOk()
{
	accept();
}

int editModalDialog::GetVal()
{
	return oldval;
}


void editModalDialog::onEdit()
{
	int newval;
	bool ok;

	QLineEdit *l = static_cast<QLineEdit*>(sender());
	disconnect(txiFrom, SIGNAL(changed()), this, SLOT(onEdit())); // hex
	disconnect(txiTo, SIGNAL(changed()), this, SLOT(onEdit())); // dec
	disconnect(txiVal, SIGNAL(changed()), this, SLOT(onEdit()));  // char

	if (l == txiFrom)  // hex
	{
		QString n = txiFrom->text();
		newval = n.toInt(&ok, 16);
	}

	if (l == txiTo)  // dec
	{
		QString n = txiFrom->text();
		newval = n.toInt(&ok, 10);
	}

	if (l == txiVal)  // char
	{
		QString n = txiFrom->text();

		if (n.length() > 1)
		{
			ok = false;
		}
		else
		{
			newval = static_cast<unsigned char>(n.at(0).toLatin1());
			ok = true;
		}
	}

	if (ok == true)
	{
		if (oldval != newval)
		{
			oldval = newval;
			txiFrom->setText(QString().sprintf("%02X", newval));
			txiTo->setText(QString().sprintf("%d", newval));
			txiVal->setText(QString().sprintf("%c", newval));
		}
	}

	connect(txiFrom, SIGNAL(changed()), this, SLOT(onEdit()));
	connect(txiTo, SIGNAL(changed()), this, SLOT(onEdit()));
	connect(txiVal, SIGNAL(changed()), this, SLOT(onEdit()));
}


const unsigned int MAXLEN = 80;

editModalDialog2::editModalDialog2(QWidget* bw, const QString curval, const QString title) :
	QDialog(bw)
{
	setupUi(this);

	setWindowTitle(title);

	lblFrom->setHidden(true);
	txiFrom->setHidden(true);

	lblTo->setHidden(true);
	txiTo->setHidden(true);

	lblVal->setText(STR_MSGCHAR);

	qDebug() << "editModalDialog::editModalDialog()";

	val = curval;

	if (val.length() > MAXLEN)
	{
		val = val.left(MAXLEN);
	}

	txiVal->setText(val);

	pushOk->setText(STR_BTNOK);
	pushCancel->setText(STR_BTNCANC);

	connect(pushOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


editModalDialog2::~editModalDialog2()
{
	qDebug() << "editModalDialog::~editModalDialog()";
}


QString editModalDialog2::GetVal()
{
	return val;
}

void editModalDialog2::onOk()
{
	val = txiVal->text();

	if (val.length() > MAXLEN)
	{
		val = val.left(MAXLEN);
	}

	accept();
}

