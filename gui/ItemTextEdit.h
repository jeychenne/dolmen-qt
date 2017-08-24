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

#ifndef ITEMTEXTEDIT_H
#define ITEMTEXTEDIT_H

#include <QTextEdit>
#include <QKeyEvent>

class ItemTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	ItemTextEdit(QWidget *parent);
	void exit();
    QString text() const;

signals:
    void editingFinished();
    void editingCancelled();

protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // ITEMTEXTEDIT_H
