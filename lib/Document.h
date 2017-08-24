/*
 * Document.h
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
 * Purpose: implements Documents, which are files containing textual data.
 * Created: 23/02/12
 */

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QtCore>
#include <QTextStream>
#include "lib/DFile.h"

class Document : public DFile
{
    Q_OBJECT

public:
    Document(const QString &path);

    void save();
    void open();
    QString metadataAsHtml(bool withDescription = true) const;
    QString text() const;
    QString leftCotext(int pos) const;
    QString rightCotext(int pos) const;


private:
    QString m_text;

    void readFromDisk();
};

#endif // DOCUMENT_H
