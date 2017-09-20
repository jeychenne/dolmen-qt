/*
 * ScriptBrowser.cpp
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
 * Purpose: display scripts in the leftmost panel.
 * Created: 15/09/2017
 */

#include "ScriptBrowser.h"

ScriptBrowser::ScriptBrowser(QWidget *parent) :
    ElementBrowser(parent)
{
#ifdef Q_OS_MAC
    headerItem()->setHidden(true);
#else
    headerItem()->setText(0, "Scripts");
#endif
}

void ScriptBrowser::displayElements()
{
    QIcon icon(":/icons/16x16/script.png");
    for (auto *elem : m_content)
    {
        Script *script = dynamic_cast<Script*>(elem);
        QTreeWidgetItem *item = new QTreeWidgetItem(this);
        item->setText(0, script->label());
        item->setToolTip(0, script->path());
        item->setIcon(0, icon);
//        item->setData(0, Qt::UserRole, script->id());

//        if (script->type() == "SearchMatch")
//        {
//            SearchMatch *qm = static_cast<SearchMatch*>(script);
//            connect(qm, SIGNAL(openDFile(DFile*,double,double)), this, SIGNAL(openDFile(DFile*,double,double)));
//        }
    }
}

void ScriptBrowser::finalizeElements()
{

}


