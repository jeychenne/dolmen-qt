/*
 * Script.h
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
 * Purpose: a Lua script
 * Created: 15/09/2017
 */

#include <QFileInfo>
#include "Script.h"

Script::Script() :
    IBrowserElement()
{

}

bool Script::readFromXml(QXmlStreamReader *reader)
{
    while (! (reader->isEndElement() && reader->name() == "Script"))
    {
        if (reader->isStartElement())
        {
            if (reader->name() == "Path") {
                m_path = reader->readElementText();
            }
            else if (reader->name() == "Label") {
                m_label = reader->readElementText();
            }
        }

        reader->readNext();
    }

    return true;
}

void Script::writeToXml(QXmlStreamWriter *writer)
{
    writer->writeStartElement("Script");

    writer->writeStartElement("Label");
    writer->writeCharacters(m_label);
    writer->writeEndElement(); // Label

    writer->writeStartElement("Path");
    writer->writeCharacters(m_path);
    writer->writeEndElement();

    writer->writeEndElement(); // Script
}

QString Script::path() const
{
    return m_path;
}

void Script::setPath(const QString &path)
{
    m_path = path;
}

QString Script::label() const
{
    if (m_label.isEmpty())
    {
        QFileInfo info(m_label);
        return info.baseName();
    }

    return m_label;
}

void Script::setLabel(const QString &label)
{
    m_label = label;
}

void Script::open()
{

}
