/*
 * dolmen.h
 *
 * Copyright (C) 2010-2017 Julien Eychenne 
 *
 * This file is part of Dolmen.
 *
 * Dolmen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (GPL) as
 * published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * Dolmen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Dolmen. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:  Julien Eychenne
 * Purpose:
 * Created: //
 */

#include "NumericValueEdit.h"


NumericValueEdit::NumericValueEdit(QString desc) : QHBoxLayout()
{
	this->desc = desc;
	op_box = new QComboBox;
	num_edit = new DMLineEdit(tr("numeric value"));

	QStringList ops;
	ops << DM_EQ << DM_NE << DM_LT << DM_LE << DM_GE << DM_GT;

	foreach (QString op, ops)
		op_box->addItem(op);

	this->addWidget(op_box);
	this->addWidget(num_edit);
	op_box->setFocusProxy(num_edit);
}

QString NumericValueEdit::description() const
{
	return desc;
}

QString NumericValueEdit::opcode() const
{
	return op_box->currentText();
}

double NumericValueEdit::value() const
{
	return num_edit->text().toDouble();
}

void NumericValueEdit::show()
{
	num_edit->show();
	op_box->show();
}

void NumericValueEdit::hide()
{
	num_edit->hide();
	op_box->hide();
}
