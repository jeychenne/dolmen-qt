/*
 * TableWidget.cpp
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

#include "TableWidget.h"

TableWidget::TableWidget(DataTable *table, QWidget *parent) :
    QTableView(parent)
{
    m_table = table;
    m_model = table->model();

    this->horizontalHeader()->show();
    this->setModel(table->model());

    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showColumnMenu(QPoint)));
}

void TableWidget::showColumnMenu(const QPoint &pos)
{
    if (selectedIndexes().isEmpty())
        return;

    ContextMenu *menu = new ContextMenu(this);

    QAction *recodeAction = new QAction(this);
    recodeAction->setText(tr("Recode values..."));
    menu->addAction(recodeAction);

    QAction *duplicateAction = new QAction(this);
    duplicateAction->setText(tr("Duplicate column"));
    menu->addAction(duplicateAction);
    duplicateAction->setEnabled(false);

    QAction *rmColAction = new QAction(this);
    rmColAction->setText(tr("Remove column"));
    menu->addAction(rmColAction);
    rmColAction->setEnabled(false);

    connect(recodeAction, SIGNAL(triggered()), this, SLOT(showRecodeDialog()));
    menu->display(pos);
}

void TableWidget::showRecodeDialog()
{
    QList<int> columns;
    QStringList oldValues;
    QString value;

    foreach (QModelIndex i, this->selectedIndexes())
        columns.append(i.column());

    m_model->recodeValues(columns, oldValues, value);

    if (Global::Output) {
        Global::Output->showMessage(QString("recoding value..."), 3000);
    }
}
