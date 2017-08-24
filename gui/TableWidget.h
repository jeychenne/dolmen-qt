/*
 * TableWidget.h
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
 * Purpose: implements a table widget
 * Created: 16/4/2012
 *
 * The development of this module has been supported by the Japanese
 * Society for the Promotion of Science (JSPS), Grant-in-Aid for Scientific
 * Research (B) n°23320121 (2011-2014).
 * Project title: A corpus-based longitudinal study of the interphonological
 * features of Japanese learners of French.
 * Project leader: Sylvain DETEY (Waseda University, Tokyo, Japan).
 */

#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableView>
#include <QHeaderView>

#include "lib/DataTable.h"
#include "gui/ContextMenu.h"
#include "utils/Global.h"

class TableWidget : public QTableView
{
    Q_OBJECT
public:
    TableWidget(DataTable *table, QWidget *parent = 0);
    
signals:
    
public slots:

private slots:
    void showColumnMenu(const QPoint & pos);
    void showRecodeDialog();
    
private:
    DataTable  *m_table;
    TableModel *m_model;
};

#endif // TABLEWIDGET_H
