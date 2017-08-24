/*
 * TableView.h
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
 * Purpose: implements a table view
 * Created: 21/4/2012
 *
 * The development of this module has been supported by the Japanese
 * Society for the Promotion of Science (JSPS), Grant-in-Aid for Scientific
 * Research (B) n°23320121 (2011-2014).
 * Project title: A corpus-based longitudinal study of the interphonological
 * features of Japanese learners of French.
 * Project leader: Sylvain DETEY (Waseda University, Tokyo, Japan).
 */

#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QFileInfo>
#include <QToolButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QToolBar>

#include "lib/DataTable.h"
#include "lib/TableModel.h"
#include "gui/View.h"
#include "gui/TableWidget.h"
#include "utils/Global.h"

class TableView : public View
{
    Q_OBJECT
public:
    TableView(QTabWidget *parent, DataTable *table);
    void leave();
    QString id() const;

signals:
    void dataTableCreated(DataTable *t);

private slots:
    void saveToDisk();

private:
    DataTable   *m_table;
    TableModel  *m_model;
    TableWidget *m_tableWidget;
    QTabWidget  *m_tabWidget;

    void setupUi();
    void setText();
};

#endif // TABLEVIEW_H
