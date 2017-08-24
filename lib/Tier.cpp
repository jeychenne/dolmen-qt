/*
 * Tier.cpp
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
 * Purpose:
 * Created: 01/09/10
 */

#include "Tier.h"

Tier::Tier()
{
	// TODO Auto-generated constructor stub

}

int Tier::countItems()
{
	return m_items.size();
}

void Tier::addItem(DPoint *point)
{
	m_items.append(point);
}

void Tier::addItem(DSpan *span)
{
	m_items.append(span);
}

Item* Tier::item(long item_no) const
{
	if (item_no >= m_items.size())
	{
		//TODO: handle this exception
		QString error = tr("Trying to access item %1 in a tier that only has %2 items").arg(item_no+1).arg(m_items.size());
		qDebug() << error;
		return NULL;
	}

	return m_items[item_no];
}

Item* Tier::item(double start, double end) const
{
	foreach (Item *i, m_items)
	{
		if (i->left() == start && i->right() == end)
			return i;
	}

	return NULL;
}

long Tier::indexOf(Item *item) const
{
	long i;

	for (i = 0; i < m_items.size(); ++i)
		if (m_items.at(i) == item)
			return i;

	return -1; // not found
}

double Tier::endTime() const
{
	return m_items.last()->right();
}

Item * Tier::itemAtTime(double t) const
{
	Item *item;

	foreach (item, m_items)
	{
		// find a point
		if (item->left() == t && item->right() == t)
			return item;
		// find a span (if two spans share a boundary, return the leftmost one)
		else if (item->left() <= t && t <= item->right())
			return item;
	}

	return NULL;
}


long Tier::indexAtTime(double t) const
{
	long i, size = m_items.size();
	Item *item;

	for (i = 0; i < size; ++i)
	{
		item = m_items.at(i);
		// find a point
		if (item->left() == t && item->right() == t)
			return i;
		// find a span (if two spans share a boundary, return the leftmost one)
		else if (item->left() <= t && t <= item->right())
			return i;
	}

	return -1;
}


Item * Tier::leftmostItemAtTime(double t) const
{
	long i;
	Item *item;

	for (i = m_items.size() -1; i >= 0; --i)
	{
		item = m_items.at(i);
		if (item->right() <= t)
        {	//qDebug("found leftmost: %lu, %s", i, item->text().toStdString().c_str());
			return item;
		}
	}

	return NULL;
}


Item * Tier::rightmostItemAtTime(double t) const
{
	long i, size = m_items.size();
	Item *item;

	for (i = 0; i < size; ++i)
	{
		item = m_items.at(i);
		if (item->left() >= t)
		{
            //qDebug("found rightmost: %lu", i);
			return item;
		}
	}

	return NULL;
}


QList<Item*> Tier::items() const
{
	return m_items;
}

tier_type_t Tier::type() const
{
    return m_type;
}

void Tier::setType(tier_type_t t)
{
    m_type = t;
}

QString Tier::label() const
{
	return m_label;
}

void Tier::setLabel(const QString &val)
{
	m_label = val;
}

void Tier::writetoXml(QXmlStreamWriter *writer)
{
	writer->writeStartElement("Tier");
	writer->writeAttribute("label", m_label);
	writer->writeStartElement("Header");
	writer->writeEndElement(); // </Header>

	writer->writeStartElement("Items");

	foreach(Item *item, m_items)
	{
		if (isInstance(item, DSpan))
		{
			DSpan *span = qobject_cast<DSpan*>(item);
			span->writeToXml(writer);
		}
		else if (isInstance(item, DPoint))
		{
			DPoint *point = qobject_cast<DPoint*>(item);
			point->writeToXml(writer);
		}
	}

	writer->writeEndElement(); // </Items>
	writer->writeEndElement(); // </Tier>
}

Tier::~Tier()
{
	// TODO Auto-generated destructor stub
}
