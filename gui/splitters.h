/*
 * splitters.h
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
 * Created: 10/10/2010
 */

#ifndef SPLITTERS_H
#define SPLITTERS_H

#ifdef USE_QT5
#include <QtWidgets>
#else
#include <QtGui>
#endif

class MacSplitterHandle : public QSplitterHandle
{
Q_OBJECT
public:
	MacSplitterHandle(Qt::Orientation orientation, QSplitter *parent);
	void paintEvent(QPaintEvent *);
	QSize sizeHint() const;
};

class Splitter : public QSplitter
{
public:
	Splitter(Qt::Orientation orientation, QWidget *parent = 0);
	QSplitterHandle *createHandle();

protected:
	void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // SPLITTERS_H
