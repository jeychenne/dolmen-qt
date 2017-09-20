/*
 * Point.cpp
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
 * Created: 01/09/10
 */

#include "Point.h"

DPoint::DPoint()
{
    m_time = -1;
	m_text = "";
}

DPoint::DPoint(double time, const QString &text = "")
{
    m_time = time;
	m_text = text;
}


double DPoint::left() const
{
    return m_time;
}

double DPoint::right() const
{
    return m_time;
}

bool DPoint::precedes(Item *item) const
{
    if (m_time < item->left())
		return true;

	else
		return false;
}

bool DPoint::follows(Item *item) const
{
    if (m_time > item->right())
		return true;

	else
		return false;
}

double DPoint::time()
{
    return m_time;
}

void DPoint::setTime(double value)
{
    m_time = value;
}

void DPoint::writeToXml(QXmlStreamWriter *writer)
{
	writer->writeStartElement("Item");
	writer->writeAttribute("class", "Point");

	writer->writeStartElement("Time");
    writer->writeCharacters(QString::number(m_time, 'f', 15));
	writer->writeEndElement(); // </Time>

	writer->writeStartElement("Text");
	writer->writeCharacters(m_text);
	writer->writeEndElement(); // </Text>

	writer->writeEndElement(); // </Item>
}


