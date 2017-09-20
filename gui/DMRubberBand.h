/*
 * DMRubberBand.h
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
 * Purpose: a custom rubberband
 * Created: 06/05/2011
 */

#ifndef DMRUBBERBAND_H
#define DMRUBBERBAND_H


#include <QPoint>
#include <QRubberBand>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>

//#include <qwt/qwt_plot.h>

class DMRubberBand : public QRubberBand
{
public:
    DMRubberBand(Shape s, QWidget *p = NULL, Qt::GlobalColor color = Qt::blue, bool transparent = true);
	void setColor(Qt::GlobalColor color);
	void setColor(QColor color);
	void setOpacity(float opacity);

protected:
	virtual void paintEvent(QPaintEvent *event);

private:
	QColor		m_color;
	bool		m_transparent;
	float		m_opacity;

};



#endif // DMRUBBERBAND_H
