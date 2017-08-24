/*
 * ItemTextEdit.h
 *
 * Copyright (C) 2010-2013 Julien Eychenne
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
 * Purpose: A custom line edit for editing an item's text in a TierView
 * Created: 14/08/2011
 */


#include "ItemTextEdit.h"

ItemTextEdit::ItemTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
    this->setFocusPolicy(Qt::StrongFocus);
}

QString ItemTextEdit::text() const
{
    return this->toPlainText();
}

void ItemTextEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        event->ignore();
        emit editingFinished();
    }
    else if (event->key() == Qt::Key_Escape)
    {
        event->ignore();
        emit editingCancelled();
    }
    else
        QTextEdit::keyPressEvent(event);
}

void ItemTextEdit::exit()
{
	this->hide();
	emit editingFinished();
}
