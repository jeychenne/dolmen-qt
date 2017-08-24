/*
 * Item.h
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


#ifndef ITEM_H_
#define ITEM_H_

#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "dolmen.h"
#include "lib/metadata.h"
#include "lib/Graph.h"

using namespace metadata;


/*
 * this interface unifies the DSpan and DPoint classes.
 * It provides unified methods to locate the right and
 * left boundaries. Since a Point has only a time property,
 * the right and left boundaries coincide.
 */
class Item : public QObject, public Vertex
{
	Q_OBJECT

	Q_PROPERTY(QString text READ text WRITE setText)

public:
	Item();

    virtual ~Item() = default;

	virtual double left() = 0;
	virtual double right() = 0;

	virtual bool precedes(Item *item) = 0;
	virtual bool follows(Item *item) = 0;

	QString text() const { return m_text; }
	void setText(const QString &);


	void addComment(Comment *c);


protected:
	QString m_text;
	QList<Comment*> m_comments;

};

#endif /* ITEM_H_ */
