/*
 * Bookmark.h
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
 * Purpose: Abstract interface for anything that can be bookmarked
 * Created: 06/04/2011
 */

#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QString>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "DFile.h"
#include "BrowserElement.h"
#include "utils/routines.h"



class Bookmark : public IBrowserElement
{
public:
    Bookmark();
    ~Bookmark() = default;
	virtual bool readFromXml(QXmlStreamReader *reader) = 0; // return true if parse is successful
	virtual void writeToXml(QXmlStreamWriter *writer) = 0;
	virtual QString type() const = 0;
    virtual void openInPraat();
	virtual QString tooltip() const = 0;

	QString notes() const;
	void setNotes(QString s);
	QString title() const;
	void setTitle(QString s);


protected:
	QString m_title, m_notes;

};

Q_DECLARE_INTERFACE(Bookmark, "net.dolmen-linguistics.bookmark/1.0")

#endif // BOOKMARK_H
