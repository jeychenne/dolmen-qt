/*
 * BottomPane.h
 *
 * Copyright (C) 2010-2013 Julien Eychenne
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
 * Purpose:
 * Created: 02/09/10
 */

#ifndef BOTTOMPANE_H
#define BOTTOMPANE_H

#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>

#include "search/Query.h"
#include "gui/ContextMenu.h"
#include "dolmen.h"
#include "utils/Global.h"
#include "utils/routines.h"
#include "LuaConsole.h"

class PluginHandler;


class BottomPane : public QTabWidget
{
	Q_OBJECT
public:
    BottomPane(QWidget *parent, sol::state_view lua);

	void appendQuery(Query*);

    void setHandler(PluginHandler *handler);

private slots:
	void displaySearchContextMenu(const QPoint &pos);
	void saveQuery();
	void onDoubleClickQuery(QModelIndex);

signals:
	void reopenQuery(int);
	void file_error(QString msg);

private:
    QWidget     *queries_tab;
	QListWidget *searches_list;
    LuaConsole    *console;


};


#endif // BOTTOMPANE_H
