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


#ifndef DMLINEEDIT_H
#define DMLINEEDIT_H

#include <QLineEdit>

class DMLineEdit : public QLineEdit
{
	Q_OBJECT

public:
    DMLineEdit(const QString &defaultText);
	QString text() const;

public slots:
	void focusInEvent(QFocusEvent *event);
	void focusOutEvent(QFocusEvent *event);

private:
	QString defaultText;

    void setDefaultText();
};

#endif // DMLINEEDIT_H
