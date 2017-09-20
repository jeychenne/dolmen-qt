/*
 * TableColumn.cpp
 *
 * Copyright (C) 2010-2017 Julien Eychenne
 * Copyright (C) 2012 Japanese Society for the Promotion of Science (JSPS)
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
 * Purpose: A table is represented as a list of typed columns.
 * Created: 15/4/2012
 *
 * The development of this module has been supported by the Japanese
 * Society for the Promotion of Science (JSPS), Grant-in-Aid for Scientific
 * Research (B) n°23320121 (2011-2014).
 * Project title: A corpus-based longitudinal study of the interphonological
 * features of Japanese learners of French.
 * Project leader: Sylvain DETEY (Waseda University, Tokyo, Japan).
 */

#include "TableColumn.h"

TableColumn::TableColumn(QString header)
{
    m_header = header;
    m_visible = true;
}

QString TableColumn::header() const
{
    return m_header;
}

void TableColumn::show()
{
    m_visible = true;
}

void TableColumn::hide()
{
    m_visible = false;
}

bool TableColumn::isVisible() const
{
    return m_visible;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ColumnOfBool::ColumnOfBool(QString header) : TableColumn(header)
{

}

QVariant ColumnOfBool::operator[](int index)
{
    return QVariant(m_values[index]);
}

QString ColumnOfBool::insertValue(int index, QString value)
{
    QVariant var(value);
    m_values.insert(index, var.toBool());

    return var.toString();
}

void ColumnOfBool::append(const QString &value)
{
    QVariant var(value);
    m_values.append(var.toBool());
}

QString ColumnOfBool::insertDefaultValue(int index)
{
    m_values.insert(index, false);
    return QVariant(false).toString();
}

void ColumnOfBool::reserve(int capacity)
{
#ifdef Q_OS_UNIX
    Q_UNUSED(capacity)
#else
    m_values.reserve(capacity);
#endif
}

int ColumnOfBool::count() const
{
    return m_values.count();
}

void ColumnOfBool::writeToXml(QXmlStreamWriter *writer)
{
    writer->writeStartElement("Column");
    writer->writeAttribute("name", m_header);
    writer->writeAttribute("type", "boolean");
    writer->writeAttribute("size", QVariant(m_values.count()).toString());

    foreach(bool value, m_values)
    {
        writer->writeStartElement("Cell");
        writer->writeCharacters(QVariant(value).toString());
        writer->writeEndElement(); // </Cell>
    }

    writer->writeEndElement(); // </Column>
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ColumnOfInt::ColumnOfInt(QString header) : TableColumn(header)
{

}

QVariant ColumnOfInt::operator[](int index)
{
    return QVariant(m_values[index]);
}

QString ColumnOfInt::insertValue(int index, QString value)
{
    QVariant v(value);
    m_values.insert(index, v.toInt());
    return v.toString();
}

void ColumnOfInt::append(const QString &value)
{
    QVariant var(value);
    m_values.append(var.toInt());
}

QString ColumnOfInt::insertDefaultValue(int index)
{
    m_values.insert(index, 0);
    return "0";
}

void ColumnOfInt::reserve(int capacity)
{
#ifdef Q_OS_UNIX
    Q_UNUSED(capacity)
#else
    m_values.reserve(capacity);
#endif

}

int ColumnOfInt::count() const
{
    return m_values.count();
}

void ColumnOfInt::writeToXml(QXmlStreamWriter *writer)
{
    writer->writeStartElement("Column");
    writer->writeAttribute("name", m_header);
    writer->writeAttribute("type", "integer");
    writer->writeAttribute("size", QVariant(m_values.count()).toString());

    foreach(int value, m_values)
    {
        writer->writeStartElement("Cell");
        writer->writeCharacters(QVariant(value).toString());
        writer->writeEndElement(); // </Cell>
    }

    writer->writeEndElement(); // </Column>
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ColumnOfReal::ColumnOfReal(QString header) : TableColumn(header)
{

}

QVariant ColumnOfReal::operator[](int index)
{
    return QVariant(m_values[index]);
}

QString ColumnOfReal::insertValue(int index, QString value)
{
    QVariant v(value);
    m_values.insert(index, v.toDouble());
    return v.toString();
}

void ColumnOfReal::append(const QString &value)
{
    QVariant var(value);
    m_values.append(var.toDouble());
}

QString ColumnOfReal::insertDefaultValue(int index)
{
    m_values.insert(index, 0.0);
    return "0.0";
}

void ColumnOfReal::reserve(int capacity)
{
#ifdef Q_OS_UNIX
    Q_UNUSED(capacity)
#else
    m_values.reserve(capacity);
#endif
}

int ColumnOfReal::count() const
{
    return m_values.count();
}

void ColumnOfReal::writeToXml(QXmlStreamWriter *writer)
{
    writer->writeStartElement("Column");
    writer->writeAttribute("name", m_header);
    writer->writeAttribute("type", "real");
    writer->writeAttribute("size", QVariant(m_values.count()).toString());

    foreach(double value, m_values)
    {
        writer->writeStartElement("Cell");
        writer->writeCharacters(QVariant(value).toString());
        writer->writeEndElement(); // </Cell>
    }

    writer->writeEndElement(); // </Column>
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ColumnOfString::ColumnOfString(QString header) : TableColumn(header)
{

}

QVariant ColumnOfString::operator[](int index)
{
    return QVariant(m_values[index]);
}

QString ColumnOfString::insertValue(int index, QString value)
{
    m_values.insert(index, value);
    return value;
}

void ColumnOfString::append(const QString &value)
{
    m_values.append(value);
}

QString ColumnOfString::insertDefaultValue(int index)
{
    m_values.insert(index, "");
    return "";
}

void ColumnOfString::reserve(int capacity)
{
#ifdef Q_OS_UNIX
    Q_UNUSED(capacity)
#else
    m_values.reserve(capacity);
#endif

}


int ColumnOfString::count() const
{
    return m_values.count();
}

void ColumnOfString::writeToXml(QXmlStreamWriter *writer)
{
    writer->writeStartElement("Column");
    writer->writeAttribute("name", m_header);
    writer->writeAttribute("type", "text");
    writer->writeAttribute("size", QVariant(m_values.count()).toString());

    foreach(QString value, m_values)
    {
        writer->writeStartElement("Cell");
        writer->writeCharacters(value);
        writer->writeEndElement(); // </Cell>
    }

    writer->writeEndElement(); // </Column>
}



