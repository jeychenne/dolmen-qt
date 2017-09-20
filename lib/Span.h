/*
 * Span.h
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


#ifndef SPAN_H_
#define SPAN_H_

#include "dolmen.h"
#include "lib/Item.h"


class DSpan : public Item
{
	Q_OBJECT
	Q_PROPERTY(double start READ start WRITE setStart)
	Q_PROPERTY(double end READ end WRITE setEnd)


public:
	DSpan();
	DSpan(double start, double end, const QString &text);

    double left() const override;
    double right() const override;

    bool precedes(Item *item) const override;
    bool follows(Item *item) const override;

	double start() const;
	void setStart(double);

	double end() const;
	void setEnd(double);

	void writeToXml(QXmlStreamWriter *);

private:
	double m_start, m_end;

};


#endif /* SPAN_H_ */
