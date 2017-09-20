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

#include "DMRubberBand.h"


DMRubberBand::DMRubberBand(Shape s, QWidget *p, Qt::GlobalColor color, bool transparent) :
        QRubberBand(s, p)
{
	m_color = QColor(color);
    m_transparent = transparent;

	/* default opacity (can be changed if needed) */
#ifdef Q_OS_UNIX
    m_opacity = 0.5;
#else
	m_opacity = 0.1;
#endif

}

void DMRubberBand::setOpacity(float opacity)
{
	m_opacity = opacity;
	m_transparent = (opacity != 0.0);
}

void DMRubberBand::setColor(Qt::GlobalColor color)
{
	m_color = QColor(color);
}

void DMRubberBand::setColor(QColor color)
{
	m_color = color;
}

void DMRubberBand::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	painter.setBackgroundMode(Qt::TransparentMode);
	painter.setPen(QPen(m_color, 1));
	painter.setBrush(QBrush(m_color, Qt::SolidPattern));
	if (m_transparent)
	{
		painter.setOpacity(m_opacity);
	}
	else
		painter.setOpacity(1);

	painter.drawRect(event->rect());
    //painter.fillRect(event->rect(), Qt::red);
}
