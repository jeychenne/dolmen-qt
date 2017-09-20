/*
 * DMLineEdit.h
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
 * Created: 15/05/2010
 */

#include "DMLineEdit.h"


DMLineEdit::DMLineEdit(const QString &defaultText) : QLineEdit()
{
	this->defaultText = defaultText;
	setDefaultText();
}

QString DMLineEdit::text() const
{
	QString txt = QLineEdit::text();

	if (txt == defaultText)
		return "";
	else
		return txt;
}

void DMLineEdit::setDefaultText()
{
	setText(defaultText);
	setStyleSheet("QLineEdit { color: gray; font-style: italic }");
}

void DMLineEdit::focusInEvent(QFocusEvent *event)
{
	if (text() == "")
	{
		setText(""); // make sure text is not default text
		setStyleSheet("QLineEdit { }");
	}

	QLineEdit::focusInEvent(event);
}

void DMLineEdit::focusOutEvent(QFocusEvent *event)
{
	if (text() == "")
		setDefaultText();

	QLineEdit::focusOutEvent(event);
}
