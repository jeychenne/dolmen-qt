/*
 * Table.h
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

#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QObject>
#include <QStringList>
#include <QAbstractTableModel>
#include <QXmlStreamWriter>

#include "lib/TableColumn.h"

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TableModel(QList<QStringList> rows, QObject *parent = NULL);
    TableModel(QObject *parent = NULL);

    int rowCount (const QModelIndex &parent = QModelIndex()) const;
    int columnCount (const QModelIndex &parent = QModelIndex()) const;
    QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
//    bool setHeaderData (int section, Qt::Orientation orientation, const QVariant &value, int role);
    bool setData (const QModelIndex &index, const QVariant & value, int role = Qt::EditRole);
    Qt::ItemFlags flags (const QModelIndex &index) const;
//    QModelIndex index(int row, int column, const QModelIndex &parent) const;
//    QModelIndex parent(const QModelIndex &child) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    QList<TableColumn*> columns() const;
    void append(TableColumn *column);
    bool isModified() const;
    void setModified(const bool v);
    void writeToExcel(QXmlStreamWriter *writer);
    void recodeValues(QList<int> columns, QStringList oldValues, QString newValue);
    QList<QStringList> asPrintableTable() const;

    //insertRows(), removeRows(), insertColumns(), and removeColumns()

private:
    QList<TableColumn*> m_cols;
    bool modified;
};

#endif // TABLEMODEL_H
