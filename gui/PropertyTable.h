/*
 * PropertyTable.h
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


#ifndef TAGTABLE_H
#define TAGTABLE_H

#include <QTableWidget>
#include <QHeaderView>
#include <QComboBox>
#include "lib/metadata.h"

using namespace metadata;

class PropertyTable : public QTableWidget
{
    Q_OBJECT
public:
    explicit PropertyTable(PropertySet properties, QWidget *parent = NULL);

    QString category(int row) const;
    QString value(int row) const;
    void resetProperties(PropertySet properties);
signals:
    
public slots:

private:
    void addProperty(Property *tag);
};

#endif // TAGTABLE_H
