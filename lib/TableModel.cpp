/*
 * Table.cpp
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
 * Purpose: implement a data table
 * Created: 15/04/2012
 *
 * The development of this module has been supported by the Japanese
 * Society for the Promotion of Science (JSPS), Grant-in-Aid for Scientific
 * Research (B) n°23320121 (2011-2014).
 * Project title: A corpus-based longitudinal study of the interphonological
 * features of Japanese learners of French.
 * Project leader: Sylvain DETEY (Waseda University, Tokyo, Japan).
 */

#include "TableModel.h"

TableModel::TableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    modified = false;
}

TableModel::TableModel(QList<QStringList> rows, QObject *parent) :
    QAbstractTableModel(parent)
{
    modified = true;
    // the first row contains types
    QStringList types = rows.takeFirst();
    // the second row contains header labels
    QStringList header = rows.takeFirst();

    int colSize = rows.count();
    int ncols = header.count();

    for (int i = 0; i < ncols; ++i)
    {
        TableColumn *column;
        QString type = types[i];

        if (type == "boolean")
            column = new ColumnOfBool(header[i]);
        else if (type == "integer")
            column = new ColumnOfInt(header[i]);
        else if (type == "real")
            column = new ColumnOfReal(header[i]);
        else
            column = new ColumnOfString(header[i]);

        column->reserve(colSize);
        m_cols.append(column);
    }

    foreach (QStringList row, rows)
    {
        for (int col = 0; col < row.count(); ++col)
        {
            m_cols[col]->append(row[col]);
        }
    }
}

//QModelIndex TableModel::index(int row, int column, const QModelIndex &parent) const
//{
//    Q_UNUSED(parent);
//    return this->createIndex(row, column);
//}

//QModelIndex TableModel::parent(const QModelIndex &child) const
//{
//    Q_UNUSED(child);
//    return QModelIndex();
//}

bool TableModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return false;
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_cols.at(0)->count();
}


int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_cols.count();
}


QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QVariant(m_cols[section]->header());
    else
        return QVariant(++section);
}

//bool TableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole)
//{

//}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    TableColumn *col = m_cols[index.column()];
    QVariant data = (*col)[index.row()];

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return data;
    default:
        return QVariant();
    }
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index); Q_UNUSED(value); Q_UNUSED(role);

    return true;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index); // | Qt::ItemIsEditable;
}


QList<TableColumn*> TableModel::columns() const
{
    return m_cols;
}

void TableModel::append(TableColumn *column)
{
    m_cols.append(column);
    modified = true;
}

bool TableModel::isModified() const
{
    return modified;
}

void TableModel::setModified(const bool v)
{
    modified = v;
}


void TableModel::writeToExcel(QXmlStreamWriter *writer)
{
    int nrow, ncol;

    ncol = m_cols.count();
    nrow = m_cols[0]->count();

    writer->writeStartElement("Table");
    writer->writeAttribute("ss:ExpandedColumnCount", QString::number(ncol, 'f', 15));
    writer->writeAttribute("ss:ExpandedRowCount", QString::number(nrow + 1, 'f', 15));
    writer->writeAttribute("x:FullColumns", "1");
    writer->writeAttribute("x:FullRows", "1");

    // write header
    writer->writeStartElement("Row");
//    writer->writeAttribute("ss:StyleID", "s62"); // bold face
    foreach (TableColumn *col, m_cols)
    {
        writer->writeStartElement("Cell");
        writer->writeStartElement("Data");
        writer->writeAttribute("ss:Type", "String");
        writer->writeCharacters(col->header());
        writer->writeEndElement(); // </Data>
        writer->writeEndElement(); // </Cell>
    }
    writer->writeEndElement(); // </Row>

    // write values
    for (int r = 0; r < nrow; ++r)
    {
        writer->writeStartElement("Row");

        for (int c = 0; c < ncol; ++c)
        {
            writer->writeStartElement("Cell");
            writer->writeStartElement("Data");
            writer->writeAttribute("ss:Type", "String");
            TableColumn *column = m_cols[c];
            writer->writeCharacters((*column)[r].toString());
            writer->writeEndElement(); // </Data>
            writer->writeEndElement(); // </Cell>
        }
        writer->writeEndElement(); // </Row>
    }
    writer->writeEndElement(); // </Table>
}

QList<QStringList> TableModel::asPrintableTable() const
{
    int nrow, ncol;
    QList<QStringList> rows;
    QStringList row;

    ncol = m_cols.count();
    nrow = m_cols[0]->count();

    foreach (TableColumn *col, m_cols)
        row << col->header();

    rows << row;

    for (int r = 0; r < nrow; ++r)
    {
        row.clear();

        for (int c = 0; c < ncol; ++c)
        {
            TableColumn *column = m_cols[c];
            row << (*column)[r].toString();
        }
        rows << row;
    }

    return rows;
}

void TableModel::recodeValues(QList<int> columns, QStringList oldValues, QString newValue)
{
    Q_UNUSED(columns); Q_UNUSED(oldValues); Q_UNUSED(newValue);
}



