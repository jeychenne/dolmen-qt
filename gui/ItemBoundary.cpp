/*
 * ItemBoundary.cpp
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

#include "ItemBoundary.h"

const int BOUNDARY_WIDTH = 3;

ItemBoundary::ItemBoundary()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


SpanBoundary::SpanBoundary(QFrame *parent) :
	ItemBoundary(),
	DMRubberBand(QRubberBand::Line, parent, Qt::blue, false)
{
	this->setOpacity(0.8);
	m_shared = false;
}

bool SpanBoundary::contains(int x) const
{
	return this->geometry().contains(x, 1, false);
}

void SpanBoundary::updatePosition(int x)
{
	QRect rect = this->geometry();
	rect.setX(x);
	rect.setWidth(BOUNDARY_WIDTH);
	this->setGeometry(rect);
}

bool SpanBoundary::isPoint() const
{
	return false;
}

bool SpanBoundary::isShared() const
{
	return m_shared;
}

void SpanBoundary::setShared()
{
	m_shared = true;
}

void SpanBoundary::setSelected(bool sel)
{
	qDebug() << "SpanBoundary::setSelected()";
	QColor color = sel ? QColor("#ffa800") : QColor(Qt::blue);
	this->setColor(color);
	this->update();
}


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

PointBoundary::PointBoundary(QFrame *parent) :
	ItemBoundary()
{
	m_parent = parent;
    rb_top = new DMRubberBand(QRubberBand::Line, parent, Qt::blue, false);
    rb_bottom = new DMRubberBand(QRubberBand::Line, parent, Qt::blue, false);

    rb_top->setOpacity(0.8);
    rb_bottom->setOpacity(0.8);
}


QPair<int,int> PointBoundary::xpos() const
{
	QPair<int,int> pos;
    pos.first = rb_top->geometry().x();
    pos.second = pos.first + rb_top->geometry().width();

	return pos;
}

bool PointBoundary::contains(int x) const
{
    return rb_top->geometry().contains(x, 1, false);
}

bool PointBoundary::isPoint() const
{
	return true;
}

bool PointBoundary::isShared() const
{
	return false;
}

/* The boundary is displayed as two half-rubberbands with 30 pixels in the middle for the label
   (returns middle point for convenience)
*/
void PointBoundary::setGeometry(int x, int y, int w, int h)
{
	const int pad = 12;
	int height = (int) floor((double)h / 2) - pad;

    rb_top->setGeometry(x, y, w, height);
	y = height + pad * 2;
    rb_bottom->setGeometry(x, y, w, height);
}

void PointBoundary::show()
{
    rb_top->show();
    rb_bottom->show();
}

void PointBoundary::hide()
{
    rb_top->hide();
    rb_bottom->hide();
}

void PointBoundary::updatePosition(int x)
{
    QRect top = rb_top->geometry();
    QRect bot = rb_bottom->geometry();

	top.setX(x);
	top.setWidth(BOUNDARY_WIDTH);
	bot.setX(x);
	bot.setWidth(BOUNDARY_WIDTH);

    rb_top->setGeometry(top);
    rb_bottom->setGeometry(bot);
}

void PointBoundary::setSelected(bool selected)
{
	QColor color = selected ? QColor("#ffa800") : QColor(Qt::blue);

    rb_top->setColor(color);
    rb_bottom->setColor(color);

    rb_top->update();
    rb_bottom->update();
}


PointBoundary::~PointBoundary()
{
    delete rb_top;
    delete rb_bottom;
}

