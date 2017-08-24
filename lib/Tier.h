/*
 * Tier.h
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

#ifndef TIER_H_
#define TIER_H_

#include <QObject>
#include <QList>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "lib/Item.h"
#include "lib/Span.h"
#include "lib/Point.h"
#include "lib/metadata.h"

using namespace metadata;

typedef enum
{
    DM_TIER_TYPE_SPAN,
    DM_TIER_TYPE_POINT
} tier_type_t;

class Tier : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString label READ label WRITE setLabel)

public:
	Tier();
	virtual ~Tier();

	void addItem(DPoint *point);
	void addItem(DSpan *span);

	int countItems();
	Item* item(long item_no) const;
	Item* item(double start, double end) const;
	QList<Item*> items() const;
    tier_type_t type() const;
    void setType(tier_type_t t);

	long indexOf(Item *item) const;
	Item* itemAtTime(double t) const;
	long indexAtTime(double t) const;
	Item *leftmostItemAtTime(double t) const;
	Item *rightmostItemAtTime(double t) const;
	double endTime() const;

	QString label() const;
	void setLabel(const QString &val);

	void writetoXml(QXmlStreamWriter *writer);
private:
	QList<Item*> m_items;
	QString m_label;
    tier_type_t m_type;

};

#endif /* TIER_H_ */
