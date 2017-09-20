/*
 * Span.cpp
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

#include "Span.h"

DSpan::DSpan()
{
    m_start = m_end = -1;
    m_text  = "";
}

DSpan::DSpan(double start, double end, const QString &text = "")
{
    m_start = start;
    m_end   = end;
    m_text  = text;
}


double DSpan::left() const
{
    return m_start;
}

double DSpan::right() const
{
    return m_end;
}

bool DSpan::precedes(Item *item) const
{
    if ((isInstance(item, DSpan) && m_end <= item->left())
        || m_end < item->left())
		return true;
	else
		return false;
}

bool DSpan::follows(Item *item) const
{
    if ((isInstance(item, DSpan) && m_start >= item->right())
        || m_start > item->right())
		return true;
	else
		return false;
}

double DSpan::start() const
{
    return m_start;
}

void DSpan::setStart(double value)
{
    m_start = value;
}

double DSpan::end() const
{
    return m_end;
}

void DSpan::setEnd(double value)
{
    m_end = value;
}

void DSpan::writeToXml(QXmlStreamWriter *writer)
{
	writer->writeStartElement("Item");
	writer->writeAttribute("class", "Span");

	writer->writeStartElement("Start");
    writer->writeCharacters(QString::number(m_start, 'f', 15));
	writer->writeEndElement(); // </Start>

	writer->writeStartElement("End");
    writer->writeCharacters(QString::number(m_end, 'f', 15));
	writer->writeEndElement(); // </End>

	writer->writeStartElement("Text");
	writer->writeCharacters(m_text);
	writer->writeEndElement(); // </Text>

	writer->writeEndElement();	// </Item>
}
