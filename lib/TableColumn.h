/*
 * TableColumn.h
 *
 * Copyright (C) 2010-2013 Julien Eychenne
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

#ifndef TABLECOLUMN_H
#define TABLECOLUMN_H

#include <QObject>
#include <QList>
#include <QVariant>
#include <QXmlStreamWriter>

class TableColumn
{
public:
    TableColumn(QString header);
    virtual QString insertValue(int index, QString value) = 0;
    virtual void append(const QString &value) = 0;
    virtual QString insertDefaultValue(int index) = 0;
    virtual int count() const = 0;
    virtual void reserve(int capacity) = 0;
    virtual QVariant operator[](int index) = 0;
    virtual void writeToXml(QXmlStreamWriter *writer) = 0;
    QString header() const;
    bool isVisible() const;
    void show();
    void hide();

protected:
    QString m_header;
    bool m_visible;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ColumnOfBool : public TableColumn
{
public:
    ColumnOfBool(QString header = QString());
    QVariant operator[](int index);
    QString insertValue(int index, QString value);
    void append(const QString &value);
    QString insertDefaultValue(int index);
    void reserve(int capacity);
    int count() const;
    void writeToXml(QXmlStreamWriter *writer);

private:
    QList<bool> m_values;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ColumnOfInt : public TableColumn
{
public:
    ColumnOfInt(QString header = QString());
    QVariant operator[](int index);
    QString insertValue(int index, QString value);
    void append(const QString &value);
    QString insertDefaultValue(int index);
    void reserve(int capacity);
    int count() const;
    void writeToXml(QXmlStreamWriter *writer);

private:
    QList<int> m_values;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ColumnOfReal : public TableColumn
{
public:
    ColumnOfReal(QString header = QString());
    QVariant operator[](int index);
    QString insertValue(int index, QString value);
    void append(const QString &value);
    QString insertDefaultValue(int index);
    void reserve(int capacity);
    int count() const;
    void writeToXml(QXmlStreamWriter *writer);

private:
    QList<double> m_values;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ColumnOfString : public TableColumn
{
public:
    ColumnOfString(QString header = QString());
    QVariant operator[](int index);
    QString insertValue(int index, QString value);
    void append(const QString &value);
    QString insertDefaultValue(int index);
    void reserve(int capacity);
    int count() const;
    void writeToXml(QXmlStreamWriter *writer);

private:
    QList<QString> m_values;
};



#endif // TABLECOLUMN_H
