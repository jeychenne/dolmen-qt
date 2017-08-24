/*
 * TableView.cpp
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

#include "TableView.h"

TableView::TableView(QTabWidget *parent, DataTable *table) : View(parent)
{
    m_table = table;
    m_model = table->model();
    m_tabWidget = parent;

    this->setupUi();
}

void TableView::setupUi()
{
    m_tableWidget = new TableWidget(m_table);
    QToolBar *toolbar = new QToolBar;

    QToolButton *save_btn = new QToolButton;
    save_btn->setIcon(QIcon(":/icons/filesave.png"));
    save_btn->setToolTip(tr("Save table to disk"));

    QToolButton *stat_btn = new QToolButton;
    stat_btn->setIcon(QIcon(":/icons/combo_chart.png"));
    stat_btn->setToolTip(tr("Statistics functions"));
    stat_btn->setEnabled(false);

    toolbar->addWidget(save_btn);
    toolbar->addWidget(stat_btn);
    toolbar->setIconSize(QSize(25,25));

    layout->addSpacing(5);
    layout->addWidget(toolbar);
    layout->addWidget(m_tableWidget);

    this->setText();

    connect(save_btn, SIGNAL(clicked()), this, SLOT(saveToDisk()));
}

void TableView::setText()
{
    QString path = m_table->path();
    int index = m_tabWidget->indexOf(this);

    if (path.isEmpty())
    {
        m_tabWidget->setTabText(index, tr("Untitled table"));
    }
    else
    {
        QFileInfo info(path);
        m_tabWidget->setTabText(index, info.fileName());
    }
}

void TableView::leave()
{

}

QString TableView::id() const
{
    return m_table->path();
}

void TableView::saveToDisk()
{
    /*
    QString line = \
            "<table color=\"red\" border=\"1\"><tr><td>row 1 cell 1</td><td>row 1 cell 2</td></tr><tr><td>row 2 cell 1</td><td>row 2 cell 2</td></tr></table>";
    Global::Output.write(line);
    */
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save data table..."), QDir::homePath(), tr("Dolmen table (*.dmt)"));


    if (! fileName.isEmpty())
    {
        if (!fileName.toLower().endsWith(".dmt"))
            fileName += ".dmt";

        m_table->setPath(fileName);
        m_table->save();
        setText();
        emit dataTableCreated(m_table);
    }
}
