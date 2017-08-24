/*
 * ItemBoundary.h
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
 * Purpose: Classes to represent boundaries in the GUI
 * Created: 19/08/2011
 */

#ifndef ITEMBOUNDARY_H
#define ITEMBOUNDARY_H

#include <cmath>
#include <QFrame>
#include "DMRubberBand.h"
#include "lib/Span.h"
#include "lib/Point.h"

class ItemBoundary
{
public:
	ItemBoundary();
    virtual ~ItemBoundary() {}
	virtual void setSelected(bool sel) = 0;
	virtual void updatePosition(int x) = 0;
	virtual bool isPoint() const = 0;
	virtual bool isShared() const = 0;
};

class SpanBoundary : public ItemBoundary, public DMRubberBand
{
public:
	SpanBoundary(QFrame *parent);
	bool contains(int x) const;
	void setSelected(bool sel);
	void updatePosition(int x);
	bool isPoint() const;
	bool isShared() const;
	void setShared();

private:
	bool m_shared;
};

class PointBoundary : public ItemBoundary
{
public:
	PointBoundary(QFrame *parent);
	~PointBoundary();
	QPair<int,int> xpos() const;
	void setGeometry(int x, int y, int w, int h);
	void show();
	void hide();
	void updatePosition(int x);
	void setSelected(bool sel);
	bool contains(int x) const;
	bool isPoint() const;
	bool isShared() const;

private:
    QFrame       *m_parent;
    DMRubberBand *rb_top, *rb_bottom;
};

#endif // ITEMBOUNDARY_H
