/*
 * PropertyTable.cpp
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
 * Purpose: tag table
 * Created: 26/03/2012
 */

#include "PropertyTable.h"

#define CAT_COL 0
#define VAL_COL 1

PropertyTable::PropertyTable(PropertySet properties, QWidget *parent) :
    QTableWidget(0, 2, parent)
{
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->verticalHeader()->hide();
    this->setHorizontalHeaderItem(CAT_COL, new QTableWidgetItem(tr("Category")));
    this->setHorizontalHeaderItem(VAL_COL, new QTableWidgetItem(tr("Value")));

    foreach (Property *tag, properties)
        this->addProperty(tag);

    this->horizontalHeader()->setStretchLastSection(true);
}

void PropertyTable::resetProperties(PropertySet properties)
{
    while (rowCount() > 0)
        removeRow(0);

    foreach (Property *tag, properties)
        this->addProperty(tag);
}

void PropertyTable::addProperty(Property *tag)
{
    int row = rowCount();

    QTableWidgetItem *cat = new QTableWidgetItem(tag->category());
    QTableWidgetItem *val = new QTableWidgetItem(tag->value());

    cat->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    val->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    this->insertRow(row);
    this->setItem(row, CAT_COL, cat);
    this->setItem(row, VAL_COL, val);

    verticalHeader()->resizeSection(row, 20);
}

QString PropertyTable::category(int row) const
{
    return item(row, CAT_COL)->text();
}

QString PropertyTable::value(int row) const
{
    return item(row, VAL_COL)->text();
}
