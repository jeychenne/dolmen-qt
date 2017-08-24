/*
 * Point.h
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

#ifndef POINT_H_
#define POINT_H_

#include "lib/Item.h"

// Point conflicts with Point in MacTypes.h
class DPoint: public Item
{
	Q_OBJECT
	Q_PROPERTY(double time READ time WRITE setTime)

public:
	DPoint();
	DPoint(double time, const QString &text);

	double left();
	double right();

	bool precedes(Item *item);
	bool follows(Item *item);

	double time();
	void setTime(double);

	void writeToXml(QXmlStreamWriter *);

private:
	double m_time;
};

#endif /* POINT_H_ */
